/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details The intention is to make this obselete very soon
 *//*******************************************************************************************/
#include <type_traits>
#include <cstdint>
#include "global.hh"
namespace PIOL {
/*! \brief Convert a 2 byte \c char array in big endian to a host 2 byte datatype
 *  \param[in] src Data in big endian order to stuff into the host datatype
 *  \return Return a two byte datatype (host endianness)
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
 *  \brief Convert a 4 byte \c char array in big endian to a host 4 byte datatype
 *  \param[in] src Data in big endian order to stuff into the host datatype
 *  \return Return a short
 *  \details src[0] contains the most significant byte in big endian. src[1] contains
 *  the least significant. Shift src[0] to be in the position of the most significant byte
 *  and OR it with src[1] which is not shifted (as it is the least significant byte.
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
}
