#include <memory>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "tglobal.hh"
#include "anc/cmpi.hh"
#include "data/datampiio.hh"
#include "object/objsegy.hh"
#define private public
#define protected public
#include "file/filesegy.hh"
#undef private
#undef protected

using namespace testing;
using namespace PIOL;

class MockObj : public Obj::Interface
{
    public :
    MockObj(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, std::shared_ptr<Data::Interface> data_)
               : Obj::Interface(piol_, name_, data_)
    {}
    MOCK_METHOD0(getFileSz, size_t(void));
    MOCK_METHOD1(readHO, void(uchar *));
};

class FileIntegrationTest : public Test
{
    protected :
    std::shared_ptr<ExSeisPIOL> piol;
    const File::SEGYOpt fileSegyOpt;
    const Obj::SEGYOpt objSegyOpt;
    const Data::MPIIOOpt dataOpt;
    Comm::MPIOpt opt;
    FileIntegrationTest()
    {
        opt.initMPI = false;
        piol = std::make_shared<ExSeisPIOL>(opt);
    }
};
////////////////////////////////////////////////////////////////////////////////////
/////////////////////// ISOLATED-CLASS SPECIFICATION TESTING ///////////////////////
////////////////////////////////////////////////////////////////////////////////////
class FileSEGYSpecTest : public FileIntegrationTest
{
    protected :
    std::shared_ptr<MockObj> mockObj;
    const size_t nt = 40U;
    const size_t ns = 200U;
    std::vector<uchar> ho;
    FileSEGYSpecTest() : FileIntegrationTest()
    {
        mockObj = std::make_shared<MockObj>(piol, notFile, nullptr);
        EXPECT_CALL(*mockObj, getFileSz()).Times(Exactly(1)).WillOnce(Return(SEGSz::getHOSz() + nt*SEGSz::getDOSz(ns)));
        ho.resize(SEGSz::getHOSz());
        for (size_t i = 0; i < SEGSz::getTextSz(); i++)
            ho[i] = getPattern(i);
        ho[3221U] = ns;
        EXPECT_CALL(*mockObj, readHO(_)).Times(Exactly(1)).WillOnce(SetArrayArgument<0>(ho.begin(), ho.end()));
    }
};

TEST_F(FileSEGYSpecTest, TestAPI)
{
    ASSERT_TRUE(ns < 0x10000);
    File::SEGY segy(piol, notFile, fileSegyOpt, mockObj);
    piol->isErr();
    EXPECT_EQ(piol, segy.piol);
    EXPECT_EQ(notFile, segy.name);
    EXPECT_EQ(mockObj, segy.obj);
    EXPECT_EQ(nt, segy.readNt());
    piol->isErr();
    EXPECT_EQ(ns, segy.readNs());
    piol->isErr();

    std::string text = segy.readText();
    EXPECT_EQ(3200U, text.size());
    EXPECT_EQ(SEGSz::getTextSz(), text.size());
    for (size_t i = 0; i < text.size(); i++)
        ASSERT_EQ(getPattern(i), reinterpret_cast<uchar &>(text[i]));
}

////////////////////////////////////////////////////////////////////////////////////
///////////////////// INTEGRATION-CLASS SPECIFICATION TESTING //////////////////////
////////////////////////////////////////////////////////////////////////////////////

//Read test of File::SEGY -> Obj::SEGY -> Data::MPIIO
TEST_F(FileIntegrationTest, SEGYReadHO)
{
    SCOPED_TRACE("SEGYReadHO");
    const size_t ns = 261U;
    const size_t nt = 400U;
    std::string smallSEGY = "tmp/smallsegy.tmp";
    File::SEGY segy(piol, smallSEGY, fileSegyOpt, objSegyOpt, dataOpt);
    piol->isErr();
    EXPECT_EQ(ns, segy.readNs());
    piol->isErr();
    EXPECT_EQ(nt, segy.readNt());
    piol->isErr();
    std::string text = segy.readText();
    EXPECT_EQ(3200U, text.size());
    for (size_t i = 0; i < text.size(); i++)
        ASSERT_EQ(getPattern(i), reinterpret_cast<uchar &>(text[i]));
}

//TODO: Add test same as above for big files

