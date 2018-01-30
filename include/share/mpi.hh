/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date Summer 2016
 *   \brief
 *   \details MPI Functions etc which are shared between communication and MPI-IO
 *//*******************************************************************************************/
#ifndef PIOLSHAREDMPI_INCLUDE_GUARD
#define PIOLSHAREDMPI_INCLUDE_GUARD
#include <limits>
#include <mpi.h>
#include <typeinfo>
#include "global.hh"

namespace PIOL {
/*! \brief Check the MPI error code and log an error event if there is an error.
 *  \param[in,out] log  The log object
 *  \param[in] file The related file. Use "" if the option is not applicable.
 *  \param[in] layer The layer one was working with (generally Comm or Data).
 *  \param[in] err The MPI error code.
 *  \param[in] stat The MPI_Status object. Provide NULL or nullptr if it is not used.
 *  \param[in] msg The message one wishes to record.
 *
 *  The side-effect of the function is to log an item with \c piol if there is an error
 *  otherwise no side-effect occurs. The function always returns.
 */
extern void printErr(Log::Logger * log, const std::string file, const Log::Layer layer, const int err,
                                        const MPI_Status * stat, std::string msg);

/*! \brief Return the fundamental MPI datatype associated with a fundamental datatype.
 *  \return The datatype. If the datatype is not known MPI_BYTE is returned.
 *  \tparam T The C++ datatype
 */
template <typename T>
#ifndef __ICC
constexpr
#endif
MPI_Datatype MPIType(void)
{
    return (typeid(T) == typeid(double)             ? MPI_DOUBLE
         : (typeid(T) == typeid(long double)        ? MPI_LONG_DOUBLE
         : (typeid(T) == typeid(char)               ? MPI_CHAR
         : (typeid(T) == typeid(uchar)              ? MPI_UNSIGNED_CHAR
         : (typeid(T) == typeid(int)                ? MPI_INT
         : (typeid(T) == typeid(long int)           ? MPI_LONG
         : (typeid(T) == typeid(size_t)             ? MPI_UNSIGNED_LONG //Watch out for this one!
         : (typeid(T) == typeid(unsigned long int)  ? MPI_UNSIGNED_LONG
         : (typeid(T) == typeid(unsigned int)       ? MPI_UNSIGNED
         : (typeid(T) == typeid(long long int)      ? MPI_LONG_LONG_INT
         : (typeid(T) == typeid(int64_t)            ? MPI_LONG_LONG_INT
         : (typeid(T) == typeid(float)              ? MPI_FLOAT
         : (typeid(T) == typeid(signed short)       ? MPI_SHORT
         : (typeid(T) == typeid(unsigned short)     ? MPI_UNSIGNED_SHORT
         : MPI_BYTE))))))))))))));
}


/*! \brief Return the known limit for Intel MPI on Fionn for a type of the given size
 *  \param[in] sz The datatype size one wishes to find the limit for
 *  \return The size in counts
 */
inline size_t getLimSz(size_t sz)
{
    //If you aren't (4096 - Chunk)/Chunk from the limit, intel mpi breaks on Fionn.
    //Probably something to do with pages.
    return (std::numeric_limits<int>::max() - (4096U - sz)) / sz;
}

/*! \brief Return the known limit for Intel MPI on Fionn for a given type.
 *  \tparam T The type one wishes to find the limit for
 *  \return The size in counts
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
