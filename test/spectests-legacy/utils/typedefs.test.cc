#include "exseisdat/utils/types/typedefs.h"
#include "exseisdat/utils/types/typedefs.hh"

#include <type_traits>

//
// Check C++ and C API have the same types
//

static_assert(
    std::is_same<exseis::utils::Integer, exseis::utils::exseis_Integer>::value,
    "exseis::utils::Integer and exseis_Integer are not the same type!");


static_assert(
    std::is_same<
        exseis::utils::Floating_point,
        exseis::utils::exseis_Floating_point>::value,
    "exseis::utils::Floating_point and exseis_Floating_point are not the same type!");


static_assert(
    std::is_same<
        exseis::utils::Trace_value,
        exseis::utils::exseis_Trace_value>::value,
    "exseis::utils::Trace_value and exseis_Trace_value are not the same type!");
