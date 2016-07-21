/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include "object/object.hh"
#include <string>
#include <typeinfo>
#include <iostream>
#include "share/casts.hh"
#include "data/datampiio.hh"
namespace PIOL { namespace Obj {
Interface::Interface(const std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const std::shared_ptr<Data::Interface> data_) : piol(piol_), name(name_), data(data_)
{
}

Interface::Interface(const std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const Data::Opt & dataOpt) : piol(piol_), name(name_)
{
    piol->record(name_, Log::Layer::Object, Log::Status::Warning, "Entered Object Interface", Log::Verb::Max);
    switch (dataOpt.getType())
    {
        case (Data::Type::MPIIO) :
        {
            piol->record(name_, Log::Layer::Object, Log::Status::Warning, "MPIIO selected", Log::Verb::Max);
            auto opt = castOptToDeriv<Data::MPIIOOpt, Data::Opt>(*piol, dataOpt, name, Log::Layer::Object);
            if (opt == nullptr)
                return;
            auto mpiio = new Data::MPIIO(piol_, name_, *opt);
            if (mpiio == nullptr)
                return;
            data = castToBase<Data::Interface, Data::MPIIO>(*piol, mpiio, name, Log::Layer::Object);

/*            auto opt = dynamic_cast<Data::MPIIOOpt const *>(&dataOpt);
            if (opt == nullptr)
            {
                piol->record(name_, Log::Layer::Object, Log::Status::Error, "MPI-IO options object is of the wrong type.", Log::Verb::None);
                return;
            }

            auto mpiio = new Data::MPIIO(piol_, name_, *opt);
            if (mpiio == nullptr)   //Issue warning on the object layer
            {
                piol->record(name_, Log::Layer::Object, Log::Status::Warning, "Could not create data layer object", Log::Verb::None);
                return;
            }
            else
                data = std::shared_ptr<Data::MPIIO>(std::move(mpiio));*/
        }
        break;
        default :

        break;
    }
    piol->record(name_, Log::Layer::Object, Log::Status::Warning, "Successful exit of Object Interface", Log::Verb::Max);
}
}}
