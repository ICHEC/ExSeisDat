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
    std::string zeroFile = "tmp/testDataTestConstructor.tmp";
    MPIIOTest()
    {
        opt.initMPI = false;
        piol = std::make_shared<ExSeisPIOL>(opt);

        std::ofstream ofs(zeroFile);
    }
    ~MPIIOTest()
    {
        std::remove(zeroFile.c_str());
    }
};

typedef MPIIOTest MPIIODeathTest;
TEST_F(MPIIODeathTest, FailedConstructor)
{
    std::string name = "!£$%^&*()<>?:@~}{fakefile1234567890";
    Data::MPIIO mio(piol, name, ioopt);

    EXPECT_EQ(piol, mio.piol);
    EXPECT_EQ(name, mio.name);

    Log::Item * item = &piol->log->que.front();

    EXPECT_EQ(name, item->file);
    EXPECT_EQ(Log::Layer::Data, item->layer);
    EXPECT_EQ(Log::Status::Error, item->stat);
    EXPECT_NE(0, item->msg.size());
    EXPECT_EQ(Log::Verb::None, item->vrbsy);
    EXPECT_EXIT(piol->isErr(), ExitedWithCode(EXIT_FAILURE), ".*8 3 Fatal Error in PIOL. . Dumping Log 0");
}
TEST_F(MPIIOTest, Constructor)
{
    EXPECT_TRUE(piol->log->que.empty()) << "Unexpected log message";
    Data::MPIIO mio(piol, zeroFile, ioopt);
    EXPECT_EQ(piol, mio.piol);
    EXPECT_EQ(zeroFile, mio.name);
    piol->log->procLog();
    EXPECT_TRUE(piol->log->que.empty()) << "Unexpected log message";
    EXPECT_TRUE(mio.file != MPI_FILE_NULL) << "File was not opened";
}

TEST_F(MPIIOTest, fileSize)
{

}

