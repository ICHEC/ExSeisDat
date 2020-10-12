////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @date March 2017
/// @brief This file contains the compute heavy kernel of the 4d-binning utility
////////////////////////////////////////////////////////////////////////////////

#include "4d.hh"

#include <iostream>


namespace exseis {
namespace apps {
inline namespace fourdbin {

void cmsg(const Communicator& communicator, std::string msg)
{
    communicator.barrier();
    if (communicator.get_rank() == 0) {
        std::cout << msg << std::endl;
    }
}

}  // namespace fourdbin
}  // namespace apps
}  // namespace exseis
