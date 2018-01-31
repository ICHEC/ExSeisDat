#include "anc/mpi.hh"
#include "cppfileapi.hh"
#include "file/file.hh"
#include "ops/agc.hh"
#include "ops/minmax.hh"
#include "ops/sort.hh"
#include "ops/taper.hh"
#include "ops/temporalfilter.hh"
#include "segymdextra.hh"
#include "tglobal.hh"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace testing;
using namespace PIOL;
using namespace File;

struct OpsTest : public Test {
    std::shared_ptr<ExSeis> piol = ExSeis::New(false);
};

/*! Get the min and the max of a set of parameters passed. This is a parallel operation. It is
 *  the collective min and max across all processes (which also must all call this file).
 *  @param[in, out] piol The PIOL object
 *  @param[in] offset The starting trace number (local).
 *  @param[in] lnt The local number of traces to process.
 *  @param[in] coord
 *  @param[out] minmax An array of structures containing the minimum item.x,  maximum item.x, minimum item.y, maximum item.y
 *  and their respective trace numbers.
*/
void getMinMax(
  ExSeisPIOL* piol,
  size_t offset,
  size_t lnt,
  const coord_t* coord,
  CoordElem* minmax)
{
    auto xlam = [](const coord_t& a) -> geom_t { return a.x; };
    auto ylam = [](const coord_t& a) -> geom_t { return a.y; };
    getMinMax<coord_t>(piol, offset, lnt, coord, xlam, ylam, minmax);
}

TEST_F(OpsTest, getMinMaxSimple)
{
    std::vector<coord_t> coord(1000);
    for (int i = 0; i < 1000; i++)
        coord[i] = {1500. + i, 1300. - i};

    std::vector<CoordElem> minmax(4);
    for (size_t offset = 0; offset < 300000; offset += 1 + offset * 10) {
        getMinMax(
          piol.get(), offset, coord.size(), coord.data(), minmax.data());
        piol->isErr();
        ASSERT_EQ(offset, minmax[0].num);
        ASSERT_EQ(offset + 999, minmax[1].num);
        ASSERT_EQ(offset + 999, minmax[2].num);
        ASSERT_EQ(offset, minmax[3].num);

        if (sizeof(geom_t) == sizeof(double)) {
            ASSERT_DOUBLE_EQ(minmax[0].val, 1500.);  //min x
            ASSERT_DOUBLE_EQ(minmax[1].val, 2499.);  //max x
            ASSERT_DOUBLE_EQ(minmax[2].val, 301.);   //min x
            ASSERT_DOUBLE_EQ(minmax[3].val, 1300.);  //max x
        }
        else {
            ASSERT_FLOAT_EQ(minmax[0].val, 1500.);  //min x
            ASSERT_FLOAT_EQ(minmax[1].val, 2499.);  //max x
            ASSERT_FLOAT_EQ(minmax[2].val, 301.);   //min x
            ASSERT_FLOAT_EQ(minmax[3].val, 1300.);  //max x
        }
    }
}

TEST_F(
  OpsTest, getMinMaxFail1)  //These fails won't surive a multi-processor example
{
    std::vector<coord_t> coord(1000);
    for (int i = 0; i < 1000; i++)
        coord[i] = {1500. + i, 1300. - i};

    std::vector<CoordElem> minmax(4);
    getMinMax(piol.get(), 10, coord.size(), coord.data(), NULL);
    piol->isErr();
}

TEST_F(
  OpsTest, getMinMaxFail2)  //These fails won't surive a multi-processor example
{
    std::vector<CoordElem> minmax(4);
    getMinMax(piol.get(), 10, 0, NULL, minmax.data());
    piol->isErr();
}

TEST_F(
  OpsTest, getMinMaxFail3)  //These fails won't surive a multi-processor example
{
    std::vector<coord_t> coord(1000);
    for (int i = 0; i < 1000; i++)
        coord[i] = {1500. + i, 1300. - i};

    std::vector<CoordElem> minmax(4);
    getMinMax(piol.get(), 10, 0, coord.data(), minmax.data());
    piol->isErr();
}

template<bool Y, bool Min>
void testMinMax(
  const std::vector<coord_t>& coord, const std::vector<CoordElem>& minmax)
{
    geom_t val = geom_t(Min ? 1 : -1) * std::numeric_limits<geom_t>::infinity();
    size_t tn  = 0;
    for (size_t i = 0; i < coord.size(); i++) {
        double t = (Y ? coord[i].y : coord[i].x);
        if ((t < val && Min) || (t > val && !Min)) {
            val = t;
            tn  = i;
        }
    }

    if (sizeof(double) == sizeof(geom_t))
        ASSERT_DOUBLE_EQ(val, minmax[2U * size_t(Y) + size_t(!Min)].val);
    else
        ASSERT_FLOAT_EQ(val, minmax[2U * size_t(Y) + size_t(!Min)].val);
    ASSERT_EQ(tn, minmax[2U * size_t(Y) + size_t(!Min)].num);
}

TEST_F(OpsTest, getMinMaxRand)
{
    srand(31337);
    std::vector<CoordElem> minmax(4);
    for (size_t l = 0; l < 20; l++) {
        size_t num = rand() % 1710880;
        std::vector<coord_t> coord(num);
        for (size_t i = 0; i < num; i++)
            coord[i] = {1.0 * rand(), 1.0 * rand()};

        getMinMax(piol.get(), 0, coord.size(), coord.data(), minmax.data());
        piol->isErr();
        testMinMax<false, false>(coord, minmax);
        testMinMax<false, true>(coord, minmax);
        testMinMax<true, false>(coord, minmax);
        testMinMax<true, true>(coord, minmax);
    }
}

TEST_F(OpsTest, SortSrcRcvBackwards)
{
    Param prm(200);
    for (size_t i = 0; i < prm.size(); i++) {
        setPrm(i, PIOL_META_xSrc, 1000.0 - geom_t(i / 20), &prm);
        setPrm(i, PIOL_META_ySrc, 1000.0 - geom_t(i % 20), &prm);
        setPrm(i, PIOL_META_xRcv, 1000.0 - geom_t(i / 10), &prm);
        setPrm(i, PIOL_META_yRcv, 1000.0 - geom_t(i % 10), &prm);
        setPrm(i, PIOL_META_gtn, i, &prm);
    }
    auto list = sort(piol.get(), PIOL_SORTTYPE_SrcRcv, &prm);
    for (size_t i = 0; i < list.size(); i++)
        ASSERT_EQ(list.size() - i - 1, list[i])
          << " i " << i << " list.size()-i-1 " << list.size() - i - 1
          << " list[i] " << list[i];
}

TEST_F(OpsTest, SortSrcRcvForwards)
{
    Param prm(200);
    for (size_t i = 0; i < prm.size(); i++) {
        setPrm(i, PIOL_META_xSrc, 1000.0 + i / 20, &prm);
        setPrm(i, PIOL_META_ySrc, 1000.0 + i % 20, &prm);
        setPrm(i, PIOL_META_xRcv, 1000.0 + i / 10, &prm);
        setPrm(i, PIOL_META_yRcv, 1000.0 + i % 10, &prm);
        setPrm(i, PIOL_META_gtn, i, &prm);
    }
    auto list = sort(piol.get(), PIOL_SORTTYPE_SrcRcv, &prm);

    for (size_t i = 0; i < list.size(); i++)
        ASSERT_EQ(i, list[i]);
}

TEST_F(OpsTest, SortSrcRcvRand)
{
    Param prm(10);
    for (size_t i = 0; i < prm.size(); i++) {
        setPrm(i, PIOL_META_ySrc, 1000.0, &prm);
        setPrm(i, PIOL_META_xRcv, 1000.0, &prm);
        setPrm(i, PIOL_META_yRcv, 1000.0, &prm);
        setPrm(i, PIOL_META_gtn, i, &prm);
    }
    setPrm(0, PIOL_META_xSrc, 5.0, &prm);
    setPrm(1, PIOL_META_xSrc, 3.0, &prm);
    setPrm(2, PIOL_META_xSrc, 1.0, &prm);
    setPrm(3, PIOL_META_xSrc, 4.0, &prm);
    setPrm(4, PIOL_META_xSrc, 2.0, &prm);
    setPrm(5, PIOL_META_xSrc, 9.0, &prm);
    setPrm(6, PIOL_META_xSrc, 6.0, &prm);
    setPrm(7, PIOL_META_xSrc, 8.0, &prm);
    setPrm(8, PIOL_META_xSrc, 7.0, &prm);
    setPrm(9, PIOL_META_xSrc, 0.0, &prm);
    auto list = sort(piol.get(), PIOL_SORTTYPE_SrcRcv, &prm);
    ASSERT_EQ(static_cast<size_t>(5), list[0]);
    ASSERT_EQ(static_cast<size_t>(3), list[1]);
    ASSERT_EQ(static_cast<size_t>(1), list[2]);
    ASSERT_EQ(static_cast<size_t>(4), list[3]);
    ASSERT_EQ(static_cast<size_t>(2), list[4]);
    ASSERT_EQ(static_cast<size_t>(9), list[5]);
    ASSERT_EQ(static_cast<size_t>(6), list[6]);
    ASSERT_EQ(static_cast<size_t>(8), list[7]);
    ASSERT_EQ(static_cast<size_t>(7), list[8]);
    ASSERT_EQ(static_cast<size_t>(0), list[9]);
}

TEST_F(OpsTest, FilterCheckLowpass)
{
    size_t N = 4;
    std::vector<trace_t> denomCalc(N + 1);
    std::vector<trace_t> numerCalc(N + 1);
    makeFilter(
      FltrType::Lowpass, numerCalc.data(), denomCalc.data(), N, 30.0, 1.2, 0.0);

    std::vector<trace_t> denomRef = {1, -3.34406784, 4.23886395, -2.40934286,
                                     0.5174782};
    std::vector<trace_t> numerRef = {0.00018321611, 0.00073286443, 0.0010992966,
                                     0.00073286443, 0.00018321611};
    for (size_t i = 0; i < N + 1; i++) {
        EXPECT_FLOAT_EQ(denomRef[i], denomCalc[i]);
        EXPECT_FLOAT_EQ(numerRef[i], numerCalc[i]);
    }
}

TEST_F(OpsTest, FilterCheckHighpass)
{
    size_t N = 4;
    std::vector<trace_t> denomCalc(N + 1);
    std::vector<trace_t> numerCalc(N + 1);
    makeFilter(
      FltrType::Highpass, numerCalc.data(), denomCalc.data(), N, 30, 1.2, 0);

    std::vector<trace_t> denomRef = {1, -3.34406784, 4.23886395, -2.40934286,
                                     0.5174782};
    std::vector<trace_t> numerRef = {0.71935955, -2.87743821, 4.31615732,
                                     -2.87743821, 0.71935955};
    for (size_t i = 0; i < N + 1; i++) {
        EXPECT_FLOAT_EQ(denomRef[i], denomCalc[i]);
        EXPECT_FLOAT_EQ(numerRef[i], numerCalc[i]);
    }
}

TEST_F(OpsTest, FilterCheckBandpass)
{
    size_t N = 4;
    std::vector<trace_t> denomCalc(2 * N + 1);
    std::vector<trace_t> numerCalc(2 * N + 1);
    makeFilter(
      FltrType::Bandpass, numerCalc.data(), denomCalc.data(), N, 30, 1.2, 6.5);
    std::vector<trace_t> denomRef = {1.0,         -4.19317484, 8.01505053,
                                     -9.44595842, 7.69031281,  -4.39670663,
                                     1.68365361,  -0.3953309,  0.04619144};
    std::vector<trace_t> numerRef = {0.03142168, 0.0,        -0.1256867,
                                     0.0,        0.18853005, 0.0,
                                     -0.1256867, 0.0,        0.03142168};
    for (size_t i = 0; i < 2 * N + 1; i++) {
        EXPECT_FLOAT_EQ(denomRef[i], denomCalc[i]);
        EXPECT_FLOAT_EQ(numerRef[i], numerCalc[i]);
    }
}

TEST_F(OpsTest, FilterCheckBandstop)
{
    size_t N = 4;
    std::vector<trace_t> denomCalc(2 * N + 1);
    std::vector<trace_t> numerCalc(2 * N + 1);
    makeFilter(
      FltrType::Bandstop, numerCalc.data(), denomCalc.data(), N, trace_t(30),
      trace_t(1.2), trace_t(6.5));
    std::vector<trace_t> denomRef = {1.0,         -4.19317484, 8.01505053,
                                     -9.44595842, 7.69031281,  -4.39670663,
                                     1.68365361,  -0.3953309,  0.04619144};
    std::vector<trace_t> numerRef = {0.21261259,  -1.38519468, 4.23471188,
                                     -7.83039071, 9.54055945,  -7.83039071,
                                     4.23471188,  -1.38519468, 0.21261259};
    for (size_t i = 0; i < 2 * N + 1; i++) {
        EXPECT_FLOAT_EQ(denomRef[i], denomCalc[i]);
        EXPECT_NEAR(numerRef[i], numerCalc[i], 5e-6);
    }
}
