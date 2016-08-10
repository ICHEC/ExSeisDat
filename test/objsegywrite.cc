#include "objsegytest.hh"
TEST_F(ObjSpecTest, SEGYTrWrite)
{
    makeSEGY();
    ExpectWriteTrHdrPattern(10U, 200U);
}

TEST_F(ObjSpecTest, SEGYTrWrite2)
{
    makeSEGY();
    ExpectWriteTrHdrPattern(10U, 200U);
}

