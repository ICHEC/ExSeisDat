#include <iconv.h>
#include <string.h>
#include <memory>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../src/file/filesegy.cc"
////////////////////////////////////////////////////////////////////////////////////
//////////////////////// Unit tests of non-class functions /////////////////////////
////////////////////////////////////////////////////////////////////////////////////
using namespace PIOL;
using namespace File;
using namespace SEGSz;

TEST(FileSEGY, ScaleBigIntegers)
{
    EXPECT_EQ(10000,  deScale(21474836470000.0));
    EXPECT_EQ(1000,   deScale(2147483647000.0));
    EXPECT_EQ(100,    deScale(214748364700.0));
    EXPECT_EQ(10,     deScale(21474836470.0));

    EXPECT_EQ(10000,  deScale(10000000000000.0));
    EXPECT_EQ(1000,   deScale(01000000000000.0));
    EXPECT_EQ(100,    deScale(00100000000000.0));
    EXPECT_EQ(10,     deScale(00010000000000.0));

    EXPECT_EQ(10000,  deScale(21474836470000.0000999));
    EXPECT_EQ(1000,   deScale(2147483647000.0000999));
    EXPECT_EQ(100,    deScale(214748364700.0000999));
    EXPECT_EQ(10,     deScale(21474836470.0000999));
}

TEST(FileSEGY, ScaleDecimals)
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
    EXPECT_EQ(-10,    deScale(214748364.7000999));
    EXPECT_EQ(-100,   deScale(21474836.4700999));
    EXPECT_EQ(-1000,  deScale(2147483.6470999));
    EXPECT_EQ(1,      deScale(2147483647.0000999));
}

TEST(FileSEGY, getSrc)
{
    auto p = std::pair<TrCrd, TrCrd>(static_cast<TrCrd>(73U), static_cast<TrCrd>(77U));
    ASSERT_EQ(p, getPair(Coord::Src));
}
TEST(FileSEGY, getCmp)
{
    auto p = std::pair<TrCrd, TrCrd>(static_cast<TrCrd>(81U), static_cast<TrCrd>(85U));
    ASSERT_EQ(p, getPair(Coord::Rcv));
}
TEST(FileSEGY, getRcv)
{
    auto p = std::pair<TrCrd, TrCrd>(static_cast<TrCrd>(181U), static_cast<TrCrd>(185U));
    ASSERT_EQ(p, getPair(Coord::Cmp));
}
TEST(FileSEGY, getLine)
{
    auto p = std::pair<TrGrd, TrGrd>(static_cast<TrGrd>(189U), static_cast<TrGrd>(193U));
    ASSERT_EQ(p, getPair(Grid::Line));
}

TEST(FileSEGY, getScaleCoord1)
{
    std::vector<uchar> tr(SEGSz::getMDSz());

    tr[70] = 0xFC;
    tr[71] = 0x18;
    geom_t scal = getMd(TrScal::ScaleCoord, tr.data());
    ASSERT_DOUBLE_EQ(1.0/1000.0, scal);
}

TEST(FileSEGY, getScaleCoord2)
{
    std::vector<uchar> tr(SEGSz::getMDSz());
//Two's complement of 1000
    tr[70] = 0x03;
    tr[71] = 0xE8;
    geom_t scal = getMd(TrScal::ScaleCoord, tr.data());
    ASSERT_DOUBLE_EQ(1000.0, scal);
}

TEST(FileSEGY, getScaleElev1)
{
    std::vector<uchar> tr(SEGSz::getMDSz());
//Two's complement of 10000
    tr[68] = 0xD8;
    tr[69] = 0xF0;
    geom_t scal = getMd(TrScal::ScaleElev, tr.data());
    ASSERT_DOUBLE_EQ(1.0/10000.0, scal);
}

TEST(FileSEGY, getScaleElev2)
{
    std::vector<uchar> tr(SEGSz::getMDSz());
    tr[68] = 0x27;
    tr[69] = 0x10;
    geom_t scal = getMd(TrScal::ScaleElev, tr.data());
    ASSERT_DOUBLE_EQ(10000.0, scal);
}

template <class T>
std::pair<int32_t, int32_t> testPack(const std::pair<T, T> pair, std::vector<uchar> & tr)
{
    //x = 0xA9876543
    size_t xMd = size_t(pair.first) - 1U;
    tr[xMd]   = 0xA9;
    tr[xMd+1] = 0x87;
    tr[xMd+2] = 0x65;
    tr[xMd+3] = 0x43;

    //y = 0xBA987654
    size_t yMd = size_t(pair.second) - 1U;
    tr[yMd]   = 0xBA;
    tr[yMd+1] = 0x98;
    tr[yMd+2] = 0x76;
    tr[yMd+3] = 0x54;

    return std::make_pair(2844222787,3130553940);
}

template <typename T>
void testGetGrid(const std::pair<T, T> pair)
{
    std::vector<uchar> tr(SEGSz::getMDSz());
    auto val = testPack(pair, tr);

    ASSERT_EQ(val.first, getMd(pair.first, tr.data()));
    ASSERT_EQ(val.second, getMd(pair.second, tr.data()));
}

void testGetCoord(const std::pair<TrCrd, TrCrd> pair, const geom_t scal)
{
    std::vector<uchar> tr(SEGSz::getMDSz());
    auto val = testPack(pair, tr);

    ASSERT_EQ(val.first*scal, getMd(pair.first, scal, tr.data()));
    ASSERT_EQ(val.second*scal, getMd(pair.second, scal, tr.data()));
}

TEST(FileSEGY, getMdGrd)
{
    SCOPED_TRACE("Line");
    testGetGrid(getPair(Grid::Line));
}

TEST(FileSEGY, getMdCrd)
{
    SCOPED_TRACE("Src");
    testGetCoord(getPair(Coord::Src), geom_t(1));
    testGetCoord(getPair(Coord::Src), geom_t(.1));
    testGetCoord(getPair(Coord::Src), geom_t(.01));
    SCOPED_TRACE("Rcv");
    testGetCoord(getPair(Coord::Rcv), geom_t(1));
    SCOPED_TRACE("Cmp");
    testGetCoord(getPair(Coord::Cmp), geom_t(1));
}

//geom_t getMd(const TrCrd item, const geom_t scale, const uchar * src)
//template <class T = int16_t> T getMd(const Hdr item, const uchar * src)
//int32_t getMd(const TrGrd item, const uchar * src)

TEST(FileSEGY, setMetadata)
{
//template <typename T = int16_t> void setMd(const Hdr item, const T src, uchar * dst)
//void setScale(const TrScal item, const int16_t scale, uchar * buf)
//void setCoord(const Coord item, const coord_t coord, const int16_t scale, uchar * buf)
//void setGrid(const Grid item, const grid_t grid, uchar * buf)
}

TEST(FileSEGY, scaleConv)
{
//geom_t scaleConv(int16_t scale)
}

