////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/DataInterface.hh"
#include "ExSeisDat/PIOL/object/object.hh"

namespace PIOL {
namespace Obj {

size_t Interface::getFileSz(void) const
{
    return data_->getFileSz();
}

void Interface::setFileSz(const size_t sz) const
{
    return data_->setFileSz(sz);
}

}  // namespace Obj
}  // namespace PIOL
