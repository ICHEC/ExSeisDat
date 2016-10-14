/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief The specfic MPI implementation of the Data layer interface
 *   \details MPI implementation of data layer features such as reading
*//*******************************************************************************************/
#ifndef PIOLDATAMPIIO_INCLUDE_GUARD
#define PIOLDATAMPIIO_INCLUDE_GUARD
#include <mpi.h>
#include <memory>
#include <typeinfo>
#include "global.hh"
#include "anc/cmpi.hh"
#include "data/data.hh"

namespace PIOL { namespace Data {
/*! \brief This templated function pointer type allows us to refer to MPI functions more compactly.
 */
template <typename U>
using MFp = std::function<int(MPI_File, MPI_Offset, void *, int, MPI_Datatype, U *)>;

/*! \brief The MPI-IO Data class.
 */
class MPIIO : public Interface
{
    public :

    /*! \brief The MPI-IO options structure.
     */
    struct Opt
    {
        typedef MPIIO Type; //!< The Type of the class this structure is nested in
        bool coll;
        MPI_Info info;      //!< The info structure to use
        size_t maxSize;     //!< The maximum size to allow to be written to disk per process in one operation
        MPI_Comm fcomm;     //!< The MPI communicator to use for file access
        Opt(void);          //!< The constructor to set default options
        ~Opt(void);         //!< The destructor
    };

    private :
    bool coll;
    MPI_File file;      //!< The MPI-IO file handle
    MPI_Comm fcomm;     //!< The MPI-IO file communicator
    MPI_Info info;      //!< \copydoc MPIIO::Opt::info
    size_t maxSize;     //!< \copydoc MPIIO::Opt::maxSize

    /*! Read a file using MPI-IO views. This function does not handle the integer limit
     *  \param[in] offset The offset in bytes from the current internal shared pointer
     *  \param[in] bsz    The size of a block in bytes
     *  \param[in] osz    The number of bytes between the \c start of blocks
     *  \param[in] sz     The number of blocks
     *  \param[out] d     The array to store the output in
     */
    void readv(csize_t offset, csize_t bsz, csize_t osz, csize_t sz, uchar * d) const;

    /*! Write a file using MPI-IO views. This function does not handle the integer limit
     *  \param[in] offset The offset in bytes from the current internal shared pointer
     *  \param[in] bsz    The size of a block in bytes
     *  \param[in] osz    The number of bytes between the \c start of blocks
     *  \param[in] sz     The number of blocks
     *  \param[in] d      The array to read data output from
     */
    void writev(csize_t offset, csize_t bsz, csize_t osz, csize_t sz, const uchar * d) const;

    /*! \brief The MPI-IO Init function.
     *  \param[in] opt  The MPI-IO options
     *  \param[in] mode The filemode
     */
    void Init(const MPIIO::Opt & opt, FileMode mode);

    void contigIO(const MFp<MPI_Status> fn, csize_t offset, csize_t sz, uchar * d, std::string msg,
                                                           csize_t bsz = 1U, csize_t osz = 1U) const;
    void listIO(const MFp<MPI_Status> fn, csize_t bsz, csize_t sz, csize_t * offset, uchar * d, std::string msg) const;

    public :

    /*! \brief The MPI-IO class constructor.
     *  \param[in] piol_ This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_ The name of the file associated with the instantiation.
     *  \param[in] opt   The MPI-IO options
     *  \param[in] mode The filemode
     */
    MPIIO(const Piol piol_, const std::string name_, const MPIIO::Opt & opt, FileMode mode = FileMode::Read);

    /*! \brief The MPI-IO class constructor.
     *  \param[in] piol_ This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_ The name of the file associated with the instantiation.
     *  \param[in] mode The filemode
     */
    MPIIO(const Piol piol_, const std::string name_, FileMode mode = FileMode::Read);

    ~MPIIO(void);

    /*! \brief Find out the file size.
     *  \return The file size in bytes.
     */
    size_t getFileSz() const;

    /*! \brief Set the file size (preallocates).
     *  \param[in] sz The size in bytes
     */
    void setFileSz(csize_t sz) const;

    /*! \brief Read from storage using MPI-IO. If \c offset + \c sz is greater than the file size then
     *  only up to the file size is read. The rest of \c d is in an undefined state.
     *  \param[in] offset The offset in bytes from the current internal shared pointer
     *  \param[in] sz     The amount of data to read from disk. d must be an array with
     *  sz elements.
     *  \param[out] d     The array to store the output in.
     */
    void read(csize_t offset, csize_t sz, uchar * d) const;

    /*! \brief Read data from storage in blocks.
     *  \param[in] offset The offset in bytes from the current internal shared pointer
     *  \param[in] bsz    The size of a block in bytes
     *  \param[in] osz    The number of bytes between the \c start of blocks
     *  \param[in] sz     The number of blocks
     *  \param[out] d     The array to store the output in
     */
    void read(csize_t offset, csize_t bsz, csize_t osz, csize_t sz, uchar * d) const;

    /*! Read a file where each block is determined from the list of offset
     *  \param[in] bsz    The size of a block in bytes
     *  \param[in] sz     The number of blocks to read and so the size of the offset array
     *  \param[in] offset The list of offsets. In bytes from the current internal shared pointer
     *  \param[out] d     The array to store the output in
     */
    void read(csize_t bsz, csize_t sz, csize_t * offset, uchar * d) const;
//TODO: Document
    void write(csize_t bsz, csize_t sz, csize_t * offset, const uchar * d) const;

    /*! \brief Write to storage.
     *  \param[in] offset The offset in bytes from the current internal shared pointer
     *  \param[in] sz     The amount of data to write to disk
     *  \param[in] d      The array to read data output from
     */
    void write(csize_t offset, csize_t sz, const uchar * d) const;

    /*! \brief Write data to storage in blocks.
     *  \param[in] offset The offset in bytes from the current internal shared pointer
     *  \param[in] bsz    The size of a block in bytes
     *  \param[in] osz    The number of bytes between the \c start of blocks
     *  \param[in] nb     The number of blocks
     *  \param[in] d      The array to read data output from
     */
    void write(csize_t offset, csize_t bsz, csize_t osz, csize_t nb, const uchar * d) const;
};
}}
#endif
