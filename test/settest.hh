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

class MockFile : public File::Interface
{
    public :
    MOCK_CONST_METHOD0(readNs, size_t(void));
    MOCK_METHOD0(readNt, size_t(void));
    MOCK_CONST_METHOD0(readInc, geom_t(void));

    MOCK_METHOD1(writeText, void(const std::string));
    MOCK_METHOD1(writeNs, void(const csize_t));
    MOCK_METHOD1(writeNt, void(const csize_t));
    MOCK_METHOD1(writeInc, void(const geom_t));

    MOCK_CONST_METHOD3(readParam, void(csize_t, csize_t, File::Param *));
    MOCK_CONST_METHOD3(readParam, void(csize_t, csize_t *, File::Param *));
    MOCK_CONST_METHOD4(readTrace, void(csize_t, csize_t, trace_t *, File::Param *));
    MOCK_CONST_METHOD4(readTrace, void(csize_t, csize_t *, trace_t *, File::Param *));

    MOCK_METHOD3(writeParam, void(csize_t, csize_t, const File::Param *));
    MOCK_METHOD4(writeTrace, void(csize_t, csize_t, trace_t *, const File::Param *));
    MOCK_METHOD4(writeTrace, void(csize_t, csize_t *, trace_t *, const File::Param *));
    MOCK_METHOD3(writeParam, void(csize_t, csize_t *, const File::Param *));
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
                    size_t nt = 1000LU + i;
                    EXPECT_CALL(*mock, readNt()).WillRepeatedly(Return(nt));
                    EXPECT_CALL(*mock, readNs()).WillRepeatedly(Return(1000U + i));
                    EXPECT_CALL(*mock, readInc()).WillRepeatedly(Return(1000. + geom_t(i)));

                    auto dec = decompose(nt, piol->comm->getNumRank(), piol->comm->getRank());
                    prm.emplace_back(dec.second);

                    size_t numFloat = prm.back().r->numFloat;
                    File::Param * tprm = &prm.back();

                    if (linear)
                        for (size_t l = 0; l < dec.second; l++)
                        {
                            setPrm(l, File::Meta::xSrc, 2000. - geom_t(dec.first + l), tprm);
                            setPrm(l, File::Meta::ySrc, 2000. - geom_t(dec.first + l), tprm);
                            setPrm(l, File::Meta::xRcv, 2000. + geom_t(dec.first + l), tprm);
                            setPrm(l, File::Meta::yRcv, 2000. + geom_t(dec.first + l), tprm);
                            setPrm(l, File::Meta::xCmp, 2000. - geom_t(dec.first + l), tprm);
                            setPrm(l, File::Meta::yCmp, 2000. - geom_t(dec.first + l), tprm);
                            setPrm(l, File::Meta::il, 2000U + dec.first + l, tprm);
                            setPrm(l, File::Meta::xl, 2000U + dec.first + l, tprm);
                            setPrm(l, File::Meta::tn, l+dec.first, tprm);
                        }
                    else
                        for (size_t l = 0; l < dec.second; l++)
                        {
                            setPrm(l, File::Meta::xSrc, 2000. - geom_t((dec.first + l) % (nt / 10U)), tprm);
                            setPrm(l, File::Meta::ySrc, 2000. + geom_t((dec.first + l) / (nt / 10U)), tprm);
                            setPrm(l, File::Meta::xRcv, 2000. + geom_t((dec.first + l) % (nt / 10U)), tprm);
                            setPrm(l, File::Meta::yRcv, 2000. - geom_t((dec.first + l) / (nt / 10U)), tprm);
                            setPrm(l, File::Meta::xCmp, 2000. + geom_t((dec.first + l) % (nt / 10U)), tprm);
                            setPrm(l, File::Meta::yCmp, 2000. + geom_t((dec.first + l) / (nt / 10U)), tprm);
                            setPrm(l, File::Meta::il, 2000U + (dec.first + l) % (nt / 10U), tprm);
                            setPrm(l, File::Meta::xl, 2000U + (dec.first + l) % (nt / 10U), tprm);
                            setPrm(l, File::Meta::tn, l+dec.first, tprm);
                        }
                    EXPECT_CALL(*mock, readParam(dec.second, An<csize_t *>(), _))
                                    .Times(Exactly(srtCnt))
                                    .WillRepeatedly(cpyprm(&prm.back()));

                    set->add(std::move(mock));
                }
    }
};

