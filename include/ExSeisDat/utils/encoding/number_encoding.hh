////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Routines for converting between number types and endianness.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_ENCODING_NUMBER_ENCODING_HH
#define EXSEISDAT_UTILS_ENCODING_NUMBER_ENCODING_HH

#include <array>
#include <cstdint>
#include <type_traits>
#include <cstring>

///
/// @namespace exseis::utils::number_encoding
///
/// @brief Routines for converting between different representations of number
///        types.
///
/// The routines in this namespace deal with conversion between endianness and
/// conversion between number formats, e.g. IBM floating point and IEEE floating
/// point formats.
///

namespace exseis {
namespace utils {
inline namespace number_encoding {

/// @brief Convert a 2 byte \c char array in big endian to a host 2 byte
///        datatype
///
/// @param[in] src Data in big endian order to stuff into the host datatype
///
/// @return Return a 2 byte datatype (host endianness)
///
/// @details src[0] contains the most significant byte in big endian. src[1]
///          contains the least significant. Shift src[0] to be in the position
///          of the most significant byte and OR it with src[1] which is not
///          shifted (as it is the least significant byte.
///
/// @pre sizeof(T) == 2
///
template<typename T>
T from_big_endian(std::array<unsigned char, 2> src)
{
    static_assert(
      std::is_integral<T>::value,
      "from_big_endian only defined for integer types.");

    static_assert(sizeof(T) == 2, "Type should be of size 2!");

    return (T(src[0]) << 8) | T(src[1] << 0);
}


/// @overload
///
/// @brief Convert a 4 byte datatype in big endian to a host 4 byte datatype
///
/// @param[in] src Data in big-endian order to stuff into the datype in
///                host-endian order.
///
/// @return Return a 4 byte datatype
///
/// @pre sizeof(T) == 4
///
template<typename T>
T from_big_endian(std::array<unsigned char, 4> src)
{
    static_assert(
      std::is_integral<T>::value,
      "from_big_endian only defined for integer types.");

    static_assert(sizeof(T) == 4, "Type should be of size 4!");

    return (T(src[0]) << 24) | (T(src[1]) << 16) | (T(src[2]) << 8)
           | T(src[3] << 0);
}


/// @overload
///
/// @brief Convert a list of bytes representing a float in big-endian order
///        into a float in native-endianness.
///
/// @param[in] src The array of bytes in big-endian order to convert.
///
/// @return The float with native endianness.
///
template<>
inline float from_big_endian<float>(std::array<unsigned char, 4> src)
{
    static_assert(
      sizeof(float) == sizeof(uint32_t),
      "from_big_endian expects float and uint32_t to have the same size!");

    // Put the input bytes into a host-endian integer type, then copy
    // the bytes to a float.

    const uint32_t int_dst = from_big_endian<uint32_t>(src);

    float float_dst = 0;
    std::memcpy(&float_dst, &int_dst, sizeof(float));

    return float_dst;
}


/// @brief Convert a set of bytes representing a big-endian number into a
///        host integer type in the host-endianness.
///
/// @tparam T     The host integer type to convert to.
/// @tparam Bytes The byte type passed in. Should be unsigned char.
///
/// @param[in] byte  The highest value byte to convert
/// @param[in] bytes The rest of the bytes to convert, passed in in big-endian
///                  order.
///
/// @return The integer type in host-endianness constructed from the big-endian
///         ordered bytes passed in.
///
/// This is a convenience function to avoid having to construct the
/// `std::array`s necessary to call the other specializations.
///
/// In big-endian, higher value bytes come before lower value bytes.
/// In the case of this function, when calling, say,
/// `from_big_endian<int16_t>(x, y)`,
/// The byte held by `x` will be represented in the highest 8 bits of the 16 bit
/// integer, and `y` will be represented in the lowest 8 bits.
///
/// @pre sizeof(T) == number of parameters
///
template<typename T, typename... Bytes>
T from_big_endian(unsigned char byte, Bytes... bytes)
{
    static_assert(
      sizeof(T) == (sizeof...(Bytes) + 1),
      "from_big_endian expects the number of bytes passed in to match the number of bytes in the requested type.");

    return from_big_endian<T>(std::array<unsigned char, sizeof...(Bytes) + 1>{
      {byte, static_cast<unsigned char>(bytes)...}});
}


/// @brief Convert a host-endian integer type to an `unsigned char` array in
///        big-endian order.
///
/// @tparam T The type to convert to big-endian.
///
/// @param[in] src The value to convert from host-endian to big-endian.
///
/// @return An array containing the bytes of `src` in big-endian order.
///
/// @details For an array `dst` holding the byte-wise representation of an
///          integer, big-endian means dst[0] holds the most significant byte
///          and dst[3] the least.
///
template<typename T>
std::array<unsigned char, sizeof(T)> to_big_endian(T src)
{
    static_assert(
      std::is_integral<T>::value,
      "to_big_endian only defined for integer types.");

    std::array<unsigned char, sizeof(T)> dst;

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

    return dst;
}


/// @brief Convert a host-endian integer type to an `unsigned char` array in
///        big-endian order.
///
/// @param[in] src The value to convert from host-endian to big-endian.
///
/// @return An array containing the bytes of `src` in big-endian order.
///
/// @details For an array `dst` holding the byte-wise representation of an
///          integer, big-endian means dst[0] holds the most significant byte
///          and dst[3] the least.
///
template<>
inline std::array<unsigned char, sizeof(float)> to_big_endian(float src)
{
    static_assert(
      sizeof(float) == sizeof(uint32_t),
      "to_big_endian<float> expects float and uint32_t to be the same size!");

    uint32_t int_src = 0;
    std::memcpy(&int_src, &src, sizeof(float));

    return to_big_endian<uint32_t>(int_src);
}


/// The \c Float_components class represents a floating point number in terms
/// of its components: {sign, exponent, significand}.
///
/// @details The number can be found natively as
///     std::pow(-1,sign) * (significand * std::pow(2,-32))
///       * std::pow(2,exponent);
///
struct Float_components {
    /// The sign of the number.
    uint32_t sign = 0;

    /// The power of 2 exponent of the number.
    int32_t exponent = 0;

    /// The left-justified significand of the number.
    uint32_t significand = 0;
};


/// Extract the sign, exponent and significand from an IBM floating point number
/// and return it as a set of Float_components.
///
/// @param[in] ibm_float_bytes The bit representation of the IBM floating point
///                            number.
/// @param[in] is_big_endian   Whether the bit representation was read from a
///                            big endian stream.
///
/// @returns The Float_components for the IBM number:
///          {sign, exponent, significand}.
///
Float_components from_IBM(
  std::array<unsigned char, 4> ibm_float_bytes, bool is_big_endian);


/// Build a native floating point representation from a set of Float_components
///
/// @param[in] components The components of the floating point number:
///                       {sign, exponent, significand}.
///
/// @returns A native float equivalent to the representation in `components`.
///
/// This function assumes the system is using IEEE 754 floating point numbers.
///
/// @pre The native `float` type is IEEE 754 with round-half-to-even rounding.
///
float to_float(Float_components components);


/// Convert an array of bytes representing an IBM single-precision floating
/// point number to a native single-precision float.
///
/// @param[in] ibm_float_bytes The byte representation of the IBM float.
/// @param[in] is_big_endian   True if the data in `ibm_float_bytes` is in
///                            big-endian order.
///
/// @return The corresponding 4 byte integer
///
/// @details This function assumes that the system uses IEEE754.
///
float from_IBM_to_float(
  std::array<unsigned char, 4> ibm_float_bytes, bool is_big_endian);

}  // namespace number_encoding
}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_UTILS_ENCODING_CHARACTER_ENCODING_HH
