////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/piol/Trace_metadata.hh"
#include "exseisdat/piol/segy/utils.hh"

#include <algorithm>
#include <cassert>
#include <numeric>
#include <stdexcept>
#include <string>

namespace exseis {
namespace piol {

Trace_metadata::Trace_metadata(Rule rules, size_t num_traces) :
    rules(std::move(rules)),
    num_traces(num_traces)
{
    for (const auto& rule_map_it : rules.rule_entry_map) {
        const auto& key   = rule_map_it.first;
        const auto& value = rule_map_it.second;

        using exseis::utils::Floating_point;
        using exseis::utils::Generic_array;
        using exseis::utils::Integer;

        entry_types.insert({key, value->type()});

        switch (value->type()) {
            case RuleEntry::MdType::Float:
                m_floating_point_entries.insert(
                  {key, Generic_array<Floating_point>(
                          std::vector<Floating_point>(num_traces))});

                assert(
                  m_floating_point_entries.at(key).size() == num_traces
                  && "Expected m_floating_point_entries[key].size() == num_traces!");
                break;

            case RuleEntry::MdType::Long:
                m_integer_entries.insert(
                  {key,
                   Generic_array<Integer>(std::vector<Integer>(num_traces))});

                assert(
                  m_integer_entries.at(key).size() == num_traces
                  && "Expected m_integer_entries[key].size() == num_traces!");

                break;

            case RuleEntry::MdType::Short:
                m_integer_entries.insert(
                  {key,
                   Generic_array<Integer>(std::vector<int16_t>(num_traces))});

                assert(
                  m_integer_entries.at(key).size() == num_traces
                  && "Expected m_integer_entries[key].size() == num_traces!");

                break;

            case RuleEntry::MdType::Index:
                m_index_entries.insert(
                  {key,
                   Generic_array<uint64_t>(std::vector<uint64_t>(num_traces))});

                assert(
                  m_index_entries.at(key).size() == num_traces
                  && "Expected m_index_entries[key].size() == num_traces!");

                break;

            case RuleEntry::MdType::Copy:
                // Explicitly ignored, handled below.
                break;
        }
    }

    // @todo: This must be file format agnostic
    if (rules.num_copy > 0) {
        // @todo put block size into the rules
        raw_metadata_block_size = segy::segy_trace_header_size();
        raw_metadata.resize(num_traces * raw_metadata_block_size);
    }
}

Trace_metadata::Trace_metadata(size_t num_traces) :
    Trace_metadata::Trace_metadata(Rule(true, true), num_traces)
{
}

Trace_metadata::~Trace_metadata() = default;


exseis::utils::Floating_point Trace_metadata::get_floating_point(
  size_t trace_index, Key entry) const
{
    const auto it = m_floating_point_entries.find(entry);

    if (it == m_floating_point_entries.end()) {
        throw Entry_not_found(std::string(
          "Trace_metadata::get_floating_point : Floating point entry not found: "
          + std::to_string(
              static_cast<std::underlying_type<Meta>::type>(entry))));
    }

    return it->second.get(trace_index);
}

void Trace_metadata::set_floating_point(
  size_t trace_index, Key entry, exseis::utils::Floating_point value)
{
    auto it = m_floating_point_entries.find(entry);

    if (it == m_floating_point_entries.end()) {
        throw Entry_not_found(std::string(
          "Trace_metadata::set_floating_point : Floating point entry not found: "
          + std::to_string(
              static_cast<std::underlying_type<Meta>::type>(entry))));
    }

    it->second.set(trace_index, value);
}

exseis::utils::Integer Trace_metadata::get_integer(
  size_t trace_index, Key entry) const
{
    const auto it = m_integer_entries.find(entry);

    if (it == m_integer_entries.end()) {
        throw Entry_not_found(std::string(
          "Trace_metadata::get_integer : Integer entry not found: "
          + std::to_string(
              static_cast<std::underlying_type<Meta>::type>(entry))));
    }

    return it->second.get(trace_index);
}

void Trace_metadata::set_integer(
  size_t trace_index, Key entry, exseis::utils::Integer value)
{
    auto it = m_integer_entries.find(entry);

    if (it == m_integer_entries.end()) {
        throw Entry_not_found(std::string(
          "Trace_metadata::set_integer : Integer entry not found: "
          + std::to_string(
              static_cast<std::underlying_type<Meta>::type>(entry))));
    }

    return it->second.set(trace_index, value);
}

size_t Trace_metadata::get_index(size_t trace_index, Key entry) const
{
    const auto it = m_index_entries.find(entry);

    if (it == m_index_entries.end()) {
        throw Entry_not_found(std::string(
          "Trace_metadata::get_index : Index entry not found: "
          + std::to_string(
              static_cast<std::underlying_type<Meta>::type>(entry))));
    }

    return it->second.get(trace_index);
}

void Trace_metadata::set_index(size_t trace_index, Key entry, size_t value)
{
    auto it = m_index_entries.find(entry);

    if (it == m_index_entries.end()) {
        throw Entry_not_found(std::string(
          "Trace_metadata::set_index : Index entry not found: "
          + std::to_string(
              static_cast<std::underlying_type<Meta>::type>(entry))));
    }

    it->second.set(trace_index, value);
}


void Trace_metadata::copy_entries(
  size_t trace_index,
  const Trace_metadata& source_metadata,
  size_t source_trace_index)
{
    if (source_metadata.raw_metadata_block_size > 0) {
        if (
          source_metadata.raw_metadata_block_size == raw_metadata_block_size) {
            const unsigned char* source_start =
              &(source_metadata
                  .raw_metadata[source_trace_index * raw_metadata_block_size]);

            std::copy_n(
              source_start, raw_metadata_block_size,
              &(raw_metadata[trace_index * raw_metadata_block_size]));
        }
    }

    for (const auto& kv_it : source_metadata.entry_types) {
        const auto& key  = kv_it.first;
        const auto& type = kv_it.second;

        if (entry_types.find(key) != entry_types.end()) {
            switch (type) {
                case RuleEntry::MdType::Float: {
                    const auto source_value =
                      source_metadata.get_floating_point(
                        source_trace_index, key);

                    set_floating_point(trace_index, key, source_value);
                } break;

                case RuleEntry::MdType::Long:
                case RuleEntry::MdType::Short: {
                    const auto source_value =
                      source_metadata.get_integer(source_trace_index, key);

                    set_integer(trace_index, key, source_value);
                } break;

                case RuleEntry::MdType::Index: {
                    const auto source_value =
                      source_metadata.get_index(source_trace_index, key);

                    set_index(trace_index, key, source_value);
                } break;

                case RuleEntry::MdType::Copy:
                    // Do nothing
                    break;
            }
        }
    }
}

size_t Trace_metadata::entry_size(Key entry) const
{
    const auto it = entry_types.find(entry);

    if (it == entry_types.end()) {
        throw Entry_not_found(std::string(
          "Trace_metadata::entry_size : entry not found: "
          + std::to_string(
              static_cast<std::underlying_type<Meta>::type>(entry))));
    }

    switch (it->second) {
        case RuleEntry::MdType::Float:
            return m_floating_point_entries.at(entry).size();

        case RuleEntry::MdType::Long:
        case RuleEntry::MdType::Short:
            return m_integer_entries.at(entry).size();

        case RuleEntry::MdType::Index:
            return m_index_entries.at(entry).size();

        case RuleEntry::MdType::Copy:
            assert(false && "Unexpected entry type MdType::Copy");
            break;
    }
    assert(false && "Unknown entry type!");
    return 0;
}


exseis::utils::Type Trace_metadata::entry_type(Key entry) const
{
    const auto it = entry_types.find(entry);

    if (it == entry_types.end()) {
        throw Entry_not_found(std::string(
          "Trace_metadata::entry_size : entry not found: "
          + std::to_string(
              static_cast<std::underlying_type<Meta>::type>(entry))));
    }

    switch (it->second) {
        case RuleEntry::MdType::Float:
            return m_floating_point_entries.at(entry).type();

        case RuleEntry::MdType::Long:
        case RuleEntry::MdType::Short:
            return m_integer_entries.at(entry).type();

        case RuleEntry::MdType::Index:
            return m_index_entries.at(entry).type();

        case RuleEntry::MdType::Copy:
            assert(false && "Unexpected entry type MdType::Copy");
            break;
    }
    assert(false && "Unknown entry type!");
    return static_cast<exseis::utils::Type>(0);
}


size_t Trace_metadata::size(void) const
{
    return num_traces;
}

size_t Trace_metadata::memory_usage(void) const
{
    // Fix this to query the Generic_array for the size of the underlying
    // storage instead, e.g. num_bytes()

    // Add the capacity of a vector value for a map iterator.
    const auto add_capacity = [](size_t init, const auto& map_it) -> size_t {
        return init + map_it.second.capacity();
    };

    // Add all the capacities of the vector values in a map, multiplied by
    // the size of the datatype they hold.
    const auto mem_usage = [&](const auto& entries) -> size_t {
        using Entries    = typename std::decay<decltype(entries)>::type;
        using Value_type = typename Entries::mapped_type::value_type;

        const auto summed_capacity = std::accumulate(
          entries.begin(), entries.end(), size_t(0), add_capacity);

        return summed_capacity * sizeof(Value_type);
    };

    return mem_usage(m_floating_point_entries) + mem_usage(m_integer_entries)
           + mem_usage(m_index_entries)
           + raw_metadata.capacity() * sizeof(unsigned char)
           + sizeof(Trace_metadata) + rules.memory_usage();
}

}  // namespace piol
}  // namespace exseis
