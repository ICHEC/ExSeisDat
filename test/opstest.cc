#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "tglobal.hh"
#include "cppfile.hh"
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

TEST_F(OpsTest, getMinMaxFail)  //These fails won't surive a multi-processor example
{
    std::vector<coord_t> coord(1000);
    for (int i = 0; i < 1000; i++)
        coord[i] = { 1500. + i, 1300. - i };

    std::vector<CoordElem> minmax(4);

    getMinMax(piol, 10, coord.size(), coord.data(), NULL);
    piol.isErr();
    getMinMax(piol, 10, coord.size(), NULL, minmax.data());
    piol.isErr();
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

