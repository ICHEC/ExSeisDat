#ifndef EXSEISDAT_PIOL_VERBOSITY_H
#define EXSEISDAT_PIOL_VERBOSITY_H

#include <stddef.h>

/// @copydoc PIOL::Verbosity
typedef size_t PIOL_Verbosity;

#ifdef __cplusplus
namespace PIOL {
/// The enumeration for the verbosity level of records
typedef PIOL_Verbosity Verbosity;
}  // namespace PIOL
#endif

/// Record no information beyond fatal errors and explicit requests
#define PIOL_VERBOSITY_NONE ((PIOL_Verbosity)0)
/// Record essential information including other errors and warnings
#define PIOL_VERBOSITY_MINIMAL ((PIOL_Verbosity)1)
/// Record some non-warnings
#define PIOL_VERBOSITY_EXTENDED ((PIOL_Verbosity)2)
/// Record a high level of information
#define PIOL_VERBOSITY_VERBOSE ((PIOL_Verbosity)3)
/// Maximum Verbosity level. (for debugging)
#define PIOL_VERBOSITY_MAX ((PIOL_Verbosity)4)

#endif  // EXSEISDAT_PIOL_VERBOSITY_H
