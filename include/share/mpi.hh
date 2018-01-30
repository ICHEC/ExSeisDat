/*******************************************************************************************//*!
 *   @file
 *   @author Cathal O Broin - cathal@ichec.ie - first commit
 *   @copyright TBD. Do not distribute
 *   @date Summer 2016
 *   @brief
 *   @details MPI Functions etc which are shared between communication and MPI-IO
 *//*******************************************************************************************/
#ifndef PIOLSHAREDMPI_INCLUDE_GUARD
#define PIOLSHAREDMPI_INCLUDE_GUARD
#include <limits>
#include <mpi.h>
#include <typeinfo>
#include "global.hh"
#include "anc/log.hh"

namespace PIOL {
/*! @brief Check the MPI error code and log an error event if there is an error.
 *  @param[in,out] log  The log object
 *  @param[in] file The related file. Use "" if the option is not applicable.
 *  @param[in] layer The layer one was working with (generally Comm or Data).
 *  @param[in] err The MPI error code.
 *  @param[in] stat The MPI_Status object. Provide NULL or nullptr if it is not used.
 *  @param[in] msg The message one wishes to record.
 *
 *  The side-effect of the function is to log an item with \c piol if there is an error
 *  otherwise no side-effect occurs. The function always returns.
 */
extern void printErr(Log::Logger * log, const std::string file, const Log::Layer layer, const int err,
                                        const MPI_Status * stat, std::string msg);

/*! @brief Return the fundamental MPI datatype associated with a fundamental datatype.
 *  @return The datatype.
 *  @tparam T The C++ datatype
 */
template <typename T>
constexpr MPI_Datatype MPIType() {
    static_assert(sizeof(T) == 0, "Unknown MPI type!");
    return MPI_BYTE;
}

template<>
constexpr MPI_Datatype MPIType<double>() { return MPI_DOUBLE; }

template<>
constexpr MPI_Datatype MPIType<long double>() { return MPI_LONG_DOUBLE; }

template<>
constexpr MPI_Datatype MPIType<char>() { return MPI_CHAR; }

template<>
constexpr MPI_Datatype MPIType<unsigned char>() { return MPI_UNSIGNED_CHAR; }

template<>
constexpr MPI_Datatype MPIType<int>() { return MPI_INT; }

template<>
constexpr MPI_Datatype MPIType<long int>() { return MPI_LONG; }

template<>
constexpr MPI_Datatype MPIType<unsigned long int>() { return MPI_UNSIGNED_LONG; }

template<>
constexpr MPI_Datatype MPIType<unsigned int>() { return MPI_UNSIGNED; }

template<>
constexpr MPI_Datatype MPIType<long long int>() { return MPI_LONG_LONG_INT; }

template<>
constexpr MPI_Datatype MPIType<float>() { return MPI_FLOAT; }

template<>
constexpr MPI_Datatype MPIType<signed short>() { return MPI_SHORT; }

template<>
constexpr MPI_Datatype MPIType<unsigned short>() { return MPI_UNSIGNED_SHORT; }


/*! @brief Return the known limit for Intel MPI on Fionn for a type of the given size
 *  @param[in] sz The datatype size one wishes to find the limit for
 *  @return The size in counts
 */
inline size_t getLimSz(size_t sz)
{
    //If you aren't (4096 - Chunk)/Chunk from the limit, intel mpi breaks on Fionn.
    //Probably something to do with pages.
    return (std::numeric_limits<int>::max() - (4096U - sz)) / sz;
}

/*! @brief Return the known limit for Intel MPI on Fionn for a given type.
 *  @tparam T The type one wishes to find the limit for
 *  @return The size in counts
 */
template <typename T>
constexpr size_t getLim()
{
    //If you aren't (4096 - Chunk)/Chunk from the limit, intel mpi breaks on Fionn.
    //Probably something to do with pages.
    //return MPI_Offset((std::numeric_limits<int>::max() - (4096U - sizeof(T))) / sizeof(T));
    //return (std::numeric_limits<int>::max() - (4096U - sizeof(T))) / sizeof(T);
    return getLimSz(sizeof(T));
}
}
#endif
