#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "tglobal.hh"
#include "ops/ops.hh"
#include "anc/cmpi.hh"
using namespace testing;
using namespace PIOL;
using namespace File;

struct OpsTest : public Test
{
    std::shared_ptr<ExSeisPIOL> piol;
    Comm::MPIOpt opt;

    OpsTest(void)
    {
        opt.initMPI = false;
        piol = std::make_shared<ExSeisPIOL>(opt);
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
        piol->isErr();
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
    piol->isErr();
    getMinMax(piol, 10, coord.size(), NULL, minmax.data());
    piol->isErr();
    getMinMax(piol, 10, 0, coord.data(), minmax.data());
    piol->isErr();
}

