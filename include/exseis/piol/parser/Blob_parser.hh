#ifndef EXSEIS_PIOL_PARSER_BLOB_PARSER_HH
#define EXSEIS_PIOL_PARSER_BLOB_PARSER_HH

#include "exseis/utils/types/Type.hh"

#include <cstddef>
#include <vector>

namespace exseis {
inline namespace piol {
inline namespace parser {

/// @brief A structure representing a small blob of data inside a larger blob of
///        data, used for sparse access to a large blob
struct Data_write_location {
    /// The beginning location of the data in the original blob
    size_t begin = 0;

    /// The first-past-the-end location of the data in the original blob
    size_t end = 0;

    /// A pointer to data representing [begin, end) in the original blob
    void* data = nullptr;
};

/// @brief A structure representing a small blob of data inside a larger blob of
///        data, used for sparse access to a large blob
struct Data_read_location {
    /// The beginning location of the data in the original blob
    size_t begin = 0;

    /// The first-past-the-end location of the data in the original blob
    size_t end = 0;

    /// A pointer to data representing [begin, end) in the original blob
    const void* data = nullptr;
};

/// @cond DOXYGEN_IGNORE
inline bool operator==(
    const Data_write_location& lhs, const Data_write_location& rhs)
{
    return lhs.begin == rhs.begin && lhs.end == rhs.end && lhs.data == rhs.data;
}

inline bool operator==(
    const Data_read_location& lhs, const Data_read_location& rhs)
{
    return lhs.begin == rhs.begin && lhs.end == rhs.end && lhs.data == rhs.data;
}
/// @endcond

/// @brief A pair defining the native type and number of items that will be
///        read or written by a parser
struct Parsed_type {
    /// The type that will be parsed
    Type type;

    /// The number of values that will be parsed
    size_t count = 1;
};

/// @brief A parser type for binary blobs to and from native representations
class Blob_parser {
  public:
    /// @virtual_destructor
    virtual ~Blob_parser() = default;

    /// @brief Get the number of Data_locations expected by this parser
    /// @returns The number of Data_locations expected by this parser
    virtual size_t number_of_data_locations() const = 0;

    /// @brief Initialize and set the offsets in a list of Data_location
    ///        in-place
    ///
    /// @param[inout] begin Iterator to the beginning of list of Data_location
    ///                     that will be initialized to the expected number of
    ///                     data locations with the expected offsets.
    /// @param[in]    end   First-past-the-end iterator to the list of
    ///                     Data_location to be initialized.
    /// @returns The iterator after the last Data_location to be set.
    virtual std::vector<exseis::Data_read_location>::iterator
    data_read_locations(
        std::vector<exseis::Data_read_location>::iterator begin,
        std::vector<exseis::Data_read_location>::iterator end) const = 0;

    /// @brief Helper function, initialize and return list of Data_location
    /// @returns The list of Data_location
    std::vector<Data_read_location> data_read_locations() const
    {
        std::vector<Data_read_location> value(number_of_data_locations());
        data_read_locations(value.begin(), value.end());
        return value;
    }

    /// @copydoc Blob_parser::data_read_locations
    virtual std::vector<exseis::Data_write_location>::iterator
    data_write_locations(
        std::vector<exseis::Data_write_location>::iterator begin,
        std::vector<exseis::Data_write_location>::iterator end) const = 0;

    /// @copydoc Blob_parser::data_read_locations() const
    std::vector<Data_write_location> data_write_locations() const
    {
        std::vector<Data_write_location> value(number_of_data_locations());
        data_write_locations(value.begin(), value.end());
        return value;
    }

    /// @brief Get the native type parsed by this parser
    /// @returns The parsed type and number of items parsed by this parser
    virtual Parsed_type parsed_type() const = 0;

    /// @brief Read the native type from the list of Data_location into the
    ///        pointer to value
    ///
    /// @param[in]  begin  Iterator to the beginning of the list of
    ///                    Data_location to be read
    /// @param[in]  end    First-past-the-end iterator of the list of
    ///                    Data_location to be read
    /// @param[out] value  A pointer to the native value to be read into
    virtual void read(
        std::vector<Data_read_location>::const_iterator begin,
        std::vector<Data_read_location>::const_iterator end,
        void* value) const = 0;

    /// @brief Write the native type from the value into the list of
    ///       Data_location
    /// @param[inout] begin  Iterator to the beginning of the list of
    ///                      Data_location to be written
    /// @param[in]    end    First-past-the-end iterator of the list of
    ///                      Data_location to be read
    /// @param[in]   value   A pointer to the native value to be written from
    virtual void write(
        std::vector<exseis::Data_write_location>::iterator begin,
        std::vector<exseis::Data_write_location>::iterator end,
        const void* value) const = 0;
};

}  // namespace parser
}  // namespace piol
}  // namespace exseis
#endif  // EXSEIS_PIOL_PARSER_BLOB_PARSER_HH
