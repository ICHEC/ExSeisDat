////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief C11 Flow API header
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_H
#define EXSEISDAT_FLOW_H

#include "ExSeisDat/PIOL/SortType.h"
#include "ExSeisDat/utils/signal_processing/AGC.h"
#include "ExSeisDat/utils/signal_processing/Taper_function.h"
#include "PIOL.h"

//
// Forward declare types used for C API handles
//
#ifdef __cplusplus
#include "ExSeisDat/Flow/Set.hh"

/// @todo DELETE ME! The functions here should be wrapped in namespaces.
using exseis::utils::exseis_Gain_function;
using exseis::utils::exseis_Taper_function;

/// @copydoc exseis::Flow::Set
typedef exseis::Flow::Set PIOL_Set;
#else
// Forward declare an opaque struct in C
/// @copydoc exseis::PIOL::Set
typedef struct PIOL_Set PIOL_Set;
#endif


#ifdef __cplusplus
// Everything from here on is C API functions needing C linkage.
extern "C" {
#endif


/*! Initialise the set.
 *  @param[in] piol    The PIOL handle
 *  @param[in] pattern The file-matching pattern
 *  @return The set handle
 */
PIOL_Set* PIOL_Set_new(const PIOL_ExSeis* piol, const char* pattern);

/*! Free (deinit) the set.
 *  @param[in,out] set The set handle
 */
void PIOL_Set_delete(PIOL_Set* set);

/*! Get the min and the max of a set of parameters passed. This is a parallel
 *  operation. It is the collective min and max across all processes (which also
 *  must all call this file).
 *  @param[in,out] set    The set handle
 *  @param[in]     m1     The first parameter type
 *  @param[in]     m2     The second parameter type
 *  @param[out]    minmax An array of structures containing the minimum item.x,
 *                        maximum item.x, minimum item.y, maximum item.y and
 *                        their respective trace numbers.
 */
void PIOL_Set_getMinMax(
  PIOL_Set* set, PIOL_Meta m1, PIOL_Meta m2, struct PIOL_CoordElem* minmax);

/*! Sort the set by the specified sort type.
 *  @param[in,out] set  The set handle
 *  @param[in]     type The sort type
 */
void PIOL_Set_sort(PIOL_Set* set, PIOL_SortType type);

/*! Sort the set using a custom comparison function
 *  @param[in,out] set  A handle for the set.
 *  @param[in]     func The custom comparison function to sort set
 */
void PIOL_Set_sort_fn(
  PIOL_Set* set,
  bool (*func)(const PIOL_File_Param* param, size_t i, size_t j));

/*! Perform tailed taper on a set of traces
 *  @param[in,out] set    A handle for the set
 *  @param[in]     taper_function The type of taper to be applied to traces.
 *  @param[in]     ntpstr The length of left-tail taper ramp.
 *  @param[in]     ntpend The length of right-tail taper ramp
 *                        (pass 0 for no ramp).
 */
void PIOL_Set_taper(
  PIOL_Set* set,
  exseis_Taper_function taper_function,
  size_t ntpstr,
  size_t ntpend);

/*! Output using the given output prefix
 *  @param[in,out] set   The set handle
 *  @param[in]     oname The output prefix
 */
void PIOL_Set_output(PIOL_Set* set, const char* oname);

/*! Set the text-header of the output
 *  @param[in,out] set    The set handle
 *  @param[in]     outmsg The output message
 */
void PIOL_Set_text(PIOL_Set* set, const char* outmsg);

/*! Summarise the current status by whatever means the PIOL instrinsically
 *  supports
 *  @param[in] set The set handle
 */
void PIOL_Set_summary(const PIOL_Set* set);

/*! Add a file to the set based on the name given
 *  @param[in,out] set The set handle
 *  @param[in]     name The input name
 */
void PIOL_Set_add(PIOL_Set* set, const char* name);

/*! Scale traces using automatic gain control for visualization
 *  @param[in,out] set The set handle
 *  @param[in] type They type of agc scaling function used
 *  @param[in] window Length of the agc window
 *  @param[in] target_amplitude Normalization value
 */
void PIOL_Set_AGC(
  PIOL_Set* set,
  exseis_Gain_function type,
  size_t window,
  exseis_Trace_value target_amplitude);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // EXSEISDAT_FLOW_H
