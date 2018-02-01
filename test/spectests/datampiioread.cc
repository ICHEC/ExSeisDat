#include "datampiiotest.hh"

size_t modifyNt(
  const size_t fs, const size_t offset, const size_t nt, const size_t ns)
{
    //We shouldn't have our ASSERT_EQ test beyond the actual number of traces which are
    //so we reduce the number of traces down to the number of traces present after
    //the given offset if the real number of traces is less than expected.
    //We support this because it is allowed behaviour.

    size_t realnt = SEGSz::getNt(fs, ns);
    if (realnt >= offset + nt)
        return nt;
    else if (realnt < offset)
        return 0;
    return realnt -= offset;
}

typedef MPIIOTest MPIIODeathTest;

TEST_F(MPIIODeathTest, FailedConstructor)
{
    makeMPIIO(notFile);

    EXPECT_EQ(piol, data->piol());
    EXPECT_EQ(notFile, data->name());

    const Log::Item* item = &piol->log->loglist().front();
    EXPECT_EQ(notFile, item->file);
    EXPECT_EQ(Log::Layer::Data, item->layer);
    EXPECT_EQ(Log::Status::Error, item->stat);
    EXPECT_NE(static_cast<size_t>(0), item->msg.size());
    EXPECT_EQ(PIOL_VERBOSITY_NONE, item->vrbsy);

    EXPECT_EXIT(
      piol->isErr(), ExitedWithCode(EXIT_FAILURE),
      ".*8 3 Fatal Error in PIOL. . Dumping Log 0");
}

////////////////////////////// MPI-IO getting the file size ///////////////////////////////////
TEST_F(MPIIOTest, Constructor)
{
    makeMPIIO(zeroFile);
    EXPECT_EQ(piol, data->piol());
    EXPECT_EQ(zeroFile, data->name());
    piol->isErr();
    piol->log->procLog();

    EXPECT_TRUE(piol->log->loglist().empty()) << "Unexpected log message";

    EXPECT_NE(nullptr, data) << "data is null";
    auto mio = std::dynamic_pointer_cast<Data::MPIIO>(data);
    EXPECT_NE(nullptr, mio) << "MPI-IO data cast failed";
    EXPECT_FALSE(mio->isFileNull()) << "File was not opened";

    piol->isErr();
    EXPECT_EQ(static_cast<size_t>(0), data->getFileSz());
}

TEST_F(MPIIOTest, SmallFileSize)
{
    makeMPIIO(smallFile);
    piol->isErr();
    EXPECT_EQ(smallSize, data->getFileSz());
}

TEST_F(MPIIOTest, LargeFileSize)
{
    makeMPIIO(largeFile);
    piol->isErr();
    EXPECT_EQ(largeSize, data->getFileSz());
}

////////////////////////////// MPI-IO reading contiguous data ///////////////////////////////////

TEST_F(MPIIOTest, BlockingReadSmall)
{
    makeMPIIO(smallFile);

    std::vector<uchar> d(smallSize);
    d.back() = getPattern(d.size() - 2);

    data->read(0, d.size() - 1, d.data());
    piol->isErr();

    EXPECT_EQ(getPattern(d.size() - 2), d.back());

    //Set the last element to zero
    d.back() = 0U;
    std::vector<uchar> test(smallSize);
    ASSERT_THAT(d, ElementsAreArray(test));
}

TEST_F(MPIIOTest, ZeroSizeReadOnLarge)
{
    makeMPIIO(plargeFile);

    std::vector<uchar> d = {getPattern(1U)};
    data->read(0, 0, d.data());
    piol->isErr();

    EXPECT_EQ(getPattern(1U), d[0]);
    EXPECT_NE(getPattern(0U), d[0]);
}

TEST_F(MPIIOTest, OffsetsBlockingReadLarge)
{
    ioopt.maxSize = magicNum1;
    makeMPIIO(plargeFile);

    //Test looping logic for big files, various offsets
    for (size_t j = 0; j < magicNum1; j += 10U) {
        size_t sz     = 16U * magicNum1 + j;
        size_t offset = (largeSize / magicNum1) * j;
        std::vector<uchar> d(sz);

        data->read(offset, d.size(), d.data());
        piol->isErr();

        for (size_t i = 0; i < d.size(); i++)
            ASSERT_EQ(d[i], getPattern(offset + i));
    }
}

TEST_F(MPIIOTest, BlockingOneByteReadLarge)
{
    makeMPIIO(plargeFile);
    //Test single value reads mid file
    for (size_t i = 0; i < magicNum1; i++) {
        size_t offset = largeSize / 2U + i;
        uchar test[2] = {getPattern(offset - 2), getPattern(offset - 1)};

        data->read(offset, 1, test);
        piol->isErr();
        EXPECT_EQ(test[0], getPattern(offset));
        EXPECT_EQ(test[1], getPattern(offset - 1));
    }
}

////////////////////////////// MPI-IO reading non-contiguous blocks of data ///////////////////////////////////
TEST_F(MPIIOTest, ReadContigZero)
{
    makeMPIIO(smallSEGYFile);
    const size_t nt = 0;
    const size_t ns = 0;
    readSmallBlocks<false>(nt, ns);
    piol->isErr();
}

TEST_F(MPIIOTest, ReadContigSSS)
{
    makeMPIIO(smallSEGYFile);
    const size_t nt = 400;
    const size_t ns = 261;
    readSmallBlocks<false>(nt, ns);
    piol->isErr();
}

//Intentionally read much beyond the end of the file to make sure that MPI-IO doesn't abort/fails.
//MPI 3.1 spec says (or at least strongly implies) it should work.
TEST_F(MPIIOTest, FarmReadContigEnd)
{
    makeMPIIO(smallSEGYFile);
    size_t nt       = 400;
    const size_t ns = 261;

    //Read extra
    nt *= 1024;
    readSmallBlocks<false>(nt, ns, 200);
    piol->isErr();
}

TEST_F(MPIIOTest, ReadContigSLS)
{
    makeMPIIO(bigTraceSEGYFile);
    const size_t nt = 40U;
    const size_t ns = 32000U;
    readSmallBlocks<false>(nt, ns, 1000U);
    piol->isErr();
}

TEST_F(MPIIOTest, FarmReadContigSLM)
{
    makeMPIIO(bigTraceSEGYFile);
    const size_t nt = 40000U;
    const size_t ns = 32000U;
    readSmallBlocks<false>(nt, ns);
    piol->isErr();
}

TEST_F(MPIIOTest, FarmReadContigSLL)
{
    makeMPIIO(largeSEGYFile);
    const size_t nt = 2000000U;
    const size_t ns = 1000U;
    readSmallBlocks<false>(nt, ns);
    piol->isErr();
}

TEST_F(MPIIOTest, FarmReadContigLLM)
{
    makeMPIIO(bigTraceSEGYFile);
    const size_t nt = 40000U;
    const size_t ns = 32000U;
    readBigBlocks<false>(nt, ns);
    piol->isErr();
}

TEST_F(MPIIOTest, FarmReadContigMLL)
{
    makeMPIIO(largeSEGYFile);
    const size_t nt = 2000000U;
    const size_t ns = 1000U;
    readBigBlocks<false>(nt, ns);
    piol->isErr();
}

TEST_F(MPIIOTest, ReadBlocksZero)
{
    makeMPIIO(smallSEGYFile);
    const size_t nt = 0;
    const size_t ns = 0;
    readSmallBlocks<true>(nt, ns);
    piol->isErr();
}

TEST_F(MPIIOTest, ReadBlocksSSS)
{
    makeMPIIO(smallSEGYFile);
    const size_t nt = 400;
    const size_t ns = 261;
    readSmallBlocks<true>(nt, ns);
    piol->isErr();
}

TEST_F(MPIIOTest, ReadBlocksEnd)
{
    makeMPIIO(smallSEGYFile);
    size_t nt       = 400;
    const size_t ns = 261;

    //Read extra
    nt *= 1024;
    readSmallBlocks<true>(nt, ns, 200);
    piol->isErr();
}

TEST_F(MPIIOTest, ReadBlocksSLS)
{
    makeMPIIO(bigTraceSEGYFile);
    const size_t nt = 40U;
    const size_t ns = 32000U;
    readSmallBlocks<true>(nt, ns, 1000U);
    piol->isErr();
}

TEST_F(MPIIOTest, FarmReadBlocksSLM)
{
    makeMPIIO(bigTraceSEGYFile);
    const size_t nt = 40000U;
    const size_t ns = 32000U;
    readSmallBlocks<true>(nt, ns);
    piol->isErr();
}

TEST_F(MPIIOTest, FarmReadBlocksSLL)
{
    makeMPIIO(largeSEGYFile);
    const size_t nt = 2000000U;
    const size_t ns = 1000U;
    readSmallBlocks<true>(nt, ns);
    piol->isErr();
}

TEST_F(MPIIOTest, FarmReadBlocksLLM)
{
    makeMPIIO(bigTraceSEGYFile);
    const size_t nt = 40000U;
    const size_t ns = 32000U;
    readBigBlocks<true>(nt, ns);
    piol->isErr();
}

TEST_F(MPIIOTest, FarmReadBlocksMLL)
{
    makeMPIIO(largeSEGYFile);
    const size_t nt = 2000000U;
    const size_t ns = 1000U;
    readBigBlocks<true>(nt, ns);
    piol->isErr();
}

///////Lists//////////
const size_t largens = 1000U;
const size_t largent = 2000000U;
const size_t smallns = 261U;
const size_t smallnt = 400U;

TEST_F(MPIIOTest, ReadListZero)
{
    makeMPIIO(smallSEGYFile);
    readList(0, 0, NULL);
    piol->isErr();
}

TEST_F(MPIIOTest, ReadListSmall)
{
    makeMPIIO(smallSEGYFile);
    auto vec = getRandomVec(smallnt / 2, smallnt, 1337);
    readList(smallnt / 2, smallns, vec.data());
    piol->isErr();
}


TEST_F(MPIIOTest, FarmReadListLarge)
{
    makeMPIIO(largeSEGYFile);
    auto vec = getRandomVec(largent / 2, largent, 1337);

    readList(largent / 2, largens, vec.data());
    piol->isErr();
}
