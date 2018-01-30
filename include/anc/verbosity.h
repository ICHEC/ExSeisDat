#ifndef PIOLANCVERBOSITY_HEADER_GUARD
#define PIOLANCVERBOSITY_HEADER_GUARD

#include <stddef.h>

/*! @brief The enumeration for the verbosity level of records
 */
typedef size_t PIOL_Verbosity;

#ifdef __cplusplus
namespace PIOL {
    typedef PIOL_Verbosity Verbosity;
}
#endif

#define PIOL_VERBOSITY_NONE     ((PIOL_Verbosity) 0) //!< Record no information beyond fatal errors and explicit requests
#define PIOL_VERBOSITY_MINIMAL  ((PIOL_Verbosity) 1) //!< Record essential information including other errors and warnings
#define PIOL_VERBOSITY_EXTENDED ((PIOL_Verbosity) 2) //!< Record some non-warnings
#define PIOL_VERBOSITY_VERBOSE  ((PIOL_Verbosity) 3) //!< Record a high level of information
#define PIOL_VERBOSITY_MAX      ((PIOL_Verbosity) 4) //!< Maximum Verbosity level. (for debugging)

#endif
