////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @date October 2016
/// @brief   Utilities for working with SEGY files.
/// @details Utilities for working with SEGY files. Namely, labels for the
///          number formats, offset for certain file header values, functions
///          for working with scaled numbers, and functions for manipulating the
///          parameter structure from buffers of trace headers.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_SEGY_UTILS_HH
#define EXSEISDAT_PIOL_SEGY_UTILS_HH

#include "exseisdat/piol/metadata/Trace_metadata.hh"
#include "exseisdat/utils/types/typedefs.hh"

namespace exseis {
namespace piol {
namespace segy {

using namespace exseis::utils::types;

/// @brief The Number Format, identifying the representation used for numbers in
///        the SEGY file.
///
enum class Segy_number_format : int16_t {
    /// IBM format, big endian
    IBM_fp32 = 1,

    /// Two's complement, 4 byte
    TC4 = 2,

    /// Two's complement, 2 byte
    TC2 = 3,

    /// Fixed-point gain (obsolete)
    FPG = 4,

    /// The IEEE format, big endian
    IEEE_fp32 = 5,

    /// Unused
    NA1 = 6,

    /// Unused
    NA2 = 7,

    /// Two's complement, 1 byte
    TC1 = 8
};


/// @brief   The global file headers for the SEGY format.
/// @details This represents the supported global headers for the SEGY format.
///          The values used are the byte offset into the file for the given
///          parameter, as defined by the SEGY standard.
///          The standard itself, however, presents them 1-indexed, so we write
///          them here in the 1-indexed form, and subtract 1 for a c-style
///          0-indexed offset.
struct Segy_file_header_byte {

    /// int16_t. The increment between traces in microseconds
    static constexpr size_t interval = 3217U - 1U;

    /// int16_t. The number of samples per trace
    static constexpr size_t num_sample = 3221U - 1U;

    /// int16_t. Trace data type. AKA format in SEGY terminology
    static constexpr size_t type = 3225U - 1U;

    /// int16_t. The sort order of the traces.
    static constexpr size_t sort = 3229U - 1U;

    /// int16_t. The unit system, i.e SI or imperial.
    static constexpr size_t units = 3255U - 1U;

    /// int16_t. The SEG-Y Revision number
    static constexpr size_t segy_format = 3501U - 1U;

    /// int16_t. Whether we are using fixed traces or not.
    static constexpr size_t fixed_trace = 3503U - 1U;

    /// int16_t. If we use header extensions or not.
    static constexpr size_t extensions = 3505U - 1U;
};


/// @brief Extract parameters from an unsigned char array into the parameter
///        structure
///
/// @param[in]  sz      The number of sets of parameters
/// @param[in]  md      The buffer in the SEG-Y trace header format
/// @param[out] prm     The parameter structure
/// @param[in]  stride  The stride to use between adjacent blocks in the input
///                     buffer.
/// @param[in]  skip    Skip the first "skip" entries when filling
///                     Trace_metadata
///
void extract_trace_metadata(
    size_t sz,
    const unsigned char* md,
    Trace_metadata& prm,
    size_t stride,
    size_t skip);


/// @brief Extract parameters from an unsigned char array into the parameter
///        structure
///
/// @param[in]  sz      The number of sets of parameters
/// @param[in]  prm     The parameter structure
/// @param[out] md      The buffer in the SEG-Y trace header format
/// @param[in]  stride  The stride to use between adjacent blocks in the input
///                     buffer.
/// @param[in]  skip    Skip the first "skip" entries when extracting entries
///                     from Trace_metadata
///
void insert_trace_metadata(
    size_t sz,
    const Trace_metadata& prm,
    unsigned char* md,
    size_t stride,
    size_t skip);


/// @brief Convert a SEG-Y scale integer to a floating point type
///
/// @param[in] segy_scalar The int16_t scale taken from the SEG-Y file
///
/// @return The scale convertered to floating point.
///
exseis::utils::Floating_point parse_scalar(int16_t segy_scalar);


/// @brief Take a coordinate and extract a suitable scale factor to represent
///         that number in 6 byte fixed point format of the SEG-Y specification.
///
///  @param[in] val The coordinate of interest.
///
///  @return An appropriate scale factor for the coordinate.
///
///  @details Convert the number from float to a 6 byte SEGY fixed-point
///           representation.  There are ten possible values for the scale
///           factor. Shown are the possible values and the form the input float
///           should have to use that scale factor.  firstly, anything smaller
///           than 4 decimal points is discarded since the approach can not
///           represent it.
///
/// Shown is the
/// position of the least significant digit, \c d:
/// @code
/// -10000 - d0000.0000
/// -1000  - d000.0000
/// -100   - d00.0000
/// -10    - d0.0000
/// -1     - d
/// 1      - d
/// 10     - d.d
/// 100    - d.dd
/// 1000   - d.ddd
/// 10000  - d.dddd
/// @endcode
///
/// @todo Handle the annoying case of numbers at or around 2147483648 with a
///       decimal somewhere.
/// @todo Add rounding before positive scale values
///
int16_t find_scalar(exseis::utils::Floating_point val);


/// @brief Return the size of the text field
///
/// @return  Returns the size of the text field in bytes
///
constexpr size_t segy_text_header_size()
{
    return 3200;
}

/// @brief Return the size of the Binary File Header in bytes.
///
/// This assumes text extensions are not used
///
/// @return Returns the size of the HO in bytes.
///
constexpr size_t segy_binary_file_header_size()
{
    return 3600;
}

/// @brief Return the size of the Standard Trace Header in bytes
///
/// @return Returns the size of the Trace Metadata in bytes
///
constexpr size_t segy_trace_header_size()
{
    return 240;
}

/// @brief Return the size of the Trace Data
///
/// @param[in] ns The number of elements in the data-field.
///
/// @return Returns the size of the data-field in bytes
///
template<typename T = float>
constexpr size_t segy_trace_data_size(size_t ns)
{
    return ns * sizeof(T);
}

/// @brief Return the size of the Trace, i.e. the Trace Metadata + Trace Data.
///
/// @param[in] ns The number of elements in the data-field.
///
/// @tparam T The datatype of the data-field. The default value is float.
///
/// @return Returns the Trace size.
///
template<typename T = float>
size_t segy_trace_size(size_t ns)
{
    return segy_trace_header_size() + segy_trace_data_size<T>(ns);
}

/// @brief Return the expected size of the file if there are nt data-objects and
///        ns elements in a data-field.
///
/// @param[in] nt The number of data objects.
/// @param[in] ns The number of elements in the data-field.
///
/// @tparam T The datatype of the data-field. The default value is float.
///
/// @return Returns the expected file size.
///
template<typename T = float>
size_t get_file_size(size_t nt, size_t ns)
{
    return segy_binary_file_header_size() + nt * segy_trace_size<T>(ns);
}

/// @brief Return the offset location of a specific data object.
///
/// @param[in] i  The location of the ith data object will be returned.
/// @param[in] ns The number of elements in the data-field.
///
/// @tparam T The datatype of the data-field. The default value is float.
///
/// @return Returns the location.
///
template<typename T = float>
size_t segy_trace_location(size_t i, size_t ns)
{
    return get_file_size<T>(i, ns);
}

/// @brief Return the offset location of a specific data-field
///
/// @param[in] i  The location of the ith data-field will be returned.
/// @param[in] ns The number of elements in the data-field.
///
/// @tparam T The datatype of the data-field. The default value is float.
///
/// @return Returns the location.
///
template<typename T = float>
size_t segy_trace_data_location(size_t i, size_t ns)
{
    return get_file_size<T>(i, ns) + segy_trace_header_size();
}

/// @brief Return the number of traces in a file given a file size
///
/// @param[in] fsz the size of a file or expected size in bytes
/// @param[in] ns  The number of elements in the data-field.
///
/// @tparam T The datatype of the data-field. The default value is float.
///
/// @return Returns the number of traces.
///
template<typename T = float>
size_t get_nt(size_t fsz, size_t ns)
{
    return (fsz - segy_binary_file_header_size()) / segy_trace_size<T>(ns);
}


}  // namespace segy
}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_SEGY_UTILS_HH
