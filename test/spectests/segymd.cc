#include "ExSeisDat/PIOL/file/segymd.hh"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <memory>
#include <string.h>

////////////////////// Unit tests of non-class functions ///////////////////////

using namespace PIOL;
using namespace File;
using namespace SEGSz;

TEST(SEGYMd, ScaleBigIntegers)
{
    if (sizeof(double) == sizeof(geom_t)) {
        EXPECT_EQ(10000, deScale(21474836470000.0));
        EXPECT_EQ(1000, deScale(2147483647000.0));
        EXPECT_EQ(100, deScale(214748364700.0));
        EXPECT_EQ(10, deScale(21474836470.0));
        EXPECT_EQ(10000, deScale(10000000000000.0));
        EXPECT_EQ(1000, deScale(01000000000000.0));
        EXPECT_EQ(100, deScale(00100000000000.0));
        EXPECT_EQ(10, deScale(00010000000000.0));

        EXPECT_EQ(10000, deScale(21474836470000.0000999));
        EXPECT_EQ(1000, deScale(2147483647000.0000999));
        EXPECT_EQ(100, deScale(214748364700.0000999));
        EXPECT_EQ(10, deScale(21474836470.0000999));
    }
    else if (sizeof(float) == sizeof(geom_t)) {
        // Fix these due to guaranteed big precision loss
        EXPECT_EQ(10000, deScale(21474836470000.0));
        EXPECT_EQ(1000, deScale(2147483647000.0));
        EXPECT_EQ(100, deScale(214748364700.0));
        EXPECT_EQ(10, deScale(21474836470.0));

        EXPECT_EQ(10000, deScale(10000000000000.0));  // ok
        EXPECT_EQ(1000, deScale(01000000000000.0));   // ok
        EXPECT_EQ(100, deScale(00100000000000.0));    // ok
        EXPECT_EQ(10, deScale(00010000000000.0));     // ok

        EXPECT_EQ(10000, deScale(21474836470000.0000999));
        EXPECT_EQ(1000, deScale(2147483647000.0000999));
        EXPECT_EQ(100, deScale(214748364700.0000999));
        EXPECT_EQ(10, deScale(21474836470.0000999));
    }
}

TEST(SEGYMd, ScaleDecimals)
{
    EXPECT_EQ(-10000, deScale(214748.3647));
    EXPECT_EQ(-1000, deScale(2147483.647));
    EXPECT_EQ(-100, deScale(21474836.47));
    EXPECT_EQ(-10, deScale(214748364.7));
    EXPECT_EQ(1, deScale(2147483647.));

    EXPECT_EQ(-10000, deScale(1.0001));
    EXPECT_EQ(-1000, deScale(1.001));
    EXPECT_EQ(-100, deScale(1.01));
    EXPECT_EQ(-10, deScale(1.1));
    EXPECT_EQ(1, deScale(1.));

    EXPECT_EQ(-10000, deScale(0.0001));
    EXPECT_EQ(-1000, deScale(0.001));
    EXPECT_EQ(-100, deScale(0.01));
    EXPECT_EQ(-10, deScale(0.1));
    EXPECT_EQ(1, deScale(0.));

    // Tests for case where round mode pushes sig figs over sizes we can handle
    EXPECT_EQ(-10000, deScale(214748.3647199));
    EXPECT_EQ(-10, deScale(214748364.7000999));
    EXPECT_EQ(-100, deScale(21474836.4700999));
    EXPECT_EQ(-1000, deScale(2147483.6470999));
    EXPECT_EQ(1, deScale(2147483647.0000999));
}

/* clang-format off */
// TEST(SEGYMd, getSrc)
// {
//     auto p = std::pair<TrCrd, TrCrd>(static_cast<TrCrd>(73U), static_cast<TrCrd>(77U));
//     ASSERT_EQ(p, getPair(Coord::Src));
// }
// TEST(SEGYMd, getCmp)
// {
//     auto p = std::pair<TrCrd, TrCrd>(static_cast<TrCrd>(81U), static_cast<TrCrd>(85U));
//     ASSERT_EQ(p, getPair(Coord::Rcv));
// }
// TEST(SEGYMd, getRcv)
// {
//     auto p = std::pair<TrCrd, TrCrd>(static_cast<TrCrd>(181U), static_cast<TrCrd>(185U));
//     ASSERT_EQ(p, getPair(Coord::CMP));
// }
// TEST(SEGYMd, getLine)
// {
//     auto p = std::pair<TrGrd, TrGrd>(static_cast<TrGrd>(189U), static_cast<TrGrd>(193U));
//     ASSERT_EQ(p, getPair(Grid::Line));
// }
//
// TEST(SEGYMd, getScaleCoord1)
// {
//     std::vector<uchar> tr(SEGSz::getMDSz());
//
//     tr[70] = 0xFC;
//     tr[71] = 0x18;
//     geom_t scal = getMd(TrScal::ScaleCoord, tr.data());
//     ASSERT_DOUBLE_EQ(1.0/1000.0, scal);
// }
//
// TEST(SEGYMd, getScaleCoord2)
// {
//     std::vector<uchar> tr(SEGSz::getMDSz());
// //Two's complement of 1000
//     tr[70] = 0x03;
//     tr[71] = 0xE8;
//     geom_t scal = getMd(TrScal::ScaleCoord, tr.data());
//     ASSERT_DOUBLE_EQ(1000.0, scal);
// }
//
// TEST(SEGYMd, getScaleElev1)
// {
//     std::vector<uchar> tr(SEGSz::getMDSz());
// //Two's complement of 10000
//     tr[68] = 0xD8;
//     tr[69] = 0xF0;
//     geom_t scal = getMd(TrScal::ScaleElev, tr.data());
//     ASSERT_DOUBLE_EQ(1.0/10000.0, scal);
// }
//
// TEST(SEGYMd, getScaleElev2)
// {
//     std::vector<uchar> tr(SEGSz::getMDSz());
//     tr[68] = 0x27;
//     tr[69] = 0x10;
//     geom_t scal = getMd(TrScal::ScaleElev, tr.data());
//     ASSERT_DOUBLE_EQ(10000.0, scal);
// }
//
// template <class T>
// std::pair<int32_t, int32_t> testPack(const std::pair<T, T> pair, std::vector<uchar> & tr)
// {
//     //x = 0xA9876543
//     size_t xMd = size_t(pair.first) - 1U;
//     tr[xMd]   = 0xA9;
//     tr[xMd+1] = 0x87;
//     tr[xMd+2] = 0x65;
//     tr[xMd+3] = 0x43;
//
//     //y = 0xBA987654
//     size_t yMd = size_t(pair.second) - 1U;
//     tr[yMd]   = 0xBA;
//     tr[yMd+1] = 0x98;
//     tr[yMd+2] = 0x76;
//     tr[yMd+3] = 0x54;
//
//     return std::make_pair(-1450744509, -1164413356);
// }
//
// template <typename T>
// void testGetGrid(const std::pair<T, T> pair)
// {
//     std::vector<uchar> tr(SEGSz::getMDSz());
//     auto val = testPack(pair, tr);
//
//     ASSERT_EQ(val.first, getMd(pair.first, tr.data()));
//     ASSERT_EQ(val.second, getMd(pair.second, tr.data()));
// }
//
// void testGetCoord(const std::pair<TrCrd, TrCrd> pair, const geom_t scal)
// {
//     std::vector<uchar> tr(SEGSz::getMDSz());
//     auto val = testPack(pair, tr);
//
//     ASSERT_EQ(val.first*scal, getMd(pair.first, scal, tr.data()));
//     ASSERT_EQ(val.second*scal, getMd(pair.second, scal, tr.data()));
// }
//
// TEST(SEGYMd, getMdGrd)
// {
//     SCOPED_TRACE("Line");
//     testGetGrid(getPair(Grid::Line));
// }
//
// TEST(SEGYMd, getMdCrd)
// {
//     SCOPED_TRACE("Src");
//     testGetCoord(getPair(Coord::Src), geom_t(1));
//     testGetCoord(getPair(Coord::Src), geom_t(.1));
//     testGetCoord(getPair(Coord::Src), geom_t(.01));
//     SCOPED_TRACE("Rcv");
//     testGetCoord(getPair(Coord::Rcv), geom_t(1));
//     SCOPED_TRACE("CMP");
//     testGetCoord(getPair(Coord::CMP), geom_t(1));
// }
//
// void testSetMd(std::vector<Hdr> item)
// {
//     std::vector<uchar> ho(SEGSz::getHOSz());
//     for (int16_t val = 0; val < 0x7FFF; val++)
//     {
//         for (size_t i = 0; i < item.size(); i++)
//             setMd(item[i], val, ho.data());
//
//         for (size_t i = 0; i < item.size(); i++)
//         {
//             ASSERT_EQ((val >> 8) & 0xFF, ho[size_t(item[i])-1U]);
//             ASSERT_EQ(val & 0xFF, ho[size_t(item[i])]);
//             ho[static_cast<size_t>(item[i])-1U] = ho[static_cast<size_t>(item[i])] = 0;
//         }
//     }
// }
//
// void testSetMd(Hdr item, size_t check)
// {
//     ASSERT_EQ(size_t(item), check);
//     testSetMd({item});
// }
//
// TEST(SEGYMd, setNs)
// {
//     testSetMd(Hdr::NumSample, 3221);
// }
//
// TEST(SEGYMd, setType)
// {
//     testSetMd(Hdr::Type, 3225);
// }
//
// TEST(SEGYMd, setIncrement)
// {
//     testSetMd(Hdr::Increment, 3217);
// }
//
// TEST(SEGYMd, setUnits)
// {
//     testSetMd(Hdr::Units, 3255);
// }
//
// TEST(SEGYMd, setSEGYFormat)
// {
//     testSetMd(Hdr::SEGYFormat, 3501);
// }
//
// TEST(SEGYMd, setFixedTrace)
// {
//     testSetMd(Hdr::FixedTrace, 3503);
// }
//
// TEST(SEGYMd, setExtensions)
// {
//     testSetMd(Hdr::Extensions, 3505);
// }
//
// //TODO: Add more tests like this for every set combination
// TEST(SEGYMd, setMdArray)
// {
//     //This test ensures no set overwrites the other
//     std::vector<Hdr> hdr = {Hdr::NumSample, Hdr::Type, Hdr::Increment,
//                             Hdr::Units, Hdr::SEGYFormat, Hdr::FixedTrace,
//                             Hdr::Extensions};
//     testSetMd(hdr);
// }
//
// void testSetScale(TrScal item, size_t check)
// {
//     std::vector<uchar> tr(SEGSz::getMDSz());
//     for (int16_t val = 0; val < 0x7FFF; val++)
//     {
//         setScale(item, val, tr.data());
//         ASSERT_EQ((val >> 8) & 0xFF, tr[size_t(check)-1U]);
//         ASSERT_EQ(val & 0xFF, tr[size_t(check)]);
//         tr[size_t(item)-1U] = 0;
//         tr[size_t(item)] = 0;
//     }
// }
//
// TEST(SEGYMd, setScaleCoord)
// {
//     testSetScale(TrScal::ScaleCoord, 71);
// }
//
// TEST(SEGYMd, setScaleElev)
// {
//     testSetScale(TrScal::ScaleElev, 69);
// }
//
// void testSetGrid(Grid item, size_t check1, size_t check2)
// {
//     std::vector<uchar> tr(SEGSz::getMDSz());
//     for (int32_t val = 2; val < 201337000; val += 201337)
//     {
//         auto p = grid_t(val, val+1LL);
//         setGrid(item, p, tr.data());
//
//         ASSERT_EQ((val >> 24) & 0xFF, tr[check1-1U]);
//         ASSERT_EQ((val >> 16) & 0xFF, tr[check1+0U]);
//         ASSERT_EQ((val >> 8) & 0xFF,  tr[check1+1U]);
//         ASSERT_EQ(val & 0xFF,         tr[check1+2U]);
//
//         val++;
//         ASSERT_EQ((val >> 24) & 0xFF, tr[check2-1U]);
//         ASSERT_EQ((val >> 16) & 0xFF, tr[check2+0U]);
//         ASSERT_EQ((val >> 8) & 0xFF,  tr[check2+1U]);
//         ASSERT_EQ(val & 0xFF,         tr[check2+2U]);
//
//         tr[check1-1U] = 0;
//         tr[check1+0U] = 0;
//         tr[check1+1U] = 0;
//         tr[check1+2U] = 0;
//
//         tr[check2-1U] = 0;
//         tr[check2+0U] = 0;
//         tr[check2+1U] = 0;
//         tr[check2+2U] = 0;
//     }
// }
//
// TEST(SEGYMd, setGrid)
// {
//     testSetGrid(Grid::Line, 189, 193);
// }
//
// void testSetCoord(Coord item, size_t check1, size_t check2)
// {
//     std::vector<uchar> tr(SEGSz::getMDSz());
//     for (int32_t val = 2; val < 201337000; val += 201337)
//     {
//         auto p = coord_t(val, val+3L);
//         for (int32_t scal = 1; scal <= 10000; scal *= 10)
//         {
//             {
//                 setCoord(item, p, scal, tr.data());
//                 auto val1 = getHost<int32_t>(&tr[size_t(check1)-1U]);
//                 auto val2 = getHost<int32_t>(&tr[size_t(check2)-1U]);
//                 ASSERT_EQ(val1, std::lround(p.x / scal)) << "val1 " << val1 << " p.x " << p.x << " val "
//                                               << val << " scal " << scal << std::endl;
//                 ASSERT_EQ(val2, std::lround(p.y / scal)) << "val1 " << val2 << " p.y " << p.y << " val "
//                                                << val << " scal " << scal << std::endl;
//                 tr[size_t(check1)-1U] = 0;
//                 tr[size_t(check2)-1U] = 0;
//             }
//             if (int32_t(p.y * scal) > 0)
//             {
//                 int nscal = -scal;
//                 setCoord(item, p, nscal, tr.data());
//
//                 auto val1 = getHost<int32_t>(&tr[size_t(check1)-1U]);
//                 auto val2 = getHost<int32_t>(&tr[size_t(check2)-1U]);
//                 ASSERT_EQ(val1/geom_t(scal), p.x) << "1st " << std::lround(val1/geom_t(scal)) << " p.x " << p.x << " val "
//                                                    << val << " scal " << scal << std::endl;
//
//                 ASSERT_EQ(val2/geom_t(scal), p.y) << "1st " << std::lround(val1/geom_t(scal)) << " p.y " << p.y << " val "
//                                                    << val << " scal " << scal << std::endl;
//                 tr[size_t(check1)-1U] = 0;
//                 tr[size_t(check2)-1U] = 0;
//             }
//         }
//     }
// }
//
// TEST(SEGYMd, setCoordSrc)
// {
//     testSetCoord(Coord::Src, 73, 77);
// }
//
// TEST(SEGYMd, setCoordRcv)
// {
//     testSetCoord(Coord::Rcv, 81, 85);
// }
//
// TEST(SEGYMd, setCoordCmp)
// {
//     testSetCoord(Coord::CMP, 181, 185);
// }
//
// //Test every legitimate possibility
// TEST(SEGYMd, scaleConv)
// {
//     EXPECT_DOUBLE_EQ(geom_t(1)/(geom_t(10000)) , scaleConv(-10000));
//     EXPECT_DOUBLE_EQ(geom_t(1)/(geom_t(1000))  , scaleConv(-1000));
//     EXPECT_DOUBLE_EQ(geom_t(1)/(geom_t(100))   , scaleConv(-100));
//     EXPECT_DOUBLE_EQ(geom_t(1)/(geom_t(10))    , scaleConv(-10));
//     EXPECT_DOUBLE_EQ(geom_t(1)     , scaleConv(-1));
//     EXPECT_DOUBLE_EQ(geom_t(1)     , scaleConv(0));
//     EXPECT_DOUBLE_EQ(geom_t(1)     , scaleConv(1));
//     EXPECT_DOUBLE_EQ(geom_t(10)    , scaleConv(10));
//     EXPECT_DOUBLE_EQ(geom_t(100)   , scaleConv(100));
//     EXPECT_DOUBLE_EQ(geom_t(1000)  , scaleConv(1000));
//     EXPECT_DOUBLE_EQ(geom_t(10000) , scaleConv(10000));
// }
//
// const TraceParam getTestPrm(void)
// {
//     TraceParam prm;
//     prm.src = {1., 2.};
//     prm.rcv = {3., 4.};
//     prm.cmp = {5., 6.};
//     prm.line = {7., 8.};
//     prm.tn = 100U;
//     return prm;
// }
//
// const std::vector<uchar> getTestMd(void)
// {
//     std::vector<uchar> md(240);
//     md[7] = 100;
//     md[71] = 1;
//     md[75] = 1;
//     md[79] = 2;
//     md[83] = 3;
//     md[87] = 4;
//     md[183] = 5;
//     md[187] = 6;
//     md[191] = 7;
//     md[195] = 8;
//     return md;
// }
//
// void TestPrm(TraceParam & prm, TraceParam prm2 = getTestPrm())
// {
//     ASSERT_EQ(prm.src.x, prm2.src.x);
//     ASSERT_EQ(prm.src.y, prm2.src.y);
//     ASSERT_EQ(prm.rcv.x, prm2.rcv.x);
//     ASSERT_EQ(prm.rcv.y, prm2.rcv.y);
//     ASSERT_EQ(prm.cmp.x, prm2.cmp.x);
//     ASSERT_EQ(prm.cmp.y, prm2.cmp.y);
//     ASSERT_EQ(prm.line.il, prm2.line.il);
//     ASSERT_EQ(prm.line.xl, prm2.line.xl);
//     ASSERT_EQ(prm.tn, prm2.tn);
// }
//
// TEST(SEGYMd, InsertExtractTraceParam)
// {
//     auto prm = getTestPrm();
//     std::vector<uchar> md(240);
//     insertTraceParam(1U, &prm, md.data());
//
//     TraceParam prm2;
//     extractTraceParam(1U, md.data(), &prm2);
//
//     TestPrm(prm, prm2);
// }
//
// TEST(SEGYMd, InsertTraceParam)
// {
//     auto prm = getTestPrm();
//     std::vector<uchar> md(240);
//     insertTraceParam(1U, &prm, md.data());
//
//     auto md2 = getTestMd();
//     ASSERT_EQ(md, md2);
// }
//
// TEST(SEGYMd, ExtractTraceParam)
// {
//     TraceParam prm;
//     auto md = getTestMd();
//     extractTraceParam(1U, md.data(), &prm);
//     TestPrm(prm);
// }
/* clang-format on */
