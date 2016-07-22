#include <memory>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "tglobal.hh"
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
    void read(size_t offset, size_t sz, uchar * d) {}
};

TEST_F(DataTest, Constructor)
{
    FakeData fake(piol, notFile);
    EXPECT_EQ(piol, fake.piol);
    EXPECT_EQ(notFile, fake.name);
}

TEST(DataOptTest, Opt)
{
    Data::Opt opt;
    EXPECT_EQ(Data::Type::MPIIO, opt.getType());
}
