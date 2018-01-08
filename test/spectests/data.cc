#include <memory>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "tglobal.hh"
#include "anc/mpi.hh"
#include "cppfileapi.hh"
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
    std::shared_ptr<ExSeis> piol = ExSeis::New();
};

struct FakeData : public Data::Interface
{
    FakeData(const std::shared_ptr<ExSeisPIOL> piol_, const std::string name_) : Data::Interface(piol_, name_)
    {

    }

    size_t getFileSz() const
    {
        return 0U;
    }

    void read(const size_t, const size_t, uchar *) const {}
    void read(const size_t, const size_t, const size_t, const size_t, uchar *) const {}
    void read(const size_t, const size_t, const size_t *, uchar *) const {}
    void setFileSz(const size_t) const {}
    void write(const size_t, const size_t, const uchar *) const {}
    void write(const size_t, const size_t, const size_t, const size_t, const uchar *) const {}
    void write(const size_t, const size_t, const size_t *, const uchar *) const {}
};

TEST_F(DataTest, Constructor)
{
    FakeData fake(piol, notFile);
    EXPECT_EQ(piol, fake.piol);
    EXPECT_EQ(notFile, fake.name);
}
