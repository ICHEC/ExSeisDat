#include "exseisdat/piol/metadata/rules/Segy_rule_entry.hh"

#include "exseisdat/test/catch2.hh"

using namespace exseis::piol;

TEST_CASE(
    "Segy_rule_entry", "[piol][metadata][rule][Segy_rule_entry][Rule_entry]")
{
    SECTION ("Segy_float_rule_entry") {
        Segy_float_rule_entry entry(
            segy::Trace_header_offsets::x_src,
            segy::Trace_header_offsets::ScaleCoord);

        REQUIRE(size_t(segy::Trace_header_offsets::x_src) == entry.loc);
        REQUIRE(
            size_t(segy::Trace_header_offsets::ScaleCoord)
            == entry.scalar_location);
    }

    SECTION ("Segy_short_rule_entry") {
        Segy_short_rule_entry entry(segy::Trace_header_offsets::ScaleCoord);

        REQUIRE(size_t(segy::Trace_header_offsets::ScaleCoord) == entry.loc);
    }

    SECTION ("Segy_long_rule_entry") {
        Segy_long_rule_entry entry(segy::Trace_header_offsets::il);

        REQUIRE(size_t(segy::Trace_header_offsets::il) == entry.loc);
    }
}
