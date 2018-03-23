#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "tglobal.hh"

#include "ExSeisDat/Flow/set.hh"
#include "ExSeisDat/PIOL/ExSeis.hh"
#include "ExSeisDat/PIOL/data/datampiio.hh"
#include "ExSeisDat/PIOL/file/file.hh"
#include "ExSeisDat/PIOL/file/ReadSEGY.hh"
#include "ExSeisDat/PIOL/file/WriteSEGY.hh"
#include "ExSeisDat/PIOL/object/objsegy.hh"

#include <numeric>

namespace PIOL {

// TODO: TEMP!
std::pair<size_t, size_t> decompose(size_t sz, size_t numRank, size_t rank);

}  // namespace PIOL

using namespace PIOL;
using namespace testing;

class MockFile : public File::ReadInterface {
  public:
    MockFile() :
        ReadInterface(
          std::shared_ptr<ExSeisPIOL>(), "", std::shared_ptr<Obj::Interface>())
    {
    }

    MOCK_CONST_METHOD0(readNs, size_t(void));
    MOCK_CONST_METHOD0(readNt, size_t(void));
    MOCK_CONST_METHOD0(readInc, geom_t(void));

    MOCK_METHOD1(writeText, void(const std::string));
    MOCK_METHOD1(writeNs, void(const size_t));
    MOCK_METHOD1(writeNt, void(const size_t));
    MOCK_METHOD1(writeInc, void(const geom_t));

    MOCK_CONST_METHOD4(
      readParam, void(const size_t, const size_t, File::Param*, const size_t));

    MOCK_CONST_METHOD4(
      readParamNonContiguous,
      void(const size_t, const size_t*, File::Param*, const size_t));

    MOCK_CONST_METHOD5(
      readTrace,
      void(const size_t, const size_t, trace_t*, File::Param*, const size_t));

    MOCK_CONST_METHOD5(
      readTraceNonContiguous,
      void(const size_t, const size_t*, trace_t*, File::Param*, const size_t));

    MOCK_CONST_METHOD5(
      readTraceNonMonotonic,
      void(const size_t, const size_t*, trace_t*, File::Param*, const size_t));
};

ACTION_P(cpyprm, src)
{
    *arg3 = *src;
}

void muting(size_t nt, size_t ns, trace_t* trc, size_t mute);

void taperMan(
  size_t nt,
  size_t ns,
  trace_t* trc,
  TaperFunc func,
  size_t nTailLft,
  size_t nTailRt);


template<class T>
std::shared_ptr<T> makeTest(std::shared_ptr<ExSeisPIOL> piol, std::string name)
{
    File::WriteSEGY::Opt f;
    File::ReadSEGY::Opt rf;
    Obj::SEGY::Opt o;
    Data::MPIIO::Opt d;
    auto data = std::make_shared<Data::MPIIO>(piol, name, d, FileMode::Test);
    auto obj = std::make_shared<Obj::SEGY>(piol, name, o, data, FileMode::Test);
    return std::make_shared<T>(piol, name, obj);
}

struct Set_public : public Set {
    using Set::Set;

    using Set::calcFunc;
    using Set::file;
    using Set::func;
    using Set::outfix;
};

struct SetTest : public Test {
    std::shared_ptr<ExSeis> piol    = ExSeis::New();
    std::unique_ptr<Set_public> set = nullptr;
    std::deque<File::Param> prm;
    Comm::MPI::Opt opt;
    const double pi = M_PI;

    void init(size_t numFile, size_t numNs, size_t numInc, size_t, bool linear)
    {
        if (set.get() != nullptr) set.release();
        set = std::make_unique<Set_public>(piol);
        for (size_t j = 0; j < numNs; j++)
            for (size_t k = 0; k < numInc; k++)
                for (size_t i = 0; i < numFile; i++) {
                    auto mock = std::make_unique<MockFile>();
                    size_t nt = 1000U + i;
                    EXPECT_CALL(*mock, readNt()).WillRepeatedly(Return(nt));
                    EXPECT_CALL(*mock, readNs())
                      .WillRepeatedly(Return(1000U + i));
                    EXPECT_CALL(*mock, readInc())
                      .WillRepeatedly(Return(1000. + geom_t(i)));

                    auto dec = decompose(
                      nt, piol->comm->getNumRank(), piol->comm->getRank());
                    prm.emplace_back(dec.second);
                    File::Param* tprm = &prm.back();

                    if (linear)
                        for (size_t l = 0; l < dec.second; l++) {
                            setPrm(
                              l, PIOL_META_xSrc, 2000. - geom_t(dec.first + l),
                              tprm);
                            setPrm(
                              l, PIOL_META_ySrc, 2000. - geom_t(dec.first + l),
                              tprm);
                            setPrm(
                              l, PIOL_META_xRcv, 2000. + geom_t(dec.first + l),
                              tprm);
                            setPrm(
                              l, PIOL_META_yRcv, 2000. + geom_t(dec.first + l),
                              tprm);
                            setPrm(
                              l, PIOL_META_xCmp, 2000. - geom_t(dec.first + l),
                              tprm);
                            setPrm(
                              l, PIOL_META_yCmp, 2000. - geom_t(dec.first + l),
                              tprm);
                            setPrm(
                              l, PIOL_META_il, 2000U + dec.first + l, tprm);
                            setPrm(
                              l, PIOL_META_xl, 2000U + dec.first + l, tprm);
                            setPrm(l, PIOL_META_tn, l + dec.first, tprm);

                            auto xS =
                              2000. - geom_t((dec.first + l) % (nt / 10U));
                            auto yR =
                              2000. + geom_t((dec.first + l) / (nt / 10U));
                            setPrm(
                              l, PIOL_META_Offset,
                              (xS - 2000.) * (xS - 2000.)
                                + (2000. - yR) * (2000. - yR),
                              tprm);
                        }
                    else
                        for (size_t l = 0; l < dec.second; l++) {
                            auto xS = 2000U - (dec.first + l) % (nt / 10U);
                            auto yR = 2000U + (dec.first + l) / (nt / 10U);
                            setPrm(l, PIOL_META_xSrc, xS, tprm);
                            setPrm(l, PIOL_META_ySrc, 2000., tprm);
                            setPrm(l, PIOL_META_xRcv, 2000., tprm);
                            setPrm(l, PIOL_META_yRcv, yR, tprm);
                            setPrm(l, PIOL_META_xCmp, xS, tprm);
                            setPrm(l, PIOL_META_yCmp, yR, tprm);
                            setPrm(l, PIOL_META_il, 4000U - xS, tprm);
                            setPrm(l, PIOL_META_xl, yR, tprm);
                            setPrm(l, PIOL_META_tn, l + dec.first, tprm);
                            setPrm(
                              l, PIOL_META_Offset,
                              (xS - 2000.) * (xS - 2000.)
                                + (2000. - yR) * (2000. - yR),
                              tprm);
                        }
                    EXPECT_CALL(
                      *mock, readTraceNonContiguous(
                               dec.second, An<const size_t*>(), _, _, 0))
                      .Times(Exactly(1U))
                      .WillRepeatedly(cpyprm(&prm.back()));

                    set->add(std::move(mock));
                }
    }

    void init(size_t numFile, size_t nt, size_t)
    {
        srand(1337);
        set.reset(new Set_public(piol));
        for (size_t i = 0; i < numFile; i++) {
            auto mock = std::make_unique<MockFile>();
            EXPECT_CALL(*mock, readNt()).WillRepeatedly(Return(nt));
            EXPECT_CALL(*mock, readNs()).WillRepeatedly(Return(1000U));
            EXPECT_CALL(*mock, readInc()).WillRepeatedly(Return(1000.));

            set->add(std::move(mock));

            // llint range = (nt / inactive);
            //
            // #warning Randomly delete traces from the lists
            // for (size_t j = 0; j < inactive; j++)
            // {
            //     size_t randj = range*j + (rand() % range);
            //     set->file[i]->lst[randj] = NOT_IN_OUTPUT;
            // }
        }
    }

    void taperTest(
      size_t nt,
      size_t ns,
      size_t mute,
      TaperFunc tapFunc,
      TaperType type,
      size_t nTailLft,
      size_t nTailRt)
    {
        set.reset(new Set_public(piol));
        auto mock = std::make_unique<MockFile>();

        std::vector<trace_t> trc(nt * ns);
        std::vector<trace_t> trcMan(nt * ns);
        File::Param prm(nt);
        std::fill(trc.begin(), trc.end(), 1.0f);
        if (mute != 0) muting(nt, ns, trc.data(), mute);
        trcMan = trc;

        std::vector<size_t> offsets(nt);
        std::iota(offsets.begin(), offsets.end(), 0U);

        EXPECT_CALL(*mock, readNt()).WillRepeatedly(Return(nt));
        EXPECT_CALL(*mock, readNs()).WillRepeatedly(Return(ns));
        EXPECT_CALL(*mock, readInc()).WillRepeatedly(Return(0.004));

        EXPECT_CALL(
          *mock,
          readTraceNonContiguous(
            nt, A<const size_t*>(), A<trace_t*>(), A<File::Param*>(), 0U))
          .Times(Exactly(1U))
          .WillRepeatedly(DoAll(
            check1(offsets.data(), offsets.size()),
            SetArrayArgument<2>(trc.begin(), trc.end())));
        set->add(std::move(mock));

        set->taper(type, nTailLft, nTailRt);
        set->outfix = "tmp/temp";
        set.reset();

        std::string name = "tmp/temp.segy";
        auto in          = makeTest<File::ReadSEGY>(piol, name);
        in->readTrace(0U, in->readNt(), trc.data());

        taperMan(nt, ns, trcMan.data(), tapFunc, nTailLft, nTailRt);
        for (size_t i = 0; i < nt; i++)
            for (size_t j = 0; j < ns; j++)
                EXPECT_FLOAT_EQ(trc[i * ns + j], trcMan[i * ns + j]);
    }

    void agcTest(
      size_t nt,
      size_t ns,
      AGCType type,
      std::function<trace_t(size_t, trace_t*, size_t)> agcFunc,
      size_t window,
      trace_t normR)
    {
        set.reset(new Set_public(piol));
        auto mock = std::make_unique<MockFile>();

        std::vector<trace_t> trc(nt * ns);
        std::vector<trace_t> trcMan(nt * ns);

        for (size_t i = 0; i < nt; i++) {
            for (size_t j = 0; j < ns; j++) {
                trc[i * ns + j] = j;  //*pow(-1.0f,j);
            }
        }

        trcMan = trc;

        EXPECT_CALL(*mock, readNt()).WillRepeatedly(Return(nt));
        EXPECT_CALL(*mock, readNs()).WillRepeatedly(Return(ns));
        EXPECT_CALL(*mock, readInc()).WillRepeatedly(Return(0.004));

        std::vector<size_t> offsets(nt);
        std::iota(offsets.begin(), offsets.end(), 0U);

        EXPECT_CALL(
          *mock,
          readTraceNonContiguous(
            nt, A<const size_t*>(), A<trace_t*>(), A<File::Param*>(), 0U))
          .Times(Exactly(1U))
          .WillRepeatedly(DoAll(
            check1(offsets.data(), offsets.size()),
            SetArrayArgument<2>(trc.begin(), trc.end())));
        set->add(std::move(mock));

        set->AGC(type, window, normR);
        set->outfix = "tmp/temp";
        set.reset();

        std::string name = "tmp/temp.segy";

        auto in = makeTest<File::ReadSEGY>(piol, name);
        in->readTrace(0U, in->readNt(), trc.data());

        size_t win;
        size_t winStr;
        size_t winCntr;
        for (size_t i = 0; i < nt; i++)
            for (size_t j = 0; j < ns; j++) {
                if (j < (window / 2U) + 1) {
                    win     = j + 1 + (window / 2U);
                    winStr  = i * ns;
                    winCntr = j;
                }
                else if ((ns - j) < (window / 2U) + 1) {
                    win     = ns - j + (window / 2U);
                    winStr  = i * ns + j - window / 2U;
                    winCntr = window / 2U;
                }
                else {
                    win     = window;
                    winStr  = i * ns + j - window / 2U;
                    winCntr = window / 2U;
                }
                std::vector<trace_t> trcWin(
                  trcMan.begin() + winStr, trcMan.begin() + winStr + win);
                ASSERT_FLOAT_EQ(
                  trc[i * ns + j], trcMan[i * ns + j] * normR
                                     / agcFunc(win, trcWin.data(), winCntr))
                  << i << " " << j << std::endl;
            }
    }

    void filterTest(
      FltrType type,
      FltrDmn domain,
      std::vector<trace_t> corners,
      const std::vector<trace_t>& trcRef,
      size_t nt = 1LU)
    {
        ASSERT_EQ(trcRef.size(), static_cast<size_t>(59));
        trace_t PI = std::acos(-1);
        size_t N   = 3;
        size_t ns  = trcRef.size() / nt;
        set.reset(new Set_public(piol));

        std::vector<trace_t> trc(trcRef.size());

        for (size_t i = 0; i < nt; i++)
            for (size_t j = 0; j < ns; j++)
                trc[i * ns + j] =
                  std::sin(4.8_t * PI * (trace_t(j)) / trace_t(ns))
                  + 1.5_t * std::cos(36_t * PI * (trace_t(j)) / trace_t(ns))
                  + 0.5_t * std::sin(48.0_t * PI * (trace_t(j)) / trace_t(ns));


        set.reset(new Set_public(piol));
        auto mock = std::make_unique<MockFile>();

        EXPECT_CALL(*mock, readNt()).WillRepeatedly(Return(nt));
        EXPECT_CALL(*mock, readNs()).WillRepeatedly(Return(ns));
        EXPECT_CALL(*mock, readInc()).WillRepeatedly(Return(0.004));

        std::vector<size_t> offsets(nt);
        std::iota(offsets.begin(), offsets.end(), 0U);

        EXPECT_CALL(
          *mock,
          readTraceNonContiguous(
            nt, A<const size_t*>(), A<trace_t*>(), A<File::Param*>(), 0U))
          .Times(Exactly(1U))
          .WillRepeatedly(DoAll(
            check1(offsets.data(), offsets.size()),
            SetArrayArgument<2>(trc.begin(), trc.end())));
        set->add(std::move(mock));

        set->temporalFilter(type, domain, PadType::Zero, 30_t, N, corners);
        set->outfix = "tmp/temp";
        set.reset();

        std::string name = "tmp/temp.segy";

        auto in = makeTest<File::ReadSEGY>(piol, name);

        in->readTrace(0U, in->readNt(), trc.data());

        for (size_t i = 0; i < nt * ns; i++)
            ASSERT_NEAR(trc[i], trcRef[i], (__GNUC__ ? 0.00011_t : .00001_t));
    }
};
