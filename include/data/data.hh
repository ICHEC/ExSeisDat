/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief The Data layer interface
 *   \details The Data layer interface is a base class which specific data I/O implementations
 *   work off
*//*******************************************************************************************/
#ifndef PIOLDATA_INCLUDE_GUARD
#define PIOLDATA_INCLUDE_GUARD
#include <memory>
#include "global.hh"
namespace PIOL { namespace Data {
/*! \brief The Data layer interface. Specific data I/O implementations
 *  work off this base class.
 */
class Interface
{
    protected :
    Piol piol;   //!< Pointer to the PIOL object.
    const std::string name;             //!< Store the file name for debugging purposes.

    public :
    /*! \brief The constructor.
     *  \param[in] piol_ This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_ The name of the file associated with the instantiation.
     */
    Interface(const Piol piol_, const std::string name_) : piol(piol_), name(name_)
    {
    }

    /*! \brief A virtual destructor to allow deletion.
     */
    virtual ~Interface(void) { }

    /*! \brief Pure virtual function to find out the file size.
     *  \return The file size in bytes.
     */
    virtual size_t getFileSz() const = 0;

    /*! \brief Pure virtual function to set the file size.
     *  \param[in] sz The size in bytes
     */
    virtual void setFileSz(csize_t sz) const = 0;

    /*! \brief Pure virtual function to read from storage.
     *  \param[in] offset The offset in bytes from the current internal shared pointer
     *  \param[in] sz     The amount of data to read from disk
     *  \param[out] d     The array to store the output in
     */
    virtual void read(csize_t offset, csize_t sz, uchar * d) const = 0;

    /*! \brief Pure virtual function to read data from storage in blocks.
     *  \param[in] offset The offset in bytes from the current internal shared pointer
     *  \param[in] bsz    The size of a block in bytes
     *  \param[in] osz    The number of bytes between the \c start of blocks
     *  \param[in] nb     The number of blocks
     *  \param[out] d     The array to store the output in
     */
    virtual void read(csize_t offset, csize_t bsz, csize_t osz, csize_t nb, uchar * d) const = 0;

    /*! \brief Pure virtual function to write to storage.
     *  \param[in] offset The offset in bytes from the current internal shared pointer
     *  \param[in] sz     The amount of data to write to disk
     *  \param[in] d      The array to read data output from
     */
    virtual void write(csize_t offset, csize_t sz, const uchar * d) const = 0;

    /*! \brief Pure virtual function to write data to storage in blocks.
     *  \param[in] offset The offset in bytes from the current internal shared pointer
     *  \param[in] bsz    The size of a block in bytes
     *  \param[in] osz    The number of bytes between the \c start of blocks
     *  \param[in] nb     The number of blocks
     *  \param[in] d      The array to read data output from
     */
    virtual void write(csize_t offset, csize_t bsz, csize_t osz, csize_t nb, const uchar * d) const = 0;
};
}}
#endif
