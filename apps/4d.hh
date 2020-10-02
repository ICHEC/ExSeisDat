////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_FOUR_D_4D_HH
#define EXSEISDAT_PIOL_FOUR_D_4D_HH

#include "exseisdat/piol/configuration/ExSeisPIOL.hh"

#include <iostream>


namespace exseis {
namespace piol {
namespace four_d {

/// Constant for alignment reasons.
#define EXSEISDAT_ALIGN 32U

/// A typedef for the precision used for coordinates during the 4dbin
typedef float fourd_t;

/*! @brief Structure for 4d binning options
 */
struct Four_d_opt {
    /// Be verbose in the output
    bool verbose = false;

    /// print the dsr value to the SEG-Y output files
    bool print_dsr = true;

    /// Constrain by inline and crossline
    bool ixline = false;
};

/*! @brief Every process will call this function and process 0 will print the
 * string.
 *  @param[in] piol The piol object.
 *  @param[in] msg The string to print.
 */
void cmsg(ExSeisPIOL* piol, std::string msg);

}  // namespace four_d
}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_FOUR_D_4D_HH
