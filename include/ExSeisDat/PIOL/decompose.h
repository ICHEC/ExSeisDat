////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The \c decompose functions decompose a range over a number of
///        processes.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_DECOMPOSE_H
#define EXSEISDAT_PIOL_DECOMPOSE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// The \c Range class represents a section of a range distributed over a
/// number of processes.
struct PIOL_Range {
    /// The offset into the range on the local process
    size_t offset;

    /// The size of the section of the range on the local process
    size_t size;
};

/*! Perform a 1d decomposition so that the load is optimally balanced.
 *  @param[in] sz The sz of the 1d domain
 *  @param[in] numRank The number of ranks to perform the decomposition over
 *  @param[in] rank The rank of the local process
 *  @return Return a pair (offset, size).
 *          The first element is the offset for the local process,
 *          the second is the size for the local process.
 */
struct PIOL_Range PIOL_decompose(size_t sz, size_t numRank, size_t rank);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus


#ifdef __cplusplus

#include "ExSeisDat/PIOL/ExSeisPIOL.hh"
#include "ExSeisDat/PIOL/ReadInterface.hh"

namespace PIOL {

/// @copydoc PIOL_Range
using Range = PIOL_Range;

/*! Perform a 1d decomposition so that the load is optimally balanced.
 *  @param[in] sz The sz of the 1d domain
 *  @param[in] numRank The number of ranks to perform the decomposition over
 *  @param[in] rank The rank of the local process
 *  @return Return a pair (offset, size).
 *          The first element is the offset for the local process,
 *          the second is the size for the local process.
 */
inline Range decompose(size_t sz, size_t numRank, size_t rank)
{
    return PIOL_decompose(sz, numRank, rank);
}

/*! An overload for a common decomposition case. Perform a decomposition of
 *  traces so that the load is optimally balanced.
 *  @param[in] piol The piol object
 *  @param[in] file A read file object. This is used to find the number of
 *             traces.
 *  @return Return a pair (offset, size).
 *          The first element is the offset for the local process,
 *          the second is the size for the local process.
 */
Range decompose(ExSeisPIOL* piol, ReadInterface* file);

}  // namespace PIOL

#endif  // __cplusplus

#endif  // EXSEISDAT_PIOL_DECOMPOSE_H
