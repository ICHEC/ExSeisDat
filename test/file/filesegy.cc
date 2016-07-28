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
    MOCK_METHOD0(getFileSz, size_t(void));
    MOCK_METHOD1(readHO, void(uchar *));
    MOCK_METHOD1(setFileSz, void(const size_t));
    MOCK_METHOD1(writeHO, void(const uchar *));
    MOCK_METHOD3(readDOMD, void(const size_t, const size_t, uchar *));
    MOCK_METHOD3(writeDOMD, void(const size_t, const size_t, const uchar *));
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
//////////////////////// Unit tests of non-class functions /////////////////////////
////////////////////////////////////////////////////////////////////////////////////
TEST(FileScale, BigIntegers)
{
    EXPECT_EQ(10000,  deScale(21474836470000.0));
    EXPECT_EQ(1000,   deScale(02147483647000.0));
    EXPECT_EQ(100,    deScale(00214748364700.0));
    EXPECT_EQ(10,     deScale(00021474836470.0));

    EXPECT_EQ(10000,  deScale(10000000000000.0));
    EXPECT_EQ(1000,   deScale(01000000000000.0));
    EXPECT_EQ(100,    deScale(00100000000000.0));
    EXPECT_EQ(10,     deScale(00010000000000.0));

    EXPECT_EQ(10000,  deScale(21474836470000.0000999));
    EXPECT_EQ(1000,   deScale(02147483647000.0000999));
    EXPECT_EQ(100,    deScale(00214748364700.0000999));
    EXPECT_EQ(10,     deScale(00021474836470.0000999));
}

TEST(FileScale, Decimals)
{
    EXPECT_EQ(-10000, deScale(214748.3647));
    EXPECT_EQ(-1000,  deScale(2147483.647));
    EXPECT_EQ(-100,   deScale(21474836.47));
    EXPECT_EQ(-10,    deScale(214748364.7));
    EXPECT_EQ(1,      deScale(2147483647.));

    EXPECT_EQ(-10000, deScale(1.0001));
    EXPECT_EQ(-1000,  deScale(1.001));
    EXPECT_EQ(-100,   deScale(1.01));
    EXPECT_EQ(-10,    deScale(1.1));
    EXPECT_EQ(1,      deScale(1.));

    EXPECT_EQ(-10000, deScale(0.0001));
    EXPECT_EQ(-1000,  deScale(0.001));
    EXPECT_EQ(-100,   deScale(0.01));
    EXPECT_EQ(-10,    deScale(0.1));
    EXPECT_EQ(1,      deScale(0.));

//Tests for case where round mode pushes sig figs over sizes we can handle
    EXPECT_EQ(-10000, deScale(214748.3647199));
    EXPECT_EQ(-10,    deScale(0214748364.7000999));
    EXPECT_EQ(-100,   deScale(0021474836.4700999));
    EXPECT_EQ(-1000,  deScale(0002147483.6470999));
    EXPECT_EQ(1,      deScale(2147483647.0000999));

}
////////////////////////////////////////////////////////////////////////////////////
/////////////////////// ISOLATED-CLASS SPECIFICATION TESTING ///////////////////////
////////////////////////////////////////////////////////////////////////////////////
class FileSEGYSpecTest : public FileIntegrationTest
{
    protected :
    const size_t nt = 40U;
    const size_t ns = 200U;
    const int inc = 10;
    const size_t format = 1;
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

void initReadHOMock(MockObj & mock, std::vector<uchar> & ho, size_t nt, size_t ns, int inc, size_t format, std::string testString)
{
    EXPECT_CALL(mock, getFileSz()).Times(Exactly(1)).WillOnce(Return(SEGSz::getHOSz() + nt*SEGSz::getDOSz(ns)));
    size_t tsz = testString.size();
    size_t tsz2 = tsz;
    char * t = &testString[0];
    char * newText = reinterpret_cast<char *>(ho.data());
    iconv_t toAsc = iconv_open("EBCDICUS//", "ASCII//");
    ::iconv(toAsc, &t, &tsz, &newText, &tsz2);
    iconv_close(toAsc);
    size_t szText = testString.size();
    for (size_t i = szText; i < SEGSz::getTextSz(); i++)
        ho[i] = ho[i % szText];
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
    initReadHOMock(*mock.get(), how, nt, ns, inc, 5, testString);

    File::SEGY segy(piol, notFile, fileSegyOpt, mock);
    EXPECT_EQ(piol, segy.piol);
    EXPECT_EQ(notFile, segy.name);
    EXPECT_EQ(mock, segy.obj);
}

TEST_F(FileSEGYSpecTest, FileReadAPI)
{
    auto mock = std::make_shared<MockObj>(piol, notFile, nullptr);
    initReadHOMock(*mock.get(), hor, nt, ns, inc, format, testString);
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

void FileWriteHO(const size_t nt, const size_t ns, const geom_t inc, std::string text, Piol piol, File::Interface * file)
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

TEST_F(FileSEGYSpecTest, FileWriteAPI)
{
    auto mock = std::make_shared<MockObj>(piol, notFile, nullptr);
    initWriteHOMock(*mock.get(), how, nt, ns, inc, 5, testString);

    File::SEGY segy(piol, notFile, fileSegyOpt, mock);
    FileWriteHO(nt, ns, geom_t(inc*SI::Micro), testString, piol, &segy);
}

TEST_F(FileSEGYSpecTest, FileWriteAPILongString)
{
    auto mock = std::make_shared<MockObj>(piol, notFile, nullptr);
    initWriteHOMock(*mock.get(), how, nt, ns, inc, 5, testString);

    //Extend the string beyond the text boundary
    //Extended text should be dropped in write call
    const size_t sz = testString.size();
    const size_t extendSz = 3400U - sz;
    testString.resize(sz + extendSz);
    for (size_t i = 3200U; i < sz+extendSz; i++)
        testString[i] = uchar(0xFF);

    File::SEGY segy(piol, notFile, fileSegyOpt, mock);
    FileWriteHO(nt, ns, geom_t(inc*SI::Micro), testString, piol, &segy);
}

TEST_F(FileSEGYSpecTest, FileWriteAPIEmptyString)
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
        ASSERT_EQ(ho[i], arg2[i]) << "Error with header byte: " << i << " |\n";
}

void initReadTraceMock(MockObj & mock, std::vector<uchar> & tr, size_t offset, size_t ns, File::Interface * file)
{
    File::grid_t line = File::grid_t(ilNum(offset), xlNum(offset));
    getBigEndian(int32_t(line.first), tr.data()+189U);
    getBigEndian(int32_t(line.second), tr.data()+193U);

    EXPECT_CALL(mock, readDOMD(offset, ns, _)).Times(Exactly(1)).WillOnce(SetArrayArgument<2>(tr.begin(), tr.end()));

    auto line2 = file->readGridPoint(File::Grid::Line, offset);
    EXPECT_EQ(line, line2);
}

TEST_F(FileSEGYSpecTest, FileReadTraceHeader)
{
    auto mock = std::make_shared<MockObj>(piol, notFile, nullptr);
    initReadHOMock(*mock.get(), how, nt, ns, inc, 5, testString);

    File::SEGY segy(piol, notFile, fileSegyOpt, mock);
    std::vector<uchar> tr(SEGSz::getMDSz());
    for (size_t i = 0; i < nt; i++)
        initReadTraceMock(*mock.get(), tr, i, ns, &segy);
}

TEST_F(FileSEGYSpecTest, FileReadTrHdrBigNs)
{
    const size_t bigns = 10000;
    auto mock = std::make_shared<MockObj>(piol, notFile, nullptr);
    initReadHOMock(*mock.get(), how, nt, bigns, inc, 5, testString);

    File::SEGY segy(piol, notFile, fileSegyOpt, mock);
    std::vector<uchar> tr(SEGSz::getMDSz());
    initReadTraceMock(*mock.get(), tr, nt/2U, bigns, &segy);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// DEATH TESTS ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef FileSEGYSpecTest FileSEGYDeathTest;

TEST_F(FileSEGYDeathTest, FileWriteAPIBadns)
{
    auto mock = std::make_shared<NiceMock<MockObj>>(piol, notFile, nullptr);
    size_t badns = 0x470000;
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
    const size_t ns = 261U;
    const size_t nt = 400U;
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
    SCOPED_TRACE("SEGYReadHO");
    const size_t ns = 261U;
    const size_t nt = 400U;
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

