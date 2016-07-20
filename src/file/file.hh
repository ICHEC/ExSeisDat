#ifndef PIOLFILE_INCLUDE_GUARD
#define PIOLFILE_INCLUDE_GUARD
#include "global.hh"
#include "object/object.hh"
#include "data/data.hh"

namespace PIOL { namespace File {
/*! \brief The File layer interface. Specific File implementations
 *  work off this base class.
 */
class Interface
{
    protected :
    std::shared_ptr<ExSeisPIOL> piol;               //!< The PIOL object.
    std::string name;                               //!< Store the file name for debugging purposes.
    std::shared_ptr<Obj::Interface> obj = nullptr;  //!< Pointer to the Object-layer object (polymorphic).
    size_t ns;                                      //!< The number of samples per trace.
    size_t nt;                                      //!< The number of traces.
    /*! \brief The constructor used for unit testing. It does not try to create an Object-layer object
     *  \param[in] piol_ This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_ The name of the file associated with the instantiation.
     *  \param[in] obj_ Pointer to the associated Object-layer object.
     */
    Interface(const std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const std::shared_ptr<Obj::Interface> obj_);
    public :
    /*! \brief The constructor.
     *  \param[in] piol_ This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_ The name of the file associated with the instantiation.
     *  \param[in] objOpt The options to use when creating the Object layer
     *  \param[in] dataOpt The options to use when creating the Data layer
     */
    Interface(const std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const Obj::Opt & objOpt, const Data::Opt & dataOpt);

    /*! \brief Pure virtual function to read the number of samples per trace
     *  \return The number of samples per trace
     */
    virtual size_t readNs(void) = 0;
    /*! \brief Pure virtual function to read the number of traces in the file
     *  \return The number of traces
     */
    virtual size_t readNt(void) = 0;
};

/*! \brief An enum of the possible derived classes for the file layer.
 */
enum class Type : size_t
{
    SEGY //!< The SEGY implementation. Currently the only option.
};

/*! \brief The base-options structure. Specific File implementations include a derived version of this.
 */
struct Opt
{
    Type type = Type::SEGY;      //!< The File type.
    /* \brief Default constructor to prevent intel warnings
     */
    Opt(void)
    {
        type = Type::SEGY;      //!< The Obj type.
    }
    /*! \brief This function returns the File type. This function is mainly included to provide a virtual function
     * to allow polymorphic behaviour.
     */
    virtual Type getType(void) const
    {
        return type;
    }
};
}}
#endif
