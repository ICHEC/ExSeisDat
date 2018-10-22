#include "mockrule.hh"

namespace exseis {
namespace piol {

Rule::Rule(bool full, bool defaults, bool extras)
{
    mock_rule().Rule_ctor(this, full, defaults, extras);
}

Rule::Rule(const std::vector<Meta>& m, bool full, bool defaults, bool extras)
{
    mock_rule().Rule_ctor(this, m, full, defaults, extras);
}

Rule::Rule(const Rule_entry_map& rule_entry_map, bool full)
{
    mock_rule().Rule_ctor(this, rule_entry_map, full);
}

Rule::~Rule()
{
    mock_rule().Rule_dtor(this);
}

bool Rule::add_rule(Meta m)
{
    return mock_rule().add_rule(this, m);
}

bool Rule::add_rule(const Rule& r)
{
    return mock_rule().add_rule(this, r);
}

void Rule::add_long(Meta m, Tr loc)
{
    mock_rule().add_long(this, m, loc);
}

void Rule::add_segy_float(Meta m, Tr loc, Tr scalar_location)
{
    mock_rule().add_segy_float(this, m, loc, scalar_location);
}

void Rule::add_short(Meta m, Tr loc)
{
    mock_rule().add_short(this, m, loc);
}

void Rule::add_index(Meta m)
{
    mock_rule().add_index(this, m);
}

void Rule::add_copy()
{
    mock_rule().add_copy(this);
}

void Rule::rm_rule(Meta m)
{
    mock_rule().rm_rule(this, m);
}

size_t Rule::extent()
{
    return mock_rule().extent(this);
}

size_t Rule::memory_usage() const
{
    return mock_rule().memory_usage(this);
}

size_t Rule::memory_usage_per_header() const
{
    return mock_rule().memory_usage_per_header(this);
}

const RuleEntry* Rule::get_entry(Meta entry) const
{
    return mock_rule().get_entry(this, entry);
}

}  // namespace piol
}  // namespace exseis
