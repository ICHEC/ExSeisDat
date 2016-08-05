#include "share/segy.hh"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
using namespace PIOL;

TEST(SEGYSizes, All)
{
    ASSERT_EQ(3600U, SEGSz::getHOSz());
    ASSERT_EQ(240U,  SEGSz::getMDSz());
    ASSERT_EQ(3600U + (1111U*4U + 240U)*3333U, SEGSz::getDOLoc(3333, 1111));
}

