#include "filesegytest.hh"

class FileSEGYReadTest : public FileIntegrationTest
{
    public :
    std::shared_ptr<MockObj> mock;
    File::Interface * file;
    std::vector<uchar> tr;
    csize_t nt = 40U;
    csize_t ns = 200U;
    const int inc = 10;
    csize_t format = 1;
    std::vector<uchar> ho;

    FileSEGYReadTest()
    {
    }
    ~FileSEGYReadTest()
    {
        if (file != nullptr)
            delete file;
    }
    void makeMock(bool testEBCDIC)
    {
        ho.resize(SEGSz::getHOSz());
        mock = std::make_shared<MockObj>(piol, notFile, nullptr);
        Mock::AllowLeak(mock.get());
        initReadHOMock(testEBCDIC);
    }
    void testHO(bool testEBCDIC)
    {
        makeMock(testEBCDIC);
        file = new File::SEGY(piol, notFile, fileSegyOpt, mock);
        piol->isErr();
    }
    void testTr()
    {
        makeMock(false);
        file = new File::SEGY(piol, notFile, fileSegyOpt, mock);
        piol->isErr();
        initTrBlock();
    }
    void initReadHOMock(bool testEBCDIC)
    {
        EXPECT_CALL(*mock, getFileSz()).Times(Exactly(1)).WillOnce(Return(SEGSz::getHOSz() + nt*SEGSz::getDOSz(ns)));
        if (testEBCDIC)
        {
            // Create an EBCDID string to convert back to ASCII in the test
            size_t tsz = testString.size();
            size_t tsz2 = tsz;
            char * t = &testString[0];
            char * newText = reinterpret_cast<char *>(ho.data());
            iconv_t toAsc = iconv_open("EBCDICUS//", "ASCII//");
            ::iconv(toAsc, &t, &tsz, &newText, &tsz2);
            iconv_close(toAsc);
        }
        else
        {
            for (size_t i = 0; i < testString.size(); i++)
                ho[i] = testString[i];
        }
        for (size_t i = testString.size(); i < SEGSz::getTextSz(); i++)
            ho[i] = ho[i % testString.size()];

        ho[NumSample] = (ns >> 8) & 0xFF;
        ho[NumSample+1] = ns & 0xFF;
        ho[Increment+1] = inc;
        ho[Type+1] = format;
        EXPECT_CALL(*mock, readHO(_)).Times(Exactly(1)).WillOnce(SetArrayArgument<0>(ho.begin(), ho.end()));
    }

    void initTrBlock()
    {
        tr.resize(nt * SEGSz::getMDSz());
        for (size_t i = 0; i < nt; i++)
        {
            uchar * md = &tr[i * SEGSz::getMDSz()];
            getBigEndian(ilNum(i), &md[il]);
            getBigEndian(xlNum(i), &md[xl]);

            int16_t scale;
            int16_t scal1 = deScale(xNum(i));
            int16_t scal2 = deScale(yNum(i));

            if (scal1 > 1 || scal2 > 1)
                scale = std::max(scal1, scal2);
            else
                scale = std::min(scal1, scal2);

            getBigEndian(scale, &md[ScaleCoord]);
            getBigEndian(int32_t(std::lround(xNum(i)/scale)), &md[xSrc]);
            getBigEndian(int32_t(std::lround(yNum(i)/scale)), &md[ySrc]);
        }
    }

    void initReadTrMock(size_t ns, size_t offset)
    {
        std::vector<uchar>::iterator iter = tr.begin() + offset*SEGSz::getMDSz();
        EXPECT_CALL(*mock.get(), readDOMD(offset, ns, 1U, _))
                    .Times(Exactly(2))
                    .WillRepeatedly(SetArrayArgument<3>(iter, iter + SEGSz::getMDSz()));

        grid_t line;
        file->readGridPoint(File::Grid::Line, offset, 1U, &line);
        ASSERT_EQ(ilNum(offset), line.first);
        ASSERT_EQ(xlNum(offset), line.second);

        coord_t src;
        file->readCoordPoint(File::Coord::Src, offset, 1U, &src);
        ASSERT_DOUBLE_EQ(xNum(offset), src.first);
        ASSERT_DOUBLE_EQ(yNum(offset), src.second);
    }

    void initReadTrHdrsMock(size_t ns, size_t nt)
    {
        EXPECT_CALL(*mock.get(), readDOMD(0, ns, nt, _))
                    .Times(Exactly(2))
                    .WillRepeatedly(SetArrayArgument<3>(tr.begin(), tr.end()));

        std::vector<File::grid_t> line(nt);
        file->readGridPoint(File::Grid::Line, 0U, nt, line.data());

        std::vector<File::coord_t> src(nt);
        file->readCoordPoint(File::Coord::Src, 0U, nt, src.data());

        for (size_t i = 0; i < nt; i++)
        {
            ASSERT_EQ(ilNum(i), line[i].first);
            ASSERT_EQ(xlNum(i), line[i].second);

            ASSERT_DOUBLE_EQ(xNum(i), src[i].first);
            ASSERT_DOUBLE_EQ(yNum(i), src[i].second);
        }
    }
};

TEST_F(FileSEGYReadTest, FileConstructor)
{
    testHO(false);
    EXPECT_EQ(piol, file->piol);
    EXPECT_EQ(notFile, file->name);
    EXPECT_EQ(mock, file->obj);
    ASSERT_TRUE(ns < 0x10000);
}

TEST_F(FileSEGYReadTest, FileReadHO)
{
    testHO(true);

    EXPECT_EQ(nt, file->readNt());
    piol->isErr();

    EXPECT_EQ(ns, file->readNs());
    piol->isErr();

    EXPECT_EQ(geom_t(inc*SI::Micro), file->readInc());
    piol->isErr();

//    EXPECT_EQ(format, static_cast<size_t>(file->format));
//    piol->isErr();

    std::string text = file->readText();
    EXPECT_EQ(SEGSz::getTextSz(), text.size());

//EBCDIC conversion check
    size_t slen = testString.size();
    for (size_t i = 0; i < text.size(); i++)
        ASSERT_EQ(testString[i % slen], text[i]) << "Loop number " << i << std::endl;
}

TEST_F(FileSEGYReadTest, FileReadHOAPI)
{
    testHO(false);

    std::string text = file->readText();
    EXPECT_EQ(3200U, text.size());
    EXPECT_EQ(SEGSz::getTextSz(), text.size());
    size_t slen = testString.size();
    for (size_t i = 0; i < text.size(); i++)
        ASSERT_EQ(testString[i % slen], text[i]) << "Loop number " << i << std::endl;
}

///////////////TRACE COORDINATES + GRIDS///////////////////////////////

TEST_F(FileSEGYReadTest, FileReadTraceHeader)
{
    testTr();
    for (size_t i = 0; i < nt; i++)
        initReadTrMock(ns, i);
}

TEST_F(FileSEGYReadTest, FileReadTrHdrBigNs)
{
    testTr();
    const size_t bigns = 10000;
    file->ns = bigns;
    initReadTrMock(bigns, nt/2U);
}

TEST_F(FileSEGYReadTest, FileReadFileTrs)
{
    testTr();
    initReadTrHdrsMock(ns, nt);
}


