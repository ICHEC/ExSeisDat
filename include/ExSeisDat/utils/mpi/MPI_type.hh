///
/// @file
/// @brief Helper class for getting an MPI_type for a given C++ type.
///
#ifndef EXSEISDAT_UTILS_MPI_MPI_TYPE_HH
#define EXSEISDAT_UTILS_MPI_MPI_TYPE_HH

namespace exseis {
namespace utils {

/*! @brief Return the fundamental MPI datatype associated with a fundamental
 *         datatype.
 *  @return The datatype.
 *  @tparam T The C++ datatype
 */
template<typename T>
inline MPI_Datatype MPI_type()
{
    static_assert(sizeof(T) == 0, "Unknown MPI type!");
    return MPI_DATATYPE_NULL;
}

/// @brief Return the MPI datatype for a \c double.
/// @return The MPI datatype for \c double.
template<>
inline MPI_Datatype MPI_type<double>()
{
    return MPI_DOUBLE;
}

/// @brief Return the MPI datatype for a \c long double.
/// @return The MPI datatype for \c long \c double.
template<>
inline MPI_Datatype MPI_type<long double>()
{
    return MPI_LONG_DOUBLE;
}

/// @brief Return the MPI datatype for a \c long double.
/// @return The MPI datatype for a \c long double.
template<>
inline MPI_Datatype MPI_type<char>()
{
    return MPI_CHAR;
}

/// @brief Return the MPI datatype for an \c unsigned \c char.
/// @return The MPI datatype for an \c unsigned \c char.
template<>
inline MPI_Datatype MPI_type<unsigned char>()
{
    return MPI_UNSIGNED_CHAR;
}

/// @brief Return the MPI datatype for an \c int.
/// @return The MPI datatype for an \c int.
template<>
inline MPI_Datatype MPI_type<int>()
{
    return MPI_INT;
}

/// @brief Return the MPI datatype for a \c long \c int.
/// @return The MPI datatype for a \c long \c int.
template<>
inline MPI_Datatype MPI_type<long int>()
{
    return MPI_LONG;
}

/// @brief Return the MPI datatype for an \c unsigned \c long \c int.
/// @return The MPI datatype for an \c unsigned \c long \c int.
template<>
inline MPI_Datatype MPI_type<unsigned long int>()
{
    return MPI_UNSIGNED_LONG;
}

/// @brief Return the MPI datatype for an \c unsigned \c int.
/// @return The MPI datatype for an \c unsigned \c int.
template<>
inline MPI_Datatype MPI_type<unsigned int>()
{
    return MPI_UNSIGNED;
}

/// @brief Return the MPI datatype for a \c long \c long \c int.
/// @return The MPI datatype for a \c long \c long \c int.
template<>
inline MPI_Datatype MPI_type<long long int>()
{
    return MPI_LONG_LONG_INT;
}

/// @brief Return the MPI datatype for a \c float.
/// @return The MPI datatype for a \c float.
template<>
inline MPI_Datatype MPI_type<float>()
{
    return MPI_FLOAT;
}

/// @brief Return the MPI datatype for a \c signed \c short.
/// @return The MPI datatype for a \c signed \c short.
template<>
inline MPI_Datatype MPI_type<signed short>()
{
    return MPI_SHORT;
}

/// @brief Return the MPI datatype for an \c unsigned \c short.
/// @return The MPI datatype for an \c unsigned \c short.
template<>
inline MPI_Datatype MPI_type<unsigned short>()
{
    return MPI_UNSIGNED_SHORT;
}

}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_UTILS_MPI_MPI_TYPE_HH
