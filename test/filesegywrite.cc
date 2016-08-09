#include "filesegytest.hh"

class FileSEGYWriteSpecTest : public FileIntegrationTest
{
    public :
    size_t nt = 40U;
    csize_t ns = 200U;
    const int inc = 10;
    csize_t format = 5;
    std::shared_ptr<MockObj> mock;
    File::Interface * file;
    std::vector<uchar> ho;

    FileSEGYWriteSpecTest()
    {
        ho.resize(SEGSz::getHOSz());
    }

    void makeSEGY()
    {
        mock = std::make_shared<MockObj>(piol, notFile, nullptr);
        Mock::AllowLeak(mock.get());

        size_t fsz = SEGSz::getHOSz() + nt*SEGSz::getDOSz(ns);
        EXPECT_CALL(*mock, getFileSz()).Times(Exactly(1)).WillOnce(Return(0U));
        EXPECT_CALL(*mock, setFileSz(fsz)).Times(Exactly(1));

        file = new File::SEGY(piol, notFile, fileSegyOpt, mock);
        piol->isErr();
    }

    void writeHO()
    {
        for (size_t i = 0U; i < std::min(testString.size(), SEGSz::getTextSz()); i++)
            ho[i] = testString[i];
        ho[NumSample+1] = ns;
        ho[Increment+1] = inc;
        ho[Type+1] = format;
        ho[3255U] = 1;
        ho[3500U] = 1;
        ho[3503U] = 1;
        ho[3505U] = 0;

        EXPECT_CALL(*mock, writeHO(_)).Times(Exactly(1)).WillOnce(extraCheck(ho.data()));
        file->writeNt(nt);
        piol->isErr();

        file->writeNs(ns);
        piol->isErr();

        file->writeInc(geom_t(inc*SI::Micro));
        piol->isErr();

        file->writeText(testString);
        piol->isErr();
    }

    void writeTrHdrGridTest(size_t offset)
    {
        std::vector<uchar> tr(SEGSz::getMDSz());
        getBigEndian(ilNum(offset), tr.data()+il);
        getBigEndian(xlNum(offset), tr.data()+xl);
        getBigEndian<int16_t>(1, &tr[ScaleCoord]);

        EXPECT_CALL(*mock, writeDOMD(offset, ns, 1U, _)).Times(Exactly(1)).WillOnce(extraTrCheck(tr.data()));

        TraceParam prm;
        prm.line = {ilNum(offset), xlNum(offset)};
        file->writeTraceParam(offset, 1U, &prm);
    }

    void initWriteTrHdrCoord(std::pair<size_t, size_t> item, std::pair<int32_t, int32_t> val, int16_t scal,
                                       size_t offset, std::vector<uchar> * tr)
    {
        getBigEndian(scal, tr->data()+70U);
        getBigEndian(val.first, tr->data()+item.first);
        getBigEndian(val.second, tr->data()+item.second);
        EXPECT_CALL(*mock, writeDOMD(offset, ns, 1U, _)).Times(Exactly(1)).WillOnce(extraTrCheck(tr->data()));
    }
};

TEST_F(FileSEGYWriteSpecTest, FileWriteHO)
{
    makeSEGY();
    writeHO();
}

TEST_F(FileSEGYWriteSpecTest, FileWriteHOLongString)
{
    //Extend the string beyond the text boundary
    //Extended text should be dropped in write call
    csize_t sz = testString.size();
    csize_t extendSz = 3400U - sz;
    testString.resize(sz + extendSz);
    for (size_t i = 3200U; i < sz+extendSz; i++)
        testString[i] = uchar(0x7F);

    makeSEGY();
    writeHO();
}

TEST_F(FileSEGYWriteSpecTest, FileWriteHOEmptyString)
{
    testString.resize(0);
    makeSEGY();
    writeHO();
}

TEST_F(FileSEGYWriteSpecTest, FileWriteTrHdrGrid)
{
    makeSEGY();
    writeHO();
    for (size_t i = 0; i < nt; i++)
        writeTrHdrGridTest(i);
}

TEST_F(FileSEGYWriteSpecTest, FileWriteTrHdrCoord1)
{
    makeSEGY();
    writeHO();
    std::vector<uchar> tr(SEGSz::getMDSz());
    initWriteTrHdrCoord({xCMP, yCMP}, {160010, 240022}, -100, 10, &tr);

    TraceParam prm;
    prm.cmp = {1600.1, 2400.22};
    file->writeTraceParam(10U, 1U, &prm);
}

TEST_F(FileSEGYWriteSpecTest, FileWriteTrHdrCoord2)
{
    makeSEGY();
    writeHO();
    std::vector<uchar> tr(SEGSz::getMDSz());
    initWriteTrHdrCoord({xSrc, ySrc}, {1600100, 3400222}, -1000, 10, &tr);

    TraceParam prm;
    prm.src = {1600.1000, 3400.2220};
    file->writeTraceParam(10U, 1U, &prm);
}

TEST_F(FileSEGYWriteSpecTest, FileWriteTrHdrCoord3)
{
    makeSEGY();
    writeHO();
    std::vector<uchar> tr(SEGSz::getMDSz());
    initWriteTrHdrCoord({xSrc, ySrc}, {1623001001,   34002220}, -10000, 10, &tr);

    TraceParam prm;
    prm.src = {162300.10009, 3400.22201};
    file->writeTraceParam(10U, 1U, &prm);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// DEATH TESTS ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef FileSEGYWriteSpecTest FileSEGYDeathTest;

TEST_F(FileSEGYDeathTest, FileWriteAPIBadns)
{
    makeSEGY();

    size_t badns = 0x470000;
    file->writeNs(badns);

    mock.reset();
    EXPECT_EXIT(piol->isErr(), ExitedWithCode(EXIT_FAILURE), ".*8 3 Fatal Error in PIOL. . Dumping Log 0");
}

#ifdef NT_LIMITS
TEST_F(FileSEGYDeathTest, FileWriteAPIBadnt)
{
    makeSEGY();

    size_t badnt = NT_LIMITS + 1;
    file->writeNt(badnt);

    mock.reset();
    EXPECT_EXIT(piol->isErr(), ExitedWithCode(EXIT_FAILURE), ".*8 3 Fatal Error in PIOL. . Dumping Log 0");
}
#endif

#ifdef NT_LIMITS
TEST_F(FileSEGYDeathTest, FileWriteAPIBadinc)
{
    makeSEGY();

    geom_t badinc = geom_t(1)/geom_t(0);
    file->writeInc(badinc);

    mock.reset();
    EXPECT_EXIT(piol->isErr(), ExitedWithCode(EXIT_FAILURE), ".*8 3 Fatal Error in PIOL. . Dumping Log 0");
}
#endif
