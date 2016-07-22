#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "tglobal.hh"
#include "anc/cmpi.hh"
#include "data/datampiio.hh"
#include "object/objsegy.hh"
#define private public
#define protected public
#include "file/file.hh"
#undef private
#undef protected

using namespace testing;
using namespace PIOL;

class FileTest : public Test
{
    protected :
    std::shared_ptr<ExSeisPIOL> piol;
    Comm::MPIOpt opt;

    FileTest()
    {
        opt.initMPI = false;
        piol = std::make_shared<ExSeisPIOL>(opt);
    }
};

//FakeFile to test the constructor of the abstract Interface class
struct FakeFile : public File::Interface
{
    FakeFile(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, std::shared_ptr<Obj::Interface> obj_) : File::Interface(piol_, name_, obj_)
    {
        inc = geom_t(10);
        text = "test";
        nt = 1101U;
        ns = 1010U;
    }

    FakeFile(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const Obj::Opt & objOpt, const Data::Opt & dataOpt) : File::Interface(piol_, name_, objOpt, dataOpt)
    {
        inc = geom_t(10);
        text = "test";
        nt = 1101U;
        ns = 1010U;
    }
};

void compareConstructor(ExSeisPIOL * piol, FakeFile & fake)
{
    EXPECT_EQ(piol, fake.piol.get());
    EXPECT_EQ(notFile, fake.name);
    EXPECT_EQ(1101U, fake.readNt());
    EXPECT_EQ(1010U, fake.readNs());
    EXPECT_EQ(geom_t(10), fake.readInc());
    EXPECT_EQ("test", fake.readText());
}

//We test the short-cut constructor. No sanity checks
TEST_F(FileTest, ShortInterfaceConstructor)
{
    std::shared_ptr<Obj::Interface> obj = nullptr;
    FakeFile fake(piol, notFile, obj);
    EXPECT_EQ(nullptr, fake.obj);
    SCOPED_TRACE("ShortInterfaceConstructor");
    compareConstructor(piol.get(), fake);
}

//In this test we pass the MPI-IO Data Options class.
//We do not use a valid name as we are not interested in the result
TEST_F(FileTest, InterfaceConstructor)
{
    const Obj::SEGYOpt objOpt;
    const Data::MPIIOOpt dataOpt;

    FakeFile fake(piol, notFile, objOpt, dataOpt);
    EXPECT_NE(nullptr, fake.obj);
    SCOPED_TRACE("InterfaceConstructor");
    compareConstructor(piol.get(), fake);
}

void BadConstructor(std::shared_ptr<ExSeisPIOL> piol, std::string name, const Obj::Opt & objOpt, const Data::Opt & dataOpt)
{
    FakeFile fake(piol, name, objOpt, dataOpt);
    EXPECT_EQ(piol, fake.piol);
    EXPECT_EQ(name, fake.name);
    EXPECT_EXIT(piol->isErr(), ExitedWithCode(EXIT_FAILURE), ".*8 3 Fatal Error in PIOL. . Dumping Log 0");
}

//In this test we pass the wrong Data Options class.
//We pass the base class instead of MPIIOOpt (the default class)
TEST_F(FileTest, BadInterfaceConstructor1)
{
    SCOPED_TRACE("BadInterface 1");
    const Obj::SEGYOpt objOpt;
    const Data::Opt dataOpt;
    BadConstructor(piol, notFile, objOpt, dataOpt);
}

//In this test we pass the wrong Object Options class.
//We pass the base class
TEST_F(FileTest, BadInterfaceConstructor2)
{
    SCOPED_TRACE("BadInterface 2");
    const Obj::Opt objOpt;
    const Data::MPIIOOpt dataOpt;
    BadConstructor(piol, notFile, objOpt, dataOpt);
}

//In this test we pass two wrong Object Options classes.
//We pass the base classes
TEST_F(FileTest, BadInterfaceConstructor3)
{
    SCOPED_TRACE("BadInterface 3");
    const Obj::Opt objOpt;
    const Data::Opt dataOpt;
    BadConstructor(piol, notFile, objOpt, dataOpt);
}
