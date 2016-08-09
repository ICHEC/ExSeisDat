#include "objsegytest.hh"

/////////////////////////// Header Object Reading //////////////////////////////////
void SEGYReadHOTest(Obj::Interface & obj, std::vector<uchar> magic)
{
    const size_t extra = 1025U;
    std::vector<uchar> ho(extra + SEGSz::getHOSz() + extra);

    for (size_t j = 0U; j < magic.size(); j++)
    {
        for (size_t i = 0U; i < extra; i++)
            ho[i] = ho[ho.size() - extra + i] = magic[j];

        obj.readHO(ho.data() + extra);

        //Overrun checks
        for (size_t i = 0U; i < extra; i++)
        {
            ASSERT_EQ(magic[j], ho[i]);
            ASSERT_EQ(magic[j], ho[ho.size() - extra + i]);
        }
        for (size_t i = 0U; i < SEGSz::getHOSz(); i++)
            ASSERT_EQ(getPattern(i), ho[i+extra]);
    }
}

typedef ObjSpecTest ObjIntegrationTest;
TEST_F(ObjIntegrationTest, SEGYZeroReadHO)
{
    Obj::SEGY segy(piol, zeroFile, segyOpt, dataOpt);
    piol->isErr();
    std::vector<uchar> ho(SEGSz::getHOSz());
    for (size_t i = 0; i < SEGSz::getHOSz(); i++)
        ho[i] = getPattern(SEGSz::getHOSz() - i);

    segy.readHO(ho.data());

    for (size_t i = 0; i < SEGSz::getHOSz(); i++)
        ASSERT_EQ(ho[i], getPattern(SEGSz::getHOSz() - i));
}

TEST_F(ObjIntegrationTest, SEGYReadHO)
{
    SCOPED_TRACE("SEGYReadHO");

    Obj::SEGY segy(piol, plargeFile, segyOpt, dataOpt);
    piol->isErr();
    SEGYReadHOTest(segy, {uchar(magicNum1), uchar(magicNum1+1)});
}

void SEGYWriteHOPattern(Obj::Interface & obj)
{
    std::vector<uchar> ho(SEGSz::getHOSz());
    for (size_t i = 0U; i < SEGSz::getHOSz(); i++)
        ho[i] = getPattern(i);
    obj.writeHO(ho.data());
}

TEST_F(ObjIntegrationTest, SEGYWriteHO)
{
    SCOPED_TRACE("SEGYReadHO");
    std::string outFile = "tmp/testOutput.tmp";
    dataOpt.mode = MPI_MODE_UNIQUE_OPEN | MPI_MODE_CREATE | MPI_MODE_RDWR | MPI_MODE_DELETE_ON_CLOSE;
    Obj::SEGY segy(piol, outFile, segyOpt, dataOpt);

    SEGYWriteHOPattern(segy);
    SEGYReadHOTest(segy, {uchar(magicNum1), uchar(magicNum1+1)});
}

//////////////////////////// Trace Header Reading //////////////////////////////////
void SEGYReadTrTest(size_t offset, const std::vector<size_t> & nsVals, const uchar magicNum, Obj::Interface * obj)
{
    const size_t extra = 1111U;
    std::vector<uchar> tr(extra + SEGSz::getMDSz() + extra);

    ASSERT_TRUE(nsVals.size() > 0);
    for (size_t j = 0U; j < nsVals.size(); j++)
    {
        size_t foff = SEGSz::getDOLoc(offset, nsVals[j]);
        for (size_t i = 0U; i < extra; i++)
            tr[i] = tr[tr.size() - extra + i] = magicNum;

        obj->readDOMD(offset, nsVals[j], 1U, tr.data() + extra);

        //Overrun checks
        for (size_t i = 0U; i < extra; i++)
        {
            ASSERT_EQ(magicNum, tr[i]);
            ASSERT_EQ(magicNum, tr[tr.size() - extra + i]);
        }
        for (size_t i = 0U; i < SEGSz::getMDSz(); i++)
            ASSERT_EQ(getPattern(foff + i), tr[i+extra]);
    }
}

TEST_F(ObjIntegrationTest, SEGYReadTrHdr)
{
    SCOPED_TRACE("SEGYReadTrHdr");
    Obj::SEGY segy(piol, plargeFile, segyOpt, dataOpt);
    piol->isErr();
    SCOPED_TRACE("Read1");
    SEGYReadTrTest(10U, {0U, 1U, 100U, 1000U, 1000U}, magicNum1, &segy);
    SCOPED_TRACE("Read2");
    SEGYReadTrTest(100U, {0U, 1U, 100U, 1000U, 1000U}, magicNum1+1U, &segy);
}

TEST_F(ObjIntegrationTest, SEGYReadTrHdrs)
{
    const size_t sz = 400;
    const size_t ns = 261;
    //const size_t sz = 20000;
    //const size_t ns = 1000;
    SCOPED_TRACE("SEGYReadTrHdr");
    std::unique_ptr<Obj::Interface> obj(std::move(new Obj::SEGY(piol, smallSEGYFile, segyOpt, dataOpt)));
    //std::unique_ptr<Obj::Interface> obj(std::move(new Obj::SEGY(piol, largeSEGYFile, segyOpt, dataOpt)));
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

void SEGYWriteTrTest(size_t offset, size_t ns, Obj::Interface * obj)
{
    std::vector<uchar> tr(SEGSz::getMDSz());
    size_t foff = SEGSz::getDOLoc(offset, ns);
    for (size_t i = 0U; i < tr.size(); i++)
        tr[i] = getPattern(foff + i);

    obj->writeDOMD(offset, ns, 1U, tr.data());
}

TEST_F(ObjIntegrationTest, SEGYWriteTrHdr)
{
    SCOPED_TRACE("SEGYReadHO");
    std::string outFile = "tmp/testOutput.tmp";
    dataOpt.mode = MPI_MODE_UNIQUE_OPEN | MPI_MODE_CREATE | MPI_MODE_RDWR | MPI_MODE_DELETE_ON_CLOSE;
    Obj::SEGY segy(piol, outFile, segyOpt, dataOpt);

    SEGYWriteTrTest(40U, 0U, &segy);
    SEGYReadTrTest(40U, {0U}, magicNum1, &segy);

    SEGYWriteTrTest(80U, 100U, &segy);
    SEGYReadTrTest(80U, {100U}, magicNum1, &segy);

    SEGYWriteTrTest(400U, 1000U, &segy);
    SEGYReadTrTest(400U, {1000U}, magicNum1, &segy);
}

////////////////////////////// File Size Testing ///////////////////////////////////
TEST_F(ObjIntegrationTest, zeroSEGYFileSize)
{
    Obj::SEGY segy(piol, zeroFile, segyOpt, dataOpt);
    piol->isErr();
    EXPECT_NE(nullptr, segy.data);
    EXPECT_EQ(0U, segy.getFileSz());
}

TEST_F(ObjIntegrationTest, SmallSEGYFileSize)
{
    Obj::SEGY segy(piol, smallFile, segyOpt, dataOpt);
    piol->isErr();
    EXPECT_NE(nullptr, segy.data);
    EXPECT_EQ(smallSize, segy.getFileSz());
}

TEST_F(ObjIntegrationTest, BigSEGYFileSize)
{
    Obj::SEGY segy(piol, largeFile, segyOpt, dataOpt);
    piol->isErr();
    EXPECT_NE(nullptr, segy.data);
    EXPECT_EQ(largeSize, segy.getFileSz());
}

