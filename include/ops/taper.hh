/*******************************************************************************************//*!
 *   \file
 *   \author Meghan Fisher - meghan.fisher@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date May 2017
 *//*******************************************************************************************/

#ifndef PIOLOPSTAPER_INCLUDE_GUARD
#define PIOLOPSTAPER_INCLUDE_GUARD
#include "global.hh"
namespace PIOL { namespace File {
/******************************************** Core *********************************************/
/*! Apply a taper to a set of traces --> used for acutal operation during output
 * \param[in] nt The number of traces
 * \parma[in] ns The number of samples in a trace
 * \param[in] trc Vector of all traces
 * \param[in] func Weight function for the taper ramp
 * \param[in] ntpstr Length of left tail of taper
 * \param[in] ntpend Length of right tail of taper
 * \return Vector of tapered traces
 */
extern void taper(size_t sz, size_t ns, trace_t * trc, std::function<trace_t(trace_t weight, trace_t ramp)> func, size_t nTailLft, size_t nTailRt);

/******************************************** Non-core *********************************************/
/*! Choose the window type for taper function
 * \param[in] type The window type
 */
extern std::function<trace_t(trace_t, trace_t)> getTap(TaperType type);
}}
#endif
