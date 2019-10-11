////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date March 2017
/// @brief This file contains the compute heavy kernel of the 4d-binning utility
////////////////////////////////////////////////////////////////////////////////

#include "4d.hh"

#include "exseisdat/piol/configuration/ExSeis.hh"

#include <iostream>


namespace exseis {
namespace piol {
namespace four_d {

void cmsg(ExSeisPIOL* piol, std::string msg)
{
    piol->comm->barrier();
    if (piol->comm->get_rank() == 0) {
        std::cout << msg << std::endl;
    }
}

}  // namespace four_d
}  // namespace piol
}  // namespace exseis
