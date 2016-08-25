/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date August 2016
 *   \brief The Data layer options base class
*//*******************************************************************************************/
#ifndef PIOLDATAOPT_INCLUDE_GUARD
#define PIOLDATAOPT_INCLUDE_GUARD

#include "global.hh"
namespace PIOL { namespace Data {
/*! \brief An enum of the possible derived classes for the data layer.
 */
enum class Type : size_t
{
    MPIIO //!< The MPIIO implementation. Currently the only option.
};

/*! \brief The base-options structure. Specific Data implementations include a derived version of this.
 */
struct Opt
{
    Type type;      //!< The Data type.
    FileMode mode;           //!< The mode to open the associated file with

    /* \brief Default constructor to prevent intel warnings
     */
    Opt(void)
    {
        type = Type::MPIIO;      //!< The Data type.
        mode = FileMode::Read;
    }

    /*! \brief This function returns the Data type. This function is mainly included to provide a virtual function
     * to allow polymorphic behaviour.
     */
    virtual Type getType(void) const
    {
        return type;
    }
};
}}
#endif

