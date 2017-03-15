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
#include "global.hh"
#include "object/object.hh"

namespace PIOL { namespace Obj {
/*! \brief The SEG-Y Obj class.
 */
class SEGY : public Interface
{
    public :
    /*! \brief The SEG-Y options structure. Currently empty.
    */
    struct Opt
    {
        typedef SEGY Type;  //!< The Type of the class this structure is nested in
        /* \brief Default constructor to prevent intel warnings
         */
        Opt(void)
        {
        }
    };

    /*! \brief The SEGY-Obj class constructor.
     *  \param[in] piol_ This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_ The name of the file associated with the instantiation.
     *  \param[in] opt_  The SEGY options
     *  \param[in] data_ Pointer to the Data layer object (polymorphic).
     *  \param[in] mode  The file mode
     */
    SEGY(const Piol piol_, const std::string name_, const Opt & opt_, std::shared_ptr<Data::Interface> data_, FileMode mode = FileMode::Read);

    /*! \brief The SEGY-Obj class constructor.
     *  \param[in] piol_ This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_ The name of the file associated with the instantiation.
     *  \param[in] data_ Pointer to the Data layer object (polymorphic).
     *  \param[in] mode  The file mode
     */
    SEGY(const Piol piol_, const std::string name_, std::shared_ptr<Data::Interface> data_, FileMode mode = FileMode::Read);

    void readHO(uchar * ho) const;

    void writeHO(const uchar * ho) const;

    void readDOMD(csize_t offset, csize_t ns, csize_t sz, uchar * md) const;

    void writeDOMD(csize_t offset, csize_t ns, csize_t sz, const uchar * md) const;

    void readDODF(csize_t offset, csize_t ns, csize_t sz, uchar * df) const;

    void writeDODF(csize_t offset, csize_t ns, csize_t sz, const uchar * df) const;

    void readDO(csize_t offset, csize_t ns, csize_t sz, uchar * d) const;

    void writeDO(csize_t offset, csize_t ns, csize_t sz, const uchar * d) const;

    void readDO(csize_t ns, csize_t sz, csize_t * offset, uchar * d) const;

    void writeDO(csize_t ns, csize_t sz, csize_t * offset, const uchar * d) const;

    void readDOMD(csize_t ns, csize_t sz, csize_t * offset, uchar * md) const;

    void writeDOMD(csize_t ns, csize_t sz, csize_t * offset, const uchar * md) const;

    void readDODF(csize_t ns, csize_t sz, csize_t * offset, uchar * df) const;

    void writeDODF(csize_t ns, csize_t sz, csize_t * offset, const uchar * df) const;
};
}}
#endif
