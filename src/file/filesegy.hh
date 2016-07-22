/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief The SEGY specific File layer interface
 *   \details SEGY implementations for File layer
*//*******************************************************************************************/
#ifndef PIOLFILESEGY_INCLUDE_GUARD
#define PIOLFILESEGY_INCLUDE_GUARD
#include <memory>
#include "global.hh"
#include "object/object.hh"
#include "file/file.hh"

namespace PIOL { namespace File {
/*! \brief The SEG-Y options structure. Currently empty.
 */
struct SEGYOpt : public Opt
{
    unit_t incFactor;   //!< The increment factor to multiply inc by.
    SEGYOpt(void);
};

/*! \brief The SEG-Y File class.
 */
class SEGY : public Interface
{
    private :
    /*! \brief Read the text and binary header and store the metadata variables in this File::SEGY object.
     *  \param[in] incFactor The increment factor to multiply inc by.
     *  \param[in] fsz The size of the file in bytes
     *  \param[in, out] The buffer to parse. The buffer is destructively modified
     */
    void procHeader(const unit_t incFactor, const size_t fsz, uchar * buf);

    /*! \brief This function initialises the class.
     */
    void Init(const File::SEGYOpt & segyOpt);

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
};
}}
#endif
