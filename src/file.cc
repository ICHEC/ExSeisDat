/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include "file/file.hh"
#include "object/objsegy.hh"
#warning replace
#include "data/datampiio.hh"
#include "share/casts.hh"
#include <iostream>
namespace PIOL { namespace File {

void Interface::Init(const Piol piol_, const std::string name_)
{
    const Obj::SEGYOpt objOpt;
    const Data::MPIIOOpt dataOpt;
    Init(piol_, name_, static_cast<const Obj::Opt &>(objOpt), static_cast<const Data::Opt &>(dataOpt));
}

void Interface::Init(const Piol piol_, const std::string name_, const Obj::Opt & objOpt, const Data::Opt & dataOpt)
{
    piol = piol_;
    name = name_;

    switch (objOpt.getType())
    {
        case (Obj::Type::SEGY) :
        {
            piol->record(name_, Log::Layer::File, Log::Status::Note, "SEGY selected", Log::Verb::Max);
            auto opt = castOptToDeriv<Obj::SEGYOpt, Obj::Opt>(piol.get(), objOpt, name, Log::Layer::File);
            if (opt == nullptr)
                return;
            auto segy = new Obj::SEGY(piol_, name_, *opt, dataOpt);

            if (segy == nullptr)
            {
                piol->record(name_, Log::Layer::Object, Log::Status::Warning, "new failed in interface constructor: " + std::to_string(__LINE__), Log::Verb::Max);
                obj = nullptr;
                return;
            }
            if (piol->log->isErr())
            {
                delete segy;
                obj = nullptr;
                return;
            }
            obj = castToBase<Obj::Interface, Obj::SEGY>(piol.get(), segy, name, Log::Layer::File);
            if (obj == nullptr)
                return;
        }
        break;
        default :
        //TODO: Add warning
        break;
    }
}

Interface::Interface(const Piol piol_, const std::string name_, const std::shared_ptr<Obj::Interface> obj_)
                        : piol(piol_), name(name_), obj(obj_)
{
}

Interface::Interface(const Piol piol_, const std::string name_, const Obj::Opt & objOpt, const Data::Opt & dataOpt)
{
    Init(piol_, name_, objOpt, dataOpt);
}

//derived class must call Init
Interface::Interface(void)
{
}

const std::string & Interface::readText(void) const
{
    return text;
}

size_t Interface::readNs(void) const
{
    return ns;
}

size_t Interface::readNt(void) const
{
    return nt;
}

geom_t Interface::readInc(void) const
{
   return inc;
}
}}
