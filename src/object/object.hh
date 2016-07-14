#ifndef PIOLOBJ_INCLUDE_GUARD
#define PIOLOBJ_INCLUDE_GUARD
#include <memory>
#include "global.hh"
#include "data/data.hh"
namespace PIOL { namespace Obj {
class Interface
{
    protected :
    std::shared_ptr<Data::Interface> data;
    std::string name;
    std::shared_ptr<ExSeisPIOL> piol;

    public :
    Interface(std::shared_ptr<ExSeisPIOL> piol, const std::string name_, const Data::Opt & dataOpt);

    virtual size_t getFileSz(void)
    {
        return data->getFileSz();
    }

    virtual void readHO(uchar * ho) = 0;
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

