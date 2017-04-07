/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \date January 2017
 *   \brief
 *   \details
 *//*******************************************************************************************/
#ifndef FOURDBIN4D_INCLUDE_GUARD
#define FOURDBIN4D_INCLUDE_GUARD
#include <iostream>
#include "cppfileapi.hh"

#define ALIGN 32U   //!< Constant for alignment reasons.

/*! A typedef to simplify const std::vector
 */
template <class T>
using cvec = const std::vector<T>;

/*! A typedef to simplify std::vector
 */
template <class T>
using vec = std::vector<T>;

typedef float fourd_t;  //!< A typedef for the precision used for coordinates during the 4dbin

struct FourDOpt
{
    uint64_t verbose : 1;   //!< Be verbose in the output
    uint64_t printDsr : 1;  //!< print the dsr value to the SEG-Y output files
    uint64_t ixline : 1;    //!< Constrain by inline and crossline
};

namespace PIOL
{
/*! Every process will call this function and process 0 will print the string.
 *  \param[in] piol The piol object.
 *  \param[in] msg The string to print.
 */
void cmsg(ExSeisPIOL * piol, std::string msg);
}
#endif
