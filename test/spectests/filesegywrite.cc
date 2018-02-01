#include "filesegytest.hh"

//TODO:: Add tests were trace header writes cause a setFileSz call

TEST_F(FileSEGYWrite, FileWriteHO)
{
    makeMockSEGY<true>();
}

TEST_F(FileSEGYWrite, FileWriteHOLongString)
{
    //Extend the string beyond the text boundary
    //Extended text should be dropped in write call
    const size_t sz       = testString.size();
    const size_t extendSz = 3400U - sz;
    testString.resize(sz + extendSz);
    for (size_t i = 3200U; i < sz + extendSz; i++)
        testString[i] = uchar(0x7F);

    makeMockSEGY<true>();
}

TEST_F(FileSEGYWrite, FileWriteHOEmptyString)
{
    testString.resize(0);
    makeMockSEGY<true>();
}

TEST_F(FileSEGYWrite, FileWriteTrHdrGrid)
{
    makeMockSEGY<true>();
    for (size_t i = 0; i < nt; i++)
        writeTrHdrGridTest(i);
}

TEST_F(FileSEGYWrite, FileWriteTrHdrCoord1)
{
    makeMockSEGY<true>();
    std::vector<uchar> tr(SEGSz::getMDSz());
    initWriteTrHdrCoord({xCMP, yCMP}, {160010, 240022}, -100, 10U, &tr);


    File::Param prm(1U);
    File::setPrm(0, PIOL_META_xCmp, 1600.1, &prm);
    File::setPrm(0, PIOL_META_yCmp, 2400.22, &prm);
    File::setPrm(0, PIOL_META_tn, 10U, &prm);
    file->writeParam(10U, 1U, &prm);
}

TEST_F(FileSEGYWrite, FileWriteTrHdrCoord2)
{
    makeMockSEGY<true>();
    std::vector<uchar> tr(SEGSz::getMDSz());
    initWriteTrHdrCoord({xSrc, ySrc}, {1600100, 3400222}, -1000, 10U, &tr);

    File::Param prm(1U);
    File::setPrm(0, PIOL_META_xSrc, 1600.1000, &prm);
    File::setPrm(0, PIOL_META_ySrc, 3400.2220, &prm);
    File::setPrm(0, PIOL_META_tn, 10U, &prm);

    file->writeParam(10U, 1U, &prm);
}

TEST_F(FileSEGYWrite, FileWriteTrHdrCoord3)
{
    makeMockSEGY<true>();
    std::vector<uchar> tr(SEGSz::getMDSz());
    initWriteTrHdrCoord({xSrc, ySrc}, {1623001001, 34002220}, -10000, 10U, &tr);

    File::Param prm(1U);
    File::setPrm(0, PIOL_META_xSrc, 162300.10009, &prm);
    File::setPrm(0, PIOL_META_ySrc, 3400.22201, &prm);
    File::setPrm(0, PIOL_META_tn, 10U, &prm);
    file->writeParam(10U, 1U, &prm);
}

TEST_F(FileSEGYWrite, FileWriteTraceNormal)
{
    nt = 100;
    ns = 300;
    makeMockSEGY<false>();
    writeTraceTest(0U, nt);
}

TEST_F(FileSEGYWrite, FileWriteTraceWPrmNormal)
{
    nt = 100;
    ns = 300;
    makeMockSEGY<false>();
    writeTraceTest<true>(0U, nt);
}

TEST_F(FileSEGYWrite, FileWriteRandomTraceNormal)
{
    nt           = 100;
    ns           = 300;
    size_t size  = 1U;
    auto offsets = getRandomVec(size, nt, 1337);
    makeMockSEGY<false>();
    writeRandomTraceTest(size, offsets);
}

TEST_F(FileSEGYWrite, FileWriteRandomTraceWPrmNormal)
{
    nt           = 100;
    ns           = 300;
    size_t size  = 1U;
    auto offsets = getRandomVec(size, nt, 1337);
    makeMockSEGY<false>();
    writeRandomTraceTest<true>(size, offsets);
}

TEST_F(FileSEGYWrite, FileWriteTraceBigNs)
{
    nt = 100;
    ns = 10000;
    makeMockSEGY<false>();
    writeTraceTest(10U, nt);
}

TEST_F(FileSEGYWrite, FileWriteRandomTraceBigNs)
{
    nt           = 100;
    ns           = 300;
    size_t size  = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    makeMockSEGY<false>();
    writeRandomTraceTest(size, offsets);
}

TEST_F(FileSEGYWrite, FileWriteTraceWPrmBigNs)
{
    nt = 100;
    ns = 10000;
    makeMockSEGY<false>();
    writeTraceTest<true>(10U, nt);
}

TEST_F(FileSEGYWrite, FileWriteRandomTraceWPrmBigNs)
{
    nt           = 100;
    ns           = 300;
    size_t size  = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    makeMockSEGY<false>();
    writeRandomTraceTest<true>(size, offsets);
}

TEST_F(FileSEGYWrite, FileWriteTraceBigOffset)
{
    nt = 3000;
    ns = 3000;
    makeMockSEGY<false>();
    writeTraceTest(3728270U, nt);
}

TEST_F(FileSEGYWrite, FileWriteRandomTraceBigOffset)
{
    nt           = 3000;
    ns           = 3000;
    size_t size  = 1U;
    auto offsets = std::vector<size_t>(size);
    offsets[0]   = 3001;
    makeMockSEGY<false>();
    writeRandomTraceTest(size, offsets);
}

TEST_F(FileSEGYWrite, FileWriteTraceWPrmBigOffset)
{
    nt = 3000;
    ns = 3000;
    makeMockSEGY<false>();
    writeTraceTest<true>(3728270U, nt);
}

TEST_F(FileSEGYWrite, FileWriteRandomTraceWPrmBigOffset)
{
    nt           = 3000;
    ns           = 3000;
    size_t size  = 1U;
    auto offsets = std::vector<size_t>(size);
    offsets[0]   = 3001;
    makeMockSEGY<false>();
    writeRandomTraceTest<true>(size, offsets);
}

TEST_F(FileSEGYWrite, FarmFileWriteTraceBigNt)
{
    nt = 3728270;
    ns = 300;
    makeMockSEGY<false>();
    writeTraceTest(0U, nt);
}

TEST_F(FileSEGYWrite, FarmFileWriteRandomTraceBigNt)
{
    nt           = 3728270;
    ns           = 300;
    size_t size  = nt / 2;
    auto offsets = getRandomVec(size, nt, 1337);
    makeMockSEGY<false>();
    writeRandomTraceTest(size, offsets);
}

TEST_F(FileSEGYWrite, FarmFileWriteTraceWPrmBigNt)
{
    nt = 3728270;
    ns = 300;
    makeMockSEGY<false>();
    writeTraceTest<true>(0U, nt);
}

TEST_F(FileSEGYWrite, FarmFileWriteRandomTraceWPrmBigNt)
{
    nt           = 3728270;
    ns           = 300;
    size_t size  = nt / 2;
    auto offsets = getRandomVec(size, nt, 1337);
    makeMockSEGY<false>();
    writeRandomTraceTest<true>(size, offsets);
}

TEST_F(FileSEGYWrite, FileWriteTraceZeroNt)
{
    nt = 0;
    ns = 10;
    makeMockSEGY<false>();
    writeTraceTest(10U, nt);
}

TEST_F(FileSEGYWrite, FileWriteRandomTraceZeroNt)
{
    nt           = 0;
    ns           = 10;
    size_t size  = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    makeMockSEGY<false>();
    writeRandomTraceTest(size, offsets);
}

TEST_F(FileSEGYWrite, FileWriteTraceWPrmZeroNt)
{
    nt = 0;
    ns = 10;
    makeMockSEGY<false>();
    writeTraceTest<true>(10U, nt);
}

TEST_F(FileSEGYWrite, FileWriteRandomTraceWPrmZeroNt)
{
    nt           = 0;
    ns           = 10;
    size_t size  = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    makeMockSEGY<false>();
    writeRandomTraceTest<true>(size, offsets);
}

TEST_F(FileSEGYWrite, FileWriteTraceZeroNs)
{
    nt = 10;
    ns = 0;
    makeMockSEGY<false>();
    writeTraceTest(10U, nt);
}

TEST_F(FileSEGYWrite, FileWriteRandomTraceZeroNs)
{
    nt           = 10;
    ns           = 0;
    size_t size  = 10U;
    auto offsets = getRandomVec(size, nt, 1337);
    makeMockSEGY<false>();
    writeRandomTraceTest(size, offsets);
}

TEST_F(FileSEGYWrite, FileWriteTraceWPrmZeroNs)
{
    nt = 10;
    ns = 0;
    makeMockSEGY<false>();
    writeTraceTest<true>(10U, nt);
}

TEST_F(FileSEGYWrite, FileWriteRandomTraceWPrmZeroNs)
{
    nt           = 10;
    ns           = 0;
    size_t size  = 10U;
    auto offsets = getRandomVec(size, nt, 1337);
    makeMockSEGY<false>();
    writeRandomTraceTest<true>(size, offsets);
}

TEST_F(FileSEGYWrite, FileWriteParam)
{
    makeMockSEGY<true>();
    writeTraceHeaderTest<false>(0U, nt);
}

TEST_F(FileSEGYWrite, FileWriteParamOne)
{
    nt = 400;
    makeMockSEGY<true>();
    writeTraceHeaderTest<false>(200, 1);
}

TEST_F(FileSEGYWrite, FileWriteParamBigOffset)
{
    nt = 10000000U;
    makeMockSEGY<true>();
    writeTraceHeaderTest<false>(nt - 1, 1);
}

TEST_F(FileSEGYWrite, FileWriteParamCopy)
{
    makeMockSEGY<true>();
    writeTraceHeaderTest<true>(0U, nt);
}

TEST_F(FileSEGYWrite, FileWriteParamOneCopy)
{
    nt = 400;
    makeMockSEGY<true>();
    writeTraceHeaderTest<true>(200, 1);
}

TEST_F(FileSEGYWrite, FileWriteParamBigOffsetCopy)
{
    nt = 10000000U;
    makeMockSEGY<true>();
    writeTraceHeaderTest<true>(nt - 1, 1);
}

/* clang-format off */
// //Akward to fit this into the current functions
// TEST_F(FileSEGYDeath, FileWriteTraceParamBigOffset)
// {
//     makeMockSEGY<true>();
//     writeTraceHeaderTest<false, false>(NT_LIMITS+1);
//     EXPECT_EXIT(piol->isErr(), ExitedWithCode(EXIT_FAILURE), ".*8 3 Fatal Error in PIOL. . Dumping Log 0");
// }
/* clang-format on */

////////////////////////////////// DEATH TESTS /////////////////////////////////
typedef FileSEGYWrite FileSEGYDeath;

TEST_F(FileSEGYDeath, FileWriteAPIBadns)
{
    ns = 0x470000;
    makeMockSEGY<false>();
    file->writeNs(ns);
    mock.reset();
    EXPECT_EXIT(
      piol->isErr(), ExitedWithCode(EXIT_FAILURE),
      ".*8 3 Fatal Error in PIOL. . Dumping Log 0");
}

#ifdef NT_LIMITS
TEST_F(FileSEGYDeath, FileWriteAPIBadnt)
{
    nt = NT_LIMITS + 1;
    makeMockSEGY<false>();
    file->writeNt(nt);

    mock.reset();
    EXPECT_EXIT(
      piol->isErr(), ExitedWithCode(EXIT_FAILURE),
      ".*8 3 Fatal Error in PIOL. . Dumping Log 0");
}
#endif

TEST_F(FileSEGYDeath, FileWriteAPIBadinc)
{
    geom_t ginc = geom_t(1) / geom_t(0);
    makeMockSEGY<false>();
    file->writeInc(ginc);

    mock.reset();
    EXPECT_EXIT(
      piol->isErr(), ExitedWithCode(EXIT_FAILURE),
      ".*8 3 Fatal Error in PIOL. . Dumping Log 0");
}
