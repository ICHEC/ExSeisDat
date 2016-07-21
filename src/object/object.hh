#ifndef PIOLOBJ_INCLUDE_GUARD
#define PIOLOBJ_INCLUDE_GUARD
#include <memory>
#include "global.hh"
#include "data/data.hh"
namespace PIOL { namespace Obj {
/*! \brief The Obj layer interface. Specific Obj implementations
 *  work off this base class.
 */
class Interface
{
    protected :
    std::shared_ptr<ExSeisPIOL> piol;                   //!< Pointer to the PIOL object.
    std::string name;                                   //!< Store the file name for debugging purposes.
    std::shared_ptr<Data::Interface> data = nullptr;    //!< Pointer to the Data layer object (polymorphic).

    /*! \brief The constructor used for unit testing. It does not try to create a Data object
     *  \param[in] piol_ This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_ The name of the file associated with the instantiation.
     *  \param[in] data_ Pointer to the associated Data layer object.
     */
    Interface(const std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const std::shared_ptr<Data::Interface> data_);
    public :
    /*! \brief The constructor.
     *  \param[in] piol_ This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_ The name of the file associated with the instantiation.
     *  \param[in] dataOpt The options to use when creating the Data layer
     */
    Interface(const std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const Data::Opt & dataOpt);

    /*! \brief Find out the file size.
     *  \return The file size in bytes.
     */
    virtual size_t getFileSz(void)
    {
        return data->getFileSz();
    }

    /*! \brief Pure virtual function to read the header object.
     *  \param[out] ho An array which the caller guarantees is long enough to hold the header object.
     */
    virtual void readHO(uchar * ho) = 0;
};
/*! \brief An enum of the possible derived classes for the object layer.
 */
enum class Type : size_t
{
    SEGY //!< The SEGY implementation. Currently the only option.
};

/*! \brief The base-options structure. Specific Obj implementations include a derived version of this.
 */
struct Opt
{
    Type type;      //!< The Obj type.
    /* \brief Default constructor to prevent intel warnings
     */
    Opt(void)
    {
        type = Type::SEGY;      //!< The Obj type.
    }
    /*! \brief This function returns the Obj type. This function is mainly included to provide a virtual function
     * to allow polymorphic behaviour.
     */
    virtual Type getType(void) const
    {
        return type;
    }
};
}}
#endif
