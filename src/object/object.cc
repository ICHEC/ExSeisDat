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
            auto opt = castOptToDeriv<Data::MPIIOOpt, Data::Opt>(piol.get(), dataOpt, name, Log::Layer::Object);
            if (opt == nullptr)
                return;
            auto mpiio = new Data::MPIIO(piol_, name_, *opt);
            data = castToBase<Data::Interface, Data::MPIIO>(piol.get(), mpiio, name, Log::Layer::Object);
            if (data == nullptr)
                return;
        }
        break;
        default :
        //TODO generate a warning
        break;
    }
    piol->record(name_, Log::Layer::Object, Log::Status::Warning, "Successful exit of Object Interface", Log::Verb::Max);
}

size_t Interface::getFileSz(void) const
{
    return data->getFileSz();
}

void Interface::setFileSz(const size_t sz) const
{
    return data->setFileSz(sz);
}
}}
