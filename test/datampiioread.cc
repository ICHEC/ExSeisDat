#include "datampiiotest.hh"

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

template <bool block>
void readBlocks(const size_t nt, const size_t ns, Data::Interface * data)
{
    size_t offset = 0;
    size_t step = (block ? SEGSz::getMDSz() : SEGSz::getDOSz(ns));
    std::vector<uchar> tr(step*nt);

    if (block)
        data->read(SEGSz::getHOSz(), SEGSz::getMDSz(), SEGSz::getDOSz(ns), nt, tr.data());
    else
        data->read(SEGSz::getHOSz(), SEGSz::getDOSz(ns)*nt, tr.data());

    for (size_t i = 0; i < nt; i++)
    {
        uchar * md = &tr[step*i];
        ASSERT_EQ(ilNum(i+offset), getHost<int32_t>(&md[188])) << i;
        ASSERT_EQ(xlNum(i+offset), getHost<int32_t>(&md[192])) << i;
    }
}

TEST_F(MPIIOTest, ReadAllBlocks)
{
    const size_t nt = 400;
    const size_t ns = 261;
    Data::MPIIO mio(piol, smallSEGYFile, ioopt);
    piol->isErr();
    size_t offset = 0;
    readBlocks<false>(nt, ns, &mio);
}


TEST_F(MPIIOTest, ReadBlocksSmall)
{
    const size_t nt = 400;
    const size_t ns = 261;
    Data::MPIIO mio(piol, smallSEGYFile, ioopt);
    piol->isErr();
    readBlocks<true>(nt, ns, &mio);
}

#ifdef BIG_TESTS
TEST_F(MPIIOTest, ReadBlocksLarge)
{
    const size_t nt = 2000000U;
    const size_t ns = 1000;
    Data::MPIIO mio(piol, largeSEGYFile, ioopt);
    piol->isErr();
    readBlocks<true>(nt, ns, &mio);
}
#endif

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
            ASSERT_EQ(d[i], getPattern(offset + i));
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

#ifdef BIG_TESTS
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
#endif
