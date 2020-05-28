#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "exseisdat/piol/segy/utils.hh"

#include "exseisdat/piol/segy/utils.hh"

#include <memory>
#include <string.h>

////////////////////// Unit tests of non-class functions ///////////////////////

using namespace exseis::piol;
using namespace exseis::piol::segy;

TEST(SEGYMd, ScaleBigIntegers)
{
    if (sizeof(double) == sizeof(exseis::utils::Floating_point)) {
        EXPECT_EQ(10000, segy::find_scalar(21474836470000.0));
        EXPECT_EQ(1000, segy::find_scalar(2147483647000.0));
        EXPECT_EQ(100, segy::find_scalar(214748364700.0));
        EXPECT_EQ(10, segy::find_scalar(21474836470.0));
        EXPECT_EQ(10000, segy::find_scalar(10000000000000.0));
        EXPECT_EQ(1000, segy::find_scalar(01000000000000.0));
        EXPECT_EQ(100, segy::find_scalar(00100000000000.0));
        EXPECT_EQ(10, segy::find_scalar(00010000000000.0));

        EXPECT_EQ(10000, segy::find_scalar(21474836470000.0000999));
        EXPECT_EQ(1000, segy::find_scalar(2147483647000.0000999));
        EXPECT_EQ(100, segy::find_scalar(214748364700.0000999));
        EXPECT_EQ(10, segy::find_scalar(21474836470.0000999));
    }
    else if (sizeof(float) == sizeof(exseis::utils::Floating_point)) {
        // Fix these due to guaranteed big precision loss
        EXPECT_EQ(10000, segy::find_scalar(21474836470000.0));
        EXPECT_EQ(1000, segy::find_scalar(2147483647000.0));
        EXPECT_EQ(100, segy::find_scalar(214748364700.0));
        EXPECT_EQ(10, segy::find_scalar(21474836470.0));

        EXPECT_EQ(10000, segy::find_scalar(10000000000000.0));  // ok
        EXPECT_EQ(1000, segy::find_scalar(01000000000000.0));   // ok
        EXPECT_EQ(100, segy::find_scalar(00100000000000.0));    // ok
        EXPECT_EQ(10, segy::find_scalar(00010000000000.0));     // ok

        EXPECT_EQ(10000, segy::find_scalar(21474836470000.0000999));
        EXPECT_EQ(1000, segy::find_scalar(2147483647000.0000999));
        EXPECT_EQ(100, segy::find_scalar(214748364700.0000999));
        EXPECT_EQ(10, segy::find_scalar(21474836470.0000999));
    }
}

TEST(SEGYMd, ScaleDecimals)
{
    EXPECT_EQ(-10000, segy::find_scalar(214748.3647));
    EXPECT_EQ(-1000, segy::find_scalar(2147483.647));
    EXPECT_EQ(-100, segy::find_scalar(21474836.47));
    EXPECT_EQ(-10, segy::find_scalar(214748364.7));
    EXPECT_EQ(1, segy::find_scalar(2147483647.));

    EXPECT_EQ(-10000, segy::find_scalar(1.0001));
    EXPECT_EQ(-1000, segy::find_scalar(1.001));
    EXPECT_EQ(-100, segy::find_scalar(1.01));
    EXPECT_EQ(-10, segy::find_scalar(1.1));
    EXPECT_EQ(1, segy::find_scalar(1.));

    EXPECT_EQ(-10000, segy::find_scalar(0.0001));
    EXPECT_EQ(-1000, segy::find_scalar(0.001));
    EXPECT_EQ(-100, segy::find_scalar(0.01));
    EXPECT_EQ(-10, segy::find_scalar(0.1));
    EXPECT_EQ(1, segy::find_scalar(0.));

    // Tests for case where round mode pushes sig figs over sizes we can handle
    EXPECT_EQ(-10000, segy::find_scalar(214748.3647199));
    EXPECT_EQ(-10, segy::find_scalar(214748364.7000999));
    EXPECT_EQ(-100, segy::find_scalar(21474836.4700999));
    EXPECT_EQ(-1000, segy::find_scalar(2147483.6470999));
    EXPECT_EQ(1, segy::find_scalar(2147483647.0000999));
}

TEST(SEGYMd, scaleConv)
{
    // Test every legitimate possibility
    EXPECT_DOUBLE_EQ(
        1 / (exseis::utils::Floating_point(10000)), segy::parse_scalar(-10000));
    EXPECT_DOUBLE_EQ(
        1 / (exseis::utils::Floating_point(1000)), segy::parse_scalar(-1000));
    EXPECT_DOUBLE_EQ(
        1 / (exseis::utils::Floating_point(100)), segy::parse_scalar(-100));
    EXPECT_DOUBLE_EQ(
        1 / (exseis::utils::Floating_point(10)), segy::parse_scalar(-10));
    EXPECT_DOUBLE_EQ(
        1 / (exseis::utils::Floating_point(1)), segy::parse_scalar(-1));
    EXPECT_DOUBLE_EQ(exseis::utils::Floating_point(1), segy::parse_scalar(0));
    EXPECT_DOUBLE_EQ(exseis::utils::Floating_point(1), segy::parse_scalar(1));
    EXPECT_DOUBLE_EQ(exseis::utils::Floating_point(10), segy::parse_scalar(10));
    EXPECT_DOUBLE_EQ(
        exseis::utils::Floating_point(100), segy::parse_scalar(100));
    EXPECT_DOUBLE_EQ(
        exseis::utils::Floating_point(1000), segy::parse_scalar(1000));
    EXPECT_DOUBLE_EQ(
        exseis::utils::Floating_point(10000), segy::parse_scalar(10000));
}
