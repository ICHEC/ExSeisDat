/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date December 2016
 *   \brief
 *   \details C11 Set API header
 *//*******************************************************************************************/
#ifndef PIOLCSET_INCLUDE_GUARD
#define PIOLCSET_INCLUDE_GUARD
#include "cfileapi.h"
#ifdef __cplusplus
extern "C"
{
#endif
typedef struct ExSeisSetWrapper * ExSeisSet;

/*! Initialise the set.
 *  \param[in] piol The PIOL handle
 *  \param[in] pattern The file-matching pattern
 */
extern ExSeisSet initSet(ExSeisHandle piol, const char * ptrn);

/*! Free (deinit) the set.
 *  \param[in] s The set handle
 */
extern void freeSet(ExSeisSet s);

/*! Get the min and the max of a set of parameters passed. This is a parallel operation. It is
 *  the collective min and max across all processes (which also must all call this file).
 *  \param[in] s The set handle
 *  \param[in] m1 The first parameter type
 *  \param[in] m2 The second parameter type
 *  \param[out] minmax An array of structures containing the minimum item.x,  maximum item.x, minimum item.y, maximum item.y
 *  and their respective trace numbers.
 */
extern void getMinMaxSet(ExSeisSet s, Meta m1, Meta m2, CoordElem * minmax);

/*! Sort the set by the specified sort type.
 *  \param[in] s The set handle
 *  \param[in] type The sort type
 */
extern void sortSet(ExSeisSet s, SortType type);

/*! Preform 2 tailed taper on a set of traces
 * \param[in] s A handle for the set
 * \param[in] type The type of taper to be applied to traces.
 * \param[in] ntpstr The length of left-tail taper ramp.
 * \param[in] ntpend The length of right-tail taper ramp.
 */
extern void taper2Tail(ExSeisSet s, TaperType type, size_t ntpstr, size_t ntpend);

/*! Preform 1 tailed taper on a set of traces
 * \param[in] s A handle for the set
 * \param[in] type The type of taper to be applied to traces.
 * \param[in] ntpstr The length of taper ramp.
 */
extern void taper1Tail(ExSeisSet s, TaperType type, size_t ntpstr);

/*! The number of traces in the input files
 *  \param[in] s The set handle
 *  \return The number of traces in the input files
 */
extern size_t getInNt(ExSeisSet s);

/*! The number of traces associated with the local process
 *  \param[in] s The set handle
 *  \return The number of traces associated with the local process
 */
extern size_t getLNtSet(ExSeisSet s);

/*! Output using the given output prefix
 *  \param[in] s The set handle
 *  \param[in] oname The output prefix
 */
extern void outputSet(ExSeisSet s, const char * oname);

/*! Set the text-header of the output
 *  \param[in] s The set handle
 *  \param[in] outmsg The output message
 */
extern void textSet(ExSeisSet s, const char * outmsg);

/*! Summarise the current status by whatever means the PIOL instrinsically supports
 *  \param[in] s The set handle
 */
extern void summarySet(ExSeisSet s);

/*! Add a file to the set based on the name given
 *  \param[in] s The set handle
 *  \param[in] name The input name
 */
extern void addSet(ExSeisSet s, const char * name);

/*! Sort the set using a custom comparison function
 *  \param[in] s A handle for the set.
 *  \param[in] func The custom comparison function to sort set
 */
extern void sortCustomSet(ExSeisSet s, bool (* func)(const CParam a, const CParam b));

/*! Scale traces using automatic gain control for visualization
 * \param[in] s The set handle
 * \param[in] type They type of agc scaling function used
 * \param[in] window Length of the agc window
 * \param[in] normR Normalization value
 */
extern void AGC(ExSeisSet s, AGCType type, size_t window, float normR);

#ifdef __cplusplus
}
#endif
#endif
