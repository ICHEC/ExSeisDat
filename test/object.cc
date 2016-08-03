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
    std::shared_ptr<ExSeisPIOL> piol;
    Comm::MPIOpt opt;
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
    FakeObject(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, std::shared_ptr<Data::Interface> data_)
               : Obj::Interface(piol_, name_, data_) { }

    FakeObject(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const Data::Opt & dataOpt)
               : Obj::Interface(piol_, name_, dataOpt) { }

    size_t getFileSz() const
    {
        return 0U;
    }

    void readHO(uchar * ho) const {}

    void getFileSz(size_t sz) const {}

    void writeHO(const uchar * ho) const {}

    void readDOMD(csize_t offset, csize_t ns, uchar * ho) const {}

    void readDOMD(csize_t offset, csize_t ns, csize_t sz, uchar * ho) const {}

    void writeDOMD(csize_t offset, csize_t ns, const uchar * ho) const {}
};
#pragma GCC diagnostic pop

//In this test we pass the MPI-IO Data Options class.
//We do not use a valid name as we are not interested in the result
TEST_F(ObjectTest, InterfaceConstructor)
{
    Data::MPIIOOpt dataOpt;
    FakeObject fake(piol, notFile, dataOpt);
    EXPECT_NE(nullptr, fake.data);
    EXPECT_EQ(piol, fake.piol);
    EXPECT_EQ(notFile, fake.name);
}

//In this test we pass the wrong Data Options class.
//We pass the base class instead of MPIIOOpt (the default class)
TEST_F(ObjectTest, BadInterfaceConstructor)
{
    Data::Opt dataOpt;
    FakeObject fake(piol, notFile, dataOpt);
    EXPECT_EQ(nullptr, fake.data);
    EXPECT_EQ(piol, fake.piol);
    EXPECT_EQ(notFile, fake.name);
}

