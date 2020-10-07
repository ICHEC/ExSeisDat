#include "exseisdat/piol/segy/utils.hh"

#include "exseisdat/test/catch2.hh"


using namespace exseis::piol;


TEST_CASE("segy::find_scalar", "[piol][segy][utils][find_scalar]")
{
    // Big integers
    if (sizeof(double) == sizeof(exseis::utils::Floating_point)) {
        REQUIRE(10000 == segy::find_scalar(21474836470000.0));
        REQUIRE(1000 == segy::find_scalar(2147483647000.0));
        REQUIRE(100 == segy::find_scalar(214748364700.0));
        REQUIRE(10 == segy::find_scalar(21474836470.0));
        REQUIRE(10000 == segy::find_scalar(10000000000000.0));
        REQUIRE(1000 == segy::find_scalar(01000000000000.0));
        REQUIRE(100 == segy::find_scalar(00100000000000.0));
        REQUIRE(10 == segy::find_scalar(00010000000000.0));

        REQUIRE(10000 == segy::find_scalar(21474836470000.0000999));
        REQUIRE(1000 == segy::find_scalar(2147483647000.0000999));
        REQUIRE(100 == segy::find_scalar(214748364700.0000999));
        REQUIRE(10 == segy::find_scalar(21474836470.0000999));
    }
    else if (sizeof(float) == sizeof(exseis::utils::Floating_point)) {
        // Fix these due to guaranteed big precision loss
        REQUIRE(10000 == segy::find_scalar(21474836470000.0));
        REQUIRE(1000 == segy::find_scalar(2147483647000.0));
        REQUIRE(100 == segy::find_scalar(214748364700.0));
        REQUIRE(10 == segy::find_scalar(21474836470.0));

        REQUIRE(10000 == segy::find_scalar(10000000000000.0));  // ok
        REQUIRE(1000 == segy::find_scalar(01000000000000.0));   // ok
        REQUIRE(100 == segy::find_scalar(00100000000000.0));    // ok
        REQUIRE(10 == segy::find_scalar(00010000000000.0));     // ok

        REQUIRE(10000 == segy::find_scalar(21474836470000.0000999));
        REQUIRE(1000 == segy::find_scalar(2147483647000.0000999));
        REQUIRE(100 == segy::find_scalar(214748364700.0000999));
        REQUIRE(10 == segy::find_scalar(21474836470.0000999));
    }

    REQUIRE(-10000 == segy::find_scalar(214748.3647));
    REQUIRE(-1000 == segy::find_scalar(2147483.647));
    REQUIRE(-100 == segy::find_scalar(21474836.47));
    REQUIRE(-10 == segy::find_scalar(214748364.7));
    REQUIRE(1 == segy::find_scalar(2147483647.));

    REQUIRE(-10000 == segy::find_scalar(1.0001));
    REQUIRE(-1000 == segy::find_scalar(1.001));
    REQUIRE(-100 == segy::find_scalar(1.01));
    REQUIRE(-10 == segy::find_scalar(1.1));
    REQUIRE(1 == segy::find_scalar(1.));

    REQUIRE(-10000 == segy::find_scalar(0.0001));
    REQUIRE(-1000 == segy::find_scalar(0.001));
    REQUIRE(-100 == segy::find_scalar(0.01));
    REQUIRE(-10 == segy::find_scalar(0.1));
    REQUIRE(1 == segy::find_scalar(0.));

    // Tests for case where round mode pushes sig figs over sizes we can handle
    REQUIRE(-10000 == segy::find_scalar(214748.3647199));
    REQUIRE(-10 == segy::find_scalar(214748364.7000999));
    REQUIRE(-100 == segy::find_scalar(21474836.4700999));
    REQUIRE(-1000 == segy::find_scalar(2147483.6470999));
    REQUIRE(1 == segy::find_scalar(2147483647.0000999));
}

TEST_CASE("segy::parse_scalar", "[piol][segy][utils][parse_scalar]")
{
    // Test every legitimate possibility
    REQUIRE(
        1 / (exseis::utils::Floating_point(10000))
        == segy::parse_scalar(-10000));
    REQUIRE(
        1 / (exseis::utils::Floating_point(1000)) == segy::parse_scalar(-1000));
    REQUIRE(
        1 / (exseis::utils::Floating_point(100)) == segy::parse_scalar(-100));
    REQUIRE(1 / (exseis::utils::Floating_point(10)) == segy::parse_scalar(-10));
    REQUIRE(1 / (exseis::utils::Floating_point(1)) == segy::parse_scalar(-1));
    REQUIRE(exseis::utils::Floating_point(1) == segy::parse_scalar(0));
    REQUIRE(exseis::utils::Floating_point(1) == segy::parse_scalar(1));
    REQUIRE(exseis::utils::Floating_point(10) == segy::parse_scalar(10));
    REQUIRE(exseis::utils::Floating_point(100) == segy::parse_scalar(100));
    REQUIRE(exseis::utils::Floating_point(1000) == segy::parse_scalar(1000));
    REQUIRE(exseis::utils::Floating_point(10000) == segy::parse_scalar(10000));
}
