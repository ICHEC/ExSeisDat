#ifndef PIOLFILESEGY_INCLUDE_GUARD
#define PIOLFILESEGY_INCLUDE_GUARD
#include <memory>
#include "global.hh"
#include "object/object.hh"
#include "file/file.hh"

namespace PIOL { namespace File {

/*! \brief The SEG-Y options structure. Currently empty.
 */
class SEGYOpt : public Opt
{
};

/*! \brief The SEG-Y File class.
 */
class SEGY : public Interface
{
    private :
    /*! \brief This function initialises the class.
     */
    void Init();

    /*! \brief Parse the given header object buffer.
     *  \param[in] buf The buffer to parse
     */
    void parseHO(const uchar * buf, const size_t fsz);

    /*! \brief The constructor used for unit testing. It does not try to create a Data object
     *  \param[in] piol_ This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_ The name of the file associated with the instantiation.
     *  \param[in] segyOpt The SEGY-File options
     *  \param[in] obj_ Pointer to the associated Obj layer object.
     */
    SEGY(const std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const File::SEGYOpt & segyOpt, const std::shared_ptr<Obj::Interface> obj_);
    public :

    /*! \brief The SEGY-Object class constructor.
     *  \param[in] piol_   This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_   The name of the file associated with the instantiation.
     *  \param[in] segyOpt The SEGY-File options
     *  \param[in] objOpt  The Object options
     *  \param[in] dataOpt The Data options
     */
    SEGY(const std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const File::SEGYOpt & segyOpt,
                                                  const Obj::Opt & objOpt, const Data::Opt & dataOpt);
    /*! \brief Read the number of samples per trace
     *  \return The number of samples per trace
     */
    size_t readNs(void);

    /*! \brief Read the number of traces in the file
     *  \return The number of traces
     */
    size_t readNt(void);
};
}}
#endif
