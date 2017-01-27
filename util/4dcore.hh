/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \date January 2017
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include "4dio.hh"
namespace PIOL { namespace FOURD {
/*! Calcuate the difference criteria between source/receiver pairs between traces. For each trace in
 *  file1 the fourdbin utility finds the trace from file2 which is the minimum distance away.
 *  \param[in] prm1 A pointer to an array of size 4 which has sx, sy, rx, ry in that order. (file1
 *  \param[in] prm2 A pointer to an array of size 4 which has sx, sy, rx, ry in that order. (file2)
 *  \return Return the dsr value.
 *
 * The definitions of ds and dr are:
 * ds = \sqrt((sx1-sx2)^2+(sy1-sy2)^2)
 * dr = \sqrt((rx1-rx2)^2+(ry1-ry2)^2)
 * The boat may be going in the opposite direction for the data in file2 so that the source from file1
 * is closer to the receiver in file2 and vice-versa. We interchange _sx->rx, _sy->ry _rx->sx _ry->sy
 * for prm2 in that case (prm2).
 * rds = \sqrt((sx1-rx2)^2+(sy1-ry2)^2)
 * rdr = \sqrt((rx1-sx2)^2+(ry1-sy2)^2)
 * dsr = (min(ds, rds) + min(dr, rdr))^2
 * \todo Compute load just jumped up. No Branching. Acceleration opportunity! */
inline geom_t dsr(geom_t xs1, geom_t ys1, geom_t xr1, geom_t yr1,
           geom_t xs2, geom_t ys2, geom_t xr2, geom_t yr2)
{
    geom_t fds = std::hypot(xs1 - xs2, ys1 - ys2);
    geom_t fdr = std::hypot(xr1 - xr2, yr1 - yr2);

    //Reverse-Boat cases.
    geom_t rds = std::hypot(xs1 - xr2, ys1 - yr2);
    geom_t rdr = std::hypot(xr1 - xs2, yr1 - ys2);

    //The sum of ds and dr will be minimised.
    //ds=0, dr=10 will be preferred over
    //ds=4.55 dr=5.5
    return std::min(fds + fdr, rds + rdr) ;

    //This gives preference to a value that minimises
    //both ds and dr to some extent.
    //ds=4.55 dr=5.5 will be preferred over
    //ds=0, dr=10
//    return ds + dr + std::abs(ds-dr);
}
/*! Perform a minimisation check with the current two vectors of parameters.
 *  \tparam Init If true, perform the initialisation sequence.
 *  \param[in] szall A vector containing the amount of data each process has from the second input file.
 *  \param[in] local A vector containing the process's parameter data from the first input file. This data
 *             is never sent to any other process.
 *  \param[in] other A vector containing the parameter data from another process.
 *  \param[in,out] min A vector containing the trace number of the trace that minimises the dsdr criteria.
 *                 This vector is updated by the loop.
 *  \param[in,out] minrs A vector containing the dsdr value of the trace that minimises the dsdr criteria.
 *                 This vector is updated by the loop.
 */
extern void initUpdate(size_t offset, Coords * local, Coords * other, vec<size_t> & min, vec<geom_t> & minrs);

/*! Perform a minimisation check with the current two vectors of parameters.
 *  \tparam Init If true, perform the initialisation sequence.
 *  \param[in] szall A vector containing the amount of data each process has from the second input file.
 *  \param[in] local A vector containing the process's parameter data from the first input file. This data
 *             is never sent to any other process.
 *  \param[in] other A vector containing the parameter data from another process.
 *  \param[in,out] min A vector containing the trace number of the trace that minimises the dsdr criteria.
 *                 This vector is updated by the loop.
 *  \param[in,out] minrs A vector containing the dsdr value of the trace that minimises the dsdr criteria.
 *                 This vector is updated by the loop.
 */
extern void update(size_t offset, Coords * local, Coords * other, vec<size_t> & min, vec<geom_t> & minrs);
}}
