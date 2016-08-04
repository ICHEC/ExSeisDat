#include <memory>
#include <fstream>
#include <cstdio>
#include <unistd.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#define UNIT_TEST
#define private public
#define protected public
#include "share/datatype.hh"
#include "tglobal.hh"
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

typedef MPIIOTest MPIIODeathTest;
TEST_F(MPIIODeathTest, FailedConstructor)
{
    Data::MPIIO mio(piol, notFile, ioopt);
    EXPECT_EQ(piol, mio.piol);
    EXPECT_EQ(notFile, mio.name);

    Log::Item * item = &piol->log->loglist.front();
    EXPECT_EQ(notFile, item->file);
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
    piol->isErr();
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
TEST_F(MPIIOTest, ReadBlocks)
{
    const size_t sz = 400;
    const size_t ns = 261;
    Data::MPIIO mio(piol, "tmp/smallsegy.tmp", ioopt);
    piol->isErr();
    size_t offset = 0;

    std::vector<uchar> tr(sz*240);
    mio.read(3600, 240, ns*4 + 240, 400, tr.data());
    piol->isErr();

    for (size_t i = 0; i < sz; i++)
    {
        uchar * md = tr.data() + i*240;
        EXPECT_EQ(ilNum(i+offset), getHost<int32_t>(&md[188])) << i;
        EXPECT_EQ(xlNum(i+offset), getHost<int32_t>(&md[192])) << i;
    }
}


TEST_F(MPIIOTest, BlockingReadSmall)
{
    Data::MPIIO mio(piol, smallFile, ioopt);
    std::vector<uchar> d(smallSize);
    d.back() = getPattern(d.size()-2);
    mio.read(0, d.size()-1, d.data());
    piol->isErr();
    EXPECT_EQ(getPattern(d.size()-2), d.back());

    //Set the last element to zero
    d.back() = 0U;
    std::vector<uchar> test(smallSize);
    ASSERT_THAT(d, ElementsAreArray(test));
}

TEST_F(MPIIOTest, ZeroSizeReadOnLarge)
{
    Data::MPIIO mio(piol, plargeFile, ioopt);

    std::vector<uchar> d = {getPattern(1U)};
    mio.read(0, 0, d.data());
    piol->isErr();

    EXPECT_EQ(getPattern(1U), d[0]);
    EXPECT_NE(getPattern(0U), d[0]);
}

TEST_F(MPIIOTest, OffsetsBlockingReadLarge)
{
    ioopt.maxSize = magicNum1;
    Data::MPIIO mio(piol, plargeFile, ioopt);

    //Test looping logic for big files, various offsets
    for (size_t j = 0; j < magicNum1; j += 10U)
    {
        size_t sz = 16U*magicNum1 + j;
        size_t offset = (largeSize / magicNum1) * j;
        std::vector<uchar> d(sz);

        mio.read(offset, d.size(), d.data());
        piol->isErr();

        for (size_t i = 0; i < d.size(); i++)
            EXPECT_EQ(d[i], getPattern(offset + i));
    }
}

TEST_F(MPIIOTest, BlockingOneByteReadLarge)
{
    Data::MPIIO mio(piol, plargeFile, ioopt);
    //Test single value reads mid file
    for (size_t i = 0; i < magicNum1; i++)
    {
        size_t offset = largeSize / 2U + i;
        uchar test[2] = {getPattern(offset-2), getPattern(offset-1)};

        mio.read(offset, 1, test);
        piol->isErr();
        EXPECT_EQ(test[0], getPattern(offset));
        EXPECT_EQ(test[1], getPattern(offset-1));
    }
}

//TODO: Find out why this is much slower on panasas
TEST_F(MPIIOTest, BlockingReadEnd)
{
    Data::MPIIO mio(piol, plargeFile, ioopt);
    piol->isErr();

    //Intentionally read much beyond the end of the file to make sure that MPI-IO doesn't abort/fails.
    //MPI 3.1 spec says (or at least strongly implies) it should work.
    std::vector<uchar> d(prefix(3));
    for (size_t j = 0; j > magicNum1; j += 10U)
    {
        mio.read(largeSize-j, d.size(), d.data());
        piol->isErr();
        for (size_t i = 0; i < j; i++)
            EXPECT_EQ(d[i], getPattern(largeSize-j + i));
    }
}

