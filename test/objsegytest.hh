#include <memory>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "global.hh"
#include "tglobal.hh"
#include "share/segy.hh"
#include "share/datatype.hh"
#include "anc/cmpi.hh"
#include "data/datampiio.hh"
#define private public
#define protected public
#include "object/object.hh"
#include "object/objsegy.hh"
#undef private
#undef protected

using namespace testing;
using namespace PIOL;

ACTION_P(extraTrCheck, tr)  //Use this when writing
{
    for (size_t i = 0; i < SEGSz::getMDSz(); i++)
        ASSERT_EQ(tr[i], arg4[i]) << "Error with header byte: " << i << " |\n";
}

class MockData : public Data::Interface
{
    public :
    MockData(const std::shared_ptr<ExSeisPIOL> piol_, const std::string name_) : Data::Interface(piol_, name_)
    {
    }

    MOCK_CONST_METHOD0(getFileSz, size_t(void));
    MOCK_CONST_METHOD3(read, void(csize_t, csize_t, uchar *));
    MOCK_CONST_METHOD5(read, void(csize_t, csize_t, csize_t, csize_t, uchar *));
    MOCK_CONST_METHOD3(write, void(csize_t, csize_t, const uchar *));
    MOCK_CONST_METHOD5(write, void(csize_t, csize_t, csize_t, csize_t, const uchar *));
    // TODO: This method is not tested
    MOCK_CONST_METHOD1(setFileSz, void(csize_t));
};

class ObjSpecTest : public Test
{
    protected :
    std::shared_ptr<ExSeisPIOL> piol;
    const Obj::SEGYOpt segyOpt;
    Data::MPIIOOpt dataOpt;
    Comm::MPIOpt opt;
    std::shared_ptr<MockData> mock;
    Obj::Interface * obj;

    ObjSpecTest()
    {
        obj = nullptr;
        opt.initMPI = false;
        piol = std::make_shared<ExSeisPIOL>(opt);
    }

    void makeSEGY()
    {
        mock = std::make_shared<MockData>(piol, notFile);
        if (obj != nullptr)
            delete obj;
        obj = new Obj::SEGY(piol, notFile, segyOpt, mock);
        piol->isErr();
    }

    ~ObjSpecTest()
    {
        if (obj != nullptr)
            delete obj;
    }

    void SEGYFileSizeTest(size_t sz)
    {
        EXPECT_CALL(*mock, getFileSz()).Times(Exactly(1)).WillOnce(Return(sz));
        piol->isErr();
        EXPECT_EQ(sz, obj->getFileSz());
    }

    void HOPatternTest(size_t off, uchar magic)
    {
        csize_t extra = 20U;
        std::vector<uchar> cHo(SEGSz::getHOSz());
        for (size_t i = 0U; i < SEGSz::getHOSz(); i++)
            cHo[i] = getPattern(off + i);
        EXPECT_CALL(*mock, read(0U, SEGSz::getHOSz(), _)).Times(Exactly(1)).WillOnce(SetArrayArgument<2>(cHo.begin(), cHo.end()));

        std::vector<uchar> ho(SEGSz::getHOSz() + 2*extra);
        for (auto i = 0U; i < extra; i++)
            ho[i] = ho[ho.size()-extra+i] = magic;

        obj->readHO(&ho[extra]);

        piol->isErr();

        for (auto i = 0U; i < SEGSz::getHOSz(); i++)
            ASSERT_EQ(getPattern(off + i), ho[extra+i]) << "Pattern " << i;
        for (auto i = 0U; i < extra; i++)
            ASSERT_EQ(magic, ho[ho.size()-extra+i]) << "Pattern Extra " << i;
    }

    void setPattern(csize_t offset, std::vector<uchar> * buf)
    {
        for (size_t i = 0U; i < SEGSz::getMDSz(); i++)
            buf->at(i) = getPattern(offset+i);
    }

    void ExpectTrHdrPattern(csize_t offset, csize_t ns, std::vector<uchar> * tr)
    {
        size_t foff = SEGSz::getDOLoc(offset, ns);
        for (size_t i = 0U; i < SEGSz::getMDSz(); i++)
            tr->at(i) = getPattern(foff+i);
        EXPECT_CALL(*mock, read(foff, SEGSz::getMDSz(), SEGSz::getDOSz(ns), 1U, _)).Times(Exactly(1)).WillOnce(SetArrayArgument<4>(tr->begin(), tr->end()));
    }

    void ExpectWriteTrHdrPattern(csize_t offset, csize_t ns)
    {
        std::vector<uchar> cTrHdr(SEGSz::getMDSz());
        std::vector<uchar> trHdr(SEGSz::getMDSz());
        for (auto i = 0U; i < cTrHdr.size(); i++)
            trHdr[i] = cTrHdr[i] = getPattern(i);
        EXPECT_CALL(*mock, write(SEGSz::getDOLoc<float>(offset, ns), SEGSz::getMDSz(),
                                 SEGSz::getDOSz<float>(ns), 1U, _)).Times(Exactly(1)).WillOnce(extraTrCheck(cTrHdr.data()));
        obj->writeDOMD(offset, ns, 1U, trHdr.data());
    }

    template <bool DOMD>
    void readTest(csize_t offset, size_t nt, csize_t ns, csize_t poff)
    {
        size_t step = (DOMD ? SEGSz::getMDSz() : SEGSz::getDOSz(ns));
        std::vector<uchar> tr(step*nt);
        std::vector<uchar> trnew(step*nt);
        for (size_t i = 0U; i < tr.size(); i++)
            tr[i] = getPattern(poff + i);

        if (DOMD)
        {
            EXPECT_CALL(*mock, read(SEGSz::getDOLoc<float>(offset, ns), SEGSz::getMDSz(), SEGSz::getDOSz(ns), nt, _))
                              .Times(Exactly(1)).WillOnce(SetArrayArgument<4>(tr.begin(), tr.end()));
            obj->readDOMD(offset, ns, nt, trnew.data());
        }
        else
        {
            EXPECT_CALL(*mock, read(SEGSz::getDODFLoc<float>(offset, ns), SEGSz::getDFSz(ns), SEGSz::getDOSz(ns), nt, _))
                              .Times(Exactly(1)).WillOnce(SetArrayArgument<4>(tr.begin(), tr.end()));
            obj->readDODF(offset, ns, nt, trnew.data());
        }

        for (size_t i = 0U; i < tr.size(); i++)
            ASSERT_EQ(trnew[i],  getPattern(poff + i));
    }
};

