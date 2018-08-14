////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date Summer 2016
/// @brief
/// @details MPI Functions etc which are shared between communication and MPI-IO
////////////////////////////////////////////////////////////////////////////////
#ifndef PIOLSHAREDMPI_INCLUDE_GUARD
#define PIOLSHAREDMPI_INCLUDE_GUARD

#include "anc/log.hh"
#include "global.hh"

#include <limits>
#include <mpi.h>
#include <typeinfo>

namespace PIOL {

/*! @brief Check the MPI error code and log an error event if there is an error.
 *  @param[in,out] log  The log object
 *  @param[in] file The related file. Use "" if the option is not applicable.
 *  @param[in] layer The layer one was working with (generally Comm or Data).
 *  @param[in] err The MPI error code.
 *  @param[in] stat The MPI_Status object. Provide NULL or nullptr if it is not
 *             used.
 *  @param[in] msg The message one wishes to record.
 *
 *  The side-effect of the function is to log an item with \c piol if there is
 *  an error otherwise no side-effect occurs. The function always returns.
 */
void printErr(
  Log::Logger* log,
  const std::string file,
  const Log::Layer layer,
  const int err,
  const MPI_Status* stat,
  std::string msg);

/*! @brief Return the fundamental MPI datatype associated with a fundamental
 *         datatype.
 *  @return The datatype.
 *  @tparam T The C++ datatype
 */
template<typename T>
inline MPI_Datatype MPIType()
{
    static_assert(sizeof(T) == 0, "Unknown MPI type!");
    return MPI_BYTE;
}

/// @brief Return the MPI datatype for a \c double.
/// @return The MPI datatype for \c double.
template<>
inline MPI_Datatype MPIType<double>()
{
    return MPI_DOUBLE;
}

/// @brief Return the MPI datatype for a \c long double.
/// @return The MPI datatype for \c long \c double.
template<>
inline MPI_Datatype MPIType<long double>()
{
    return MPI_LONG_DOUBLE;
}

/// @brief Return the MPI datatype for a \c long double.
/// @return The MPI datatype for a \c long double.
template<>
inline MPI_Datatype MPIType<char>()
{
    return MPI_CHAR;
}

/// @brief Return the MPI datatype for an \c unsigned \c char.
/// @return The MPI datatype for an \c unsigned \c char.
template<>
inline MPI_Datatype MPIType<unsigned char>()
{
    return MPI_UNSIGNED_CHAR;
}

/// @brief Return the MPI datatype for an \c int.
/// @return The MPI datatype for an \c int.
template<>
inline MPI_Datatype MPIType<int>()
{
    return MPI_INT;
}

/// @brief Return the MPI datatype for a \c long \c int.
/// @return The MPI datatype for a \c long \c int.
template<>
inline MPI_Datatype MPIType<long int>()
{
    return MPI_LONG;
}

/// @brief Return the MPI datatype for an \c unsigned \c long \c int.
/// @return The MPI datatype for an \c unsigned \c long \c int.
template<>
inline MPI_Datatype MPIType<unsigned long int>()
{
    return MPI_UNSIGNED_LONG;
}

/// @brief Return the MPI datatype for an \c unsigned \c int.
/// @return The MPI datatype for an \c unsigned \c int.
template<>
inline MPI_Datatype MPIType<unsigned int>()
{
    return MPI_UNSIGNED;
}

/// @brief Return the MPI datatype for a \c long \c long \c int.
/// @return The MPI datatype for a \c long \c long \c int.
template<>
inline MPI_Datatype MPIType<long long int>()
{
    return MPI_LONG_LONG_INT;
}

/// @brief Return the MPI datatype for a \c float.
/// @return The MPI datatype for a \c float.
template<>
inline MPI_Datatype MPIType<float>()
{
    return MPI_FLOAT;
}

/// @brief Return the MPI datatype for a \c signed \c short.
/// @return The MPI datatype for a \c signed \c short.
template<>
inline MPI_Datatype MPIType<signed short>()
{
    return MPI_SHORT;
}

/// @brief Return the MPI datatype for an \c unsigned \c short.
/// @return The MPI datatype for an \c unsigned \c short.
template<>
inline MPI_Datatype MPIType<unsigned short>()
{
    return MPI_UNSIGNED_SHORT;
}


/*! @brief Return the known limit for Intel MPI on Fionn for a type of the given
 *         size
 *  @param[in] sz The datatype size one wishes to find the limit for
 *  @return The size in counts
 */
inline size_t getLimSz(size_t sz)
{
    // If you aren't (4096 - Chunk)/Chunk from the limit, intel mpi breaks on
    // Fionn.  Probably something to do with pages.
    return (std::numeric_limits<int>::max() - (4096U - sz)) / sz;
}

/*! @brief Return the known limit for Intel MPI on Fionn for a given type.
 *  @tparam T The type one wishes to find the limit for
 *  @return The size in counts
 */
template<typename T>
constexpr size_t getLim()
{
    // If you aren't (4096 - Chunk)/Chunk from the limit, intel mpi breaks on
    // Fionn.  Probably something to do with pages.
    // return MPI_Offset(
    //   (std::numeric_limits<int>::max() - (4096U - sizeof(T))) / sizeof(T));
    // return (std::numeric_limits<int>::max() - (4096U - sizeof(T)))
    //        / sizeof(T);
    return getLimSz(sizeof(T));
}

}  // namespace PIOL

#endif
