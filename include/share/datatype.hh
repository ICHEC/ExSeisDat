/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *//*******************************************************************************************/
#ifndef ANCDATATYPE_INCLUDE_GUARD
#define ANCDATATYPE_INCLUDE_GUARD
#include <type_traits>
#include <cstdint>
#include "global.hh"

namespace PIOL {
/*! \brief Convert a 2 byte \c char array in big endian to a host 2 byte datatype
 *  \param[in] src Data in big endian order to stuff into the host datatype
 *  \return Return a 2 byte datatype (host endianness)
 *  \details src[0] contains the most significant byte in big endian. src[1] contains
 *  the least significant. Shift src[0] to be in the position of the most significant byte
 *  and OR it with src[1] which is not shifted (as it is the least significant byte.
 */
template <typename T, typename std::enable_if<sizeof(T) == 2U, T>::type = 0>
T getHost(const uchar * src)
{
    return (T(src[0]) << 8) |
            T(src[1]);
}

/*! \overload
 *  \brief Convert a 4 byte datatype in big endian to a host 4 byte datatype
 *  \param[in] src Data in big endian order to stuff into the host datatype
 *  \return Return a short
 */
template <typename T, typename std::enable_if<sizeof(T) == 4U, T>::type = 0>
T getHost(const uchar * src)
{
    return (T(src[0]) << 24) |
           (T(src[1]) << 16) |
           (T(src[2]) << 8) |
            T(src[3]);
}

/*! \brief Convert a host 4 byte type to a 4 byte \c char array in big endian
 *  \tparam T The host type
 *  \param[in] src The input 4 byte type with host endianness
 *  \param[out] dst A pointer to 4 bytes of data where the big endian
 *  data will be stored.
 *  \details Big endian means dst[0] holds the most significant byte and d[3] the least.
 */
//template <typename T,
//typename std::enable_if<sizeof(T) == 4U, T>::type = 0 >
template <typename T, typename std::enable_if<sizeof(T) == 4U, T>::type = 0>
void getBigEndian(const T src, uchar * dst)
{
    dst[0] = src >> 24 & 0xFF;
    dst[1] = src >> 16 & 0xFF;
    dst[2] = src >> 8 & 0xFF;
    dst[3] = src & 0xFF;
}

/*! \overload
 *  \brief Convert a host 2 byte type to a 2 byte \c char array in big endian
 *  \tparam The return type (type must be 2 bytes long)
 *  \param[in] src The input short with host endianness
 *  \param[out] dst A pointer to 2 bytes of data where the big endian
 *  data will be stored.
 *  \details Big endian means dst[0] holds the most significant byte and d[1] the least.
 */
template <typename T, typename std::enable_if<sizeof(T) == 2U, T>::type = 0>
void getBigEndian(const T src, uchar * dst)
{
    dst[0] = src >> 8 & 0xFF;
    dst[1] = src & 0xFF;
}

/*! \brief Reverse the byte sequence of a 4 byte \c char array
 *  \param[in] src Data to be reversed
 *  \details This can be used to switch between endianness representations.
 */
extern void reverse4Bytes(uchar * src);

/*! Convert the underlying bit representation of a 4 byte integer to a float.
 *  \param[in] i The input 32 bit integer
 *  \return The corresponding float
 *  \details One could think of this function as providing the cast
 *  reinterpret_cast<float>(i). It uses a union to avoid strict
 *  aliasing rules.
 */
extern float tofloat(const uint32_t i);

/*! Convert the underlying bit representation of a float to a 4 byte integer.
 *  \param[in] f The input float
 *  \return The corresponding 4 byte integer
 *  \details This function is the same as the above but in reverse.
 */
extern uint32_t toint(const float f);

/*! Convert the underlying bit representation of a 4 byte integer whose bits are
 *  in IBM format to an IEEE754 float.
 *  \param[in] f The input float
 *  \param[in] bigEndian True if the data is in big endian format
 *  \return The corresponding 4 byte integer
 *  \details This function assumes that the system uses IEEE754.
 */
extern float convertIBMtoIEEE(const float f, bool bigEndian);
}
#endif
