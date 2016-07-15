#include <memory>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "global.hh"
#include "anc/cmpi.hh"
#define private public
#define protected public
#include "data/data.hh"
#undef private
#undef protected

using namespace testing;
using namespace PIOL;
class DataTest : public Test
{
    protected :
    std::shared_ptr<ExSeisPIOL> piol;
    Comm::MPIOpt opt;
    DataTest()
    {
        opt.initMPI = false;
        piol = std::make_shared<ExSeisPIOL>(opt);
    }
};

struct FakeData : public Data::Interface
{
    FakeData(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_) : Data::Interface(piol_, name_)
    {

    }
    size_t getFileSz()
    {
        return 0U;
    }
    void read(size_t offset, uchar * d, size_t sz) {}
};

TEST_F(DataTest, Constructor)
{
    std::string name = "!£$%^&*()<>?:@~}{fakefile1234567890";
    FakeData fake(piol, name);
    EXPECT_EQ(piol, fake.piol);
    EXPECT_EQ(name, fake.name);
}

TEST(DataOptTest, Opt)
{
    Data::Opt opt;
    opt.setType(Data::Type::MPIIO);
    EXPECT_EQ(Data::Type::MPIIO, opt.getType());
}
