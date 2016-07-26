/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief The SEGY implementation of the Object layer interface
 *   \details The SEGY specific implementation of the Object layer interface
*//*******************************************************************************************/
#ifndef PIOLOBJSEGY_INCLUDE_GUARD
#define PIOLOBJSEGY_INCLUDE_GUARD
#include <memory>
#include "anc/piol.hh"
#include "global.hh"
#include "object/object.hh"
#include "data/data.hh"

namespace PIOL { namespace Obj {

/*! \brief The SEG-Y options structure. Currently empty.
 */
struct SEGYOpt : public Opt
{
    /* \brief Default constructor to prevent intel warnings
     */
    SEGYOpt(void)
    {
    }
};

/*! \brief The SEG-Y Obj class.
 */
class SEGY : public Interface
{
    protected :
    /*! \brief The constructor used for unit testing. It does not try to create a Data object
     *  \param[in] piol_ This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_ The name of the file associated with the instantiation.
     *  \param[in] segyOpt_ The SEGY-Obj options
     *  \param[in] data_ Pointer to the associated Data layer object.
     */
    SEGY(const std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const SEGYOpt & segyOpt_, const std::shared_ptr<Data::Interface> data_);

    public :
    /*! \brief The SEGY-Obj class constructor.
     *  \param[in] piol_ This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_ The name of the file associated with the instantiation.
     *  \param[in] segyOpt_ The SEGY-Obj options
     *  \param[in] dataOpt The Data options
     */
    SEGY(const std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const SEGYOpt & segyOpt_, const Data::Opt & dataOpt);

    /*! \brief Read the header object.
     *  \param[out] ho An array which the caller guarantees is long enough to hold the SEG-Y header (3200 bytes).
     */
    void readHO(uchar * ho);

    /*! \brief Write the header object.
     *  \param[out] ho An array which the caller guarantees is as long enough the header object.
     */
    void writeHO(const uchar * ho);

    /*! Read the data-object metadata.
     *  \param[in] offset The trace number we are interested in.
     *  \param[in] ns The number of samples per trace.
     *  \param[out] ho An array which the caller guarantees is long enough for
     *  the specific trace header.
     *  \todo Extend this function to read more than one trace
     */
    void readDOMD(const size_t offset, const size_t ns, uchar * ho);

    /*! Write the data-object metadata.
     *  \param[in] offset The trace number we are interested in.
     *  \param[in] ns The number of samples per trace.
     *  \param[out] ho An array which the caller guarantees is long enough for
     *  the specific trace header.
     *  \todo Extend this function to write more than one trace
     */
    void writeDOMD(const size_t offset, const size_t ns, const uchar * ho);

};
}}
#endif
