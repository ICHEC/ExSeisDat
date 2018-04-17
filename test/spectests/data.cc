#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "tglobal.hh"

#include "ExSeisDat/PIOL/CommunicatorMPI.hh"
#include "ExSeisDat/PIOL/DataInterface.hh"
#include "ExSeisDat/PIOL/ExSeis.hh"

#include <memory>

using namespace testing;
using namespace exseis::PIOL;

class DataTest : public Test {
  protected:
    std::shared_ptr<ExSeis> piol = ExSeis::New();
};

struct FakeData : public DataInterface {
    FakeData(const std::shared_ptr<ExSeisPIOL> piol_, const std::string name_) :
        DataInterface(piol_, name_)
    {
    }

    size_t getFileSz() const { return 0U; }

    void read(const size_t, const size_t, uchar*) const {}
    void read(
      const size_t, const size_t, const size_t, const size_t, uchar*) const
    {
    }
    void read(const size_t, const size_t, const size_t*, uchar*) const {}
    void setFileSz(const size_t) const {}
    void write(const size_t, const size_t, const uchar*) const {}
    void write(
      const size_t,
      const size_t,
      const size_t,
      const size_t,
      const uchar*) const
    {
    }
    void write(const size_t, const size_t, const size_t*, const uchar*) const {}
};

TEST_F(DataTest, Constructor)
{
    FakeData fake(piol, notFile);
    EXPECT_EQ(piol, fake.piol());
    EXPECT_EQ(notFile, fake.name());
}
