#include <fstream>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "comm/mpi.hh"
#include "block/blockmpiio.hh"
using namespace testing;
using namespace PIOL;
class BlockTest : public Test
{
    protected :
    std::shared_ptr<Comms::MPI> comm;
    std::string inName;
    std::string outName;
    BlockTest()
    {
        comm = std::make_shared<Comms::MPI>(MPI_COMM_WORLD);
        inName = "inTest.segy";
        outName = "temp.segy";
    }
//    virtual ~BlockTest() { }
//    virtual void SetUp(){}
    virtual void TearDown()
    {
        
    }

};

TEST_F(BlockTest, Size)
{
    size_t sz = 1024;
    {
        Block::MPIIO out(comm, outName);
        out.setFileSz(sz*sizeof(char));
        EXPECT_EQ(sz, out.getFileSz());
    }
    {
        std::ifstream test(outName);
        EXPECT_TRUE(test.good());
    }
}

