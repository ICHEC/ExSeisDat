#include "exseisdat/piol/metadata/rules/Rule.hh"

#include "exseisdat/test/catch2.hh"

#include "exseisdat/piol/metadata/rules/Segy_rule_entry.hh"


// TODO: Add test for param_utils::cpyPrm called with different sets of rules,
// i.e dst and
//       src disagree on rules

TEST_CASE("Rule", "[piol][metadata][rules][Rule]")
{
    using namespace exseis::piol;

    SECTION ("List") {
        std::vector<Trace_metadata_key> meta = {
            Trace_metadata_key::x_src, Trace_metadata_key::y_src,
            Trace_metadata_key::x_rcv, Trace_metadata_key::y_rcv};

        std::vector<segy::Trace_header_offsets> locs = {
            segy::Trace_header_offsets::x_src,
            segy::Trace_header_offsets::y_src,
            segy::Trace_header_offsets::x_rcv,
            segy::Trace_header_offsets::y_rcv};

        Rule rule(
            std::initializer_list<Trace_metadata_key>{
                Trace_metadata_key::x_src, Trace_metadata_key::y_src,
                Trace_metadata_key::x_rcv, Trace_metadata_key::y_rcv},
            false);

        rule.rm_rule(Trace_metadata_key::ltn);
        rule.rm_rule(Trace_metadata_key::gtn);


        SECTION ("List") {
            for (auto& m : rule.rule_entry_map) {
                const auto* entry =
                    dynamic_cast<const Segy_float_rule_entry*>(m.second.get());

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
                    size_t(segy::Trace_header_offsets::ScaleCoord)
                    == entry->scalar_location);
            }
            REQUIRE(
                rule.extent()
                == size_t(locs[3])
                       - size_t(segy::Trace_header_offsets::ScaleCoord) + 4U);
        }

        SECTION ("setPrm") {
            rule.add_long(
                Trace_metadata_key::dsdr, segy::Trace_header_offsets::SrcMeas);
            rule.add_short(
                Trace_metadata_key::il, segy::Trace_header_offsets::ScaleElev);

            Trace_metadata prm(rule, 100);
            for (size_t i = 0; i < 100; i++) {
                prm.set_floating_point(
                    i, Trace_metadata_key::x_src,
                    exseis::utils::Floating_point(i) + 1.);

                prm.set_floating_point(
                    i, Trace_metadata_key::y_src,
                    exseis::utils::Floating_point(i) + 2.);

                prm.set_floating_point(
                    i, Trace_metadata_key::x_rcv,
                    exseis::utils::Floating_point(i) + 3.);

                prm.set_floating_point(
                    i, Trace_metadata_key::y_rcv,
                    exseis::utils::Floating_point(i) + 4.);

                prm.set_integer(
                    i, Trace_metadata_key::dsdr, exseis::utils::Integer(i + 1));

                prm.set_integer(i, Trace_metadata_key::il, short(i + 2));
            }

            for (size_t i = 0; i < 100; i++) {
                if (sizeof(exseis::utils::Floating_point) == sizeof(double)) {
                    REQUIRE(
                        prm.get_floating_point(i, Trace_metadata_key::x_src)
                        == Approx(exseis::utils::Floating_point(i + 1)));

                    REQUIRE(
                        prm.get_floating_point(i, Trace_metadata_key::y_src)
                        == Approx(exseis::utils::Floating_point(i + 2)));

                    REQUIRE(
                        prm.get_floating_point(i, Trace_metadata_key::x_rcv)
                        == Approx(exseis::utils::Floating_point(i + 3)));

                    REQUIRE(
                        prm.get_floating_point(i, Trace_metadata_key::y_rcv)
                        == Approx(exseis::utils::Floating_point(i + 4)));
                }
                else {
                    REQUIRE(
                        prm.get_floating_point(i, Trace_metadata_key::x_src)
                        == Approx(exseis::utils::Floating_point(i + 1)));

                    REQUIRE(
                        prm.get_floating_point(i, Trace_metadata_key::y_src)
                        == Approx(exseis::utils::Floating_point(i + 2)));

                    REQUIRE(
                        prm.get_floating_point(i, Trace_metadata_key::x_rcv)
                        == Approx(exseis::utils::Floating_point(i + 3)));

                    REQUIRE(
                        prm.get_floating_point(i, Trace_metadata_key::y_rcv)
                        == Approx(exseis::utils::Floating_point(i + 4)));
                }

                REQUIRE(
                    prm.get_integer(i, Trace_metadata_key::dsdr)
                    == exseis::utils::Integer(i + 1));
                REQUIRE(
                    prm.get_integer(i, Trace_metadata_key::il) == short(i + 2));
            }
        }
    }

    SECTION ("Empty") {
        Rule rule{false, false};

        SECTION ("AddRmLongRules") {
            rule.add_long(
                Trace_metadata_key::xl, segy::Trace_header_offsets::il);

            REQUIRE(
                rule.get_entry(Trace_metadata_key::xl)->loc
                == size_t(segy::Trace_header_offsets::il));
            REQUIRE(rule.extent() == static_cast<size_t>(4));

            rule.rm_rule(Trace_metadata_key::xl);
            REQUIRE(nullptr == rule.get_entry(Trace_metadata_key::xl));
        }

        SECTION ("AddRmFloatRules") {
            rule.add_segy_float(
                Trace_metadata_key::dsdr, segy::Trace_header_offsets::SrcMeas,
                segy::Trace_header_offsets::SrcMeasExp);
            REQUIRE(nullptr != rule.get_entry(Trace_metadata_key::dsdr));

            const auto* frule = dynamic_cast<const Segy_float_rule_entry*>(
                rule.get_entry(Trace_metadata_key::dsdr));
            REQUIRE(nullptr != frule);

            REQUIRE(frule->loc == size_t(segy::Trace_header_offsets::SrcMeas));
            REQUIRE(
                frule->scalar_location
                == size_t(segy::Trace_header_offsets::SrcMeasExp));

            REQUIRE(rule.extent() == static_cast<size_t>(6));

            rule.rm_rule(Trace_metadata_key::dsdr);
            REQUIRE(nullptr == rule.get_entry(Trace_metadata_key::dsdr));
        }

        SECTION ("Extent") {
            rule.add_segy_float(
                Trace_metadata_key::dsdr, segy::Trace_header_offsets::SrcMeas,
                segy::Trace_header_offsets::ScaleCoord);
            REQUIRE(
                rule.extent()
                == size_t(segy::Trace_header_offsets::SrcMeas) + 4U
                       - size_t(segy::Trace_header_offsets::ScaleCoord));
        }
    }

    SECTION ("Default") {
        Rule rule{true, true};

        const auto count_type = [](const Rule& rule, Rule_entry::MdType type) {
            const auto eq_type =
                [&](const Rule::Rule_entry_map::value_type& entry_it) {
                    return entry_it.second->type() == type;
                };

            return std::count_if(
                rule.rule_entry_map.begin(), rule.rule_entry_map.end(),
                eq_type);
        };

        SECTION ("Constructor") {
            REQUIRE(rule.rule_entry_map.size() == static_cast<size_t>(12));
            REQUIRE(
                count_type(rule, Rule_entry::MdType::Float)
                == static_cast<size_t>(6));
            REQUIRE(
                count_type(rule, Rule_entry::MdType::Long)
                == static_cast<size_t>(4));
            REQUIRE(
                count_type(rule, Rule_entry::MdType::Short)
                == static_cast<size_t>(0));
            REQUIRE(
                count_type(rule, Rule_entry::MdType::Index)
                == static_cast<size_t>(2));
            REQUIRE(rule.extent() == static_cast<size_t>(240));
        }

        SECTION ("MakeCopy") {
            Rule r(rule.rule_entry_map, false);

            REQUIRE(r.rule_entry_map.size() == static_cast<size_t>(12));
            REQUIRE(
                count_type(rule, Rule_entry::MdType::Float)
                == static_cast<size_t>(6));
            REQUIRE(
                count_type(rule, Rule_entry::MdType::Long)
                == static_cast<size_t>(4));
            REQUIRE(
                count_type(rule, Rule_entry::MdType::Short)
                == static_cast<size_t>(0));
            REQUIRE(
                count_type(rule, Rule_entry::MdType::Index)
                == static_cast<size_t>(2));
            REQUIRE(r.extent() == static_cast<size_t>(192));
        }

        SECTION ("MakeCopyFull") {
            Rule r(rule.rule_entry_map, true);

            REQUIRE(r.rule_entry_map.size() == static_cast<size_t>(12));
            REQUIRE(
                count_type(rule, Rule_entry::MdType::Float)
                == static_cast<size_t>(6));
            REQUIRE(
                count_type(rule, Rule_entry::MdType::Long)
                == static_cast<size_t>(4));
            REQUIRE(
                count_type(rule, Rule_entry::MdType::Short)
                == static_cast<size_t>(0));
            REQUIRE(
                count_type(rule, Rule_entry::MdType::Index)
                == static_cast<size_t>(2));
            REQUIRE(r.extent() == static_cast<size_t>(240));
        }
    }
}
