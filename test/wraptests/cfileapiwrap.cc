//
// We want to inject some definitions into the cfileapi.cc file.
// Specifically, we want to make a number of template functions call
// external functions so we can intercept them with mock functions.
//

// Externed templates defined in the appropriate headers
#include "mockparam.hh"

// Include the cfileapi.cc file
#include "../../api/cfileapi.cc"
