////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief C11 Flow API header
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_H
#define EXSEISDAT_FLOW_H

#include "exseisdat/piol.h"
#include "exseisdat/piol/operations/sort_operations/SortType.h"
#include "exseisdat/utils/signal_processing/AGC.h"
#include "exseisdat/utils/signal_processing/Taper_function.h"

//
// Forward declare types used for C API handles
//
#ifdef __cplusplus
#include "exseisdat/flow/Set.hh"

/// @copydoc exseis::flow::Set
typedef exseis::flow::Set PIOL_Set;
#else
// Forward declare an opaque struct in C
/// @copydoc exseis::piol::Set
typedef struct PIOL_Set PIOL_Set;
#endif


#ifdef __cplusplus
namespace exseis {
namespace piol {

using namespace exseis::utils;

// Everything from here on is C API functions needing C linkage.
extern "C" {
#endif  // __cplusplus

/// @name PIOL Set functions
///
/// @{

/*! @brief Initialise the set.
 *  @param[in] piol    The PIOL handle
 *  @param[in] pattern The file-matching pattern
 *  @return The set handle
 */
PIOL_Set* piol_set_new(const piol_exseis* piol, const char* pattern);

/*! @brief Free (deinit) the set.
 *  @param[in,out] set The set handle
 */
void piol_set_delete(PIOL_Set* set);

/*! @brief Get the min and the max of a set of parameters passed.
 *
 *  @details This is a parallel operation. It is the collective min and max
 *           across all processes (which also must all call this file).
 *  @param[in,out] set    The set handle
 *  @param[in]     m1     The first parameter type
 *  @param[in]     m2     The second parameter type
 *  @param[out]    minmax An array of structures containing the minimum item.x,
 *                        maximum item.x, minimum item.y, maximum item.y and
 *                        their respective trace numbers.
 */
void piol_set_get_min_max(
    PIOL_Set* set,
    exseis_Trace_metadata_key m1,
    exseis_Trace_metadata_key m2,
    struct PIOL_CoordElem* minmax);

/*! @brief Sort the set by the specified sort type.
 *  @param[in,out] set  The set handle
 *  @param[in]     type The sort type
 */
void piol_set_sort(PIOL_Set* set, exseis_SortType type);

/*! @brief Sort the set using a custom comparison function
 *  @param[in,out] set  A handle for the set.
 *  @param[in]     func The custom comparison function to sort set
 */
void piol_set_sort_fn(
    PIOL_Set* set,
    bool (*func)(const piol_file_trace_metadata* param, size_t i, size_t j));

/*! @brief Perform tailed taper on a set of traces
 *  @param[in,out] set    A handle for the set
 *  @param[in]     taper_function The type of taper to be applied to traces.
 *  @param[in]     ntpstr The length of left-tail taper ramp.
 *  @param[in]     ntpend The length of right-tail taper ramp
 *                        (pass 0 for no ramp).
 */
void piol_set_taper(
    PIOL_Set* set,
    exseis_Taper_function taper_function,
    size_t ntpstr,
    size_t ntpend);

/*! @brief Output using the given output prefix
 *  @param[in,out] set   The set handle
 *  @param[in]     oname The output prefix
 */
void piol_set_output(PIOL_Set* set, const char* oname);

/*! @brief Set the text-header of the output
 *  @param[in,out] set    The set handle
 *  @param[in]     outmsg The output message
 */
void piol_set_text(PIOL_Set* set, const char* outmsg);

/*! @brief Summarise the current status by whatever means the PIOL
 *         instrinsically supports
 *  @param[in] set The set handle
 */
void piol_set_summary(const PIOL_Set* set);

/*! @brief Add a file to the set based on the name given
 *  @param[in,out] set The set handle
 *  @param[in]     name The input name
 */
void piol_set_add(PIOL_Set* set, const char* name);

/*! @brief Scale traces using automatic gain control for visualization
 *  @param[in,out] set The set handle
 *  @param[in] type They type of agc scaling function used
 *  @param[in] window Length of the agc window
 *  @param[in] target_amplitude Normalization value
 */
void piol_set_agc(
    PIOL_Set* set,
    exseis_Gain_function type,
    size_t window,
    exseis_Trace_value target_amplitude);

/// @} PIOL Set functions

#ifdef __cplusplus
}  // extern "C"
}  // namespace piol
}  // namespace exseis
#endif  // __cplusplus

#endif  // EXSEISDAT_FLOW_H
