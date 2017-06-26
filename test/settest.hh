#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "tglobal.hh"
#define private public
#define protected public
#include "file/file.hh"
#include "flow/set.hh"
#undef private
#undef protected
#include "file/filesegy.hh"
#include "cppfileapi.hh"
#include "object/objsegy.hh"
#include "data/datampiio.hh"

namespace PIOL {
extern std::pair<size_t, size_t> decompose(size_t sz, size_t numRank, size_t rank);     //TODO: TEMP!
}

using namespace PIOL;
using namespace testing;

class MockFile : public File::ReadInterface
{
    public :
    MOCK_CONST_METHOD0(readNs, size_t(void));
    MOCK_METHOD0(readNt, size_t(void));
    MOCK_CONST_METHOD0(readInc, geom_t(void));

    MOCK_METHOD1(writeText, void(const std::string));
    MOCK_METHOD1(writeNs, void(const csize_t));
    MOCK_METHOD1(writeNt, void(const csize_t));
    MOCK_METHOD1(writeInc, void(const geom_t));

    MOCK_CONST_METHOD4(readParam, void(csize_t, csize_t, File::Param *, csize_t));
    MOCK_CONST_METHOD4(readParam, void(csize_t, csize_t *, File::Param *, csize_t));
    MOCK_CONST_METHOD5(readTrace, void(csize_t, csize_t, trace_t *, File::Param *, csize_t));
    MOCK_CONST_METHOD5(readTrace, void(csize_t, csize_t *, trace_t *, File::Param *, csize_t));
    MOCK_CONST_METHOD5(readTraceNonMono, void(csize_t, csize_t *, trace_t *, File::Param *, csize_t));
};

ACTION_P(cpyprm, src)
{
   *arg3 = *src;
}

extern void muting(size_t nt, size_t ns, trace_t * trc, size_t mute);
extern void taperMan(size_t nt, size_t ns, trace_t * trc, TaperFunc func, size_t nTailLft, size_t nTailRt);


template <class T>
std::shared_ptr<T> makeTest(Piol piol, std::string name)
{
        File::WriteSEGY::Opt f;
        File::ReadSEGY::Opt rf;
        Obj::SEGY::Opt o;
        Data::MPIIO::Opt d;
        auto data = std::make_shared<Data::MPIIO>(piol, name, d, FileMode::Test);
        auto obj = std::make_shared<Obj::SEGY>(piol, name, o, data, FileMode::Test);
        return std::make_shared<T>(piol, name, obj);
}

struct SetTest : public Test
{
    std::shared_ptr<ExSeisPIOL> piol;
    std::unique_ptr<Set> set;
    std::deque<File::Param> prm;
    Comm::MPI::Opt opt;
    const double pi = M_PI;

    SetTest(void)
    {
        opt.initMPI = false;
        piol = std::make_shared<ExSeisPIOL>(opt);
        set = nullptr;
    }
    void init(size_t numFile, size_t numNs, size_t numInc, size_t srtCnt, bool linear)
    {
        if (set.get() != nullptr)
            set.release();
        set = std::make_unique<Set>(piol);
        for (size_t j = 0; j < numNs; j++)
            for (size_t k = 0; k < numInc; k++)
                for (size_t i = 0; i < numFile; i++)
                {
                    auto mock = std::make_unique<MockFile>();
                    size_t nt = 1000U + i;
                    EXPECT_CALL(*mock, readNt()).WillRepeatedly(Return(nt));
                    EXPECT_CALL(*mock, readNs()).WillRepeatedly(Return(1000U + i));
                    EXPECT_CALL(*mock, readInc()).WillRepeatedly(Return(1000. + geom_t(i)));

                    auto dec = decompose(nt, piol->comm->getNumRank(), piol->comm->getRank());
                    prm.emplace_back(dec.second);
                    File::Param * tprm = &prm.back();

                    if (linear)
                        for (size_t l = 0; l < dec.second; l++)
                        {
                            setPrm(l, Meta::xSrc, 2000. - geom_t(dec.first + l), tprm);
                            setPrm(l, Meta::ySrc, 2000. - geom_t(dec.first + l), tprm);
                            setPrm(l, Meta::xRcv, 2000. + geom_t(dec.first + l), tprm);
                            setPrm(l, Meta::yRcv, 2000. + geom_t(dec.first + l), tprm);
                            setPrm(l, Meta::xCmp, 2000. - geom_t(dec.first + l), tprm);
                            setPrm(l, Meta::yCmp, 2000. - geom_t(dec.first + l), tprm);
                            setPrm(l, Meta::il, 2000U + dec.first + l, tprm);
                            setPrm(l, Meta::xl, 2000U + dec.first + l, tprm);
                            setPrm(l, Meta::tn, l+dec.first, tprm);

                            auto xS = 2000. - geom_t((dec.first + l) % (nt / 10U));
                            auto yR = 2000. + geom_t((dec.first + l) / (nt / 10U));
                            setPrm(l, Meta::Offset, (xS-2000.)*(xS-2000.) + (2000.-yR)*(2000.-yR), tprm);
                        }
                    else
                        for (size_t l = 0; l < dec.second; l++)
                        {
                            auto xS = 2000U - (dec.first + l) % (nt / 10U);
                            auto yR = 2000U + (dec.first + l) / (nt / 10U);
                            setPrm(l, Meta::xSrc, xS, tprm);
                            setPrm(l, Meta::ySrc, 2000., tprm);
                            setPrm(l, Meta::xRcv, 2000., tprm);
                            setPrm(l, Meta::yRcv, yR, tprm);
                            setPrm(l, Meta::xCmp, xS, tprm);
                            setPrm(l, Meta::yCmp, yR, tprm);
                            setPrm(l, Meta::il, 4000U - xS, tprm);
                            setPrm(l, Meta::xl, yR, tprm);
                            setPrm(l, Meta::tn, l+dec.first, tprm);
                            setPrm(l, Meta::Offset, (xS-2000.)*(xS-2000.) + (2000.-yR)*(2000.-yR), tprm);
                        }
                    EXPECT_CALL(*mock, readTrace(dec.second, An<csize_t *>(), _, _, 0))
                                                .Times(Exactly(1U))
                                                .WillRepeatedly(cpyprm(&prm.back()));

                    set->add(std::move(mock));
                }
    }

    void init(size_t numFile, size_t nt, size_t inactive)
    {
        constexpr size_t NOT_IN_OUTPUT = std::numeric_limits<size_t>::max();
        srand(1337);
        if (set.get() != nullptr)
            set.release();
        set = std::make_unique<Set>(piol);
        for (size_t i = 0; i < numFile; i++)
        {
            auto mock = std::make_unique<MockFile>();
            EXPECT_CALL(*mock, readNt()).WillRepeatedly(Return(nt));
            EXPECT_CALL(*mock, readNs()).WillRepeatedly(Return(1000U));
            EXPECT_CALL(*mock, readInc()).WillRepeatedly(Return(1000.));

            set->add(std::move(mock));

 /*           llint range = (nt / inactive);

#warning Randomly delete traces from the lists
            for (size_t j = 0; j < inactive; j++)
            {
                size_t randj = range*j + (rand() % range);
                set->file[i]->lst[randj] = NOT_IN_OUTPUT;
            }*/
        }
    }

    void taperTest(size_t nt, size_t ns, size_t mute, TaperFunc tapFunc, TaperType type, size_t nTailLft, size_t nTailRt)
    {
        if (set.get() != nullptr)
            set.release();
        set = std::make_unique<Set>(piol);
        auto mock = std::make_unique<MockFile>();

        std::vector<trace_t> trc(nt * ns);
        std::vector<trace_t> trcMan(nt * ns);
        File::Param prm(nt);
        std::fill(trc.begin(), trc.end(), 1.0f);
        if (mute != 0)
            muting(nt, ns, trc.data(), mute);
        trcMan = trc;

        std::vector<size_t> offsets(nt);
        std::iota(offsets.begin(), offsets.end(), 0U);

        EXPECT_CALL(*mock, readNt()).WillRepeatedly(Return(nt));
        EXPECT_CALL(*mock, readNs()).WillRepeatedly(Return(ns));
        EXPECT_CALL(*mock, readInc()).WillRepeatedly(Return(0.004));

        EXPECT_CALL(*mock, readTrace(nt, A<csize_t *>(), A<trace_t *>(), A<File::Param *>(), 0U))
                    .Times(Exactly(1U))
                    .WillRepeatedly(DoAll(check1(offsets.data(), offsets.size()),
                                    SetArrayArgument<2>(trc.begin(), trc.end())));
        set->add(std::move(mock));

        set->taper(type, nTailLft, nTailRt);
        set->outfix = "tmp/temp";
        set.reset();

        std::string name = "tmp/temp.segy";
        auto in = makeTest<File::ReadSEGY>(piol, name);
        in->readTrace(0U, in->readNt(), trc.data());

        taperMan(nt, ns, trcMan.data(), tapFunc, nTailLft, nTailRt);
        for (size_t i = 0; i < nt; i++)
            for (size_t j = 0; j < ns; j++)
                EXPECT_FLOAT_EQ(trc[i*ns+j], trcMan[i*ns+j]);
    }

    void agcTest(size_t nt, size_t ns, AGCType type, std::function<trace_t(size_t, trace_t *,size_t)> agcFunc, size_t window, trace_t normR)
    {
        if (set.get() != nullptr)
            set.release();
        set = std::make_unique<Set>(piol);
        auto mock = std::make_unique<MockFile>();

        std::vector<trace_t> trc(nt*ns);
        std::vector<trace_t> trcMan(nt*ns);

        for (size_t i = 0; i < nt; i++)
            for (size_t j = 0; j < ns; j++)
                trc[i*ns + j] = j;//*pow(-1.0f,j);

        trcMan = trc;

        EXPECT_CALL(*mock, readNt()).WillRepeatedly(Return(nt));
        EXPECT_CALL(*mock, readNs()).WillRepeatedly(Return(ns));
        EXPECT_CALL(*mock, readInc()).WillRepeatedly(Return(0.004));

        std::vector<size_t> offsets(nt);
        std::iota(offsets.begin(), offsets.end(), 0U);

        EXPECT_CALL(*mock, readTrace(nt, A<csize_t *>(), A<trace_t *>(), A<File::Param *>(), 0U))
                    .Times(Exactly(1U))
                    .WillRepeatedly(DoAll(check1(offsets.data(), offsets.size()),
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
            for (size_t j = 0; j < ns; j++)
            {
                if (j < (window/2U)+1)
                {
                    win = j + 1 + (window/2U);
                    winStr = i*ns;
                    winCntr = j;
                }
                else if ((ns - j) < (window/2U) + 1)
                {
                    win = ns - j+ (window/2U);
                    winStr = i*ns + j -  window/2U;
                    winCntr = window/2U;
                }
                else
                {
                    win = window;
                    winStr = i*ns + j - window/2U;
                    winCntr = window/2U;
                }
                std::vector<trace_t> trcWin(trcMan.begin()+winStr, trcMan.begin()+winStr+win);
                ASSERT_FLOAT_EQ(trc[i*ns+j], trcMan[i*ns+j]*normR/agcFunc(win,trcWin.data(),winCntr)) << i << " " << j << std::endl;
            }
    }
};

