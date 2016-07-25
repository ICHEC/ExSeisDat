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
#define private public
#define protected public
#include "file/filesegy.hh"
#undef private
#undef protected

using namespace testing;
using namespace PIOL;


class MockObj : public Obj::Interface
{
    public :
    MockObj(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, std::shared_ptr<Data::Interface> data_)
               : Obj::Interface(piol_, name_, data_)
    {}
    MOCK_METHOD0(getFileSz, size_t(void));
    MOCK_METHOD1(readHO, void(uchar *));
    MOCK_METHOD1(setFileSz, void(const size_t));
    MOCK_METHOD1(writeHO, void(const uchar *));
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
    std::shared_ptr<MockObj> mockRead;
    std::shared_ptr<MockObj> mockWrite;
    const size_t nt = 40U;
    const size_t ns = 200U;
    const int inc = 10;
    const int format = 1;
    std::vector<uchar> hor;
    std::vector<uchar> how;
    FileSEGYSpecTest() : FileIntegrationTest()
    {
        mockRead = std::make_shared<MockObj>(piol, notFile, nullptr);
        mockWrite = std::make_shared<MockObj>(piol, notFile, nullptr);
        hor.resize(SEGSz::getHOSz());
        how.resize(SEGSz::getHOSz());
    }
};

void initReadMock(MockObj & mock, std::vector<uchar> & ho, size_t ns, size_t nt, int inc, int format, std::string testString)
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
    ho[3221U] = ns;
    ho[3217U] = inc;
    ho[3225U] = format;
    EXPECT_CALL(mock, readHO(_)).Times(Exactly(1)).WillOnce(SetArrayArgument<0>(ho.begin(), ho.end()));
}

TEST_F(FileSEGYSpecTest, FileReadAPI)
{
    initReadMock(*mockRead.get(), hor, ns, nt, inc, format, testString);
    ASSERT_TRUE(ns < 0x10000);
    File::SEGY segy(piol, notFile, fileSegyOpt, mockRead);
    piol->isErr();

    EXPECT_EQ(piol, segy.piol);
    EXPECT_EQ(notFile, segy.name);
    EXPECT_EQ(mockRead, segy.obj);
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

void initWriteMock(MockObj & mock, std::vector<uchar> & ho, size_t nt, size_t ns, int inc, int format, std::string testString)
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

TEST_F(FileSEGYSpecTest, FileWriteAPI)
{
    initWriteMock(*mockWrite.get(), how, nt, ns, inc, 5, testString);

    File::SEGY segy(piol, notFile, fileSegyOpt, mockWrite);
    piol->isErr();

    EXPECT_EQ(piol, segy.piol);
    EXPECT_EQ(notFile, segy.name);
    EXPECT_EQ(mockWrite, segy.obj);

    segy.writeNt(nt);
    piol->isErr();

    segy.writeNs(ns);
    piol->isErr();

    segy.writeInc(geom_t(inc*SI::Micro));
    piol->isErr();

    segy.writeText(testString);
    piol->isErr();
}

////////////////////////////////////////////////////////////////////////////////////
///////////////////// INTEGRATION-CLASS SPECIFICATION TESTING //////////////////////
////////////////////////////////////////////////////////////////////////////////////

//Read test of File::SEGY -> Obj::SEGY -> Data::MPIIO
TEST_F(FileIntegrationTest, SEGYReadHO)
{
    SCOPED_TRACE("SEGYReadHO");
    const size_t ns = 261U;
    const size_t nt = 400U;
    std::string smallSEGY = "tmp/smallsegy.tmp";
    File::SEGY segy(piol, smallSEGY, fileSegyOpt, objSegyOpt, dataOpt);
    piol->isErr();
    EXPECT_EQ(ns, segy.readNs());
    piol->isErr();
    EXPECT_EQ(nt, segy.readNt());
    piol->isErr();
    std::string text = segy.readText();
    EXPECT_EQ(3200U, text.size());
    for (size_t i = 0; i < text.size(); i++)
        ASSERT_EQ(getPattern(i), reinterpret_cast<uchar &>(text[i]));
}

//TODO: Add test same as above for big files

//Wrute test of File::SEGY -> Obj::SEGY -> Data::MPIIO
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

