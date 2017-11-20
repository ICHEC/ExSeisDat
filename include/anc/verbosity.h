#ifndef PIOLANCVERBOSITY_HEADER_GUARD
#define PIOLANCVERBOSITY_HEADER_GUARD

/*! \brief The enumeration for the verbosity level of records
 */
typedef size_t PIOL_Verbosity;

#ifdef __cplusplus
namespace PIOL {
    typedef PIOL_Verbosity Verbosity;
}
#endif

const PIOL_Verbosity PIOL_VERBOSITY_NONE     = 0; //!< Record no information beyond fatal errors and explicit requests
const PIOL_Verbosity PIOL_VERBOSITY_MINIMAL  = 1; //!< Record essential information including other errors and warnings
const PIOL_Verbosity PIOL_VERBOSITY_EXTENDED = 2; //!< Record some non-warnings
const PIOL_Verbosity PIOL_VERBOSITY_VERBOSE  = 3; //!< Record a high level of information
const PIOL_Verbosity PIOL_VERBOSITY_MAX      = 4; //!< Maximum Verbosity level. (for debugging)

#endif
