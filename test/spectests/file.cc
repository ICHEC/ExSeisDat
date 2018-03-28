#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "tglobal.hh"

#include "ExSeisDat/PIOL/ExSeis.hh"
#include "ExSeisDat/PIOL/ReadDirect.hh"
#include "ExSeisDat/PIOL/WriteInterface.hh"

#include "ExSeisDat/PIOL/anc/mpi.hh"
#include "ExSeisDat/PIOL/data/datampiio.hh"
#include "ExSeisDat/PIOL/object/objsegy.hh"


using namespace testing;
using namespace PIOL;

class FileTest : public Test {
  protected:
    std::shared_ptr<ExSeis> piol = ExSeis::New();
};

// FakeFile to test the constructor of the abstract Interface class
struct FakeReadFile : public File::ReadInterface {
    struct Opt {
        typedef FakeReadFile Type;
    };
    FakeReadFile(
      std::shared_ptr<ExSeisPIOL> piol_,
      const std::string name_,
      std::shared_ptr<Obj::Interface> obj_) :
        File::ReadInterface(piol_, name_, obj_)
    {
        inc  = geom_t(10);
        text = "test";
        nt   = 1101U;
        ns   = 1010U;
    }

    size_t readNt(void) const { return nt; }
    void readTrace(
      const size_t, const size_t, trace_t*, File::Param*, size_t) const
    {
    }
    void readParam(const size_t, const size_t, File::Param*, size_t) const {}

    void readTraceNonContiguous(
      const size_t, const size_t*, trace_t*, File::Param*, size_t) const
    {
    }
    void readTraceNonMonotonic(
      const size_t, const size_t*, trace_t*, File::Param*, size_t) const
    {
    }
    void readParamNonContiguous(
      const size_t, const size_t*, File::Param*, size_t) const
    {
    }

    // Import protected members into public access.
    using File::ReadInterface::name;
    using File::ReadInterface::obj;
    using File::ReadInterface::piol;
};

struct FakeWriteFile : public File::WriteInterface {
    struct Opt {
        typedef FakeWriteFile Type;
    };
    FakeWriteFile(
      std::shared_ptr<ExSeisPIOL> piol_,
      const std::string name_,
      std::shared_ptr<Obj::Interface> obj_) :
        File::WriteInterface(piol_, name_, obj_)
    {
        inc  = geom_t(10);
        text = "test";
        nt   = 1101U;
        ns   = 1010U;
    }

    void writeText(const std::string) {}
    void writeNs(const size_t) {}
    void writeNt(const size_t) {}
    void writeInc(const geom_t) {}
    void writeTrace(
      const size_t, const size_t, trace_t*, const File::Param*, size_t)
    {
    }
    void writeParam(const size_t, const size_t, const File::Param*, size_t) {}

    void writeTrace(
      const size_t, const size_t*, trace_t*, const File::Param*, size_t)
    {
    }
    void writeParam(const size_t, const size_t*, const File::Param*, size_t) {}
};

void compareConstructor(ExSeisPIOL* piol, FakeReadFile& fake)
{
    EXPECT_EQ(piol, fake.piol.get());
    EXPECT_EQ(tempFile, fake.name);
    EXPECT_EQ(1010U, fake.readNs());
    EXPECT_EQ(geom_t(10), fake.readInc());
    EXPECT_EQ("test", fake.readText());
}

// We test the constructor
TEST_F(FileTest, Constructor)
{
    std::shared_ptr<Obj::Interface> obj = nullptr;
    FakeReadFile fake(piol, tempFile, obj);
    EXPECT_EQ(nullptr, fake.obj);
    compareConstructor(piol.get(), fake);
}

typedef FileTest FileDeathTest;


// In this test we pass the MPI-IO Data Options class within an invalid name
TEST_F(FileDeathTest, BadNameConstructor)
{
    File::ReadDirect file(piol, notFile);
    EXPECT_EXIT(
      piol->isErr(), ExitedWithCode(EXIT_FAILURE),
      ".*8 3 Fatal Error in PIOL. . Dumping Log 0");
}
