#include "mockrule.hh"

namespace exseis {
namespace piol {
inline namespace metadata {
inline namespace rules {


Rule::Rule(bool full, bool defaults, bool extras)
{
    mock_rule().Rule_ctor(this, full, defaults, extras);
}

Rule::Rule(
    const std::vector<Trace_metadata_key>& m,
    bool full,
    bool defaults,
    bool extras)
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

bool Rule::add_rule(Trace_metadata_key m)
{
    return mock_rule().add_rule(this, m);
}

bool Rule::add_rule(const Rule& r)
{
    return mock_rule().add_rule(this, r);
}

void Rule::add_long(Trace_metadata_key m, Trace_header_offsets loc)
{
    mock_rule().add_long(this, m, loc);
}

void Rule::add_segy_float(
    Trace_metadata_key m,
    Trace_header_offsets loc,
    Trace_header_offsets scalar_location)
{
    mock_rule().add_segy_float(this, m, loc, scalar_location);
}

void Rule::add_short(Trace_metadata_key m, Trace_header_offsets loc)
{
    mock_rule().add_short(this, m, loc);
}

void Rule::add_index(Trace_metadata_key m)
{
    mock_rule().add_index(this, m);
}

void Rule::add_copy()
{
    mock_rule().add_copy(this);
}

void Rule::rm_rule(Trace_metadata_key m)
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

const Rule_entry* Rule::get_entry(Trace_metadata_key entry) const
{
    return mock_rule().get_entry(this, entry);
}

}  // namespace rules
}  // namespace metadata
}  // namespace piol
}  // namespace exseis
