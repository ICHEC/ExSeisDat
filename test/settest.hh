#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "tglobal.hh"
#define private public
#define protected public
#include "file/file.hh"
#include "set/set.hh"
#include "set.hh"
#undef private
#undef protected

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
   *arg2 = *src;
}

struct SetTest : public Test
{
    std::shared_ptr<ExSeisPIOL> piol;
    std::unique_ptr<Set> set;
    std::deque<File::Param> prm;
    Comm::MPI::Opt opt;
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
                    EXPECT_CALL(*mock, readParam(dec.second, An<csize_t *>(), _, _))
                                    .Times(Exactly(srtCnt))
                                    .WillRepeatedly(cpyprm(&prm.back()));

                    set->add(std::move(mock));
                }
    }


    void init(size_t numFile, size_t nt, size_t inactive)
    {
//        constexpr size_t NOT_IN_OUTPUT = std::numeric_limits<size_t>::max();
 //       srand(1337);
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

/*            llint range = (nt / inactive);
            for (size_t j = 0; j < inactive; j++)
            {
                size_t randj = range*j + (rand() % range);
                set->file[i]->olst[randj] = NOT_IN_OUTPUT;
            }*/
        }
    }
};

