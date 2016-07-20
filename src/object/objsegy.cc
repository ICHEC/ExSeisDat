#include "object/objsegy.hh"
#include <memory>
#include "data/data.hh"
namespace PIOL { namespace Obj {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
SEGY::SEGY(std::shared_ptr<ExSeisPIOL> piol_, std::string name_, const SEGYOpt & segyOpt_, std::shared_ptr<Data::Interface> data_) : Interface(piol_, name_, data_)
{
}

SEGY::SEGY(std::shared_ptr<ExSeisPIOL> piol_, std::string name_, const SEGYOpt & segyOpt, const Data::Opt & dataOpt) : Interface(piol_, name_, dataOpt)
{
}
#pragma GCC diagnostic pop

void SEGY::readHO(uchar * ho)
{
    data->read(0U, ho, SEGSz::getHOSz());
}
}}
