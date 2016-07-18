#ifndef PIOLOBJSEGY_INCLUDE_GUARD
#define PIOLOBJSEGY_INCLUDE_GUARD
#include <memory>
#include "anc/piol.hh"
#include "global.hh"
#include "object/object.hh"
#include "share/segy.hh"

namespace PIOL { namespace Obj {
class SEGYOpt : public Opt
{
    public :
    SEGYOpt(void)
    {
    }
};

class SEGY : public Interface
{
    protected :
    SEGY(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const SEGYOpt & segyOpt_, std::shared_ptr<Data::Interface> data_);
    public :
    SEGY(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const SEGYOpt & segyOpt_, const Data::Opt & dataOpt);
    void readHO(uchar * ho);
};
}}
#endif
