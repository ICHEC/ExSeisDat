////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Routines for converting between number types and endianness.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_ENCODING_NUMBER_ENCODING_HH
#define EXSEISDAT_UTILS_ENCODING_NUMBER_ENCODING_HH

#include "ExSeisDat/utils/typedefs.h"

#include <cstdint>
#include <type_traits>


namespace exseis {
namespace utils {

/*! @brief Convert a 2 byte \c char array in big endian to a host 2 byte
 *         datatype
 *  @param[in] src Data in big endian order to stuff into the host datatype
 *  @return Return a 2 byte datatype (host endianness)
 *  @details src[0] contains the most significant byte in big endian. src[1]
 *           contains the least significant. Shift src[0] to be in the position
 *           of the most significant byte and OR it with src[1] which is not
 *           shifted (as it is the least significant byte.
 */
template<typename T, typename std::enable_if<sizeof(T) == 2U, T>::type = 0>
T getHost(const uchar* src)
{
    return (T(src[0]) << 8) | T(src[1]);
}


/*! @overload
 *  @brief Convert a 4 byte datatype in big endian to a host 4 byte datatype
 *  @param[in] src Data in big endian order to stuff into the host datatype
 *  @return Return a short
 */
template<typename T, typename std::enable_if<sizeof(T) == 4U, T>::type = 0>
T getHost(const uchar* src)
{
    return (T(src[0]) << 24) | (T(src[1]) << 16) | (T(src[2]) << 8) | T(src[3]);
}


/*! @brief Convert a host type to \c char array in big endian order.
 *  @tparam T The host type
 *  @param[in] src The input type with host endianness
 *  @param[out] dst A pointer to data where the big endian data will be stored.
 *                  (pointer to array of size sizeof(T))
 *
 *  @details Big endian means dst[0] holds the most significant byte and d[3]
 *           the least.
 */
template<typename T>
void getBigEndian(const T src, uchar* dst)
{

    // For dst[i], we want the ith byte of src.
    // We do that by shifting the ith byte into the 1st byte position and
    // reading that first byte.
    //
    // Say sizeof(T) = 4.
    // Moving the ith byte to the 1st position is equivalent to right
    // shift by (4 - i - 1) bytes.
    //
    // Perhaps more obviously, we want to count from low to high in dst, and
    // from high to low in the bytes of src.
    //

    const size_t t_size = sizeof(T);

    for (size_t i = 0; i < t_size; i++) {
        // Find the bitwise right shift necessary to move byte i to byte 1.
        const size_t byte_shift = t_size - i - 1;
        const size_t bit_shift  = byte_shift * 8;

        // Read byte 1 into dst.
        dst[i] = (src >> bit_shift) & 0xFF;
    }
}


/*! @brief Reverse the byte sequence of a 4 byte \c char array
 *  @param[in] src Data to be reversed
 *  @details This can be used to switch between endianness representations.
 */
void reverse4Bytes(uchar* src);


/*! Convert the underlying bit representation of a 4 byte integer to a float.
 *  @param[in] i The input 32 bit integer
 *  @return The corresponding float
 *  @details One could think of this function as providing the cast
 *           reinterpret_cast<float>(i).
 */
float tofloat(const uint32_t i);


/*! Convert the underlying bit representation of a float to a 4 byte integer.
 *  @param[in] f The input float
 *  @return The corresponding 4 byte integer
 *  @details This function is the same as the above but in reverse.
 */
uint32_t toint(const float f);


/// The \c FloatComponents class represents a floating point number in terms
/// of its components: {sign, exponent, significand}.
///
/// @details The number can be found natively as
///     std::pow(-1,sign) * (significand * std::pow(2,-32))
///       * std::pow(2,exponent);
///
struct FloatComponents {
    /// The sign of the number.
    uint32_t sign = 0;

    /// The power of 2 exponent of the number.
    int32_t exponent = 0;

    /// The left-justified significand of the number.
    uint32_t significand = 0;
};


/// Extract the sign, exponent and significand from an IBM floating point number
/// and return it as a set of FloatComponents.
/// @param[in] ibm_float  The bit representation of the IBM floating point
///                       number.
/// @param[in] big_endian Whether the bit representation was read from a big
///                       endian stream.
/// @returns The FloatComponents for the IBM number:
///          {sign, exponent, significand}.
///
FloatComponents from_IBM(uint32_t ibm_float, bool big_endian);


/// Build an IEEE floating point representation from a set of FloatComponents
/// @param[in] components The components of the floating point number:
///                       {sign, exponent, significand}.
/// @returns The bit representation of an IEEE floating point number.
///
uint32_t to_IEEE(FloatComponents components);

/*! Convert the underlying bit representation of a 4 byte integer whose bits are
 *  in IBM format to an IEEE754 float.
 *  @param[in] f The input float
 *  @param[in] bigEndian True if the data is in big endian format
 *  @return The corresponding 4 byte integer
 *  @details This function assumes that the system uses IEEE754.
 */
float convertIBMtoIEEE(const float f, bool bigEndian);

}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_UTILS_ENCODING_CHARACTER_ENCODING_HH
