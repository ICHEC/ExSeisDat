////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
////////////////////////////////////////////////////////////////////////////////

#include "exseis/piol/metadata/Trace_metadata.hh"
#include "exseis/piol/segy/utils.hh"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <map>
#include <numeric>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace exseis {
inline namespace piol {
inline namespace metadata {


Trace_metadata::Trace_metadata(
    const Entry_type_map& entry_type_map, size_t number_of_traces) :
    m_number_of_traces{number_of_traces}, m_entry_types{entry_type_map}
{
    for (const auto& entry_it : entry_type_map) {
        const auto& key   = entry_it.first;
        const auto& type  = entry_it.second.type;
        const auto& count = entry_it.second.count;

        const auto add_floating_point = [&](auto value) {
            using Type = typename std::decay<decltype(value)>::type;

            m_floating_point_entries.insert(
                {key, Generic_array<double>(
                          std::vector<Type>(m_number_of_traces * count))});
        };
        const auto add_integer = [&](auto value) {
            using Type = typename std::decay<decltype(value)>::type;

            m_integer_entries.insert(
                {key, Generic_array<int64_t>(
                          std::vector<Type>(m_number_of_traces * count))});
        };
        const auto add_index = [&](auto value) {
            using Type = typename std::decay<decltype(value)>::type;

            m_index_entries.insert(
                {key, Generic_array<uint64_t>(
                          std::vector<Type>(m_number_of_traces * count))});
        };

        switch (type) {
            case Type::Double:
                add_floating_point(double());
                break;
            case Type::Float:
                add_floating_point(float());
                break;

            case Type::UInt64:
                add_index(uint64_t());
                break;

            case Type::Int64:
                add_integer(int64_t());
                break;
            case Type::UInt32:
                add_integer(uint32_t());
                break;
            case Type::Int32:
                add_integer(int32_t());
                break;
            case Type::UInt16:
                add_integer(uint16_t());
                break;
            case Type::Int16:
                add_integer(int16_t());
                break;
            case Type::UInt8:
                add_integer(uint8_t());
                break;
            case Type::Int8:
                add_integer(int8_t());
                break;
        }
    }
}


Floating_point Trace_metadata::get_floating_point(
    size_t trace_index, Key entry) const
{
    const auto it = m_floating_point_entries.find(entry);

    if (it == m_floating_point_entries.end()) {
        throw Entry_not_found(std::string(
            "Trace_metadata::get_floating_point : Floating point entry not found: "
            + to_string(entry)));
    }

    return it->second.get(trace_index);
}

void Trace_metadata::set_floating_point(
    size_t trace_index, Key entry, Floating_point value)
{
    auto it = m_floating_point_entries.find(entry);

    if (it == m_floating_point_entries.end()) {
        throw Entry_not_found(std::string(
            "Trace_metadata::set_floating_point : Floating point entry not found: "
            + to_string(entry)));
    }

    const auto& entry_type = m_entry_types.at(entry);

    it->second.set(trace_index * entry_type.count, value);
}

Integer Trace_metadata::get_integer(size_t trace_index, Key entry) const
{
    const auto it = m_integer_entries.find(entry);

    if (it == m_integer_entries.end()) {
        throw Entry_not_found(std::string(
            "Trace_metadata::get_integer : Integer entry not found: "
            + to_string(entry)));
    }

    const auto& entry_type = m_entry_types.at(entry);

    return it->second.get(trace_index * entry_type.count);
}

void Trace_metadata::set_integer(size_t trace_index, Key entry, Integer value)
{
    auto it = m_integer_entries.find(entry);

    if (it == m_integer_entries.end()) {
        throw Entry_not_found(std::string(
            "Trace_metadata::set_integer : Integer entry not found: "
            + to_string(entry)));
    }

    const auto& entry_type = m_entry_types.at(entry);

    return it->second.set(trace_index * entry_type.count, value);
}

size_t Trace_metadata::get_index(size_t trace_index, Key entry) const
{
    const auto it = m_index_entries.find(entry);

    if (it == m_index_entries.end()) {
        throw Entry_not_found(std::string(
            "Trace_metadata::get_index : Index entry not found: "
            + to_string(entry)));
    }

    const auto& entry_type = m_entry_types.at(entry);

    return it->second.get(trace_index * entry_type.count);
}

void Trace_metadata::set_index(size_t trace_index, Key entry, size_t value)
{
    auto it = m_index_entries.find(entry);

    if (it == m_index_entries.end()) {
        throw Entry_not_found(std::string(
            "Trace_metadata::set_index : Index entry not found: "
            + to_string(entry)));
    }

    const auto& entry_type = m_entry_types.at(entry);

    it->second.set(trace_index * entry_type.count, value);
}

template<
    typename DataLocation,
    typename FloatingPointEntries,
    typename IndexEntries,
    typename IntegerEntries>
static DataLocation get_data_location_impl(
    const std::map<Trace_metadata::Key, Trace_metadata_info>& m_entry_types,
    FloatingPointEntries&& m_floating_point_entries,
    IndexEntries&& m_index_entries,
    IntegerEntries&& m_integer_entries,
    size_t trace_index,
    Trace_metadata::Key entry)
{
    auto it = m_entry_types.find(entry);

    if (it == m_entry_types.end()) {
        throw Trace_metadata::Entry_not_found(std::string(
            "Trace_metadata::get_data_location : Entry not found: "
            + to_string(entry)));
    }

    const auto& type  = it->second.type;
    const auto& count = it->second.count;

    const auto floating_point_data_location = [&](auto value) {
        using Type = typename std::decay<decltype(value)>::type;

        const auto begin = trace_index * count * sizeof(Type);
        return DataLocation{begin, begin + count * sizeof(Type),
                            m_floating_point_entries.at(entry).data() + begin};
    };
    const auto index_data_location = [&](auto value) {
        using Type = typename std::decay<decltype(value)>::type;

        const auto begin = trace_index * count * sizeof(Type);
        return DataLocation{begin, begin + count * sizeof(Type),
                            m_index_entries.at(entry).data() + begin};
    };
    const auto integer_data_location = [&](auto value) {
        using Type = typename std::decay<decltype(value)>::type;

        const auto begin = trace_index * count * sizeof(Type);
        return DataLocation{begin, begin + count * sizeof(Type),
                            m_integer_entries.at(entry).data() + begin};
    };

    switch (type) {
        case Type::Double:
            return floating_point_data_location(double());
        case Type::Float:
            return floating_point_data_location(float());

        case Type::UInt64:
            return index_data_location(uint64_t());

        case Type::Int64:
            return integer_data_location(int64_t());
        case Type::UInt32:
            return integer_data_location(uint32_t());
        case Type::Int32:
            return integer_data_location(int32_t());
        case Type::UInt16:
            return integer_data_location(uint16_t());
        case Type::Int16:
            return integer_data_location(int16_t());
        case Type::UInt8:
            return integer_data_location(uint8_t());
        case Type::Int8:
            return integer_data_location(int8_t());
    }

    return {};
}

Data_read_location Trace_metadata::get_data_read_location(
    size_t trace_index, Key entry) const
{
    return get_data_location_impl<Data_read_location>(
        m_entry_types, m_floating_point_entries, m_index_entries,
        m_integer_entries, trace_index, entry);
}

Data_write_location Trace_metadata::get_data_write_location(
    size_t trace_index, Key entry)
{
    return get_data_location_impl<Data_write_location>(
        m_entry_types, m_floating_point_entries, m_index_entries,
        m_integer_entries, trace_index, entry);
}


void Trace_metadata::copy_entries(
    size_t trace_index,
    const Trace_metadata& source_metadata,
    size_t source_trace_index)
{
    const auto& source_entry_types = source_metadata.entry_types();
    for (const auto& entry : m_entry_types) {
        const auto source_entry_it = source_entry_types.find(entry.first);
        if (source_entry_it != source_entry_types.end()) {

            // Assuming they're the same type if they have the same key
            // TODO: don't just assume!

            assert(entry.second.type == source_entry_it->second.type);
            assert(entry.second.count == source_entry_it->second.count);

            auto write_location =
                get_data_write_location(trace_index, entry.first);
            auto read_location = source_metadata.get_data_read_location(
                source_trace_index, entry.first);

            std::memcpy(
                write_location.data, read_location.data,
                read_location.end - read_location.begin);
        }
    }
}

size_t Trace_metadata::entry_size(Key entry) const
{
    const auto it = m_entry_types.find(entry);

    if (it == m_entry_types.end()) {
        throw Entry_not_found(std::string(
            "Trace_metadata::entry_size : entry not found: "
            + to_string(entry)));
    }

    switch (it->second.type) {
        case Type::Double:
        case Type::Float:
            return m_floating_point_entries.at(entry).size();

        case Type::UInt64:
            return m_index_entries.at(entry).size();

        case Type::Int64:
        case Type::UInt32:
        case Type::Int32:
        case Type::UInt16:
        case Type::Int16:
        case Type::UInt8:
        case Type::Int8:
            return m_integer_entries.at(entry).size();
    }
    assert(false && "Unknown entry type!");
    return 0;
}


const Trace_metadata::Entry_type_map& Trace_metadata::entry_types() const
{
    return m_entry_types;
}


size_t Trace_metadata::size() const
{
    return m_number_of_traces;
}

}  // namespace metadata
}  // namespace piol
}  // namespace exseis
