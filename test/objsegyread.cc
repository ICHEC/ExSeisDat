#include "objsegytest.hh"
TEST_F(ObjSpecTest, TestBypassConstructor)
{
    makeSEGY();
    EXPECT_EQ(piol, obj->piol);
    EXPECT_EQ(notFile, obj->name);
    EXPECT_EQ(mock, obj->data);
}

TEST_F(ObjSpecTest, ZeroSEGYFileSize)
{
    makeSEGY();
    SEGYFileSizeTest(0U);
}

TEST_F(ObjSpecTest, SmallSEGYFileSize)
{
    makeSEGY();
    SEGYFileSizeTest(40U*prefix(2U));
}

TEST_F(ObjSpecTest, BigSEGYFileSize)
{
    makeSEGY();
    SEGYFileSizeTest(8U*prefix(4U));
}

TEST_F(ObjSpecTest, SEGYHORead)
{
    makeSEGY();
    HOPatternTest(7, 107);
    HOPatternTest(10, 46);
    HOPatternTest(0, 0);
}
/*
TEST_F(ObjSpecTest, SEGYTrRead)
{
    const size_t ns = 200U;
    const size_t offset = 10U;
    const size_t extra = 1111U;

    makeSEGY();

    std::vector<uchar> cTrHdr(SEGSz::getMDSz());
    ExpectTrHdrPattern(traceNum, ns, &cTrHdr);

    piol->isErr();

    std::vector<uchar> trHdr(SEGSz::getMDSz() + extra);
    for (auto i = 0U; i < extra; i++)
        trHdr[trHdr.size()-extra+i] = magicNum1;

    segy.readDOMD(traceNum, ns, 1U, trHdr.data());

    for (auto i = 0U; i < SEGSz::getMDSz(); i++)
    {
        auto pat = getPattern(SEGSz::getDOLoc(traceNum, ns) + i);
        ASSERT_EQ(pat, trHdr[i]) << "tr Pattern " << i;
    }
    for (auto i = 0U; i < extra; i++)
        ASSERT_EQ(magicNum1, trHdr[trHdr.size()-extra+i]) << "tr Pattern Extra " << i;
}*/

TEST_F(ObjSpecTest, SEGYTrRead)
{
    const size_t ns = 200U;
    const size_t offset = 10U;
    const size_t extra = 1111U;

    makeSEGY();
    readTest<true>(10U, 1U, 200, 13);
}
