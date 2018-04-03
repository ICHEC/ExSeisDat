////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/DataInterface.hh"
#include "ExSeisDat/PIOL/ObjectInterface.hh"
#include "ExSeisDat/PIOL/ObjectSEGY.hh"

namespace PIOL {

std::shared_ptr<ObjectInterface> makeDefaultObj(
  std::shared_ptr<ExSeisPIOL> piol, std::string name, FileMode mode)
{
    auto data = std::make_shared<DataMPIIO>(piol, name, mode);
    return std::make_shared<ObjectSEGY>(piol, name, data, mode);
}

size_t ObjectInterface::getFileSz(void) const
{
    return data_->getFileSz();
}

void ObjectInterface::setFileSz(const size_t sz) const
{
    return data_->setFileSz(sz);
}

}  // namespace PIOL
