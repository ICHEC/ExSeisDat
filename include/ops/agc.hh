/*******************************************************************************************//*!
 *   \file
 *   \author Meghan Fisher - meghan.fisher@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date May 2017
 *   \brief
*//*******************************************************************************************/
#ifndef PIOLOPSAGC_INCLUDE_GUARD
#define PIOLOPSAGC_INCLUDE_GUARD
#include "global.hh"

namespace PIOL { namespace File {

/*! Find the normalized root mean square (RMS) of traces in a rectangualr window
 * \param[in] trc Trace amplitudes
 * \param[in] strt Starting iterator for traces in window
 * \param[in] window Window length
 * \param[in] normR Value to which traces are normalized
 * \param[in] winCntr Window center iterator
 * \return The normalized trace value using RMS
 */
trace_t rms(trace_t * trc, size_t strt, size_t window, trace_t normR, size_t winCntr);

/*! Find the normalized root mean square (RMS) of traces in a triangular window
 * \param[in] trc Trace amplitudes
 * \param[in] strt Starting iterator for traces in window
 * \param[in] window Window length
 * \param[in] normR Value to which traces are normalized
 * \param[in] winCntr Window center iterator
 * \return The normalized trace value using RMS with a triangular window
 */
trace_t rmsTri(trace_t * trc, size_t strt, size_t window, trace_t normR, size_t winCntr);

/*! Find the normalized mean absolute value (MAV) of traces in a rectangualr window
 * \param[in] trc Trace amplitudes
 * \param[in] strt Starting iterator for traces in window
 * \param[in] window Window length
 * \param[in] normR Value to which traces are normalized
 * \param[in] winCntr Window center iterator
 * \return The normalized trace value using MAV
 */
trace_t meanAbs(trace_t * trc, size_t strt, size_t window, trace_t normR, size_t winCntr);

/*! Find the normalized median value inside a window trace amplitude
 * \param[in] trc Trace amplitudes
 * \param[in] strt Starting iterator for traces in window
 * \param[in] window Window length
 * \param[in] normR Value to which traces are normalized
 * \param[in] winCntr Window center iterator
 * \return The normalized median trace value
 */
trace_t median(trace_t * trc, size_t strt, size_t window, trace_t normR, size_t winCntr);

/*! Apply automatic gain control to a set of tapers --> used for actual operation during output
 * \param[in] nt The number of traces
 * \parma[in] ns The number of samples in a trace
 * \param[in] trc Vector of all traces
 * \param[in] func Staistical function used to scale traces
 * \param[in] window Length of the agc window
 * \param[in] normR Value to which traces are normalized
 */
void agc(size_t nt, size_t ns, trace_t * trc, std::function<trace_t(trace_t * trc, size_t strt, size_t win,
         trace_t normR, size_t winCntr)> func, size_t window, trace_t normR);
}
}
#endif
