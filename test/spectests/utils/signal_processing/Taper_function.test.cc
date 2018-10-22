#include "exseisdat/utils/signal_processing/Taper_function.h"
#include "exseisdat/utils/signal_processing/Taper_function.hh"

// Check C++ and C API have the same type
static_assert(
  std::is_same<exseis::utils::Taper_function, exseis_Taper_function>::value,
  "exseis::utils::Taper_function and exseis_Taper_function are not the same type!");
