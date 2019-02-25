#include "exseisdat/utils/typedefs.h"
#include "exseisdat/utils/typedefs.hh"

#include <type_traits>

//
// Check C++ and C API have the same types
//

static_assert(
    std::is_same<exseis::utils::Integer, exseis_Integer>::value,
    "exseis::utils::Integer and exseis_Integer are not the same type!");


static_assert(
    std::is_same<exseis::utils::Floating_point, exseis_Floating_point>::value,
    "exseis::utils::Floating_point and exseis_Floating_point are not the same type!");


static_assert(
    std::is_same<exseis::utils::Trace_value, exseis_Trace_value>::value,
    "exseis::utils::Trace_value and exseis_Trace_value are not the same type!");
