#ifndef PIOLDATA_INCLUDE_GUARD
#define PIOLDATA_INCLUDE_GUARD
#include <memory>
#include "anc/piol.hh"
namespace PIOL { namespace Data {
/*! \brief The Data layer interface. Specific data I/O implementations
 *  work off this base class.
 */
class Interface
{
    protected :
    std::shared_ptr<ExSeisPIOL> piol;   //!< Pointer to the PIOL object.
    const std::string name;             //!< Store the file name for debugging purposes.
    public :
    /*! \brief The constructor.
     *  \param[in] piol_ This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_ The name of the file associated with the instantiation.
     */
    Interface(const std::shared_ptr<ExSeisPIOL> piol_, const std::string name_) : piol(piol_), name(name_)
    {
    }
    /*! \brief Pure virtual function to find out the file size.
     *  \return The file size in bytes.
     */
    virtual size_t getFileSz() = 0;

    /*! \brief Pure virtual function to read from storage.
     *  \param[in] offset The offset in bytes from the current internal shared pointer
     *  \param[out] d     The array to store the output in
     *  \param[in] sz     The amount of data to read from disk
     */
    virtual void read(size_t offset, uchar * d, size_t sz) = 0;
};
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
    /* \brief Default constructor to prevent intel warnings
     */
    Opt(void)
    {
        type = Type::MPIIO;      //!< The Data type.
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
