#include "object/object.hh"
#include <string>
#include "data/datampiio.hh"
namespace PIOL { namespace Obj {

Interface::Interface(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, std::shared_ptr<Data::Interface> data_) : piol(piol_), name(name_), data(data_)
{
}

Interface::Interface(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const Data::Opt & dataOpt) : piol(piol_), name(name_)
{
    switch (dataOpt.getType())
    {
        case (Data::Type::MPIIO) :
        {
            auto mpiio = std::make_shared<Data::MPIIO>(piol_, name_, dynamic_cast<const Data::MPIIOOpt &>(dataOpt));
            data = mpiio;
        }
        break;
        default :

        break;
    }
}
}}
