#include "object/objsegy.hh"
#include <memory>
#include "data/data.hh"
namespace PIOL { namespace Obj {

SEGY::SEGY(std::shared_ptr<ExSeisPIOL> piol_, std::string name_, const SEGYOpt & segyOpt, const Data::Opt & dataOpt) : Interface(piol_, name_, dataOpt)
{
}

void SEGY::readHO(uchar * ho)
{
    data->read(0U, ho, SEGSz::getHOSz());
}
}}
