#include "objsegytest.hh"

/////////////////////////// Header Object Reading //////////////////////////////////
TEST_F(ObjIntegrationTest, SEGYZeroReadHO)
{
    makeRealSEGY<false>(zeroFile);
    std::vector<uchar> ho(SEGSz::getHOSz());
    for (size_t i = 0; i < SEGSz::getHOSz(); i++)
        ho[i] = getPattern(SEGSz::getHOSz() - i);

    obj->readHO(ho.data());

    for (size_t i = 0; i < SEGSz::getHOSz(); i++)
        ASSERT_EQ(ho[i], getPattern(SEGSz::getHOSz() - i));
}

TEST_F(ObjIntegrationTest, SEGYReadHO)
{
    makeRealSEGY<false>(plargeFile);
    SEGYReadHOTest({uchar(magicNum1), uchar(magicNum1+1)});
}

TEST_F(ObjIntegrationTest, SEGYReadTrHdr)
{
    makeRealSEGY<false>(plargeFile);
    SCOPED_TRACE("Read1");
    SEGYReadTrTest(10U, {0U, 1U, 100U, 1000U, 1000U}, magicNum1);
    SCOPED_TRACE("Read2");
    SEGYReadTrTest(100U, {0U, 1U, 100U, 1000U, 1000U}, magicNum1+1U);
}

TEST_F(ObjIntegrationTest, SEGYReadTrHdrs)
{
    const size_t sz = 400;
    const size_t ns = 261;
    //const size_t sz = 20000;
    //const size_t ns = 1000;

    makeRealSEGY<false>(smallSEGYFile);
    piol->isErr();
    size_t offset = 0;

    std::vector<uchar> tr(sz*SEGSz::getMDSz());
    obj->readDOMD(offset, ns, sz, tr.data());
    piol->isErr();

    for (size_t i = 0; i < sz; i++)
    {
        uchar * md = tr.data() + i*SEGSz::getMDSz();
        ASSERT_EQ(ilNum(i+offset), getHost<int32_t>(&md[188])) << i;
        ASSERT_EQ(xlNum(i+offset), getHost<int32_t>(&md[192])) << i;
    }
}

