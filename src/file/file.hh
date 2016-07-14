#ifndef PIOLFILE_INCLUDE_GUARD
#define PIOLFILE_INCLUDE_GUARD
#include "global.hh"
#include "object/object.hh"
#include "data/data.hh"

namespace PIOL { namespace File {
class Interface
{
    protected :
    std::shared_ptr<Obj::Interface> obj;
    std::string name;
    std::shared_ptr<ExSeisPIOL> piol;

//Publicly exposed information
    size_t ns;
    size_t nt;

    Interface(std::shared_ptr<ExSeisPIOL> piol, const std::string name_, const Obj::Opt & objOpt, const Data::Opt & dataOpt);

    virtual size_t readNs(void) = 0;
    virtual size_t readNt(void) = 0;
};

enum class Type
{
    SEGY
};

class Opt
{
    Type type;
    public :
    virtual Type getType(void) const
    {
        return type;
    }
};
}}
#endif
