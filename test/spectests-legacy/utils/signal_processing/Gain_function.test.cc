#include "exseisdat/utils/signal_processing/Gain_function.h"
#include "exseisdat/utils/signal_processing/Gain_function.hh"

#include <type_traits>

// Check C++ and C API have the same type
static_assert(
    std::is_same<
        exseis::utils::Gain_function,
        exseis::utils::exseis_Gain_function>::value,
    "exseis::utils::Gain_function and exseis_Gain_function are not the same type!");
