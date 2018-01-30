/*******************************************************************************************//*!
 *   @file
 *   @author Cathal O Broin - cathal@ichec.ie - first commit
 *   @copyright TBD. Do not distribute
 *   @date October 2016
 *   @brief
 *   @details
 *//*******************************************************************************************/
#ifndef PIOLFILESEGYMD_INCLUDE_GUARD
#define PIOLFILESEGYMD_INCLUDE_GUARD
#include "global.hh"
#include "share/segy.hh"
#include "share/units.hh"
#include "share/param.hh"
#include "share/datatype.hh"
namespace PIOL { namespace File {
/*! The Datatype (or format in SEG-Y terminology)
 */
enum class Format : int16_t
{
    IBM  = 1,   //<! IBM format, big endian
    TC4  = 2,   //<! Two's complement, 4 byte
    TC2  = 3,   //<! Two's complement, 2 byte
    FPG  = 4,   //<! Fixed-point gain (obsolete)
    IEEE = 5,   //<! The IEEE format, big endian
    NA1  = 6,   //<! Unused
    NA2  = 7,   //<! Unused
    TC1  = 8    //<! Two's complement, 1 byte
};

/*! Header offsets as defined in the specification. Actual offset is the value minus one.
 */
enum class Hdr : size_t
{
    Increment  = 3217U, //!< int16_t. The increment between traces in microseconds
    NumSample  = 3221U, //!< int16_t. The number of samples per trace
    Type       = 3225U, //!< int16_t. Trace data type. AKA format in SEGY terminology
    Sort       = 3229U, //!< int16_t. The sort order of the traces.
    Units      = 3255U, //!< int16_t. The unit system, i.e SI or imperial.
    SEGYFormat = 3501U, //!< int16_t. The SEG-Y Revision number
    FixedTrace = 3503U, //!< int16_t. Whether we are using fixed traces or not.
    Extensions = 3505U, //!< int16_t. If we use header extensions or not.
};

/*! @brief Convert a SEG-Y scale integer to a floating point type
 *  @param[in] scale The int16_t scale taken from the SEG-Y file
 *  @return The scale convertered to floating point.
 */
extern geom_t scaleConv(int16_t scale);

/*! @fn int16_t PIOL::File::deScale(const geom_t val)
 * @brief Take a coordinate and extract a suitable scale factor to represent that number
 * in 6 byte fixed point format of the SEG-Y specification.
 * @param[in] val The coordinate of interest.
 * @return An appropriate scale factor for the coordinate.
 * @details Convert the number from float to a 6 byte SEGY fixed-point representation.
 * There are ten possible values for the scale factor. Shown are the possible values
 * and the form the input float should have to use that scale factor.
 * firstly, anything smaller than 4 decimal points is discarded since the approach
 * can not represent it.
*//*
 * Shown is the
 * position of the least significant digit:
 * -10000 - \d0000.0000
 * -1000  - \d000.0000
 * -100   - \d00.0000
 * -10    - \d0.0000
 * -1     - \d
 * 1      - \d
 * 10     - \d.\d
 * 100    - \d.\d\d
 * 1000   - \d.\d\d\d
 * 10000  - \d.\d\d\d\d
 * @todo Handle the annoying case of numbers at or around 2147483648 with a decimal somewhere.
 * @todo Add rounding before positive scale values
*/
extern int16_t deScale(const geom_t val);

/*! @brief Get the header metadata value from the binary header.
 *  @param[in] item The header item of interest.
 *  @param[in] src The buffer of the header object.
 *  @return Return the header item value.
 */
template <class T = int16_t>
T getMd(const Hdr item, const uchar * src)
{
    switch (item)
    {
        case Hdr::Increment :
        case Hdr::Type :
        case Hdr::NumSample :
        case Hdr::Units :
        case Hdr::SEGYFormat :
        case Hdr::FixedTrace :
        case Hdr::Extensions :
        return T(getHost<int16_t>(&src[size_t(item)-1U]));
        default :
        return T(0);
        break;
    }
}

/*! @brief Set the header metadata value corresponding to the item specified
 *  @param[in] item The header item of interest
 *  @param[in] src The metadata value to insert into the buffer.
 *  @param[in, out] dst The header as an array of uchar.
 */
template <typename T = int16_t>
void setMd(const Hdr item, const T src, uchar * dst)
{
    switch (item)
    {
        case Hdr::Increment :
        case Hdr::Type :
        case Hdr::NumSample :
        case Hdr::Units :
        case Hdr::SEGYFormat :
        case Hdr::FixedTrace :
        case Hdr::Extensions :
        getBigEndian<int16_t>(src, &dst[static_cast<size_t>(item)-1U]);
        default :
        break;
    }
}

/*! @brief Extract parameters from an unsigned char array into the parameter structure
 *  @param[in] sz The number of sets of parameters
 *  @param[in] md The buffer in the SEG-Y trace header format
 *  @param[out] prm The parameter structure
 *  @param[in] stride The stride to use between adjacent blocks in the input buffer.
 *  @param[in] skip Skip the first "skip" entries when filling Param
 */
extern void extractParam(size_t sz, const uchar * md, Param * prm, size_t stride, size_t skip);

/*! @brief Extract parameters from an unsigned char array into the parameter structure
 *  @param[in] sz The number of sets of parameters
 *  @param[in] prm The parameter structure
 *  @param[out] md The buffer in the SEG-Y trace header format
 *  @param[in] stride The stride to use between adjacent blocks in the input buffer.
 *  @param[in] skip Skip the first "skip" entries when extracting entries from Param
 */
extern void insertParam(size_t sz, const Param * prm, uchar * md, size_t stride, size_t skip);
}}
#endif
