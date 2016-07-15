#include <memory>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "global.hh"
#include "anc/cmpi.hh"
#include "data/data.hh"
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
    Comm::MPIOpt opt;
    const Obj::SEGYOpt segyOpt;
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
TEST_F(ObjSEGYTest, SEGYFileSize)
{
    auto mock = std::make_shared<MockData>(piol, name);
    EXPECT_CALL(*mock, getFileSz()).Times(1).WillOnce(Return(40000));
    std::shared_ptr<Data::Interface> data = mock;

    Obj::SEGY segy(piol, name, segyOpt, data);
    segy.getFileSz();
}
