#include <memory>
#include <fstream>
#include <cstdio>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#define UNIT_TEST
#define private public
#define protected public
#include "global.hh"
#include "data/datampiio.hh"
#undef private
#undef protected

using namespace testing;
using namespace PIOL;
class MPIIOTest : public Test
{
    protected :
    std::shared_ptr<ExSeisPIOL> piol;
    Comm::MPIOpt opt;
    Data::MPIIOOpt ioopt;
    MPIIOTest()
    {
        opt.initMPI = false;
        piol = std::make_shared<ExSeisPIOL>(opt);
    }
};

TEST_F(MPIIOTest, FailedConstructor)
{
    std::string name = "!£$%^&*()<>?:@~}{fakefile1234567890";

    EXPECT_EXIT(Data::MPIIO mio(piol, name, ioopt), ExitedWithCode(EXIT_FAILURE), ".* 8 3 Fatal Error in PIOL. (code: 1) Dumping Log 0");

/*    EXPECT_EQ(piol, mio.piol);
    EXPECT_EQ(name, mio.name);

    Log::Item * item = &piol->log->que.front();

    EXPECT_EQ(piol->comm->getRank(), item->rank) << "Rank wrong";
    EXPECT_EQ(name, item->file);
    EXPECT_EQ(Log::Layer::Data, item->layer);
    EXPECT_EQ(Log::Status::Error, item->stat);
    EXPECT_NE(0, item->msg.size());
    EXPECT_EQ(Log::Verb::None, item->vrbsy);*/
}
TEST_F(MPIIOTest, Constructor)
{
    std::string name = "tmp/testDataTestConstructor.tmp";
    {   //Make an empty file
        std::ofstream ofs(name);
    }
    //ASSERT_EQ(true, std::exists(name.c_str()));

    Data::MPIIO mio(piol, name, ioopt);
    EXPECT_EQ(piol, mio.piol);
    EXPECT_EQ(name, mio.name);
    EXPECT_TRUE(piol->log->que.empty()) << "Unexpected log message";
    ASSERT_EQ(0, std::remove(name.c_str())) << "Failed to remove a temporary file";
}

