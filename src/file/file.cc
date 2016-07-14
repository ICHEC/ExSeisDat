#include "file/file.hh"
#include "object/objsegy.hh"
namespace PIOL { namespace File {
Interface::Interface(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const Obj::Opt & objOpt, const Data::Opt & dataOpt) : piol(piol_), name(name_)
{
    switch (objOpt.getType())
    {
        case (Obj::Type::SEGY) :
        {
            auto segy = std::make_shared<Obj::SEGY>(piol_, name_, dynamic_cast<const Obj::SEGYOpt &>(objOpt), dataOpt);
            obj = segy;
        }
        break;
        default :

        break;
    }
}
}}
