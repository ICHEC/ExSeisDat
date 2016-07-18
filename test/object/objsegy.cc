#include <memory>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "global.hh"
#include "tglobal.hh"
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
    MockData(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_) : Data::Interface(piol_, name_) {}
    MOCK_METHOD0(getFileSz, size_t(void));
    MOCK_METHOD3(read, void(size_t, uchar *, size_t));
};

class ObjSEGYTest : public Test
{
    protected :
    std::shared_ptr<ExSeisPIOL> piol;
    const Obj::SEGYOpt segyOpt;
    Comm::MPIOpt opt;
    std::string name = "randomName";
    ObjSEGYTest()
    {
        opt.initMPI = false;
        piol = std::make_shared<ExSeisPIOL>(opt);
    }
};

TEST_F(ObjSEGYTest, TestBypassConstructor)
{
    auto mock = std::make_shared<MockData>(piol, name);
    std::shared_ptr<Data::Interface> data = mock;

    Obj::SEGY segy(piol, name, segyOpt, data);
    EXPECT_EQ(piol, segy.piol);
    EXPECT_EQ(name, segy.name);
    EXPECT_EQ(data, segy.data);
}

void SEGYFileSizeTest(std::shared_ptr<ExSeisPIOL> piol, std::string name, const Obj::SEGYOpt & segyOpt, size_t sz)
{
    auto mock = std::make_shared<MockData>(piol, name);
    EXPECT_CALL(*mock, getFileSz()).Times(1).WillOnce(Return(sz));
    std::shared_ptr<Data::Interface> data = mock;
    Obj::SEGY segy(piol, name, segyOpt, data);
    piol->isErr();
    EXPECT_EQ(sz, segy.getFileSz());
}

typedef ObjSEGYTest ObjSpecTest;
TEST_F(ObjSpecTest, SmallSEGYFileSize)
{
    size_t sz = 40*prefix(2U);
    EXPECT_EQ(sz, 40*1024U*1024U);
    SCOPED_TRACE("SmallSEGYFileSize");
    SEGYFileSizeTest(piol, name, segyOpt, sz);
}

TEST_F(ObjSpecTest, BigSEGYFileSize)
{
    size_t sz = 8U*prefix(4U);
    SCOPED_TRACE("BigSEGYFileSize");
    SEGYFileSizeTest(piol, name, segyOpt, sz);
}

typedef ObjSEGYTest ObjIntegrationTest;
TEST_F(ObjIntegrationTest, SmallSEGYFileSize)
{
    Data::MPIIOOpt dataOpt;
    Obj::SEGY segy(piol, smallFile, segyOpt, dataOpt);
    piol->isErr();
    EXPECT_NE(nullptr, segy.data);
    EXPECT_EQ(smallSize, segy.getFileSz());
}

TEST_F(ObjIntegrationTest, BigSEGYFileSize)
{
    Data::MPIIOOpt dataOpt;
    Obj::SEGY segy(piol, largeFile, segyOpt, dataOpt);
    piol->isErr();
    EXPECT_EQ(largeSize, segy.getFileSz());
}

