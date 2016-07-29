#include <memory>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "global.hh"
#include "tglobal.hh"
#include "share/segy.hh"
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

class MockData : public Data::Interface
{
    public :
    MockData(const std::shared_ptr<ExSeisPIOL> piol_, const std::string name_) : Data::Interface(piol_, name_)
    {
    }

    MOCK_METHOD0(getFileSz, size_t(void));
    MOCK_METHOD3(read, void(size_t, size_t, uchar *));
    MOCK_METHOD1(setFileSz, void(size_t));
    MOCK_METHOD3(write, void(size_t, size_t, const uchar *));
};

class ObjSpecTest : public Test
{
    protected :
    std::shared_ptr<ExSeisPIOL> piol;
    const Obj::SEGYOpt segyOpt;
    Data::MPIIOOpt dataOpt;
    Comm::MPIOpt opt;

    ObjSpecTest()
    {
        opt.initMPI = false;
        piol = std::make_shared<ExSeisPIOL>(opt);
    }
};

////////////////////////////////////////////////////////////////////////////////////
/////////////////////// ISOLATED-CLASS SPECIFICATION TESTING ///////////////////////
////////////////////////////////////////////////////////////////////////////////////

TEST_F(ObjSpecTest, TestBypassConstructor)
{
    auto mock = std::make_shared<MockData>(piol, notFile);
    std::shared_ptr<Data::Interface> data = mock;

    Obj::SEGY segy(piol, notFile, segyOpt, data);
    EXPECT_EQ(piol, segy.piol);
    EXPECT_EQ(notFile, segy.name);
    EXPECT_EQ(data, segy.data);
}

void SEGYFileSizeTest(std::shared_ptr<ExSeisPIOL> piol, std::string name, const Obj::SEGYOpt & segyOpt, size_t sz)
{
    auto mock = std::make_shared<MockData>(piol, name);
    EXPECT_CALL(*mock, getFileSz()).Times(Exactly(1)).WillOnce(Return(sz));

    Obj::SEGY segy(piol, name, segyOpt, mock);
    piol->isErr();
    EXPECT_EQ(sz, segy.getFileSz());
}

TEST_F(ObjSpecTest, SmallSEGYFileSize)
{
    size_t sz = 40U*prefix(2U);
    SCOPED_TRACE("SmallSEGYFileSize");
    SEGYFileSizeTest(piol, notFile, segyOpt, sz);
}

TEST_F(ObjSpecTest, BigSEGYFileSize)
{
    size_t sz = 8U*prefix(4U);
    SCOPED_TRACE("BigSEGYFileSize");
    SEGYFileSizeTest(piol, notFile, segyOpt, sz);
}

void ExpectHOPattern(MockData * mock, std::vector<uchar> * ho)
{
    for (size_t i = 0U; i < 3600U; i++)
        ho->at(i) = getPattern(i);
    EXPECT_CALL(*mock, read(0U, 3600U, _)).Times(Exactly(1)).WillOnce(SetArrayArgument<2>(ho->begin(), ho->end()));
}

//TODO: Move to separate file
TEST(SEGYSizes, All)
{
    ASSERT_EQ(3600U, SEGSz::getHOSz());
    ASSERT_EQ(240U,  SEGSz::getMDSz());
    ASSERT_EQ(3600U + (1111U*4U + 240U)*3333U, SEGSz::getDOLoc(3333, 1111));
}

void ExpectTrHdrPattern(size_t offset, size_t ns, MockData * mock, std::vector<uchar> * tr)
{
    size_t foff = SEGSz::getDOLoc(offset, ns);
    for (size_t i = 0U; i < SEGSz::getMDSz(); i++)
        tr->at(i) = getPattern(foff+i);
    EXPECT_CALL(*mock, read(foff, SEGSz::getMDSz(), _)).Times(Exactly(1)).WillOnce(SetArrayArgument<2>(tr->begin(), tr->end()));
}

TEST_F(ObjSpecTest, SEGYHORead)
{
    SCOPED_TRACE("SEGYRead");
    const size_t extra = 1111U;

    auto mock = std::make_shared<MockData>(piol, notFile);
    std::vector<uchar> cHo(SEGSz::getHOSz());

    ExpectHOPattern(mock.get(), &cHo);

    Obj::SEGY segy(piol, notFile, segyOpt, mock);
    piol->isErr();

    std::vector<uchar> ho(SEGSz::getHOSz() + extra);
    for (auto i = 0U; i < extra; i++)
        ho[ho.size()-extra+i] = magicNum1;

    segy.readHO(ho.data());
    piol->isErr();

    for (auto i = 0U; i < SEGSz::getHOSz(); i++)
        ASSERT_EQ(getPattern(i), ho[i]) << "Pattern " << i;
    for (auto i = 0U; i < extra; i++)
        ASSERT_EQ(magicNum1, ho[ho.size()-extra+i]) << "Pattern Extra " << i;
}

TEST_F(ObjSpecTest, SEGYTrRead)
{
    SCOPED_TRACE("SEGYRead");
    const size_t ns = 200U;
    const size_t traceNum = 10U;
    const size_t extra = 1111U;

    auto mock = std::make_shared<MockData>(piol, notFile);
    std::vector<uchar> cTrHdr(SEGSz::getMDSz());
    ExpectTrHdrPattern(traceNum, ns, mock.get(), &cTrHdr);

    Obj::SEGY segy(piol, notFile, segyOpt, mock);

    piol->isErr();

    std::vector<uchar> trHdr(SEGSz::getMDSz() + extra);
    for (auto i = 0U; i < extra; i++)
        trHdr[trHdr.size()-extra+i] = magicNum1;

    segy.readDOMD(traceNum, ns, trHdr.data());

    for (auto i = 0U; i < SEGSz::getMDSz(); i++)
    {
        auto pat = getPattern(SEGSz::getDOLoc(traceNum, ns) + i);
        ASSERT_EQ(pat, trHdr[i]) << "tr Pattern " << i;
    }
    for (auto i = 0U; i < extra; i++)
        ASSERT_EQ(magicNum1, trHdr[trHdr.size()-extra+i]) << "tr Pattern Extra " << i;
}

ACTION_P(extraTrCheck, ho)  //Use this when writing
{
    for (size_t i = 0; i < SEGSz::getMDSz(); i++)
        ASSERT_EQ(ho[i], arg2[i]) << "Error with header byte: " << i << " |\n";
}

void ExpectWriteTrHdrPattern(size_t offset, size_t ns, MockData * mock, std::vector<uchar> * tr)
{
    size_t foff = SEGSz::getDOLoc(offset, ns);
    EXPECT_CALL(*mock, write(foff, SEGSz::getMDSz(), _)).Times(Exactly(1)).WillOnce(extraTrCheck(tr->data()));
}

TEST_F(ObjSpecTest, SEGYTrWrite)
{
    SCOPED_TRACE("SEGYRead");
    const size_t ns = 200U;
    const size_t traceNum = 10U;

    auto mock = std::make_shared<MockData>(piol, notFile);
    std::vector<uchar> cTrHdr(SEGSz::getMDSz());
    for (auto i = 0U; i < cTrHdr.size(); i++)
        cTrHdr[i] = getPattern(i);

    Obj::SEGY segy(piol, notFile, segyOpt, mock);
    piol->isErr();

    std::vector<uchar> trHdr(SEGSz::getMDSz());
    for (auto i = 0U; i < trHdr.size(); i++)
        trHdr[i] = getPattern(i);

    ExpectWriteTrHdrPattern(traceNum, ns, mock.get(), &cTrHdr);
    segy.writeDOMD(traceNum, ns, trHdr.data());
}
////////////////////////////////////////////////////////////////////////////////////
///////////////////// INTEGRATION-CLASS SPECIFICATION TESTING //////////////////////
////////////////////////////////////////////////////////////////////////////////////

/////////////////////////// Header Object Reading //////////////////////////////////
void SEGYReadHOTest(Obj::Interface & obj, std::vector<uchar> magic)
{
    const size_t extra = 1025U;
    std::vector<uchar> ho(extra + SEGSz::getHOSz() + extra);

    for (size_t j = 0U; j < magic.size(); j++)
    {
        for (size_t i = 0U; i < extra; i++)
            ho[i] = ho[ho.size() - extra + i] = magic[j];

        obj.readHO(ho.data() + extra);

        //Overrun checks
        for (size_t i = 0U; i < extra; i++)
        {
            ASSERT_EQ(magic[j], ho[i]);
            ASSERT_EQ(magic[j], ho[ho.size() - extra + i]);
        }
        for (size_t i = 0U; i < SEGSz::getHOSz(); i++)
            ASSERT_EQ(getPattern(i), ho[i+extra]);
    }
}

typedef ObjSpecTest ObjIntegrationTest;
TEST_F(ObjIntegrationTest, SEGYZeroReadHO)
{
    Obj::SEGY segy(piol, zeroFile, segyOpt, dataOpt);
    piol->isErr();
    std::vector<uchar> ho(SEGSz::getHOSz());
    for (size_t i = 0; i < SEGSz::getHOSz(); i++)
        ho[i] = getPattern(SEGSz::getHOSz() - i);

    segy.readHO(ho.data());

    for (size_t i = 0; i < SEGSz::getHOSz(); i++)
        ASSERT_EQ(ho[i], getPattern(SEGSz::getHOSz() - i));
}


TEST_F(ObjIntegrationTest, SEGYReadHO)
{
    SCOPED_TRACE("SEGYReadHO");

    Obj::SEGY segy(piol, plargeFile, segyOpt, dataOpt);
    piol->isErr();
    SEGYReadHOTest(segy, {uchar(magicNum1), uchar(magicNum1+1)});
}

void SEGYWriteHOPattern(Obj::Interface & obj)
{
    std::vector<uchar> ho(SEGSz::getHOSz());
    for (size_t i = 0U; i < SEGSz::getHOSz(); i++)
        ho[i] = getPattern(i);
    obj.writeHO(ho.data());
}

TEST_F(ObjIntegrationTest, SEGYWriteHO)
{
    SCOPED_TRACE("SEGYReadHO");
    std::string outFile = "tmp/testOutput.tmp";
    dataOpt.mode = MPI_MODE_UNIQUE_OPEN | MPI_MODE_CREATE | MPI_MODE_RDWR | MPI_MODE_DELETE_ON_CLOSE;
    Obj::SEGY segy(piol, outFile, segyOpt, dataOpt);

    SEGYWriteHOPattern(segy);
    SEGYReadHOTest(segy, {uchar(magicNum1), uchar(magicNum1+1)});
}

//////////////////////////// Trace Header Reading //////////////////////////////////
void SEGYReadTrTest(size_t offset, const std::vector<size_t> & nsVals, const uchar magicNum, Obj::Interface * obj)
{
    const size_t extra = 1111U;
    std::vector<uchar> tr(extra + SEGSz::getMDSz() + extra);

    ASSERT_TRUE(nsVals.size() > 0);
    for (size_t j = 0U; j < nsVals.size(); j++)
    {
        size_t foff = SEGSz::getDOLoc(offset, nsVals[j]);
        for (size_t i = 0U; i < extra; i++)
            tr[i] = tr[tr.size() - extra + i] = magicNum;

        obj->readDOMD(offset, nsVals[j], tr.data() + extra);

        //Overrun checks
        for (size_t i = 0U; i < extra; i++)
        {
            ASSERT_EQ(magicNum, tr[i]);
            ASSERT_EQ(magicNum, tr[tr.size() - extra + i]);
        }
        for (size_t i = 0U; i < SEGSz::getMDSz(); i++)
            ASSERT_EQ(getPattern(foff + i), tr[i+extra]);
    }
}

TEST_F(ObjIntegrationTest, SEGYReadTrHdr)
{
    SCOPED_TRACE("SEGYReadHO");
    Obj::SEGY segy(piol, plargeFile, segyOpt, dataOpt);
    piol->isErr();
    SCOPED_TRACE("Read1");
    SEGYReadTrTest(10U, {0U, 1U, 100U, 1000U, 1000U}, magicNum1, &segy);
    SCOPED_TRACE("Read2");
    SEGYReadTrTest(100U, {0U, 1U, 100U, 1000U, 1000U}, magicNum1+1U, &segy);
}

void SEGYWriteTrTest(size_t offset, size_t ns, Obj::Interface * obj)
{
    std::vector<uchar> tr(SEGSz::getMDSz());
    size_t foff = SEGSz::getDOLoc(offset, ns);
    for (size_t i = 0U; i < tr.size(); i++)
        tr[i] = getPattern(foff + i);

    obj->writeDOMD(offset, ns, tr.data());
}

TEST_F(ObjIntegrationTest, SEGYWriteTrHdr)
{
    SCOPED_TRACE("SEGYReadHO");
    std::string outFile = "tmp/testOutput.tmp";
    dataOpt.mode = MPI_MODE_UNIQUE_OPEN | MPI_MODE_CREATE | MPI_MODE_RDWR | MPI_MODE_DELETE_ON_CLOSE;
    Obj::SEGY segy(piol, outFile, segyOpt, dataOpt);

    SEGYWriteTrTest(40U, 0U, &segy);
    SEGYReadTrTest(40U, {0U}, magicNum1, &segy);

    SEGYWriteTrTest(80U, 100U, &segy);
    SEGYReadTrTest(80U, {100U}, magicNum1, &segy);

    SEGYWriteTrTest(400U, 1000U, &segy);
    SEGYReadTrTest(400U, {1000U}, magicNum1, &segy);

}

////////////////////////////// File Size Testing ///////////////////////////////////
TEST_F(ObjIntegrationTest, zeroSEGYFileSize)
{
    Obj::SEGY segy(piol, zeroFile, segyOpt, dataOpt);
    piol->isErr();
    EXPECT_NE(nullptr, segy.data);
    EXPECT_EQ(0U, segy.getFileSz());
}

TEST_F(ObjIntegrationTest, SmallSEGYFileSize)
{
    Obj::SEGY segy(piol, smallFile, segyOpt, dataOpt);
    piol->isErr();
    EXPECT_NE(nullptr, segy.data);
    EXPECT_EQ(smallSize, segy.getFileSz());
}

TEST_F(ObjIntegrationTest, BigSEGYFileSize)
{
    Obj::SEGY segy(piol, largeFile, segyOpt, dataOpt);
    piol->isErr();
    EXPECT_NE(nullptr, segy.data);
    EXPECT_EQ(largeSize, segy.getFileSz());
}

