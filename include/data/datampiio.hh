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
#include "global.hh"
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
        bool coll;          //!< Whether collective read/write operations will be used
        MPI_Info info;      //!< The info structure to use
        size_t maxSize;     //!< The maximum size to allow to be written to disk per process in one operation
        MPI_Comm fcomm;     //!< The MPI communicator to use for file access
        Opt(void);          //!< The constructor to set default options
        ~Opt(void);         //!< The destructor
    };

    private :
    bool coll;          //!< Whether collective read/write operations will be used
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
    void readv(const size_t offset, const size_t bsz, const size_t osz, const size_t sz, uchar * d) const;

    /*! Write a file using MPI-IO views. This function does not handle the integer limit
     *  \param[in] offset The offset in bytes from the current internal shared pointer
     *  \param[in] bsz    The size of a block in bytes
     *  \param[in] osz    The number of bytes between the \c start of blocks
     *  \param[in] sz     The number of blocks
     *  \param[in] d      The array to read data output from
     */
    void writev(const size_t offset, const size_t bsz, const size_t osz, const size_t sz, const uchar * d) const;

    /*! \brief The MPI-IO Init function.
     *  \param[in] opt  The MPI-IO options
     *  \param[in] mode The filemode
     */
    void Init(const MPIIO::Opt & opt, FileMode mode);

    /*! \brief Perform I/O on contiguous or monotonically increasing blocked data
     *  \param[in] fn The MPI-IO style function to perform the I/O with
     *  \param[in] offset The offset in bytes from the current internal shared pointer
     *  \param[in] sz The amount of data to read from disk. d must be an array with
     *             sz elements.
     *  \param[in, out] d The array to get the input from or store the output in.
     *  \param[in] msg The message to be written if there is an error
     *  \param[in] bsz The block size in bytes (if not contiguous)
     *  \param[in] osz The stride size in bytes (block start to block start)
     */
    void contigIO(const MFp<MPI_Status> fn, const size_t offset, const size_t sz, uchar * d, std::string msg,
                                                            const size_t bsz = 1U, const size_t osz = 1U) const;

    /*! \brief Perform I/O on blocks of data where each block starts at the location specified by an array of offsets.
     *  \param[in] fn The MPI-IO style function to perform the I/O with
     *  \param[in] bsz The block size in bytes.
     *  \param[in] sz The amount of blocks to read
     *  \param[in] offset An array of offsets in bytes from the current internal shared pointer
     *  \param[in, out] d The array to get the input from or store the output in.
     *  \param[in] msg The message to be written if there is an error
     */
    void listIO(const MFp<MPI_Status> fn, const size_t bsz, const size_t sz, const size_t * offset, uchar * d, std::string msg) const;

    public :

    /*! \brief The MPI-IO class constructor.
     *  \param[in] piol_ This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_ The name of the file associated with the instantiation.
     *  \param[in] opt   The MPI-IO options
     *  \param[in] mode The filemode
     */
    MPIIO(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const MPIIO::Opt & opt, FileMode mode = FileMode::Read);

    /*! \brief The MPI-IO class constructor.
     *  \param[in] piol_ This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_ The name of the file associated with the instantiation.
     *  \param[in] mode The filemode
     */
    MPIIO(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, FileMode mode = FileMode::Read);

    ~MPIIO(void);

    /// Test if file == MPI_FILE_NULL
    bool isFileNull() const;

    size_t getFileSz() const;

    void setFileSz(const size_t sz) const;

    void read(const size_t offset, const size_t sz, uchar * d) const;

    void read(const size_t offset, const size_t bsz, const size_t osz, const size_t sz, uchar * d) const;

    void read(const size_t bsz, const size_t sz, const size_t * offset, uchar * d) const;

    void write(const size_t bsz, const size_t sz, const size_t * offset, const uchar * d) const;

    void write(const size_t offset, const size_t sz, const uchar * d) const;

    void write(const size_t offset, const size_t bsz, const size_t osz, const size_t nb, const uchar * d) const;
};
}}
#endif
