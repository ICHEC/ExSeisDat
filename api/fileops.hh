/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date December 2016
 *   \brief
 *   \details Operation calls in the file-layer.
 *//*******************************************************************************************/
#ifndef PIOLAPIFILEOPS_INCLUDE_GUARD
#define PIOLAPIFILEOPS_INCLUDE_GUARD
#include <assert.h>
#include <memory>
#include "global.hh"
#include "file/file.hh"
#include "ops/minmax.hh"
#include "ops/sort.hh"
#include "share/api.hh"

//#warning Make redundant
#include "cppfileapi.hh"
namespace PIOL { namespace File {
extern Compare<Param> getComp(SortType type);

    extern std::function<float(float, float)> getTap(TaperType type);
//extern void getMinMax(ExSeisPIOL * piol, size_t offset, size_t lnt, const coord_t * coord, CoordElem * minmax);

//TODO: Write a test for this function
/*! Get the min and the max of a set of parameters passed. This is a parallel operation. It is
 *  the collective min and max across all processes (which also must all call this file).
 *  \param[in, out] piol The PIOL object
 *  \param[in] offset The starting trace number (local).
 *  \param[in] lnt The local number of traces to process.
 *  \param[in] item The coordinate item of interest.
 *  \param[in] prm An array of trace parameter structures
 *  \param[out] minmax An array of structures containing the minimum item.x,  maximum item.x, minimum item.y, maximum item.y
 *  and their respective trace numbers.
 */
//extern void getMinMax(ExSeisPIOL * piol, size_t offset, size_t lnt, Meta m, const Param * prm, CoordElem * minmax);
extern void getMinMax(ExSeisPIOL * piol, size_t offset, size_t sz, Meta m1, Meta m2, const Param * prm, CoordElem * minmax);

/*! Perform a sort on the given parameter structure.
 *  \param[in] piol The PIOL object
 *  \param[in] type The sort type
 *  \param[in,out] prm The trace parameter structure.
 *  \return Return a vector which is a list of the ordered trace numbers. i.e the 0th member
 *          is the position of the 0th trace post-sort.
 */
extern std::vector<size_t> sort(ExSeisPIOL * piol, SortType type, Param * prm);

/*! Check that the file obeys the expected ordering.
 *  \param[in] src The input file.
 *  \param[in] dec The decomposition: a pair which contains the offset (first) and the number of traces for the local process.
 *  \return Return true if the local ordering is correct.
 */
extern bool checkOrder(ReadInterface * src, std::pair<size_t, size_t> dec, SortType type);
}}
#endif
