////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @date January 2017
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEIS_PIOL_FOUR_D_4DCORE_HH
#define EXSEIS_PIOL_FOUR_D_4DCORE_HH

#include "4dio.hh"

#include <cassert>

namespace exseis {
namespace apps {
inline namespace fourdbin {

/*! @brief Error check the mpi error value.
 *
 *  @param[in] err The mpi error value.
 */
inline void mpi_err(int err)
{
    (void)err;
    assert(err == MPI_SUCCESS);
}

/*! @brief For each trace, insert into min the trace number from coords2
 *         (distributed) that minimise the difference between the respective
 *         src/rcv coordinates.
 *
 *  @param[in] communicator  The communicator to decompose the problem over
 *  @param[in] dsrmax The maximum possible distance an acceptable solution can
 *                    be away from the coords1 trace.  The smaller the value,
 *                    the quicker the search but the lower the tolerance in a
 *                    match.
 *  @param[in] coords1 The coordinates for each trace in file 1. The coordinates
 *                     should be distributed across all processes.
 *  @param[in] coords2 The coordinates for each trace in file 2. The coordinates
 *                     should be distributed across all processes.
 *  @param[in] opt Options
 *  @param[out] min After the function is called this vector will contain in the
 *                  entry for each trace the minimum trace. A minimum trace may
 *                  not be present if the only acceptable trace was more than
 *                  dsrmax away.
 *  @param[out] minrs The dsr value associated with the trace match in min.
 */
void calc_4d_bin(
    const Communicator_mpi& communicator,
    fourd_t dsrmax,
    const Coords* crd1,
    const Coords* crd2,
    Four_d_opt opt,
    std::vector<size_t>& min,
    std::vector<fourd_t>& minrs);

}  // namespace fourdbin
}  // namespace apps
}  // namespace exseis

#endif  // EXSEIS_PIOL_FOUR_D_4DCORE_HH
