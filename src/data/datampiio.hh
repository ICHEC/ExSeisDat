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
/*! \brief The MPI-IO options structure.
 */
struct MPIIOOpt : public Opt
{
    int mode;           //!< The mode to open the associated file with
    MPI_Info info;      //!< The info structure to use
    size_t maxSize;     //!< The maximum size to allow to be written to disk per process in one operation
    MPIIOOpt(void);     //!< The constructor to set default options
};

/*! \brief The MPI-IO Data class.
 */
class MPIIO : public Interface
{
    private :
    MPI_File file;      //!< The MPI file handle
    MPI_Comm comm;      //!< The MPI communicator for MPI-IO
    MPI_Info info;      //!< \copydoc MPIIOOpt::info
    size_t maxSize;     //!< \copydoc MPIIOOpt::maxSize
    public :
    /*! \brief The MPI-IO class constructor.
     *  \param[in] piol_ This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_ The name of the file associated with the instantiation.
     *  \param[in] opt   The MPI-IO options
     */
    MPIIO(const std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const MPIIOOpt & opt);

    ~MPIIO(void);

    /*! \brief Find out the file size.
     *  \return The file size in bytes.
     */
    size_t getFileSz() const;

    /*! \brief Set the file size (preallocates).
     *  \param[in] sz The size in bytes
     */
    void setFileSz(const size_t sz) const;

    /*! \brief Read from storage using MPI-IO. If \c offset + \c sz is greater than the file size then
     *  only up to the file size is read. The rest of \c d is in an undefined state.
     *  \param[in] offset The offset in bytes from the current internal shared pointer
     *  \param[in] sz     The amount of data to read from disk. d must be an array with
     *  sz elements.
     *  \param[out] d     The array to store the output in.
     */
    void read(const size_t offset, const size_t sz, uchar * d) const;

    /*! \brief Write to storage.
     *  \param[in] offset The offset in bytes from the current internal shared pointer
     *  \param[in] sz     The amount of data to write to disk
     *  \param[in] d      The array to read data output from
     */
    void write(const size_t offset, const size_t sz, const uchar * d) const;
};
}}
#endif
