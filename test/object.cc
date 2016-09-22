#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "tglobal.hh"
#include "anc/cmpi.hh"
#include "data/datampiio.hh"
#define private public
#define protected public
#include "object/object.hh"
#undef private
#undef protected

using namespace testing;
using namespace PIOL;

class ObjectTest : public Test
{
    protected :
    Piol piol;
    Comm::MPI::Opt opt;
    ObjectTest()
    {
        opt.initMPI = false;
        piol = std::make_shared<ExSeisPIOL>(opt);
    }
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
//FakeObject to test the constructor of the abstract Interface class
struct FakeObject : public Obj::Interface
{
    FakeObject(Piol piol_, const std::string name_)
               : Obj::Interface(piol_, name_, nullptr) { }

    size_t getFileSz() const
    {
        return 0U;
    }

    void readHO(uchar * ho) const {}

    void getFileSz(size_t sz) const {}

    void writeHO(const uchar * ho) const {}

    void readDOMD(csize_t offset, csize_t ns, csize_t sz, uchar * ho) const {}

    void writeDOMD(csize_t offset, csize_t ns, csize_t sz, const uchar * ho) const {}

    void readDODF(csize_t offset, csize_t ns, csize_t sz, uchar * ho) const {}

    void writeDODF(csize_t offset, csize_t ns, csize_t sz, const uchar * ho) const {}
};
#pragma GCC diagnostic pop

//In this test we pass the MPI-IO Data Options class.
//Valid options, invalid name
typedef ObjectTest ObjDeathTest;
TEST_F(ObjDeathTest, InterfaceConstructor)
{
    FakeObject fake(piol, notFile);
    EXPECT_EQ(nullptr, fake.data);
    EXPECT_EQ(piol, fake.piol);
    EXPECT_EQ(notFile, fake.name);
    EXPECT_EXIT(piol->isErr(), ExitedWithCode(EXIT_FAILURE), ".*8 3 Fatal Error in PIOL. . Dumping Log 0");
}

//In this test we pass the wrong Data Options class.
//We pass the base class instead of MPIIOOpt (the default class)
TEST_F(ObjDeathTest, BadInterfaceConstructor)
{
    FakeObject fake(piol, tempFile);
    EXPECT_EQ(nullptr, fake.data);
    EXPECT_EQ(piol, fake.piol);
    EXPECT_EQ(tempFile, fake.name);
    EXPECT_EXIT(piol->isErr(), ExitedWithCode(EXIT_FAILURE), ".*8 3 Fatal Error in PIOL. . Dumping Log 0");
}

