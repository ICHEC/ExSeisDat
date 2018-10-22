////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief C API for \ref exseisdat/piol/operations/minmax.hh
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_OPERATIONS_MINMAX_H
#define EXSEISDAT_PIOL_OPERATIONS_MINMAX_H

#include "exseisdat/utils/typedefs.h"
#include "stddef.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/*! A structure to hold a reference to a single coordinate and the corresponding
 *  trace number.
 *  Defined as struct for C-compatibility.
 */
struct PIOL_CoordElem {
    /// The value
    exseis_Floating_point val;

    /// The trace number
    size_t num;
};

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // EXSEISDAT_PIOL_OPERATIONS_MINMAX_H
