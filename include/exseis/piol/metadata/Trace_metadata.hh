////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief   The Trace_metadata class
/// @details The Trace_metadata class is used to store the trace parameter
///          structure.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEIS_PIOL_METADATA_TRACE_METADATA_HH
#define EXSEIS_PIOL_METADATA_TRACE_METADATA_HH

#include "exseis/piol/metadata/Trace_metadata_key.hh"
#include "exseis/piol/parser/Blob_parser.hh"
#include "exseis/utils/generic/Generic_array.hh"
#include "exseis/utils/types/typedefs.hh"

#include <cassert>
#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace exseis {
inline namespace piol {
inline namespace metadata {

/// @brief Info about the trace metadata
struct Trace_metadata_info {
    /// The type of data that will be read
    Type type;
    /// The number of values that will be read for each trace
    size_t count = 1;
};

/// @brief Class for initialising the trace parameter structure
///        and storing a structure with the necessary rules.
///
class Trace_metadata {
  public:
    /// @brief The key representing the metadata entry
    using Key = Trace_metadata_key;

    /// @brief A map for the metadata entry key to the list of entries for each
    ///        trace block.
    ///
    template<typename T>
    using Entry_map = std::map<Key, Generic_array<T>>;

    /// @brief An exception class thrown when a metadata entry is queried which
    ///        hasn't been added to this instance.
    ///
    /// @details A common cause for this being thrown is where the key exists in
    ///          the metadata structure, but the wrong type has been queried.
    ///          e.g. A floating point entry for that key exists, but an integer
    ///               entry for the key is requested.
    ///
    class Entry_not_found : public std::out_of_range {
        using out_of_range::out_of_range;
    };

    /// Map type for the stored metadata info
    using Entry_type_map = std::map<Key, Trace_metadata_info>;

  private:
    /// @brief Floating point entries
    Entry_map<Floating_point> m_floating_point_entries;

    /// @brief Integer entries.
    Entry_map<Integer> m_integer_entries;

    /// @brief trace number array.
    Entry_map<uint64_t> m_index_entries;

    /// @brief the number of trace entries being held
    size_t m_number_of_traces;

    /// @brief A list of the type of a given metadata entry.
    Entry_type_map m_entry_types;

  public:
    /// @copybrief
    /// @param[in] entry_type_map    A map of Trace_metadata_info
    /// @param[in] number_of_traces  The number of traces to store the
    ///                              metadata for
    Trace_metadata(
        const Entry_type_map& entry_type_map, size_t number_of_traces);


    /// @name @special_member_functions
    /// @{

    /// @copy_constructor{default}
    Trace_metadata(const Trace_metadata&) = default;
    /// @copy_assignment{default}
    Trace_metadata& operator=(const Trace_metadata&) = default;

    /// @move_constructor{default}
    Trace_metadata(Trace_metadata&&) = default;
    /// @move_assignment{default}
    Trace_metadata& operator=(Trace_metadata&&) = default;

    /// @}


    /// @brief Get a floating point metadata entry
    ///
    /// @param[in] entry       The metadata entry to get.
    /// @param[in] trace_index The index of the trace block to get the metadata
    ///                        for.
    ///
    /// @returns The value of the metadata indexed by `entry` for trace block
    ///          `trace_index`.
    ///
    Floating_point get_floating_point(size_t trace_index, Key entry) const;


    /// @brief Set a floating point metadata entry
    ///
    /// @param[in] entry       The metadata entry to set.
    /// @param[in] trace_index The index of the trace block to set the metadata
    ///                        for.
    /// @param[in] value       The value to set the metadata entry to.
    ///
    void set_floating_point(
        size_t trace_index, Key entry, Floating_point value);


    /// @brief Get a integer metadata entry
    ///
    /// @param[in] entry       The metadata entry to get.
    /// @param[in] trace_index The index of the trace block to get the metadata
    ///                        for.
    ///
    /// @returns The value of the metadata indexed by `entry` for trace block
    ///          `trace_index`.
    ///
    Integer get_integer(size_t trace_index, Key entry) const;


    /// @brief Set a integer metadata entry
    ///
    /// @param[in] entry       The metadata entry to set.
    /// @param[in] trace_index The index of the trace block to set the metadata
    ///                        for.
    /// @param[in] value       The value to set the metadata entry to.
    ///
    void set_integer(size_t trace_index, Key entry, Integer value);


    /// @brief Get a index metadata entry
    ///
    /// @param[in] entry       The metadata entry to get.
    /// @param[in] trace_index The index of the trace block to get the metadata
    ///                        for.
    ///
    /// @returns The value of the metadata indexed by `entry` for trace block
    ///          `trace_index`.
    ///
    size_t get_index(size_t trace_index, Key entry) const;


    /// @brief Set a index metadata entry
    ///
    /// @param[in] entry       The metadata entry to set.
    /// @param[in] trace_index The index of the trace block to set the metadata
    ///                        for.
    /// @param[in] value       The value to set the metadata entry to.
    ///
    void set_index(size_t trace_index, Key entry, size_t value);

    /// @brief Get a sparse index into the raw storage for the given entry
    ///
    /// @param[in] trace_index  The trace index to get the storage for
    /// @param[in] entry        The key of the metadata to get the storage for
    ///
    /// @returns A sparse index into the raw storage for the given entry
    ///
    Data_read_location get_data_read_location(
        size_t trace_index, Key entry) const;

    /// @copydoc get_data_read_location
    Data_write_location get_data_write_location(size_t trace_index, Key entry);


    /// @brief Copy the metadata entries from `source_metadata` for trace index
    ///        `source_trace_index` into the metadata entries of the current
    ///        instance at `trace_index`.
    ///
    /// @param[in] trace_index        The trace index of the current instance to
    ///                               copy the metadata entries into.
    /// @param[in] source_metadata    The Trace_metadata object to copy the
    ///                               metadata entries from.
    /// @param[in] source_trace_index The trace index to copy the entries of
    ///                               `source_metadata` from.
    void copy_entries(
        size_t trace_index,
        const Trace_metadata& source_metadata,
        size_t source_trace_index);


    /// @brief Get the number of elements for a given metadata entry.
    ///
    /// @param[in] entry The metadata entry key.
    ///
    /// @return The number of elements for the given metadata entry.
    ///
    size_t entry_size(Key entry) const;


    /// @brief Get the underlying type for the given metadata entry.
    ///
    /// @return A map from keys to entry types
    ///
    const Entry_type_map& entry_types() const;


    /// @brief Get the data for a given metadata entry.
    ///
    /// The parameter `T` should match the underlying storage for the data.
    /// If they don't match, `nullptr` will be returned.
    ///
    /// When `T` is `unsigned char`, the data will be returned as an array
    /// of `unsigned char`, regardless of the underlying storage type.
    /// This can be useful for using the data with, say, MPI, without having
    /// to know the correct types in advance.
    ///
    /// For a metadata entry that we know is storing its data as a float:
    /// @code
    ///     assert(
    ///         trace_metadata.entry_type(my_entry)
    ///         == Type::Float);
    ///
    ///     float* data = trace_metadata.entry_data<float>(my_entry);
    ///
    ///     assert(data != nullptr);
    ///
    ///     for (size_t i = 0; i < trace_metadata.entry_size(my_entry); i++) {
    ///         do_something(data[i]);
    ///     }
    /// @endcode
    ///
    /// @tparam T The type of the metadata entry.
    ///
    /// @param[in] entry The key for the metadata entry.
    ///
    /// @return The underlying data for the metadata storage, as a pointer to an
    ///         array of `T`.
    ///
    template<typename T>
    const T* entry_data(Key entry) const;

    /// @copydoc entry_data(Key entry) const
    template<typename T>
    T* entry_data(Key entry);


    /// @brief Return the number of sets of trace parameters.
    ///
    /// @return Number of sets
    ///
    size_t size() const;
};


template<typename T>
T* Trace_metadata::entry_data(Key entry)
{
    const auto it = m_entry_types.find(entry);

    if (it == m_entry_types.end()) {
        throw Entry_not_found(std::string(
            "Trace_metadata::entry_data : entry not found: "
            + to_string(entry)));
    }

    const auto find_and_test = [&](auto& entries) -> T* {
        auto& entry_array = entries.at(entry);
        if (entry_array.type() == exseis::type<T>) {
            return static_cast<T*>(entry_array.data());
        }

        if (std::is_same<T, unsigned char>::value) {
            return entry_array.data();
        }

        return nullptr;
    };

    switch (it->second.type) {
        case Type::Double:
        case Type::Float:
            return find_and_test(m_floating_point_entries);

        case Type::UInt64:
            return find_and_test(m_index_entries);

        case Type::Int64:
        case Type::Int32:
        case Type::UInt32:
        case Type::Int16:
        case Type::UInt16:
        case Type::Int8:
        case Type::UInt8:
            return find_and_test(m_integer_entries);
    }

    assert(false && "Unknown entry type!");
    return nullptr;
}


template<typename T>
const T* Trace_metadata::entry_data(Key entry) const
{
    const auto it = m_entry_types.find(entry);

    if (it == m_entry_types.end()) {
        throw Entry_not_found(std::string(
            "Trace_metadata::entry_data : entry not found: "
            + to_string(entry)));
    }

    const auto find_and_test = [&](const auto& entries) -> const T* {
        const auto& entry_array = entries.at(entry);
        if (entry_array.type() == exseis::type<T>) {
            return reinterpret_cast<const T*>(entry_array.data());
        }

        if (std::is_same<T, unsigned char>::value) {
            return entry_array.data();
        }

        return nullptr;
    };

    switch (it->second.type) {
        case Type::Double:
        case Type::Float:
            return find_and_test(m_floating_point_entries);

        case Type::UInt64:
            return find_and_test(m_index_entries);

        case Type::Int64:
        case Type::Int32:
        case Type::UInt32:
        case Type::Int16:
        case Type::UInt16:
        case Type::Int8:
        case Type::UInt8:
            return find_and_test(m_integer_entries);
    }
    assert(false && "Unknown entry type!");
}

}  // namespace metadata
}  // namespace piol
}  // namespace exseis

#endif  // EXSEIS_PIOL_METADATA_TRACE_METADATA_HH
