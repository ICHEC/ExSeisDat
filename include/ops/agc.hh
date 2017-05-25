/*******************************************************************************************//*!
 *   \file
 *   \author Meghan Fisher - meghan.fisher@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date May 2017
 *   \brief The Automatic Gain Control Operation
 *   \details This function allows for automatic gain control to be applied to traces in order
 *   to equalize signal amplitude for display purposes. It uses a user defined window to adjust
 *   the gain using a user defined normalization value.  Gain is normalised against  a) RMS
 *   amplitude, b) RMS amplitude with triangle window, c) Mean Absolute Value amplitude or
 *   d) Median amplitude. AGC can be performed on individual traces or entire gathers, where
 *   the same gain is applied to the jth sample in every trace. The window has a spatial
 *   componet.
 *//*******************************************************************************************/
#ifndef PIOLOPSAGC_INCLUDE_GUARD
#define PIOLOPSAGC_INCLUDE_GUARD
#include "global.hh"
#include "share/api.hh"

namespace PIOL { namespace File {
typedef std::function<trace_t(trace_t *, size_t, trace_t, llint)> AGCFunc;  //!< A typedef for the agc stats function

/************************************** Core ***********************************************************/
/*! Apply automatic gain control to a set of tapers --> used for actual operation during output
 * \param[in] nt The number of traces
 * \param[in] ns The number of samples in a trace
 * \param[in] trc Vector of all traces
 * \param[in] func Statistical function used to scale traces
 * \param[in] window Length of the agc window
 * \param[in] normR Value to which traces are normalised
 */
extern void AGC(csize_t nt, csize_t ns, trace_t * trc, AGCFunc func, size_t window, trace_t normR);

/************************************** Non-core ***********************************************************/
/*! Find the normalised root mean square (RMS) of traces in a rectangular window
 * \param[in] trc Trace amplitudes
 * \param[in] window Window length
 * \param[in] normR Value to which traces are normalised
 * \param[in] winCntr Window center iterator
 * \return The normalised trace value using RMS
 */
extern trace_t AGCRMS(trace_t * trc, size_t window, trace_t normR, llint winCntr);

/*! Find the normalised root mean square (RMS) of traces in a triangular window
 * \param[in] trc Trace amplitudes
 * \param[in] window Window length
 * \param[in] normR Value to which traces are normalised
 * \param[in] winCntr Window center iterator
 * \return The normalised trace value using RMS with a triangular window
 */
extern trace_t AGCRMSTri(trace_t * trc, size_t window, trace_t normR, llint winCntr);

/*! Find the normalised mean absolute value (MAV) of traces in a rectangualr window
 * \param[in] trc Trace amplitudes
 * \param[in] window Window length
 * \param[in] normR Value to which traces are normalised
 * \param[in] winCntr Window center iterator
 * \return The normalised trace value using MAV
 */
extern trace_t AGCMeanAbs(trace_t * trc, size_t window, trace_t normR, llint winCntr);

/*! Find the normalised median value inside a window trace amplitude
 * \param[in] trc Trace amplitudes
 * \param[in] window Window length
 * \param[in] normR Value to which traces are normalised
 * \param[in] winCntr Window center iterator
 * \return The normalised median trace value
 */
extern trace_t AGCMedian(trace_t * trc, size_t window, trace_t normR, llint winCntr);

/*! Chooses the type of AGC function
 * \param[in] type The statistical fucntion type
 * \return Return the appropriate AGC function
 */
extern AGCFunc getAGCFunc(AGCType type);
}}
#endif
