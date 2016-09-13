#include <iconv.h>
#include <string.h>
#include <memory>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../src/filesegy.cc"
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
    ASSERT_EQ(p, getPair(Coord::CMP));
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

    return std::make_pair(-1450744509, -1164413356);
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
    SCOPED_TRACE("CMP");
    testGetCoord(getPair(Coord::CMP), geom_t(1));
}

void testSetMd(std::vector<Hdr> item)
{
    std::vector<uchar> ho(SEGSz::getHOSz());
    for (int16_t val = 0; val < 0x7FFF; val++)
    {
        for (size_t i = 0; i < item.size(); i++)
            setMd(item[i], val, ho.data());

        for (size_t i = 0; i < item.size(); i++)
        {
            ASSERT_EQ((val >> 8) & 0xFF, ho[size_t(item[i])-1U]);
            ASSERT_EQ(val & 0xFF, ho[size_t(item[i])]);
            ho[static_cast<size_t>(item[i])-1U] = ho[static_cast<size_t>(item[i])] = 0;
        }
    }
}

void testSetMd(Hdr item, size_t check)
{
    ASSERT_EQ(size_t(item), check);
    testSetMd({item});
}

TEST(FileSEGY, setNs)
{
    testSetMd(Hdr::NumSample, 3221);
}

TEST(FileSEGY, setType)
{
    testSetMd(Hdr::Type, 3225);
}

TEST(FileSEGY, setIncrement)
{
    testSetMd(Hdr::Increment, 3217);
}

TEST(FileSEGY, setUnits)
{
    testSetMd(Hdr::Units, 3255);
}

TEST(FileSEGY, setSEGYFormat)
{
    testSetMd(Hdr::SEGYFormat, 3501);
}

TEST(FileSEGY, setFixedTrace)
{
    testSetMd(Hdr::FixedTrace, 3503);
}

TEST(FileSEGY, setExtensions)
{
    testSetMd(Hdr::Extensions, 3505);
}

//TODO: Add more tests like this for every set combination
TEST(FileSEGY, setMdArray)
{
    //This test ensures no set overwrites the other
    std::vector<Hdr> hdr = {Hdr::NumSample, Hdr::Type, Hdr::Increment,
                            Hdr::Units, Hdr::SEGYFormat, Hdr::FixedTrace,
                            Hdr::Extensions};
    testSetMd(hdr);
}

void testSetScale(TrScal item, size_t check)
{
    std::vector<uchar> tr(SEGSz::getMDSz());
    for (int16_t val = 0; val < 0x7FFF; val++)
    {
        setScale(item, val, tr.data());
        ASSERT_EQ((val >> 8) & 0xFF, tr[size_t(check)-1U]);
        ASSERT_EQ(val & 0xFF, tr[size_t(check)]);
        tr[size_t(item)-1U] = 0;
        tr[size_t(item)] = 0;
    }
}

TEST(FileSEGY, setScaleCoord)
{
    testSetScale(TrScal::ScaleCoord, 71);
}

TEST(FileSEGY, setScaleElev)
{
    testSetScale(TrScal::ScaleElev, 69);
}

void testSetGrid(Grid item, size_t check1, size_t check2)
{
    std::vector<uchar> tr(SEGSz::getMDSz());
    for (int32_t val = 2; val < 201337000; val += 201337)
    {
        auto p = grid_t(val, val+1LL);
        setGrid(item, p, tr.data());

        ASSERT_EQ((val >> 24) & 0xFF, tr[check1-1U]);
        ASSERT_EQ((val >> 16) & 0xFF, tr[check1+0U]);
        ASSERT_EQ((val >> 8) & 0xFF,  tr[check1+1U]);
        ASSERT_EQ(val & 0xFF,         tr[check1+2U]);

        val++;
        ASSERT_EQ((val >> 24) & 0xFF, tr[check2-1U]);
        ASSERT_EQ((val >> 16) & 0xFF, tr[check2+0U]);
        ASSERT_EQ((val >> 8) & 0xFF,  tr[check2+1U]);
        ASSERT_EQ(val & 0xFF,         tr[check2+2U]);

        tr[check1-1U] = 0;
        tr[check1+0U] = 0;
        tr[check1+1U] = 0;
        tr[check1+2U] = 0;

        tr[check2-1U] = 0;
        tr[check2+0U] = 0;
        tr[check2+1U] = 0;
        tr[check2+2U] = 0;
    }
}

TEST(FileSEGY, setGrid)
{
    testSetGrid(Grid::Line, 189, 193);
}

void testSetCoord(Coord item, size_t check1, size_t check2)
{
    std::vector<uchar> tr(SEGSz::getMDSz());
    for (int32_t val = 2; val < 201337000; val += 201337)
    {
        auto p = coord_t(val, val+3L);
        for (int32_t scal = 1; scal <= 10000; scal *= 10)
        {
            {
                setCoord(item, p, scal, tr.data());
                auto val1 = getHost<int32_t>(&tr[size_t(check1)-1U]);
                auto val2 = getHost<int32_t>(&tr[size_t(check2)-1U]);
                ASSERT_EQ(val1, std::lround(p.x / scal)) << "val1 " << val1 << " p.x " << p.x << " val "
                                              << val << " scal " << scal << std::endl;
                ASSERT_EQ(val2, std::lround(p.y / scal)) << "val1 " << val2 << " p.y " << p.y << " val "
                                               << val << " scal " << scal << std::endl;
                tr[size_t(check1)-1U] = 0;
                tr[size_t(check2)-1U] = 0;
            }
            if (int32_t(p.y * scal) > 0)
            {
                int nscal = -scal;
                setCoord(item, p, nscal, tr.data());

                auto val1 = getHost<int32_t>(&tr[size_t(check1)-1U]);
                auto val2 = getHost<int32_t>(&tr[size_t(check2)-1U]);
                ASSERT_EQ(val1/geom_t(scal), p.x) << "1st " << std::lround(val1/geom_t(scal)) << " p.x " << p.x << " val "
                                                   << val << " scal " << scal << std::endl;

                ASSERT_EQ(val2/geom_t(scal), p.y) << "1st " << std::lround(val1/geom_t(scal)) << " p.y " << p.y << " val "
                                                   << val << " scal " << scal << std::endl;
                tr[size_t(check1)-1U] = 0;
                tr[size_t(check2)-1U] = 0;
            }
        }
    }
}

TEST(FileSEGY, setCoordSrc)
{
    testSetCoord(Coord::Src, 73, 77);
}
TEST(FileSEGY, setCoordRcv)
{
    testSetCoord(Coord::Rcv, 81, 85);
}
TEST(FileSEGY, setCoordCmp)
{
    testSetCoord(Coord::CMP, 181, 185);
}

//Test every legitimate possibility
TEST(FileSEGY, scaleConv)
{
    EXPECT_DOUBLE_EQ(geom_t(1)/(geom_t(10000)) , scaleConv(-10000));
    EXPECT_DOUBLE_EQ(geom_t(1)/(geom_t(1000))  , scaleConv(-1000));
    EXPECT_DOUBLE_EQ(geom_t(1)/(geom_t(100))   , scaleConv(-100));
    EXPECT_DOUBLE_EQ(geom_t(1)/(geom_t(10))    , scaleConv(-10));
    EXPECT_DOUBLE_EQ(geom_t(1)     , scaleConv(-1));
    EXPECT_DOUBLE_EQ(geom_t(1)     , scaleConv(0));
    EXPECT_DOUBLE_EQ(geom_t(1)     , scaleConv(1));
    EXPECT_DOUBLE_EQ(geom_t(10)    , scaleConv(10));
    EXPECT_DOUBLE_EQ(geom_t(100)   , scaleConv(100));
    EXPECT_DOUBLE_EQ(geom_t(1000)  , scaleConv(1000));
    EXPECT_DOUBLE_EQ(geom_t(10000) , scaleConv(10000));
}
