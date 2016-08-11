#include "objsegytest.hh"

TEST_F(ObjIntegrationTest, SEGYWriteHO)
{
    makeRealSEGY<true>(tempFile);
    std::string outFile = "tmp/testOutput.tmp";

    SEGYWriteHOPattern();
    SEGYReadHOTest({uchar(magicNum1), uchar(magicNum1+1)});
}

TEST_F(ObjIntegrationTest, SEGYWriteTrHdr)
{
    makeRealSEGY<true>(tempFile);

    SEGYWriteTrTest(40U, 0U);
    SEGYReadTrTest(40U, {0U}, magicNum1);

    SEGYWriteTrTest(80U, 100U);
    SEGYReadTrTest(80U, {100U}, magicNum1);

    SEGYWriteTrTest(400U, 1000U);
    SEGYReadTrTest(400U, {1000U}, magicNum1);
}

