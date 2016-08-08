#include <iconv.h>
#include <string.h>
#include <memory>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "tglobal.hh"
#include "anc/cmpi.hh"
#include "data/datampiio.hh"
#include "object/objsegy.hh"
#include "share/units.hh"
#include "share/segy.hh"
#include "share/datatype.hh"
#define private public
#define protected public
#include "file/filesegy.hh"
#undef private
#undef protected

namespace PIOL { namespace File {
extern int16_t deScale(const geom_t val);
}}
using PIOL::File::deScale;

using namespace testing;
using namespace PIOL;

class MockObj : public Obj::Interface
{
    public :
    MockObj(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, std::shared_ptr<Data::Interface> data_)
               : Obj::Interface(piol_, name_, data_) {}
    MOCK_CONST_METHOD0(getFileSz, size_t(void));
    MOCK_CONST_METHOD1(readHO, void(uchar *));
    MOCK_CONST_METHOD1(setFileSz, void(csize_t));
    MOCK_CONST_METHOD1(writeHO, void(const uchar *));
    MOCK_CONST_METHOD3(readDOMD, void(csize_t, csize_t, uchar *));
    MOCK_CONST_METHOD4(readDOMD, void(csize_t, csize_t, csize_t, uchar *));
    MOCK_CONST_METHOD3(writeDOMD, void(csize_t, csize_t, const uchar *));
};

class FileIntegrationTest : public Test
{
    protected :
    std::shared_ptr<ExSeisPIOL> piol;
    const File::SEGYOpt fileSegyOpt;
    const Obj::SEGYOpt objSegyOpt;
    Data::MPIIOOpt dataOpt;
    Comm::MPIOpt opt;
    std::string testString = {"This is a string for testing EBCDIC conversion etc."};
    FileIntegrationTest()
    {
        opt.initMPI = false;
        piol = std::make_shared<ExSeisPIOL>(opt);
    }
};

////////////////////////////////////////////////////////////////////////////////////
/////////////////////// ISOLATED-CLASS SPECIFICATION TESTING ///////////////////////
////////////////////////////////////////////////////////////////////////////////////
class FileSEGYSpecTest : public FileIntegrationTest
{
    protected :
    csize_t nt = 40U;
    csize_t ns = 200U;
    const int inc = 10;
    csize_t format = 1;
    std::vector<uchar> hor;
    std::vector<uchar> how;
    FileSEGYSpecTest() : FileIntegrationTest()
    {
        hor.resize(SEGSz::getHOSz());
        how.resize(SEGSz::getHOSz());
    }
    ~FileSEGYSpecTest()
    {
    }
};

void initReadHOMock(MockObj & mock, std::vector<uchar> & ho, size_t nt, size_t ns, int inc, size_t format, std::string testString, bool testEBCDIC)
{
    EXPECT_CALL(mock, getFileSz()).Times(Exactly(1)).WillOnce(Return(SEGSz::getHOSz() + nt*SEGSz::getDOSz(ns)));
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
    ho[3220U] = (ns >> 8) & 0xFF;
    ho[3221U] = ns & 0xFF;
    ho[3217U] = inc;
    ho[3225U] = format;
    EXPECT_CALL(mock, readHO(_)).Times(Exactly(1)).WillOnce(SetArrayArgument<0>(ho.begin(), ho.end()));
}

void CompileCheck(File::Interface & file)
{
    size_t nt = file.readNt();
    size_t ns = file.readNs();
    std::string text = file.readText();
    geom_t inc = file.readInc();

    file.writeNt(nt);
    file.writeNs(ns);
    file.writeText(text);
    file.writeInc(inc);
}

TEST_F(FileSEGYSpecTest, FileConstructor)
{
    auto mock = std::make_shared<MockObj>(piol, notFile, nullptr);
    initReadHOMock(*mock.get(), how, nt, ns, inc, 5, testString, false);

    File::SEGY segy(piol, notFile, fileSegyOpt, mock);
    EXPECT_EQ(piol, segy.piol);
    EXPECT_EQ(notFile, segy.name);
    EXPECT_EQ(mock, segy.obj);
}

TEST_F(FileSEGYSpecTest, FileReadHO)
{
    auto mock = std::make_shared<MockObj>(piol, notFile, nullptr);
    initReadHOMock(*mock.get(), hor, nt, ns, inc, format, testString, true);
    ASSERT_TRUE(ns < 0x10000);
    File::SEGY segy(piol, notFile, fileSegyOpt, mock);

    piol->isErr();

    EXPECT_EQ(piol, segy.piol);
    EXPECT_EQ(notFile, segy.name);
    EXPECT_EQ(mock, segy.obj);
    EXPECT_EQ(nt, segy.readNt());
    piol->isErr();

    EXPECT_EQ(ns, segy.readNs());
    piol->isErr();

    EXPECT_EQ(geom_t(inc*SI::Micro), segy.readInc());
    piol->isErr();

    EXPECT_EQ(format, static_cast<size_t>(segy.format));
    piol->isErr();

    std::string text = segy.readText();
    EXPECT_EQ(3200U, text.size());
    EXPECT_EQ(SEGSz::getTextSz(), text.size());
//EBCDIC conversion check
    size_t slen = testString.size();
    for (size_t i = 0; i < text.size(); i++)
        ASSERT_EQ(testString[i % slen], text[i]) << "Loop number " << i << std::endl;
}

TEST_F(FileSEGYSpecTest, FileReadHOAPI)
{
    auto mock = std::make_shared<MockObj>(piol, notFile, nullptr);
    initReadHOMock(*mock.get(), hor, nt, ns, inc, format, testString, false);
    ASSERT_TRUE(ns < 0x10000);

    File::SEGY segy(piol, notFile, fileSegyOpt, mock);

    std::string text = segy.readText();
    EXPECT_EQ(3200U, text.size());
    EXPECT_EQ(SEGSz::getTextSz(), text.size());
    size_t slen = testString.size();
    for (size_t i = 0; i < text.size(); i++)
        ASSERT_EQ(testString[i % slen], text[i]) << "Loop number " << i << std::endl;
}

ACTION_P(extraCheck, ho)
{
    for (size_t i = 0; i < SEGSz::getHOSz(); i++)
        ASSERT_EQ(ho[i], arg0[i]) << "Error with header byte: " << i << " |\n";
}

void initWriteHOMock(MockObj & mock, std::vector<uchar> & ho, size_t nt, size_t ns, int inc, int format, std::string testString)
{
    size_t fsz = SEGSz::getHOSz() + nt*SEGSz::getDOSz(ns);
    EXPECT_CALL(mock, getFileSz()).Times(Exactly(1)).WillOnce(Return(0U));
    EXPECT_CALL(mock, setFileSz(fsz)).Times(Exactly(1));

    for (size_t i = 0U; i < std::min(testString.size(), SEGSz::getTextSz()); i++)
        ho[i] = testString[i];

    ho[3221U] = ns;
    ho[3217U] = inc;
    ho[3225U] = format;
    ho[3255U] = 1;
    ho[3500U] = 1;
    ho[3503U] = 1;
    ho[3505U] = 0;
    EXPECT_CALL(mock, writeHO(_)).Times(Exactly(1)).WillOnce(extraCheck(ho.data()));
}

void FileWriteHO(csize_t nt, csize_t ns, const geom_t inc, std::string text, Piol piol, File::Interface * file)
{
    piol->isErr();

    file->writeNt(nt);
    piol->isErr();

    file->writeNs(ns);
    piol->isErr();

    file->writeInc(inc);
    piol->isErr();

    file->writeText(text);
    piol->isErr();
}

TEST_F(FileSEGYSpecTest, FileWriteHO)
{
    auto mock = std::make_shared<MockObj>(piol, notFile, nullptr);
    initWriteHOMock(*mock.get(), how, nt, ns, inc, 5, testString);

    File::SEGY segy(piol, notFile, fileSegyOpt, mock);
    FileWriteHO(nt, ns, geom_t(inc*SI::Micro), testString, piol, &segy);
}

TEST_F(FileSEGYSpecTest, FileWriteHOLongString)
{
    auto mock = std::make_shared<MockObj>(piol, notFile, nullptr);
    initWriteHOMock(*mock.get(), how, nt, ns, inc, 5, testString);

    //Extend the string beyond the text boundary
    //Extended text should be dropped in write call
    csize_t sz = testString.size();
    csize_t extendSz = 3400U - sz;
    testString.resize(sz + extendSz);
    for (size_t i = 3200U; i < sz+extendSz; i++)
        testString[i] = uchar(0x7F);

    File::SEGY segy(piol, notFile, fileSegyOpt, mock);
    FileWriteHO(nt, ns, geom_t(inc*SI::Micro), testString, piol, &segy);
}

TEST_F(FileSEGYSpecTest, FileWriteHOEmptyString)
{
    testString.resize(0);
    auto mock = std::make_shared<MockObj>(piol, notFile, nullptr);
    initWriteHOMock(*mock.get(), how, nt, ns, inc, 5, testString);

    File::SEGY segy(piol, notFile, fileSegyOpt, mock);
    FileWriteHO(nt, ns, geom_t(inc*SI::Micro), testString, piol, &segy);
}

///////////////TRACE COORDINATES + GRIDS///////////////////////////////
ACTION_P(extraTrCheck, ho)  //Use this when writing
{
    for (size_t i = 0; i < SEGSz::getMDSz(); i++)
        ASSERT_EQ(ho[i], arg2[i]) << "Error with trace header byte: " << i << " |\n";
}

class FileSEGYReadSpecTest : public FileSEGYSpecTest
{
    public :
    std::shared_ptr<MockObj> mock;
    File::Interface * file;
    std::vector<uchar> tr;

    FileSEGYReadSpecTest()
    {
        mock = std::make_shared<MockObj>(piol, notFile, nullptr);
        Mock::AllowLeak(mock.get());

        initReadHOMock(*mock.get(), how, nt, ns, inc, 5, testString, false);
        file = new File::SEGY(piol, notFile, fileSegyOpt, mock);
        initTrBlock(nt);
    }

    void initTrBlock(size_t nt)
    {
        tr.resize(nt * SEGSz::getMDSz());
        for (size_t i = 0; i < nt; i++)
        {
            uchar * md = &tr[i * SEGSz::getMDSz()];
            getBigEndian(ilNum(i), &md[188U]);
            getBigEndian(xlNum(i), &md[192U]);

            int16_t scale;
            int16_t scal1 = deScale(xNum(i));
            int16_t scal2 = deScale(yNum(i));

            if (scal1 > 1 || scal2 > 1)
                scale = std::max(scal1, scal2);
            else
                scale = std::min(scal1, scal2);

            getBigEndian(scale, &md[70U]);
            getBigEndian(int32_t(std::lround(xNum(i)/scale)), &md[72U]);
            getBigEndian(int32_t(std::lround(yNum(i)/scale)), &md[76U]);
        }
    }

    void initReadTrMock(size_t ns, size_t offset)
    {
        std::vector<uchar>::iterator iter = tr.begin() + offset*SEGSz::getMDSz();
        EXPECT_CALL(*mock.get(), readDOMD(offset, ns, _))
                    .Times(Exactly(2))
                    .WillRepeatedly(SetArrayArgument<2>(iter, iter + SEGSz::getMDSz()));

        auto line = file->readGridPoint(File::Grid::Line, offset);
        ASSERT_EQ(ilNum(offset), line.first);
        ASSERT_EQ(xlNum(offset), line.second);

        auto src = file->readCoordPoint(File::Coord::Src, offset);
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

TEST_F(FileSEGYReadSpecTest, FileReadTraceHeader)
{
    for (size_t i = 0; i < nt; i++)
        initReadTrMock(ns, i);
}

TEST_F(FileSEGYReadSpecTest, FileReadTrHdrBigNs)
{
    const size_t bigns = 10000;
    file->ns = bigns;
    initReadTrMock(bigns, nt/2U);
}

TEST_F(FileSEGYReadSpecTest, FileReadFileTrs)
{
    initReadTrHdrsMock(ns, nt);
}

class FileSEGYWriteSpecTest : public FileSEGYSpecTest
{
    public :
    std::shared_ptr<MockObj> mock;
    File::Interface * segy;

    FileSEGYWriteSpecTest()
    {
        mock = std::make_shared<MockObj>(piol, notFile, nullptr);
        initWriteHOMock(*mock.get(), how, nt, ns, inc, 5, testString);
        Mock::AllowLeak(mock.get());
        segy = new File::SEGY(piol, notFile, fileSegyOpt, mock);
        FileWriteHO(nt, ns, geom_t(inc*SI::Micro), testString, piol, segy);    //I need to see ns and nt
    }

    void initWriteTrHdrGrid(MockObj & mock, size_t ns, size_t offset, File::Interface * file)
    {
        std::vector<uchar> tr(SEGSz::getMDSz());
        getBigEndian(ilNum(offset), tr.data()+188U);
        getBigEndian(xlNum(offset), tr.data()+192U);
        EXPECT_CALL(mock, writeDOMD(offset, ns, _)).Times(Exactly(1)).WillOnce(extraTrCheck(tr.data()));
        file->writeGridPoint(File::Grid::Line, offset, {ilNum(offset), xlNum(offset)});
    }

    void initWriteTrHdrCoord(MockObj & mock, std::pair<size_t, size_t> item, std::pair<int32_t, int32_t> val, int16_t scal,
                                                                         size_t ns, size_t offset, std::vector<uchar> * tr)
    {
        getBigEndian(scal, tr->data()+70U);
        getBigEndian(val.first, tr->data()+item.first);
        getBigEndian(val.second, tr->data()+item.second);
        EXPECT_CALL(mock, writeDOMD(offset, ns, _)).Times(Exactly(1)).WillOnce(extraTrCheck(tr->data()));
    }
};

TEST_F(FileSEGYWriteSpecTest, FileWriteTrHdrGrid)
{
    for (size_t i = 0; i < nt; i++)
        initWriteTrHdrGrid(*mock.get(), ns, i, segy);
}

TEST_F(FileSEGYWriteSpecTest, FileWriteTrHdrCoord1)
{
    std::vector<uchar> tr(SEGSz::getMDSz());
    initWriteTrHdrCoord(*mock.get(), {180U, 184U}, {160010, 240022}, -100, ns, 10, &tr);
    segy->writeCoordPoint(File::Coord::Cmp, 10,     {1600.1, 2400.22});
}

TEST_F(FileSEGYWriteSpecTest, FileWriteTrHdrCoord2)
{
    std::vector<uchar> tr(SEGSz::getMDSz());
    initWriteTrHdrCoord(*mock.get(), {72U, 76U}, {1600100,    3400222}, -1000, ns, 10, &tr);
    segy->writeCoordPoint(File::Coord::Src, 10,   {1600.1000, 3400.2220});
}

TEST_F(FileSEGYWriteSpecTest, FileWriteTrHdrCoord3)
{
    std::vector<uchar> tr(SEGSz::getMDSz());
    initWriteTrHdrCoord(*mock.get(), {72U, 76U}, {1623001001,   34002220}, -10000, ns, 10, &tr);
    segy->writeCoordPoint(File::Coord::Src, 10,   {162300.10009, 3400.22201});
}


///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// DEATH TESTS ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef FileSEGYSpecTest FileSEGYDeathTest;

TEST_F(FileSEGYDeathTest, FileWriteAPIBadns)
{
    size_t badns = 0x470000;
    auto mock = std::make_shared<NiceMock<MockObj>>(piol, notFile, nullptr);
    ASSERT_TRUE(mock);
    File::SEGY segy(piol, notFile, fileSegyOpt, mock);
    piol->isErr();

    EXPECT_EQ(piol, segy.piol);
    EXPECT_EQ(notFile, segy.name);
    EXPECT_EQ(mock, segy.obj);

    segy.writeNt(nt);
    piol->isErr();

    segy.writeInc(geom_t(inc*SI::Micro));
    piol->isErr();

    segy.writeText(testString);
    piol->isErr();

    segy.writeNs(badns);

    mock.reset();
    EXPECT_EXIT(piol->isErr(), ExitedWithCode(EXIT_FAILURE), ".*8 3 Fatal Error in PIOL. . Dumping Log 0");
}

#ifdef NT_LIMITS
TEST_F(FileSEGYDeathTest, FileWriteAPIBadnt)
{
    auto mock = std::make_shared<NiceMock<MockObj>>(piol, notFile, nullptr);
    size_t badnt = NT_LIMITS + 1;
    File::SEGY segy(piol, notFile, fileSegyOpt, mock);
    piol->isErr();

    EXPECT_EQ(piol, segy.piol);
    EXPECT_EQ(notFile, segy.name);
    EXPECT_EQ(mock, segy.obj);

    segy.writeInc(geom_t(inc*SI::Micro));
    piol->isErr();

    segy.writeText(testString);
    piol->isErr();

    segy.writeNs(ns);
    piol->isErr();

    segy.writeNt(badnt);

    mock.reset();
    EXPECT_EXIT(piol->isErr(), ExitedWithCode(EXIT_FAILURE), ".*8 3 Fatal Error in PIOL. . Dumping Log 0");
}
#endif

TEST_F(FileSEGYDeathTest, FileWriteBadInc)
{
    auto mock = std::make_shared<NiceMock<MockObj>>(piol, notFile, nullptr);
    geom_t badinc = geom_t(1)/geom_t(0);
    File::SEGY segy(piol, notFile, fileSegyOpt, mock);
    piol->isErr();

    EXPECT_EQ(piol, segy.piol);
    EXPECT_EQ(notFile, segy.name);
    EXPECT_EQ(mock, segy.obj);

    segy.writeNt(nt);
    piol->isErr();

    segy.writeNs(ns);
    piol->isErr();

    segy.writeText(testString);
    piol->isErr();

    segy.writeInc(badinc);

    mock.reset();
    EXPECT_EXIT(piol->isErr(), ExitedWithCode(EXIT_FAILURE), ".*8 3 Fatal Error in PIOL. . Dumping Log 0");
}
//TODO: Test Null text
//TODO:: Test Text too long

////////////////////////////////////////////////////////////////////////////////////
///////////////////// INTEGRATION-CLASS SPECIFICATION TESTING //////////////////////
////////////////////////////////////////////////////////////////////////////////////
//Read test of File::SEGY -> Obj::SEGY -> Data::MPIIO
TEST_F(FileIntegrationTest, SEGYReadHO)
{
    csize_t ns = 261U;
    csize_t nt = 400U;
    std::string smallSEGY = "tmp/smallsegy.tmp";
    File::SEGY segy(piol, smallSEGY, fileSegyOpt, objSegyOpt, dataOpt);
    piol->isErr();
    EXPECT_EQ(ns, segy.readNs());
    piol->isErr();
    EXPECT_EQ(nt, segy.readNt());
    piol->isErr();
    EXPECT_EQ(File::SEGY::Format::IBM, segy.format);
    EXPECT_DOUBLE_EQ(double(20e-6), segy.readInc());
}

//TODO: Add test same as above for big files

//Write test of File::SEGY -> Obj::SEGY -> Data::MPIIO
TEST_F(FileIntegrationTest, SEGYWriteHO)
{
    SCOPED_TRACE("SEGYWriteHO");
    csize_t ns = 261U;
    csize_t nt = 400U;
    std::string outFile = "tmp/testOutput.tmp";

    std::shared_ptr<Obj::Interface> obj;
    dataOpt.mode = MPI_MODE_UNIQUE_OPEN | MPI_MODE_CREATE | MPI_MODE_RDWR | MPI_MODE_DELETE_ON_CLOSE;
    {
        File::SEGY segy(piol, outFile, fileSegyOpt, objSegyOpt, dataOpt);
        piol->isErr();

        segy.writeNs(ns);
        piol->isErr();
        segy.writeNt(nt);
        piol->isErr();
        segy.writeText(testString);
        piol->isErr();

        obj = segy.obj;         //steal object layer for read
    }
    {
        File::SEGY segy(piol, outFile, fileSegyOpt, obj);

        EXPECT_EQ(ns, segy.readNs());
        piol->isErr();
        EXPECT_EQ(nt, segy.readNt());
        piol->isErr();

        std::string temp = segy.readText();
        piol->isErr();
        ASSERT_TRUE(testString.size() <= temp.size());
        for (size_t i = 0; i < testString.size(); i++)
            EXPECT_EQ(testString[i], temp[i]);
    }
}

//Write test of File::SEGY -> Obj::SEGY -> Data::MPIIO
TEST_F(FileIntegrationTest, SEGYWriteTraceGrid)
{
    SCOPED_TRACE("SEGYReadHO");
    csize_t ns = 261U;
    csize_t nt = 400U;
    std::string outFile = "tmp/testOutput.tmp";
    File::grid_t grid(ilNum(201), xlNum(201));

    std::shared_ptr<Obj::Interface> obj;
    dataOpt.mode = MPI_MODE_UNIQUE_OPEN | MPI_MODE_CREATE | MPI_MODE_RDWR | MPI_MODE_DELETE_ON_CLOSE;

    {
        File::SEGY segy(piol, outFile, fileSegyOpt, objSegyOpt, dataOpt);
        piol->isErr();
        segy.ns = ns;
        segy.writeNt(nt);
        piol->isErr();

        segy.writeGridPoint(File::Grid::Line, 201, grid);
        obj = segy.obj;         //steal object layer for read
    }
    {
        File::SEGY segy(piol, outFile, fileSegyOpt, obj);

        File::grid_t grd = segy.readGridPoint(File::Grid::Line, 201);
        EXPECT_EQ(grid, grd);
    }
}

//Write test of File::SEGY -> Obj::SEGY -> Data::MPIIO
TEST_F(FileIntegrationTest, SEGYWriteTraceCoord)
{
    SCOPED_TRACE("SEGYReadHO");
    csize_t ns = 261U;
    csize_t nt = 400U;
    std::string outFile = "tmp/testOutput.tmp";
    File::coord_t coord(1600, 2000);

    std::shared_ptr<Obj::Interface> obj;
    dataOpt.mode = MPI_MODE_UNIQUE_OPEN | MPI_MODE_CREATE | MPI_MODE_RDWR | MPI_MODE_DELETE_ON_CLOSE;

    {
        File::SEGY segy(piol, outFile, fileSegyOpt, objSegyOpt, dataOpt);
        piol->isErr();
        segy.ns = ns;
        segy.writeNt(nt);
        piol->isErr();

        segy.writeCoordPoint(File::Coord::Cmp, 200, coord);
        obj = segy.obj;         //steal object layer for read
    }
    {
        File::SEGY segy(piol, outFile, fileSegyOpt, obj);
        File::coord_t crd = segy.readCoordPoint(File::Coord::Cmp, 200);
        EXPECT_EQ(coord, crd);

    }
}


