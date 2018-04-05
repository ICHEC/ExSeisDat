////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Meghan Fisher - meghan.fisher@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date May 2017
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_OPERATIONS_TAPER_HH
#define EXSEISDAT_PIOL_OPERATIONS_TAPER_HH

#include "ExSeisDat/PIOL/TaperType.h"
#include "ExSeisDat/PIOL/typedefs.h"

#include <functional>

namespace PIOL {

/// A typedef for the taper function
typedef std::function<trace_t(trace_t, trace_t)> TaperFunc;

/************************************ Core ************************************/
/*! Apply a taper to a set of traces.
 * @param[in] sz The number of traces
 * @param[in] ns The number of samples per trace
 * @param[in] trc Vector of all traces
 * @param[in] func Weight function for the taper ramp
 * @param[in] nTailLft Length of left tail of taper
 * @param[in] nTailRt Length of right tail of taper
 * @return Vector of tapered traces
 */
void taper(
  size_t sz,
  size_t ns,
  trace_t* trc,
  TaperFunc func,
  size_t nTailLft,
  size_t nTailRt);

/********************************** Non-core **********************************/
/*! Choose the window type for taper function
 *  @param[in] type The window type
 *  @return Return the appropriate taper function
 */
TaperFunc getTap(TaperType type);

}  // namespace PIOL

#endif  // EXSEISDAT_PIOL_OPERATIONS_TAPER_HH
