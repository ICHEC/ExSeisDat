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
#include "global.hh"
namespace PIOL { namespace Data {
/*! \brief The Data layer interface. Specific data I/O implementations
 *  work off this base class.
 */
class Interface
{
    protected :
    std::shared_ptr<ExSeisPIOL> piol;  //!< Pointer to the PIOL object.
    Log::Logger * log;                 //!< For convienience
    const std::string name;            //!< Store the file name for debugging purposes.

    public :
    /*! \brief The constructor.
     *  \param[in] piol_ This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_ The name of the file associated with the instantiation.
     */
    Interface(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_) : piol(piol_), log(piol_->log.get()), name(name_)
    {
    }

    /*! \brief A virtual destructor to allow deletion.
     */
    virtual ~Interface(void) = default;

    /*! \brief find out the file size.
     *  \return The file size in bytes.
     */
    virtual size_t getFileSz() const = 0;

    /*! \brief set the file size.
     *  \param[in] sz The size in bytes
     */
    virtual void setFileSz(csize_t sz) const = 0;

    /*! \brief Read from storage.
     *  \param[in] offset The offset in bytes from the current internal shared pointer
     *  \param[in] sz     The amount of data to read from disk
     *  \param[out] d     The array to store the output in
     */
    virtual void read(csize_t offset, csize_t sz, uchar * d) const = 0;

    /*! \brief Read data from storage in blocks.
     *  \param[in] offset The offset in bytes from the current internal shared pointer
     *  \param[in] bsz    The size of a block in bytes
     *  \param[in] osz    The number of bytes between the \c start of blocks
     *  \param[in] nb     The number of blocks
     *  \param[out] d     The array to store the output in
     */
    virtual void read(csize_t offset, csize_t bsz, csize_t osz, csize_t nb, uchar * d) const = 0;

    /*! read a file where each block is determined from the list of offset
     *  \param[in] bsz    The size of a block in bytes
     *  \param[in] sz     The number of blocks to read and so the size of the offset array
     *  \param[in] offset The list of offsets (in bytes from the current internal shared pointer)
     *  \param[out] d     The array to store the output in
     */
    virtual void read(csize_t bsz, csize_t sz, csize_t * offset, uchar * d) const = 0;

    /*! write a file where each block is determined from the list of offset
     *  \param[in] bsz    The size of a block in bytes
     *  \param[in] sz     The number of blocks to write and so the size of the offset array
     *  \param[in] offset The list of offsets (in bytes from the current internal shared pointer)
     *  \param[in] d     The array to get the input from
     */
    virtual void write(csize_t bsz, csize_t sz, csize_t * offset, const uchar * d) const = 0;

    /*! \brief Write to storage.
     *  \param[in] offset The offset in bytes from the current internal shared pointer
     *  \param[in] sz     The amount of data to write to disk
     *  \param[in] d      The array to read data output from
     */
    virtual void write(csize_t offset, csize_t sz, const uchar * d) const = 0;

    /*! \brief Write data to storage in blocks.
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
