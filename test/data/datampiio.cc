#include <memory>
#include <fstream>
#include <cstdio>
#include <unistd.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#define UNIT_TEST
#define private public
#define protected public
#include "tglobal.hh"
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
    ~MPIIOTest()
    {
    }
};

TEST_F(MPIIOTest, SelfTest)
{
    EXPECT_NE(0, magicNum1);
    EXPECT_EQ(0, magicNum1 / 0xFF);
    struct stat stats;
    EXPECT_EQ(0, stat(zeroFile.c_str(), &stats));
    EXPECT_EQ(0, stat(smallFile.c_str(), &stats));
    EXPECT_EQ(0, stat(largeFile.c_str(), &stats));
    EXPECT_EQ(0, stat(plargeFile.c_str(), &stats));
}

typedef MPIIOTest MPIIODeathTest;
TEST_F(MPIIODeathTest, FailedConstructor)
{
    std::string name = "!£$%^&*()<>?:@~}{fakefile1234567890";
    Data::MPIIO mio(piol, name, ioopt);

    EXPECT_EQ(piol, mio.piol);
    EXPECT_EQ(name, mio.name);

    Log::Item * item = &piol->log->loglist.front();

    EXPECT_EQ(name, item->file);
    EXPECT_EQ(Log::Layer::Data, item->layer);
    EXPECT_EQ(Log::Status::Error, item->stat);
    EXPECT_NE(0, item->msg.size());
    EXPECT_EQ(Log::Verb::None, item->vrbsy);
    EXPECT_EXIT(piol->isErr(), ExitedWithCode(EXIT_FAILURE), ".*8 3 Fatal Error in PIOL. . Dumping Log 0");
}
TEST_F(MPIIOTest, Constructor)
{
    Data::MPIIO mio(piol, zeroFile, ioopt);
    EXPECT_EQ(piol, mio.piol);
    EXPECT_EQ(zeroFile, mio.name);
    piol->log->procLog();
    EXPECT_TRUE(piol->log->loglist.empty()) << "Unexpected log message";
    EXPECT_TRUE(mio.file != MPI_FILE_NULL) << "File was not opened";
}

TEST_F(MPIIOTest, ZeroFileSize)
{
    Data::MPIIO mio(piol, zeroFile, ioopt);
    piol->isErr();
    EXPECT_EQ(0, mio.getFileSz());
}
TEST_F(MPIIOTest, SmallFileSize)
{
    Data::MPIIO mio(piol, smallFile, ioopt);
    piol->isErr();
    EXPECT_EQ(smallSize, mio.getFileSz());
}
TEST_F(MPIIOTest, LargeFileSize)
{
    Data::MPIIO mio(piol, largeFile, ioopt);
    piol->isErr();
    EXPECT_EQ(largeSize, mio.getFileSz());
}

TEST_F(MPIIOTest, BlockingReadSmall)
{
    Data::MPIIO mio(piol, smallFile, ioopt);
    std::vector<uchar> d(smallSize);
    d.back() = 'a';
    mio.read(0, d.data(), d.size()-1);
    piol->isErr();
    EXPECT_EQ('a', d.back());

    d.back() = '\0';
    std::vector<uchar> test(smallSize);
    ASSERT_THAT(d, ElementsAreArray(test));
}

TEST_F(MPIIOTest, BlockingReadLarge)
{
    size_t temp = ioopt.maxSize;
    ioopt.maxSize = magicNum1;
    Data::MPIIO mio(piol, plargeFile, ioopt);
    ioopt.maxSize = temp; //reset for the next tests

    std::vector<uchar> d = {uchar(magicNum1)};
    mio.read(0, d.data(), 0);
    piol->isErr();

    //If this test and the next pass, pick a different
    //magic number to initialise d.
    EXPECT_NE(d[0], getPattern(0));
    EXPECT_EQ(magicNum1, d[0]);
}

TEST_F(MPIIOTest, OffsetsBlockingReadLarge)
{
    size_t temp = ioopt.maxSize;
    ioopt.maxSize = magicNum1;
    Data::MPIIO mio(piol, plargeFile, ioopt);
    ioopt.maxSize = temp; //reset for the next tests

    //Test looping logic for big files, various offsets
    for (size_t j = 0; j < magicNum1; j += 10U)
    {
        size_t sz = 16U*magicNum1 + j;
        size_t offset = (largeSize / magicNum1) * j;
        std::vector<uchar> d(sz);

        mio.read(offset, d.data(), d.size());
        piol->isErr();

        for (size_t i = 0; i < d.size(); i++)
            EXPECT_EQ(d[i], getPattern(offset + i));
    }
}

TEST_F(MPIIOTest, BlockingLargeFileSmallRead)
{
    Data::MPIIO mio(piol, plargeFile, ioopt);
    //Test single value reads mid file
    for (size_t i = 0; i < magicNum1; i++)
    {
        size_t offset = largeSize / 2U + i;
        uchar test[2];

        mio.read(offset, test, 1);
        piol->isErr();
        EXPECT_EQ(test[0], getPattern(offset));
    }
}

TEST_F(MPIIOTest, BlockingReadEnd)
{
    Data::MPIIO mio(piol, plargeFile, ioopt);
    piol->isErr();

    //Intentionally read much beyond the end of the file to make sure that MPI-IO doesn't abort/fails.
    //MPI 3.1 spec says (or at least strongly implies) it should work.
    std::vector<uchar> d(prefix(3));
    for (size_t j = 0; j > magicNum1; j += 10U)
    {
        mio.read(largeSize-j, d.data(), d.size());
        piol->isErr();
        for (size_t i = 0; i < j; i++)
            EXPECT_EQ(d[i], getPattern(largeSize-j + i));
    }
}

