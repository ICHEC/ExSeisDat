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
 * \param[in] window Length of the window
 * \param[in] trc Subset of trace amplitudes contained in window
 * \param[in] normR Value to which traces are normalized
 * \return The normalized trace value using RMS
 */
trace_t rms(size_t window, trace_t * trc, trace_t normR);

/*! Find the normalized root mean square (RMS) of traces in a triangular window
 * \param[in] window Length of the window
 * \param[in] trc Subset of trace amplitudes contained in window
 * \param[in] normR Value to which traces are normalized
 * \return The normalized trace value using RMS with a triangular window
 */
trace_t rmsTri(size_t window, trace_t * trc, trace_t normR);

/*! Find the normalized mean absolute value (MAV) of traces in a rectangualr window
 * \param[in] window Length of the window
 * \param[in] trc Subset of trace amplitudes contained in window
 * \param[in] normR Value to which traces are normalized
 * \return The normalized trace value using MAV
 */
trace_t meanAbs(size_t window, trace_t * trc, trace_t normR);

/*! Find the normalized median value inside a window trace amplitude
 * \param[in] window Length of the window
 * \param[in] trc Subset of trace amplitudes contained in window
 * \param[in] normR Value to which traces are normalized
 * \return The normalized median trace value
 */
trace_t median(size_t window, trace_t * trc, trace_t normR);

/*! Apply automatic gain control to a set of tapers --> used for actual operation during output
 * \param[in] nt The number of traces
 * \parma[in] ns The number of samples in a trace
 * \param[in] trc Vector of all traces
 * \param[in] func Staistical function used to scale traces
 * \param[in] window Length of the agc window
 * \param[in] normR Value to which traces are normalized
 */
void agc(size_t nt, size_t ns, trace_t * trc, std::function<trace_t(size_t win, trace_t * trcWin, trace_t normR)> func,
         size_t window, trace_t normR);
}
}
#endif
