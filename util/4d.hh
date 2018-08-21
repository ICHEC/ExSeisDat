////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTIL_4D_HH
#define EXSEISDAT_UTIL_4D_HH

#include "ExSeisDat/PIOL/ExSeisPIOL.hh"

#include <iostream>

/// Constant for alignment reasons.
#define ALIGN 32U

/// A typedef for the precision used for coordinates during the 4dbin
typedef float fourd_t;

/*! Structure for 4d binning options
 */
struct FourDOpt {
    /// Be verbose in the output
    bool verbose = false;

    /// print the dsr value to the SEG-Y output files
    bool printDsr = true;

    /// Constrain by inline and crossline
    bool ixline = false;
};

namespace exseis {
namespace PIOL {

/*! Every process will call this function and process 0 will print the string.
 *  @param[in] piol The piol object.
 *  @param[in] msg The string to print.
 */
void cmsg(ExSeisPIOL* piol, std::string msg);

}  // namespace PIOL
}  // namespace exseis

#endif  // EXSEISDAT_UTIL_4D_HH
