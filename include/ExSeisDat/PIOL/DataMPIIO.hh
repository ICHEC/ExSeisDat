////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief   The specfic MPI implementation of the Data layer interface
/// @details MPI implementation of data layer features such as reading.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_DATAMPIIO_HH
#define EXSEISDAT_PIOL_DATAMPIIO_HH

#include "ExSeisDat/PIOL/DataInterface.hh"
#include "ExSeisDat/utils/typedefs.h"

#include <functional>

namespace exseis {
namespace PIOL {

/*! The file modes possible for files.
 */
enum class FileMode : size_t {
    /// Read-only mode
    Read,

    /// Write-only mode
    Write,

    /// Read or write
    ReadWrite,

    /// A test mode
    Test
};


/*! @brief This templated function pointer type allows us to refer to MPI
 *         functions more compactly.
 */
template<typename U>
using MFp =
  std::function<int(MPI_File, MPI_Offset, void*, int, MPI_Datatype, U*)>;

/*! @brief The MPI-IO Data class.
 */
class DataMPIIO : public DataInterface {
  public:
    /*! @brief The MPI-IO options structure.
     */
    struct Opt {
        /// The Type of the class this structure is nested in
        typedef DataMPIIO Type;

        /// Whether collective read/write operations will be used
        bool coll;

        /// The info structure to use
        MPI_Info info;

        /// The maximum size to allow to be written to disk per process in one
        /// operation
        size_t maxSize;

        /// The MPI communicator to use for file access
        MPI_Comm fcomm;

        /// The constructor to set default options
        Opt(void);

        /// The destructor
        ~Opt(void);
    };

  private:
    /// Whether collective read/write operations will be used
    bool coll;

    /// The MPI-IO file handle
    MPI_File file;

    /// The MPI-IO file communicator
    MPI_Comm fcomm;

    /// @copydoc DataMPIIO::Opt::info
    MPI_Info info;

    /// @copydoc DataMPIIO::Opt::maxSize
    size_t maxSize;

    /*! Read a file using MPI-IO views. This function does not handle the
     *  integer limit
     *  @param[in] offset The offset in bytes from the current internal shared
     *                    pointer
     *  @param[in] bsz    The size of a block in bytes
     *  @param[in] osz    The number of bytes between the \c start of blocks
     *  @param[in] sz     The number of blocks
     *  @param[out] d     The array to store the output in
     */
    void readv(
      size_t offset, size_t bsz, size_t osz, size_t sz, unsigned char* d) const;

    /*! Write a file using MPI-IO views. This function does not handle the
     *  integer limit
     *  @param[in] offset The offset in bytes from the current internal shared
     *                    pointer
     *  @param[in] bsz    The size of a block in bytes
     *  @param[in] osz    The number of bytes between the \c start of blocks
     *  @param[in] sz     The number of blocks
     *  @param[in] d      The array to read data output from
     */
    void writev(
      size_t offset,
      size_t bsz,
      size_t osz,
      size_t sz,
      const unsigned char* d) const;

    /*! @brief The MPI-IO Init function.
     *  @param[in] opt  The MPI-IO options
     *  @param[in] mode The filemode
     */
    void Init(const DataMPIIO::Opt& opt, FileMode mode);

    /*! @brief Perform I/O on contiguous or monotonically increasing blocked
     *         data
     *  @param[in] fn The MPI-IO style function to perform the I/O with
     *  @param[in] offset The offset in bytes from the current internal shared
     *                    pointer
     *  @param[in] sz The amount of data to read from disk. d must be an array
     *                with sz elements.
     *  @param[in, out] d The array to get the input from or store the output
     *                    in.
     *  @param[in] msg The message to be written if there is an error
     *  @param[in] bsz The block size in bytes (if not contiguous)
     *  @param[in] osz The stride size in bytes (block start to block start)
     */
    void contigIO(
      MFp<MPI_Status> fn,
      size_t offset,
      size_t sz,
      unsigned char* d,
      std::string msg,
      size_t bsz = 1U,
      size_t osz = 1U) const;

    /*! @brief Perform I/O on blocks of data where each block starts at the
     *         location specified by an array of offsets.
     *  @param[in] fn The MPI-IO style function to perform the I/O with
     *  @param[in] bsz The block size in bytes.
     *  @param[in] sz The amount of blocks to read
     *  @param[in] offset An array of offsets in bytes from the current internal
     *                    shared pointer
     *  @param[in, out] d The array to get the input from or store the output
     *                    in.
     *  @param[in] msg The message to be written if there is an error
     */
    void listIO(
      MFp<MPI_Status> fn,
      size_t bsz,
      size_t sz,
      const size_t* offset,
      unsigned char* d,
      std::string msg) const;

  public:
    /*! @brief The MPI-IO class constructor.
     *  @param[in] piol_ This PIOL ptr is not modified but is used to
     *                   instantiate another shared_ptr.
     *  @param[in] name_ The name of the file associated with the instantiation.
     *  @param[in] opt   The MPI-IO options
     *  @param[in] mode The filemode
     */
    DataMPIIO(
      std::shared_ptr<ExSeisPIOL> piol_,
      std::string name_,
      const DataMPIIO::Opt& opt,
      FileMode mode = FileMode::Read);

    /*! @brief The MPI-IO class constructor.
     *  @param[in] piol_ This PIOL ptr is not modified but is used to
     *                   instantiate another shared_ptr.
     *  @param[in] name_ The name of the file associated with the instantiation.
     *  @param[in] mode The filemode
     */
    DataMPIIO(
      std::shared_ptr<ExSeisPIOL> piol_,
      std::string name_,
      FileMode mode = FileMode::Read);

    ~DataMPIIO();

    /// Test if file == MPI_FILE_NULL
    /// @return Returns \c true if the MPI_File is MPI_FILE_NULL
    bool isFileNull() const;

    /// Get the size of the file
    /// @return The size of the file
    size_t getFileSz() const;

    /// Set the size of the file, either by truncating or expanding it.
    /// @param[in] sz The new size of the file.
    void setFileSz(size_t sz) const;

    /// Read a contiguous chunk of size \c sz beginning a position \c offset
    /// from the file into the buffer \c d.
    /// @param[in]  offset The file offset to start reading at
    /// @param[in]  sz     The amount to read
    /// @param[out] d      The buffer to read into
    ///                    (pointer to array of size \c sz)
    void read(size_t offset, size_t sz, unsigned char* d) const;

    /// Write a contiguous chunk of size \c sz beginning a position \c offset
    /// from the buffer \c d into the file.
    /// @param[in]  offset The file offset to start writing at
    /// @param[in]  sz     The amount to write
    /// @param[out] d      The buffer to write from
    ///                    (pointer to array of size \c sz)
    void write(size_t offset, size_t sz, const unsigned char* d) const;


    /// Read a file in regularly spaced, non-contiguous blocks.
    /// @param[in]  offset The position in the file to start reading from
    /// @param[in]  bsz    The block size to read in bytes
    /// @param[in]  osz    The stride size in bytes, i.e. the total size from
    ///                    the start of one block to the next
    /// @param[in]  sz     The number of blocks to be read
    /// @param[out] d      Pointer to the buffer to read the data into
    ///                    (pointer to array of size \c bsz*sz)
    void read(
      size_t offset, size_t bsz, size_t osz, size_t sz, unsigned char* d) const;


    /// Write to a file in regularly spaced, non-contiguous blocks.
    /// @param[in] offset The position in the file to start writing to
    /// @param[in] bsz    The block size to write in bytes
    /// @param[in] osz    The stride size in bytes, i.e. the total size from
    ///                    the start of one block to the next
    /// @param[in] nb     The number of blocks to be written
    /// @param[in] d      Pointer to the buffer to write the data from
    ///                   (pointer to array of size \c bsz*sz)
    void write(
      size_t offset,
      size_t bsz,
      size_t osz,
      size_t nb,
      const unsigned char* d) const;

    /// Read a file in irregularly spaced, non-contiguous chunks
    /// @param[in]  bsz    The block size to read in bytes
    /// @param[in]  sz     The number of blocks to read
    /// @param[in]  offset Pointer to array of block offsets (size \c sz)
    /// @param[out] d      Pointer to the buffer to read the data into
    ///                    (pointer to array of size \c bsz*sz)
    void read(
      size_t bsz, size_t sz, const size_t* offset, unsigned char* d) const;

    /// Write to a file in irregularly spaced, non-contiguous chunks
    /// @param[in]  bsz    The block size to write in bytes
    /// @param[in]  sz     The number of blocks to write
    /// @param[in]  offset Pointer to array of block offsets (size \c sz)
    /// @param[out] d      Pointer to the buffer to write the data from
    ///                    (pointer to array of size \c bsz*sz)
    void write(
      size_t bsz,
      size_t sz,
      const size_t* offset,
      const unsigned char* d) const;
};

}  // namespace PIOL
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_DATAMPIIO_HH
