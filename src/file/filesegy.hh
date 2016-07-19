#ifndef PIOLFILESEGY_INCLUDE_GUARD
#define PIOLFILESEGY_INCLUDE_GUARD
#include <memory>
#include "global.hh"
#include "object/object.hh"
#include "file/file.hh"

namespace PIOL { namespace File {
class SEGYOpt : public Opt
{
    public :
    SEGYOpt(void)
    {
    }
};

class SEGY : public Interface
{
    private :
    void Init();
    void parseHO(const uchar * buf, size_t fsz);
    SEGY(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, std::shared_ptr<Obj::Interface> obj_);
    public :
    SEGY(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_,
               const File::SEGYOpt & segyOpt, const Obj::Opt & objOpt, const Data::Opt & dataOpt);
    size_t readNs(void);
    size_t readNt(void);
};
}}
#endif
