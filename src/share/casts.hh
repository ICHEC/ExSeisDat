#ifndef PIOLSHARECASTS_INCLUDE_GUARD
#define PIOLSHARECASTS_INCLUDE_GUARD
#include <string>
#include "anc/piol.hh"
namespace PIOL {
template <class ODeriv, class OBase>
ODeriv const * castOptToDeriv(ExSeisPIOL & piol, const OBase & Opt, const std::string name, const Log::Layer layer)
{
    auto opt = dynamic_cast<ODeriv const *>(&Opt);
    if (opt == nullptr)
        piol.record(name, layer, Log::Status::Error, "Options object is of the wrong type.", Log::Verb::None);
    return opt;
}

template <class TBase, class TDeriv>
std::shared_ptr<TBase> castToBase(ExSeisPIOL & piol, TDeriv * layObj, const std::string name, const Log::Layer layer)
{
    if (layObj != nullptr)
        return std::shared_ptr<TBase>(std::move(layObj));

    //Issue warning on the layer
    piol.record(name, layer, Log::Status::Warning, "Could not create layer object", Log::Verb::None);
    return nullptr;
}
}
#endif

