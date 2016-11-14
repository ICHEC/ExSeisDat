#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "tglobal.hh"
#include "cppfileapi.hh"
#include "ops/ops.hh"
#include "anc/cmpi.hh"
using namespace testing;
using namespace PIOL;
using namespace File;

struct OpsTest : public Test
{
    ExSeis piol;
    OpsTest(void) : piol(false)
    {
    }
};

TEST_F(OpsTest, getMinMaxSimple)
{
    std::vector<coord_t> coord(1000);
    for (int i = 0; i < 1000; i++)
        coord[i] = { 1500. + i, 1300. - i };

    std::vector<CoordElem> minmax(4);
    for (size_t offset = 0; offset < 300000; offset += 1 + offset * 10)
    {
        getMinMax(piol, offset, coord.size(), coord.data(), minmax.data());
        piol.isErr();
        ASSERT_DOUBLE_EQ(minmax[0].val, 1500.); //min x
        ASSERT_EQ(offset, minmax[0].num);
        ASSERT_DOUBLE_EQ(minmax[1].val, 2499.); //max x
        ASSERT_EQ(offset+999, minmax[1].num);
        ASSERT_DOUBLE_EQ(minmax[2].val, 301.); //min x
        ASSERT_EQ(offset+999, minmax[2].num);
        ASSERT_DOUBLE_EQ(minmax[3].val, 1300.); //max x
        ASSERT_EQ(offset, minmax[3].num);
    }
}

TEST_F(OpsTest, getMinMaxFail1)  //These fails won't surive a multi-processor example
{
    std::vector<coord_t> coord(1000);
    for (int i = 0; i < 1000; i++)
        coord[i] = { 1500. + i, 1300. - i };

    std::vector<CoordElem> minmax(4);
    getMinMax(piol, 10, coord.size(), coord.data(), NULL);
    piol.isErr();
}

TEST_F(OpsTest, getMinMaxFail2)  //These fails won't surive a multi-processor example
{
    std::vector<CoordElem> minmax(4);
    getMinMax(piol, 10, 0, NULL, minmax.data());
    piol.isErr();
}

TEST_F(OpsTest, getMinMaxFail3)  //These fails won't surive a multi-processor example
{
    std::vector<coord_t> coord(1000);
    for (int i = 0; i < 1000; i++)
        coord[i] = { 1500. + i, 1300. - i };

    std::vector<CoordElem> minmax(4);
    getMinMax(piol, 10, 0, coord.data(), minmax.data());
    piol.isErr();
}

template <bool Y, bool Min>
void testMinMax(const std::vector<coord_t> & coord, const std::vector<CoordElem> & minmax)
{
    double val = double(Min ? 1 : -1) * std::numeric_limits<double>::infinity();
    size_t tn = 0;
    for (size_t i = 0; i < coord.size(); i++)
    {
        double t = (Y ? coord[i].y : coord[i].x);
        if ((t < val && Min) || (t > val && !Min))
        {
            val = t;
            tn = i;
        }
    }
    ASSERT_DOUBLE_EQ(val, minmax[2U*size_t(Y) + size_t(!Min)].val);
    ASSERT_EQ(tn, minmax[2U*size_t(Y) + size_t(!Min)].num);
}

TEST_F(OpsTest, getMinMaxRand)
{
    srand(31337);
    std::vector<CoordElem> minmax(4);
    for (size_t l = 0; l < 20; l++)
    {
        size_t num = rand() % 1710880;
        std::vector<coord_t> coord(num);
        for (size_t i = 0; i < num; i++)
            coord[i] = { rand(), rand() };

        getMinMax(piol, 0, coord.size(), coord.data(), minmax.data());
        piol.isErr();
        testMinMax<false, false>(coord, minmax);
        testMinMax<false, true>(coord, minmax);
        testMinMax<true, false>(coord, minmax);
        testMinMax<true, true>(coord, minmax);
    }
}

//extern std::vector<size_t> Sort(ExSeisPIOL * piol, size_t nt, size_t offset, std::vector<PrmEntry> & coords, Compare<PrmEntry> comp);
TEST_F(OpsTest, SortSrcRcvBackwards)
{
    std::vector<PrmEntry> coords(200);
    for (size_t i = 0; i < coords.size(); i++)
    {
        coords[i].tn = i;
        coords[i].src.x = 1000.0 - i / 20;
        coords[i].src.y = 1000.0 - i % 20 ;
        coords[i].rcv.x = 1000.0 - i / 10;
        coords[i].rcv.y = 1000.0 - i % 10;
    }
    auto list = Sort(piol, SortType::SrcRcv, coords.size(), 0, coords);

    for (size_t i = 0; i < list.size(); i++)
        ASSERT_EQ(list.size() - i-1, list[i]);
}

TEST_F(OpsTest, SortSrcRcvForwards)
{
    std::vector<PrmEntry> coords(200);
    for (size_t i = 0; i < coords.size(); i++)
    {
        coords[i].tn = i;
        coords[i].src.x = 1000.0 + i / 20;
        coords[i].src.y = 1000.0 + i % 20 ;
        coords[i].rcv.x = 1000.0 + i / 10;
        coords[i].rcv.y = 1000.0 + i % 10;
    }
    auto list = Sort(piol, SortType::SrcRcv, coords.size(), 0, coords);

    for (size_t i = 0; i < list.size(); i++)
        ASSERT_EQ(i, list[i]);
}


TEST_F(OpsTest, SortSrcRcvRand)
{
    std::vector<PrmEntry> coords(10);
    for (size_t i = 0; i < coords.size(); i++)
    {
        coords[i].tn = i;
        coords[i].src.y = 1000.0;
        coords[i].rcv.x = 1000.0;
        coords[i].rcv.y = 1000.0;
    }
    coords[0].src.x = 5.;
    coords[1].src.x = 3.;
    coords[2].src.x = 1.;
    coords[3].src.x = 4.;
    coords[4].src.x = 2.;
    coords[5].src.x = 9.;
    coords[6].src.x = 6.;
    coords[7].src.x = 8.;
    coords[8].src.x = 7.;
    coords[9].src.x = 0.;
    auto list = Sort(piol, SortType::SrcRcv, coords.size(), 0, coords);
    ASSERT_EQ(5, list[0]);
    ASSERT_EQ(3, list[1]);
    ASSERT_EQ(1, list[2]);
    ASSERT_EQ(4, list[3]);
    ASSERT_EQ(2, list[4]);
    ASSERT_EQ(9, list[5]);
    ASSERT_EQ(6, list[6]);
    ASSERT_EQ(8, list[7]);
    ASSERT_EQ(7, list[8]);
    ASSERT_EQ(0, list[9]);
}


