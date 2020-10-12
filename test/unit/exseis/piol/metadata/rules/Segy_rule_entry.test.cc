#include "exseis/piol/metadata/rules/Segy_rule_entry.hh"

#include "exseis/test/catch2.hh"

TEST_CASE(
    "Segy_rule_entry", "[piol][metadata][rule][Segy_rule_entry][Rule_entry]")
{
    SECTION ("Segy_float_rule_entry") {
        exseis::Segy_float_rule_entry entry(
            exseis::segy::Trace_header_offsets::x_src,
            exseis::segy::Trace_header_offsets::ScaleCoord);

        REQUIRE(size_t(exseis::segy::Trace_header_offsets::x_src) == entry.loc);
        REQUIRE(
            size_t(exseis::segy::Trace_header_offsets::ScaleCoord)
            == entry.scalar_location);
    }

    SECTION ("Segy_short_rule_entry") {
        exseis::Segy_short_rule_entry entry(
            exseis::segy::Trace_header_offsets::ScaleCoord);

        REQUIRE(
            size_t(exseis::segy::Trace_header_offsets::ScaleCoord)
            == entry.loc);
    }

    SECTION ("Segy_long_rule_entry") {
        exseis::Segy_long_rule_entry entry(
            exseis::segy::Trace_header_offsets::il);

        REQUIRE(size_t(exseis::segy::Trace_header_offsets::il) == entry.loc);
    }
}
