//
// We want to inject some definitions into the cfileapi.cc file.
// Specifically, we want to make a number of template functions call
// external functions so we can intercept them with mock functions.
//

// Include the C bindings from src/ which call the templated functions
#include "PIOL_C_bindings.cc"
#include "flow/Flow_C_bindings.cc"
