#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "tglobal.hh"
#include "anc/cmpi.hh"
#include "data/datampiio.hh"
#include "object/objsegy.hh"
#define private public
#define protected public
#include "cppfileapi.hh"
#include "file/file.hh"
#undef private
#undef protected

using namespace testing;
using namespace PIOL;

class FileTest : public Test
{
    protected :
    std::shared_ptr<ExSeisPIOL> piol;
    Comm::MPI::Opt opt;

    FileTest()
    {
        opt.initMPI = false;
        piol = std::make_shared<ExSeisPIOL>(opt);
    }
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
//FakeFile to test the constructor of the abstract Interface class
struct FakeFile : public File::Interface
{
    struct Opt
    {
        typedef FakeFile Type;
    };
    FakeFile(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, std::shared_ptr<Obj::Interface> obj_) : File::Interface(piol_, name_, obj_)
    {
        inc = geom_t(10);
        text = "test";
        nt = 1101U;
        ns = 1010U;
    }

    File::coord_t readCoordPoint(File::Coord item, csize_t i) const { return File::coord_t(0, 0); }
    void readCoordPoint(File::Coord item, csize_t offset, csize_t sz, File::coord_t * coord) const {}
    void readGridPoint(File::Grid item, csize_t offset, csize_t sz, File::grid_t * grid) const {}
    File::grid_t readGridPoint(File::Grid item, csize_t i) const { return File::grid_t(llint(0), llint(0)); }
    void writeText(const std::string text_) {}
    void writeNs(csize_t ns_) {}
    void writeNt(csize_t nt_) {}
    void writeInc(const geom_t inc_) {}
    void readTrace(csize_t offset, csize_t sz, trace_t * trace) const { }
    void writeTrace(csize_t offset, csize_t sz, trace_t * trace) { }
    void writeTraceParam(csize_t offset, csize_t sz, const File::TraceParam * prm) {}
    void readTraceParam(csize_t offset, csize_t sz, File::TraceParam * prm) const {}
};
#pragma GCC diagnostic pop

void compareConstructor(ExSeisPIOL * piol, FakeFile & fake)
{
    EXPECT_EQ(piol, fake.piol.get());
    EXPECT_EQ(tempFile, fake.name);
    EXPECT_EQ(1101U, fake.readNt());
    EXPECT_EQ(1010U, fake.readNs());
    EXPECT_EQ(geom_t(10), fake.readInc());
    EXPECT_EQ("test", fake.readText());
}

//We test the constructor
TEST_F(FileTest, Constructor)
{
    std::shared_ptr<Obj::Interface> obj = nullptr;
    FakeFile fake(piol, tempFile, obj);
    EXPECT_EQ(nullptr, fake.obj);
    compareConstructor(piol.get(), fake);
}

typedef FileTest FileDeathTest;

//In this test we pass the MPI-IO Data Options class within an invalid name
TEST_F(FileDeathTest, BadNameConstructor)
{
    File::Direct file(piol, tempFile);
    EXPECT_EQ(piol, file->piol);
    EXPECT_EQ(tempFile, file->name);
    EXPECT_EXIT(piol->isErr(), ExitedWithCode(EXIT_FAILURE), ".*8 3 Fatal Error in PIOL. . Dumping Log 0");
}
