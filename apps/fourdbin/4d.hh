////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEIS_PIOL_FOUR_D_4D_HH
#define EXSEIS_PIOL_FOUR_D_4D_HH

#include "exseis/utils/communicator/Communicator_mpi.hh"


namespace exseis {
namespace apps {
inline namespace fourdbin {

/// Constant for alignment reasons.
#define EXSEIS_ALIGN 32U

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
void cmsg(const Communicator& communicator, std::string msg);

}  // namespace fourdbin
}  // namespace apps
}  // namespace exseis

#endif  // EXSEIS_PIOL_FOUR_D_4D_HH
