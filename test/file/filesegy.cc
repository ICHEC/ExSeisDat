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

class FileSEGYTest : public Test
{
    protected :
    std::shared_ptr<ExSeisPIOL> piol;
    const File::SEGYOpt fileSegyOpt;
    const Obj::SEGYOpt objSegyOpt;
    const Data::MPIIOOpt dataOpt;
    Comm::MPIOpt opt;
    std::shared_ptr<MockObj> mockObj;
    FileSEGYTest()
    {
        opt.initMPI = false;
        piol = std::make_shared<ExSeisPIOL>(opt);
        mockObj = std::make_shared<MockObj>(piol, notFile, nullptr);
    }
};

TEST_F(FileSEGYTest, TestBypassConstructor)
{
    EXPECT_CALL(*mockObj, getFileSz()).WillOnce(Return(3600));
    File::SEGY segy(piol, notFile, mockObj);
    EXPECT_EQ(piol, segy.piol);
    EXPECT_EQ(notFile, segy.name);
    EXPECT_EQ(mockObj, segy.obj);
}

typedef FileSEGYTest FileSpecTest;
TEST_F(FileSpecTest, NoTraceFileTest)
{
    EXPECT_CALL(*mockObj, getFileSz()).WillOnce(Return(3600));
    std::vector<uchar> ho(SEGSz::getHOSz());
    ho[3217] = 200;

    EXPECT_CALL(*mockObj, readHO(_)).WillOnce(SetArrayArgument<0>(ho.end(), ho.begin()));

    File::SEGY segy(piol, notFile, mockObj);
    piol->isErr();
}

typedef FileSEGYTest FileIntegrationTest;
TEST_F(FileIntegrationTest, SEGYReadHO)
{
    SCOPED_TRACE("SEGYReadHO");
    File::SEGY segy(piol, plargeFile, fileSegyOpt, objSegyOpt, dataOpt);
    piol->isErr();
}

