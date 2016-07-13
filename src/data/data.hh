#ifndef PIOLDATA_INCLUDE_GUARD
#define PIOLDATA_INCLUDE_GUARD
#include <memory>
#include "anc/piol.hh"
namespace PIOL { namespace Data {
class Interface
{
    protected :
    std::shared_ptr<ExSeisPIOL> piol;
    const std::string name;   //!< Store the file name for debugging purposes
    public :
    Interface(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_) : piol(piol_), name(name_)
    {
    }
    virtual void setFileSz(size_t) = 0;
    virtual size_t getFileSz(void) = 0;
    virtual void setView(size_t) = 0;
};

class Options
{


};
}}
#endif
