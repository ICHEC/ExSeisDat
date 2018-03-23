////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @date January 2017
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////
#ifndef FOURDBIN4D_INCLUDE_GUARD
#define FOURDBIN4D_INCLUDE_GUARD

#include "ExSeisDat/PIOL/anc/piol.hh"

#include <iostream>

/// Constant for alignment reasons.
#define ALIGN 32U

/// A typedef for the precision used for coordinates during the 4dbin
typedef float fourd_t;

/*! Structure for 4d binning options
 */
struct FourDOpt {
    /// Be verbose in the output
    uint64_t verbose : 1;
    /// print the dsr value to the SEG-Y output files
    uint64_t printDsr : 1;
    /// Constrain by inline and crossline
    uint64_t ixline : 1;

    /*! FourdOpt constructor
     */
    FourDOpt(void) : verbose(false), printDsr(true), ixline(false) {}
};

namespace PIOL {

/*! Every process will call this function and process 0 will print the string.
 *  @param[in] piol The piol object.
 *  @param[in] msg The string to print.
 */
void cmsg(ExSeisPIOL* piol, std::string msg);

}  // namespace PIOL
#endif
