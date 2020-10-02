#include "dynsegymdtest.hh"

// TODO: Add test for param_utils::cpyPrm called with different sets of rules,
// i.e dst and
//       src disagree on rules

TEST(, SEGYFloat)
{
    Segy_float_rule_entry entry(
        segy::Trace_header_offsets::x_src,
        segy::Trace_header_offsets::ScaleCoord);
    ASSERT_EQ(size_t(segy::Trace_header_offsets::x_src), entry.loc);
    ASSERT_EQ(
        size_t(segy::Trace_header_offsets::ScaleCoord), entry.scalar_location);
}

TEST(Rule_entry, SEGYShort)
{
    Segy_short_rule_entry entry(segy::Trace_header_offsets::ScaleCoord);
    ASSERT_EQ(size_t(segy::Trace_header_offsets::ScaleCoord), entry.loc);
}

TEST(Rule_entry, SEGYLong)
{
    Segy_long_rule_entry entry(segy::Trace_header_offsets::il);
    ASSERT_EQ(size_t(segy::Trace_header_offsets::il), entry.loc);
}


TEST_F(RuleFixList, List)
{
    size_t i = 0;
    for (auto& m : rule->rule_entry_map) {
        const auto entry =
            dynamic_cast<const Segy_float_rule_entry*>(m.second.get());

        int match = 0;
        for (auto& me : meta) {
            if (me == m.first) {
                match++;
            }
        }
        EXPECT_EQ(match, 1) << i;

        match = 0;
        for (auto loc : locs) {
            if (static_cast<size_t>(loc) == entry->loc) {
                match++;
            }
        }
        ASSERT_EQ(match, 1) << i;
        ASSERT_EQ(
            size_t(segy::Trace_header_offsets::ScaleCoord),
            entry->scalar_location)
            << i;
    }
    ASSERT_EQ(
        rule->extent(),
        size_t(locs[3]) - size_t(segy::Trace_header_offsets::ScaleCoord) + 4U);
}

TEST_F(RuleFixEmpty, AddRmLongRules)
{
    rule->add_long(Trace_metadata_key::xl, segy::Trace_header_offsets::il);
    ASSERT_EQ(
        rule->get_entry(Trace_metadata_key::xl)->loc,
        size_t(segy::Trace_header_offsets::il));
    ASSERT_EQ(rule->extent(), static_cast<size_t>(4));
    rule->rm_rule(Trace_metadata_key::xl);
    ASSERT_EQ(NULL, rule->get_entry(Trace_metadata_key::xl));
}

TEST_F(RuleFixEmpty, AddRmFloatRules)
{
    rule->add_segy_float(
        Trace_metadata_key::dsdr, segy::Trace_header_offsets::SrcMeas,
        segy::Trace_header_offsets::SrcMeasExp);
    ASSERT_NE(nullptr, rule->get_entry(Trace_metadata_key::dsdr));
    const auto frule = dynamic_cast<const Segy_float_rule_entry*>(
        rule->get_entry(Trace_metadata_key::dsdr));
    ASSERT_NE(nullptr, frule);
    ASSERT_EQ(frule->loc, size_t(segy::Trace_header_offsets::SrcMeas));
    ASSERT_EQ(
        frule->scalar_location, size_t(segy::Trace_header_offsets::SrcMeasExp));
    ASSERT_EQ(rule->extent(), static_cast<size_t>(6));
    rule->rm_rule(Trace_metadata_key::dsdr);
    ASSERT_EQ(nullptr, rule->get_entry(Trace_metadata_key::dsdr));
}

TEST_F(RuleFixEmpty, Extent)
{
    rule->add_segy_float(
        Trace_metadata_key::dsdr, segy::Trace_header_offsets::SrcMeas,
        segy::Trace_header_offsets::ScaleCoord);
    ASSERT_EQ(
        rule->extent(), size_t(segy::Trace_header_offsets::SrcMeas) + 4U
                            - size_t(segy::Trace_header_offsets::ScaleCoord));
}

TEST_F(RuleFixList, setPrm)
{
    rule->add_long(
        Trace_metadata_key::dsdr, segy::Trace_header_offsets::SrcMeas);
    rule->add_short(
        Trace_metadata_key::il, segy::Trace_header_offsets::ScaleElev);

    Trace_metadata prm(*rule, 100);
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
            ASSERT_DOUBLE_EQ(
                prm.get_floating_point(i, Trace_metadata_key::x_src),
                exseis::utils::Floating_point(i + 1));

            ASSERT_DOUBLE_EQ(
                prm.get_floating_point(i, Trace_metadata_key::y_src),
                exseis::utils::Floating_point(i + 2));

            ASSERT_DOUBLE_EQ(
                prm.get_floating_point(i, Trace_metadata_key::x_rcv),
                exseis::utils::Floating_point(i + 3));

            ASSERT_DOUBLE_EQ(
                prm.get_floating_point(i, Trace_metadata_key::y_rcv),
                exseis::utils::Floating_point(i + 4));
        }
        else {
            ASSERT_FLOAT_EQ(
                prm.get_floating_point(i, Trace_metadata_key::x_src),
                exseis::utils::Floating_point(i + 1));

            ASSERT_FLOAT_EQ(
                prm.get_floating_point(i, Trace_metadata_key::y_src),
                exseis::utils::Floating_point(i + 2));

            ASSERT_FLOAT_EQ(
                prm.get_floating_point(i, Trace_metadata_key::x_rcv),
                exseis::utils::Floating_point(i + 3));

            ASSERT_FLOAT_EQ(
                prm.get_floating_point(i, Trace_metadata_key::y_rcv),
                exseis::utils::Floating_point(i + 4));
        }

        ASSERT_EQ(
            prm.get_integer(i, Trace_metadata_key::dsdr),
            exseis::utils::Integer(i + 1));
        ASSERT_EQ(prm.get_integer(i, Trace_metadata_key::il), short(i + 2));
    }
}

namespace {
/// @brief Predicate generator for count_if, running over a std::map with a
///        Rule_entry as the mapped type. Returns a predicate which returns true
///        if the mapped type is equal to the requested type.
///
/// @param[in] type The MdType to compare against.
///
/// @return A predicate which returns true of the mapped type of a std::map
///         iterator has the type `type`.
std::function<bool(const Rule::Rule_entry_map::value_type&)> eq_type(
    Rule_entry::MdType type)
{
    return [=](const Rule::Rule_entry_map::value_type& entry_it) -> bool {
        return entry_it.second->type() == type;
    };
}

/// @brief Counts the number of entries of `type` in `rule`.
///
/// @param[in] rule The rule structure to count from.
/// @param[in] type The type of data to count.
///
/// @return The number of entries of `type` in `rule`.
///
size_t count_type(const Rule& rule, Rule_entry::MdType type)
{
    return std::count_if(
        rule.rule_entry_map.begin(), rule.rule_entry_map.end(), eq_type(type));
}
}  // namespace


TEST_F(RuleFixDefault, Constructor)
{
    ASSERT_EQ(rule->rule_entry_map.size(), static_cast<size_t>(12));
    ASSERT_EQ(
        count_type(*rule, Rule_entry::MdType::Float), static_cast<size_t>(6));
    ASSERT_EQ(
        count_type(*rule, Rule_entry::MdType::Long), static_cast<size_t>(4));
    ASSERT_EQ(
        count_type(*rule, Rule_entry::MdType::Short), static_cast<size_t>(0));
    ASSERT_EQ(
        count_type(*rule, Rule_entry::MdType::Index), static_cast<size_t>(2));
    ASSERT_EQ(rule->extent(), static_cast<size_t>(240));
}

TEST_F(RuleFixDefault, MakeCopy)
{
    auto r = new Rule(rule->rule_entry_map, false);
    ASSERT_EQ(r->rule_entry_map.size(), static_cast<size_t>(12));
    ASSERT_EQ(
        count_type(*rule, Rule_entry::MdType::Float), static_cast<size_t>(6));
    ASSERT_EQ(
        count_type(*rule, Rule_entry::MdType::Long), static_cast<size_t>(4));
    ASSERT_EQ(
        count_type(*rule, Rule_entry::MdType::Short), static_cast<size_t>(0));
    ASSERT_EQ(
        count_type(*rule, Rule_entry::MdType::Index), static_cast<size_t>(2));
    ASSERT_EQ(r->extent(), static_cast<size_t>(192));
}

TEST_F(RuleFixDefault, MakeCopyFull)
{
    auto r = new Rule(rule->rule_entry_map, true);
    ASSERT_EQ(r->rule_entry_map.size(), static_cast<size_t>(12));
    ASSERT_EQ(
        count_type(*rule, Rule_entry::MdType::Float), static_cast<size_t>(6));
    ASSERT_EQ(
        count_type(*rule, Rule_entry::MdType::Long), static_cast<size_t>(4));
    ASSERT_EQ(
        count_type(*rule, Rule_entry::MdType::Short), static_cast<size_t>(0));
    ASSERT_EQ(
        count_type(*rule, Rule_entry::MdType::Index), static_cast<size_t>(2));
    ASSERT_EQ(r->extent(), static_cast<size_t>(240));
}
