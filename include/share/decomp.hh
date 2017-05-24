/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date May 2017
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include "global.hh"
namespace PIOL {
/*! Perform a 1d decomposition so that the load is optimally balanced.
 *  \param[in] sz The sz of the 1d domain
 *  \param[in] numRank The number of ranks to perform the decomposition over
 *  \param[in] rank The rank of the local process
 *  \return Return a pair, the first element is the offset for the local process,
 *          the second is the size for the local process.
 */
std::pair<size_t, size_t> decompose(size_t sz, size_t numRank, size_t rank);

#warning document
std::pair<size_t, size_t> decompose(ExSeisPIOL * piol, File::ReadInterface * file);
}
