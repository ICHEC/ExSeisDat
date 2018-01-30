/*******************************************************************************************//*!
 *   @file
 *   @author Cathal O Broin - cathal@ichec.ie - first commit
 *   @date January 2017
 *   @brief
 *   @details
 *//*******************************************************************************************/
#include "4dio.hh"
namespace PIOL { namespace FOURD {
/*! Error check the mpi error value.
 *  @param[in] err The mpi error value.
 */
inline void MPIErr(int err)
{
    assert(err == MPI_SUCCESS);
}

/*! For each trace, insert into min the trace number from coords2 (distributed)
 *  that minimise the difference between the respective src/rcv coordinates.
 *  @param[in] piol The piol object.
 *  @param[in] dsrmax The maximum possible distance an acceptable solution can be away from the coords1 trace.
 *             The smaller the value, the quicker the search but the lower the tolerance in a match.
 *  @param[in] coords1 The coordinates for each trace in file 1. The coordinates should be distributed
 *             across all processes.
 *  @param[in] coords2 The coordinates for each trace in file 2. The coordinates should be distributed
 *             across all processes.
 *  @param[in] opt Options
 *  @param[out] min After the function is called this vector will contain in the entry for each trace
 *             the minimum trace. A minimum trace may not be present if the only acceptable trace was more than dsrmax
 *             away.
 *  @param[out] minrs The dsr value associated with the trace match in min.
 */
extern void calc4DBin(ExSeisPIOL * piol, const fourd_t dsrmax, const Coords * coords1, const Coords * coords2,
                                         const FourDOpt opt, vec<size_t> & min, vec<fourd_t> & minrs);

}}
