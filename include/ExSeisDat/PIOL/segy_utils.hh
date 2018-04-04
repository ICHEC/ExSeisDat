////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date October 2016
/// @brief   Utilities for working with SEGY files.
/// @details Utilities for working with SEGY files. Namely, labels for the
///          number formats, offset for certain file header values, functions
///          for working with scaled numbers, and functions for manipulating the
///          parameter structure from buffers of trace headers.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_SEGY_UTILS_HH
#define EXSEISDAT_PIOL_SEGY_UTILS_HH

#include "ExSeisDat/PIOL/Param.hh"
#include "ExSeisDat/PIOL/share/datatype.hh"
#include "ExSeisDat/PIOL/share/units.hh"
#include "ExSeisDat/PIOL/typedefs.h"

namespace PIOL {
namespace SEGY_utils {

/*! The Number Format, identifying the representation used for numbers in the
 *  SEGY file.
 */
enum class SEGYNumberFormat : int16_t {
    /// IBM format, big endian
    IBM = 1,

    /// Two's complement, 4 byte
    TC4 = 2,

    /// Two's complement, 2 byte
    TC2 = 3,

    /// Fixed-point gain (obsolete)
    FPG = 4,

    /// The IEEE format, big endian
    IEEE = 5,

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
namespace SEGYFileHeaderByte {

/// int16_t. The increment between traces in microseconds
constexpr size_t Interval = 3217U - 1U;

/// int16_t. The number of samples per trace
constexpr size_t NumSample = 3221U - 1U;

/// int16_t. Trace data type. AKA format in SEGY terminology
constexpr size_t Type = 3225U - 1U;

/// int16_t. The sort order of the traces.
constexpr size_t Sort = 3229U - 1U;

/// int16_t. The unit system, i.e SI or imperial.
constexpr size_t Units = 3255U - 1U;

/// int16_t. The SEG-Y Revision number
constexpr size_t SEGYFormat = 3501U - 1U;

/// int16_t. Whether we are using fixed traces or not.
constexpr size_t FixedTrace = 3503U - 1U;

/// int16_t. If we use header extensions or not.
constexpr size_t Extensions = 3505U - 1U;

}  // namespace SEGYFileHeaderByte


/*! @brief Extract parameters from an unsigned char array into the parameter
 *         structure
 *  @param[in] sz The number of sets of parameters
 *  @param[in] md The buffer in the SEG-Y trace header format
 *  @param[out] prm The parameter structure
 *  @param[in] stride The stride to use between adjacent blocks in the input
 *                    buffer.
 *  @param[in] skip Skip the first "skip" entries when filling Param
 */
void extractParam(
  size_t sz, const uchar* md, Param* prm, size_t stride, size_t skip);


/*! @brief Extract parameters from an unsigned char array into the parameter
 *         structure
 *  @param[in] sz The number of sets of parameters
 *  @param[in] prm The parameter structure
 *  @param[out] md The buffer in the SEG-Y trace header format
 *  @param[in] stride The stride to use between adjacent blocks in the input
 *                    buffer.
 *  @param[in] skip Skip the first "skip" entries when extracting entries from
 *                  Param
 */
void insertParam(
  size_t sz, const Param* prm, uchar* md, size_t stride, size_t skip);


/*! @brief Convert a SEG-Y scale integer to a floating point type
 *  @param[in] segy_scalar The int16_t scale taken from the SEG-Y file
 *  @return The scale convertered to floating point.
 */
geom_t parse_scalar(int16_t segy_scalar);


/*! @brief Take a coordinate and extract a suitable scale factor to represent
 *         that number in 6 byte fixed point format of the SEG-Y specification.
 *  @param[in] val The coordinate of interest.
 *  @return An appropriate scale factor for the coordinate.
 *  @details Convert the number from float to a 6 byte SEGY fixed-point
 *           representation.  There are ten possible values for the scale
 *           factor. Shown are the possible values and the form the input float
 *           should have to use that scale factor.  firstly, anything smaller
 *           than 4 decimal points is discarded since the approach can not
 *           represent it.
 *
 * Shown is the
 * position of the least significant digit, \c d:
 * -10000 - d0000.0000
 * -1000  - d000.0000
 * -100   - d00.0000
 * -10    - d0.0000
 * -1     - d
 * 1      - d
 * 10     - d.d
 * 100    - d.dd
 * 1000   - d.ddd
 * 10000  - d.dddd
 * @todo Handle the annoying case of numbers at or around 2147483648 with a
 *       decimal somewhere.
 * @todo Add rounding before positive scale values
 */
int16_t find_scalar(geom_t val);


/*! @brief An enumeration containing important SEG-Y sizes
 */
enum class Size : size_t {
    Text = 3200U,

    /// The size of the SEG-Y header (text extensions not used)
    HO = 3600U,

    /// The SEG-Y trace header size
    DOMd = 240U
};

/*! @brief Return the size of the text field
 *  @return  Returns the size of the text field in bytes
 */
constexpr size_t getTextSz()
{
    return static_cast<size_t>(Size::Text);
}

/*! @brief Return the size of the Header Object (assuming text extensions are
 *         not used)
 *  @return Returns the size of the HO in bytes.
 */
constexpr size_t getHOSz()
{
    return static_cast<size_t>(Size::HO);
}

/*! @brief Return the size of the Data-Object Metadata object
 *  @return Returns the size of the DOMD in bytes
 */
constexpr size_t getMDSz()
{
    return static_cast<size_t>(Size::DOMd);
}

/*! @brief Return the size of the Data-Object Field object
 *  @param[in] ns The number of elements in the data-field.
 *  @return Returns the size of the data-field in bytes
 */
template<typename T = float>
constexpr size_t getDFSz(const size_t ns)
{
    return ns * sizeof(T);
}

/*! @brief Return the size of the Data-Object.
 *  @param[in] ns The number of elements in the data-field.
 *  @tparam T The datatype of the data-field. The default value is float.
 *  @return Returns the DO size.
 */
template<typename T = float>
size_t getDOSz(const size_t ns)
{
    return getMDSz() + getDFSz<T>(ns);
}

/*! @brief Return the expected size of the file if there are nt data-objects and
 *         ns elements in a data-field.
 *  @param[in] nt The number of data objects.
 *  @param[in] ns The number of elements in the data-field.
 *  @tparam T The datatype of the data-field. The default value is float.
 *  @return Returns the expected file size.
 */
template<typename T = float>
size_t getFileSz(const size_t nt, const size_t ns)
{
    return getHOSz() + nt * getDOSz<T>(ns);
}

/*! @brief Return the offset location of a specific data object.
 *  @param[in] i The location of the ith data object will be returned.
 *  @param[in] ns The number of elements in the data-field.
 *  @tparam T The datatype of the data-field. The default value is float.
 *  @return Returns the location.
 */
template<typename T = float>
size_t getDOLoc(const size_t i, const size_t ns)
{
    return getFileSz<T>(i, ns);
}

/*! @brief Return the offset location of a specific data-field
 *  @param[in] i The location of the ith data-field will be returned.
 *  @param[in] ns The number of elements in the data-field.
 *  @tparam T The datatype of the data-field. The default value is float.
 *  @return Returns the location.
 */
template<typename T = float>
size_t getDODFLoc(const size_t i, const size_t ns)
{
    return getFileSz<T>(i, ns) + getMDSz();
}

/*! @brief Return the number of traces in a file given a file size
 *  @param[in] fsz the size of a file or expected size in bytes
 *  @param[in] ns The number of elements in the data-field.
 *  @tparam T The datatype of the data-field. The default value is float.
 *  @return Returns the number of traces.
 */
template<typename T = float>
size_t getNt(const size_t fsz, const size_t ns)
{
    return (fsz - SEGY_utils::getHOSz()) / SEGY_utils::getDOSz<T>(ns);
}


}  // namespace SEGY_utils
}  // namespace PIOL

#endif  // EXSEISDAT_PIOL_SEGY_UTILS_HH
