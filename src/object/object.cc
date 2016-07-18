#include "object/object.hh"
#include <string>
#include "data/datampiio.hh"
#include <typeinfo>
#include <iostream>
namespace PIOL { namespace Obj {
Interface::Interface(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, std::shared_ptr<Data::Interface> data_) : piol(piol_), name(name_), data(data_)
{
}

Interface::Interface(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const Data::Opt & dataOpt) : piol(piol_), name(name_)
{
    piol->record(name_, Log::Layer::Object, Log::Status::Warning, "Entered Object Interface", Log::Verb::Max);
    switch (dataOpt.getType())
    {
        case (Data::Type::MPIIO) :
        {
            piol->record(name_, Log::Layer::Object, Log::Status::Warning, "MPIIO selected", Log::Verb::Max);

            Data::MPIIOOpt const * opt = dynamic_cast<Data::MPIIOOpt const *>(&dataOpt);
            if (opt == nullptr)
            {
                piol->record(name_, Log::Layer::Object, Log::Status::Error, "MPI-IO options object is of the wrong type.", Log::Verb::None);
                return;
            }

            auto mpiio = new Data::MPIIO(piol_, name_, *opt);
            if (mpiio == nullptr)   //Issue warning on the object layer
            {
                piol->record(name_, Log::Layer::Object, Log::Status::Warning, "Could not create data dayer object", Log::Verb::None);
                return;
            }
            else
                data = std::shared_ptr<Data::MPIIO>(std::move(mpiio));
        }
        break;
        default :

        break;
    }
    piol->record(name_, Log::Layer::Object, Log::Status::Warning, "Successful exit of Object Interface", Log::Verb::Max);
}
}}
