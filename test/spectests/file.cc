#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "tglobal.hh"

#include "ExSeisDat/PIOL/ExSeis.hh"
#include "ExSeisDat/PIOL/ReadDirect.hh"
#include "ExSeisDat/PIOL/WriteInterface.hh"

#include "ExSeisDat/PIOL/CommunicatorMPI.hh"
#include "ExSeisDat/PIOL/DataMPIIO.hh"
#include "ExSeisDat/PIOL/ObjectSEGY.hh"


using namespace testing;
using namespace exseis::PIOL;

class FileTest : public Test {
  protected:
    std::shared_ptr<ExSeis> piol = ExSeis::New();
};

// FakeFile to test the constructor of the abstract Interface class
struct FakeReadFile : public ReadInterface {
    struct Opt {
        typedef FakeReadFile Type;
    };
    FakeReadFile(
      std::shared_ptr<ExSeisPIOL> piol_,
      const std::string name_,
      std::shared_ptr<ObjectInterface> obj_) :
        ReadInterface(piol_, name_, obj_)
    {
        inc  = exseis::utils::Floating_point(10);
        text = "test";
        nt   = 1101U;
        ns   = 1010U;
    }

    size_t readNt(void) const { return nt; }
    void readTrace(
      const size_t,
      const size_t,
      exseis::utils::Trace_value*,
      Param*,
      size_t) const
    {
    }
    void readParam(const size_t, const size_t, Param*, size_t) const {}

    void readTraceNonContiguous(
      const size_t,
      const size_t*,
      exseis::utils::Trace_value*,
      Param*,
      size_t) const
    {
    }
    void readTraceNonMonotonic(
      const size_t,
      const size_t*,
      exseis::utils::Trace_value*,
      Param*,
      size_t) const
    {
    }
    void readParamNonContiguous(
      const size_t, const size_t*, Param*, size_t) const
    {
    }

    // Import protected members into public access.
    using ReadInterface::name;
    using ReadInterface::obj;
    using ReadInterface::piol;
};

struct FakeWriteFile : public WriteInterface {
    struct Opt {
        typedef FakeWriteFile Type;
    };
    FakeWriteFile(
      std::shared_ptr<ExSeisPIOL> piol_,
      const std::string name_,
      std::shared_ptr<ObjectInterface> obj_) :
        WriteInterface(piol_, name_, obj_)
    {
        inc  = exseis::utils::Floating_point(10);
        text = "test";
        nt   = 1101U;
        ns   = 1010U;
    }

    void writeText(const std::string) {}
    void writeNs(const size_t) {}
    void writeNt(const size_t) {}
    void writeInc(const exseis::utils::Floating_point) {}
    void writeTrace(
      const size_t,
      const size_t,
      exseis::utils::Trace_value*,
      const Param*,
      size_t)
    {
    }
    void writeParam(const size_t, const size_t, const Param*, size_t) {}

    void writeTrace(
      const size_t,
      const size_t*,
      exseis::utils::Trace_value*,
      const Param*,
      size_t)
    {
    }
    void writeParam(const size_t, const size_t*, const Param*, size_t) {}
};

void compareConstructor(ExSeisPIOL* piol, FakeReadFile& fake)
{
    EXPECT_EQ(piol, fake.piol.get());
    EXPECT_EQ(tempFile, fake.name);
    EXPECT_EQ(1010U, fake.readNs());
    EXPECT_EQ(exseis::utils::Floating_point(10), fake.readInc());
    EXPECT_EQ("test", fake.readText());
}

// We test the constructor
TEST_F(FileTest, Constructor)
{
    std::shared_ptr<ObjectInterface> obj = nullptr;
    FakeReadFile fake(piol, tempFile, obj);
    EXPECT_EQ(nullptr, fake.obj);
    compareConstructor(piol.get(), fake);
}

typedef FileTest FileDeathTest;


// In this test we pass the MPI-IO Data Options class within an invalid name
TEST_F(FileDeathTest, BadNameConstructor)
{
    ReadDirect file(piol, notFile);
    EXPECT_EXIT(
      piol->isErr(), ExitedWithCode(EXIT_FAILURE),
      ".*8 3 Fatal Error in PIOL. . Dumping Log 0");
}
