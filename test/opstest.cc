#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "tglobal.hh"
#include "cppfileapi.hh"
#include "fileops.hh"
#include "anc/mpi.hh"
#include "file/file.hh"
#include "segymdextra.hh"
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

/*! Get the min and the max of a set of parameters passed. This is a parallel operation. It is
 *  the collective min and max across all processes (which also must all call this file).
 *  \param[in, out] piol The PIOL object
 *  \param[in] offset The starting trace number (local).
 *  \param[in] lnt The local number of traces to process.
 *  \param[in] coord
 *  \param[out] minmax An array of structures containing the minimum item.x,  maximum item.x, minimum item.y, maximum item.y
 *  and their respective trace numbers.
*/
void getMinMax(ExSeisPIOL * piol, size_t offset, size_t lnt, const coord_t * coord, CoordElem * minmax)
{
    auto xlam = [](const coord_t & a) -> geom_t { return a.x; };
    auto ylam = [](const coord_t & a) -> geom_t { return a.y; };
    getMinMax<coord_t>(piol, offset, lnt, coord, xlam, ylam, minmax);
}
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
        ASSERT_EQ(offset, minmax[0].num);
        ASSERT_EQ(offset+999, minmax[1].num);
        ASSERT_EQ(offset+999, minmax[2].num);
        ASSERT_EQ(offset, minmax[3].num);

        if (sizeof(geom_t) == sizeof(double))
        {
            ASSERT_DOUBLE_EQ(minmax[0].val, 1500.); //min x
            ASSERT_DOUBLE_EQ(minmax[1].val, 2499.); //max x
            ASSERT_DOUBLE_EQ(minmax[2].val, 301.); //min x
            ASSERT_DOUBLE_EQ(minmax[3].val, 1300.); //max x
        }
        else
        {
            ASSERT_FLOAT_EQ(minmax[0].val, 1500.); //min x
            ASSERT_FLOAT_EQ(minmax[1].val, 2499.); //max x
            ASSERT_FLOAT_EQ(minmax[2].val, 301.); //min x
            ASSERT_FLOAT_EQ(minmax[3].val, 1300.); //max x
        }

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
    geom_t val = geom_t(Min ? 1 : -1) * std::numeric_limits<geom_t>::infinity();
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

    if (sizeof(double) == sizeof(geom_t))
        ASSERT_DOUBLE_EQ(val, minmax[2U*size_t(Y) + size_t(!Min)].val);
    else
        ASSERT_FLOAT_EQ(val, minmax[2U*size_t(Y) + size_t(!Min)].val);
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

TEST_F(OpsTest, SortSrcRcvBackwards)
{
    Param prm(200);
    for (size_t i = 0; i < prm.size(); i++)
    {
        setPrm(i, Meta::xSrc, 1000.0 - geom_t(i / 20), &prm);
        setPrm(i, Meta::ySrc, 1000.0 - geom_t(i % 20), &prm);
        setPrm(i, Meta::xRcv, 1000.0 - geom_t(i / 10), &prm);
        setPrm(i, Meta::yRcv, 1000.0 - geom_t(i % 10), &prm);
        setPrm(i, Meta::gtn, i, &prm);
    }
    auto list = sort(piol, SortType::SrcRcv, &prm);
    for (size_t i = 0; i < list.size(); i++)
        ASSERT_EQ(list.size() - i-1, list[i]) << " i " << i << " list.size()-i-1 " << list.size()-i-1  << " list[i] " << list[i];
}

TEST_F(OpsTest, SortSrcRcvForwards)
{
    Param prm(200);
    for (size_t i = 0; i < prm.size(); i++)
    {
        setPrm(i, Meta::xSrc, 1000.0 + i / 20, &prm);
        setPrm(i, Meta::ySrc, 1000.0 + i % 20, &prm);
        setPrm(i, Meta::xRcv, 1000.0 + i / 10, &prm);
        setPrm(i, Meta::yRcv, 1000.0 + i % 10, &prm);
        setPrm(i, Meta::gtn, i, &prm);
    }
    auto list = sort(piol, SortType::SrcRcv, &prm);

    for (size_t i = 0; i < list.size(); i++)
        ASSERT_EQ(i, list[i]);
}


TEST_F(OpsTest, SortSrcRcvRand)
{
    Param prm(10);
    for (size_t i = 0; i < prm.size(); i++)
    {
        setPrm(i, Meta::ySrc, 1000.0, &prm);
        setPrm(i, Meta::xRcv, 1000.0, &prm);
        setPrm(i, Meta::yRcv, 1000.0, &prm);
        setPrm(i, Meta::gtn, i, &prm);
    }
    setPrm(0, Meta::xSrc, 5.0, &prm);
    setPrm(1, Meta::xSrc, 3.0, &prm);
    setPrm(2, Meta::xSrc, 1.0, &prm);
    setPrm(3, Meta::xSrc, 4.0, &prm);
    setPrm(4, Meta::xSrc, 2.0, &prm);
    setPrm(5, Meta::xSrc, 9.0, &prm);
    setPrm(6, Meta::xSrc, 6.0, &prm);
    setPrm(7, Meta::xSrc, 8.0, &prm);
    setPrm(8, Meta::xSrc, 7.0, &prm);
    setPrm(9, Meta::xSrc, 0.0, &prm);
    auto list = sort(piol, SortType::SrcRcv, &prm);
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


