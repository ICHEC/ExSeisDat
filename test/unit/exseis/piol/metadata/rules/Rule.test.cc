#include "exseis/piol/metadata/rules/Rule.hh"

#include "exseis/test/catch2.hh"

#include "exseis/piol/metadata/rules/Segy_rule_entry.hh"


// TODO: Add test for param_utils::cpyPrm called with different sets of rules,
// i.e dst and
//       src disagree on rules

TEST_CASE("Rule", "[piol][metadata][rules][Rule]")
{
    SECTION ("List") {
        std::vector<exseis::Trace_metadata_key> meta = {
            exseis::Trace_metadata_key::x_src,
            exseis::Trace_metadata_key::y_src,
            exseis::Trace_metadata_key::x_rcv,
            exseis::Trace_metadata_key::y_rcv};

        std::vector<exseis::segy::Trace_header_offsets> locs = {
            exseis::segy::Trace_header_offsets::x_src,
            exseis::segy::Trace_header_offsets::y_src,
            exseis::segy::Trace_header_offsets::x_rcv,
            exseis::segy::Trace_header_offsets::y_rcv};

        exseis::Rule rule(
            std::initializer_list<exseis::Trace_metadata_key>{
                exseis::Trace_metadata_key::x_src,
                exseis::Trace_metadata_key::y_src,
                exseis::Trace_metadata_key::x_rcv,
                exseis::Trace_metadata_key::y_rcv},
            false);

        rule.rm_rule(exseis::Trace_metadata_key::ltn);
        rule.rm_rule(exseis::Trace_metadata_key::gtn);


        SECTION ("List") {
            for (auto& m : rule.rule_entry_map) {
                const auto* entry =
                    dynamic_cast<const exseis::Segy_float_rule_entry*>(
                        m.second.get());

                int match = 0;
                for (auto& me : meta) {
                    if (me == m.first) {
                        match++;
                    }
                }
                REQUIRE(match == 1);

                match = 0;
                for (auto loc : locs) {
                    if (static_cast<size_t>(loc) == entry->loc) {
                        match++;
                    }
                }
                REQUIRE(match == 1);
                REQUIRE(
                    size_t(exseis::segy::Trace_header_offsets::ScaleCoord)
                    == entry->scalar_location);
            }
            REQUIRE(
                rule.extent()
                == size_t(locs[3])
                       - size_t(exseis::segy::Trace_header_offsets::ScaleCoord)
                       + 4U);
        }

        SECTION ("setPrm") {
            rule.add_long(
                exseis::Trace_metadata_key::dsdr,
                exseis::segy::Trace_header_offsets::SrcMeas);
            rule.add_short(
                exseis::Trace_metadata_key::il,
                exseis::segy::Trace_header_offsets::ScaleElev);

            exseis::Trace_metadata trace_metadata(rule, 100);
            for (size_t i = 0; i < 100; i++) {
                trace_metadata.set_floating_point(
                    i, exseis::Trace_metadata_key::x_src,
                    exseis::Floating_point(i) + 1.);

                trace_metadata.set_floating_point(
                    i, exseis::Trace_metadata_key::y_src,
                    exseis::Floating_point(i) + 2.);

                trace_metadata.set_floating_point(
                    i, exseis::Trace_metadata_key::x_rcv,
                    exseis::Floating_point(i) + 3.);

                trace_metadata.set_floating_point(
                    i, exseis::Trace_metadata_key::y_rcv,
                    exseis::Floating_point(i) + 4.);

                trace_metadata.set_integer(
                    i, exseis::Trace_metadata_key::dsdr,
                    exseis::Integer(i + 1));

                trace_metadata.set_integer(
                    i, exseis::Trace_metadata_key::il, short(i + 2));
            }

            for (size_t i = 0; i < 100; i++) {
                if (sizeof(exseis::Floating_point) == sizeof(double)) {
                    REQUIRE(
                        trace_metadata.get_floating_point(
                            i, exseis::Trace_metadata_key::x_src)
                        == Approx(exseis::Floating_point(i + 1)));

                    REQUIRE(
                        trace_metadata.get_floating_point(
                            i, exseis::Trace_metadata_key::y_src)
                        == Approx(exseis::Floating_point(i + 2)));

                    REQUIRE(
                        trace_metadata.get_floating_point(
                            i, exseis::Trace_metadata_key::x_rcv)
                        == Approx(exseis::Floating_point(i + 3)));

                    REQUIRE(
                        trace_metadata.get_floating_point(
                            i, exseis::Trace_metadata_key::y_rcv)
                        == Approx(exseis::Floating_point(i + 4)));
                }
                else {
                    REQUIRE(
                        trace_metadata.get_floating_point(
                            i, exseis::Trace_metadata_key::x_src)
                        == Approx(exseis::Floating_point(i + 1)));

                    REQUIRE(
                        trace_metadata.get_floating_point(
                            i, exseis::Trace_metadata_key::y_src)
                        == Approx(exseis::Floating_point(i + 2)));

                    REQUIRE(
                        trace_metadata.get_floating_point(
                            i, exseis::Trace_metadata_key::x_rcv)
                        == Approx(exseis::Floating_point(i + 3)));

                    REQUIRE(
                        trace_metadata.get_floating_point(
                            i, exseis::Trace_metadata_key::y_rcv)
                        == Approx(exseis::Floating_point(i + 4)));
                }

                REQUIRE(
                    trace_metadata.get_integer(
                        i, exseis::Trace_metadata_key::dsdr)
                    == exseis::Integer(i + 1));
                REQUIRE(
                    trace_metadata.get_integer(
                        i, exseis::Trace_metadata_key::il)
                    == short(i + 2));
            }
        }
    }

    SECTION ("Empty") {
        exseis::Rule rule{false, false};

        SECTION ("AddRmLongRules") {
            rule.add_long(
                exseis::Trace_metadata_key::xl,
                exseis::segy::Trace_header_offsets::il);

            REQUIRE(
                rule.get_entry(exseis::Trace_metadata_key::xl)->loc
                == size_t(exseis::segy::Trace_header_offsets::il));
            REQUIRE(rule.extent() == static_cast<size_t>(4));

            rule.rm_rule(exseis::Trace_metadata_key::xl);
            REQUIRE(nullptr == rule.get_entry(exseis::Trace_metadata_key::xl));
        }

        SECTION ("AddRmFloatRules") {
            rule.add_segy_float(
                exseis::Trace_metadata_key::dsdr,
                exseis::segy::Trace_header_offsets::SrcMeas,
                exseis::segy::Trace_header_offsets::SrcMeasExp);
            REQUIRE(
                nullptr != rule.get_entry(exseis::Trace_metadata_key::dsdr));

            const auto* frule =
                dynamic_cast<const exseis::Segy_float_rule_entry*>(
                    rule.get_entry(exseis::Trace_metadata_key::dsdr));
            REQUIRE(nullptr != frule);

            REQUIRE(
                frule->loc
                == size_t(exseis::segy::Trace_header_offsets::SrcMeas));
            REQUIRE(
                frule->scalar_location
                == size_t(exseis::segy::Trace_header_offsets::SrcMeasExp));

            REQUIRE(rule.extent() == static_cast<size_t>(6));

            rule.rm_rule(exseis::Trace_metadata_key::dsdr);
            REQUIRE(
                nullptr == rule.get_entry(exseis::Trace_metadata_key::dsdr));
        }

        SECTION ("Extent") {
            rule.add_segy_float(
                exseis::Trace_metadata_key::dsdr,
                exseis::segy::Trace_header_offsets::SrcMeas,
                exseis::segy::Trace_header_offsets::ScaleCoord);
            REQUIRE(
                rule.extent()
                == size_t(exseis::segy::Trace_header_offsets::SrcMeas) + 4U
                       - size_t(
                           exseis::segy::Trace_header_offsets::ScaleCoord));
        }
    }

    SECTION ("Default") {
        exseis::Rule rule{true, true};

        const auto count_type = [](const exseis::Rule& rule,
                                   exseis::Rule_entry::MdType type) {
            const auto eq_type =
                [&](const exseis::Rule::Rule_entry_map::value_type& entry_it) {
                    return entry_it.second->type() == type;
                };

            return std::count_if(
                rule.rule_entry_map.begin(), rule.rule_entry_map.end(),
                eq_type);
        };

        SECTION ("Constructor") {
            REQUIRE(rule.rule_entry_map.size() == static_cast<size_t>(12));
            REQUIRE(
                count_type(rule, exseis::Rule_entry::MdType::Float)
                == static_cast<size_t>(6));
            REQUIRE(
                count_type(rule, exseis::Rule_entry::MdType::Long)
                == static_cast<size_t>(4));
            REQUIRE(
                count_type(rule, exseis::Rule_entry::MdType::Short)
                == static_cast<size_t>(0));
            REQUIRE(
                count_type(rule, exseis::Rule_entry::MdType::Index)
                == static_cast<size_t>(2));
            REQUIRE(rule.extent() == static_cast<size_t>(240));
        }

        SECTION ("MakeCopy") {
            exseis::Rule r(rule.rule_entry_map, false);

            REQUIRE(r.rule_entry_map.size() == static_cast<size_t>(12));
            REQUIRE(
                count_type(rule, exseis::Rule_entry::MdType::Float)
                == static_cast<size_t>(6));
            REQUIRE(
                count_type(rule, exseis::Rule_entry::MdType::Long)
                == static_cast<size_t>(4));
            REQUIRE(
                count_type(rule, exseis::Rule_entry::MdType::Short)
                == static_cast<size_t>(0));
            REQUIRE(
                count_type(rule, exseis::Rule_entry::MdType::Index)
                == static_cast<size_t>(2));
            REQUIRE(r.extent() == static_cast<size_t>(192));
        }

        SECTION ("MakeCopyFull") {
            exseis::Rule r(rule.rule_entry_map, true);

            REQUIRE(r.rule_entry_map.size() == static_cast<size_t>(12));
            REQUIRE(
                count_type(rule, exseis::Rule_entry::MdType::Float)
                == static_cast<size_t>(6));
            REQUIRE(
                count_type(rule, exseis::Rule_entry::MdType::Long)
                == static_cast<size_t>(4));
            REQUIRE(
                count_type(rule, exseis::Rule_entry::MdType::Short)
                == static_cast<size_t>(0));
            REQUIRE(
                count_type(rule, exseis::Rule_entry::MdType::Index)
                == static_cast<size_t>(2));
            REQUIRE(r.extent() == static_cast<size_t>(240));
        }
    }
}
