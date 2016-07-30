/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details The intention is to make this obselete very soon
 *//*******************************************************************************************/
#ifndef PIOLSHARECASTS_INCLUDE_GUARD
#define PIOLSHARECASTS_INCLUDE_GUARD
#include <string>
#include "global.hh"
namespace PIOL {

/*! \brief Cast the options object to a derived class type and error check the cast.
 *  \tparam OBase The base class
 *  \tparam ODeriv The derived class
 *  \param[in] piol The PIOL object used for error logging
 *  \param[in] bopt The options object (in base class type)
 *  \param[in] name The name of the file associated with the object
 *  \param[in] layer The layer of ExSeisPIOL for logging purposes
 *  \return Returns a pointer to the derived options object
 */
template <class ODeriv, class OBase> inline
const ODeriv * castOptToDeriv(ExSeisPIOL * piol, const OBase & bopt, const std::string name, const Log::Layer layer)
{
    auto opt = dynamic_cast<ODeriv const *>(&bopt);
    if (opt == nullptr)
        piol->record(name, layer, Log::Status::Error, "Options object is of the wrong type.", Log::Verb::None);
    return opt;
}
/*! \brief Cast the derived class to its base class within a shared_ptr
 *  \tparam TBase   The base class
 *  \tparam TDeriv  The derived class
 *  \param[in] piol The PIOL object used for error logging
 *  \param[in] layObj The object in derived class type
 *  \param[in] name The name of the file associated with the object
 *  \param[in] layer The layer of ExSeisPIOL for logging purposes
 *  \return Returns a shared ptr to the object case to the base class
 */
template <class TBase, class TDeriv> inline
std::shared_ptr<TBase> castToBase(ExSeisPIOL * piol, TDeriv * layObj, const std::string name, const Log::Layer layer)
{
    if (layObj != nullptr)
        return std::shared_ptr<TBase>(std::move(layObj));

    //Issue warning on the layer
    piol->record(name, layer, Log::Status::Warning, "Could not create layer object", Log::Verb::None);
    return nullptr;
}
}
#endif

