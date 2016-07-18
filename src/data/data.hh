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
    virtual ~Interface() {}
    virtual size_t getFileSz() = 0;
    virtual void read(size_t offset, uchar * d, size_t sz) = 0;
};

enum class Type
{
    MPIIO
};

class Opt
{
    Type type = Type::MPIIO;
    public :
    virtual void setType(const Type type_)
    {
        type = type_;
    }

    virtual Type getType(void) const
    {
        return type;
    }
};
}}
#endif
