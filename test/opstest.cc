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

TEST_F(OpsTest, min)
{
    std::vector<coord_t> vals(1000);
    for (int i = 0; i < 1000; i++)
        vals[i] = { 1500. + i, 1300. - i };
    vals[400] = std::make_pair(1., 1.);

    ASSERT_EQ(1., xmin(piol, vals.size(), vals.data()));
}

