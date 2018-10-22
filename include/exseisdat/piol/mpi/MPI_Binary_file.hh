////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief   The specfic MPI implementation of the Data layer interface
/// @details MPI implementation of data layer features such as reading.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_MPI_MPI_BINARY_FILE_HH
#define EXSEISDAT_PIOL_MPI_MPI_BINARY_FILE_HH

#include "exseisdat/piol/Binary_file.hh"
#include "exseisdat/utils/mpi/MPI_max_array_length.hh"
#include "exseisdat/utils/typedefs.hh"

///
/// @namespace exseis::piol::mpi
///
/// @brief MPI-based implementations of ExSeisPIOL interfaces.
///

namespace exseis {
namespace piol {
inline namespace mpi {

/*! The file modes possible for files.
 */
enum FileMode : int {
    /// Read-only mode
    Read = MPI_MODE_RDONLY | MPI_MODE_UNIQUE_OPEN,

    /// Write-only mode
    Write = MPI_MODE_CREATE | MPI_MODE_WRONLY | MPI_MODE_UNIQUE_OPEN,

    /// Read or write
    ReadWrite = MPI_MODE_CREATE | MPI_MODE_RDWR | MPI_MODE_UNIQUE_OPEN
};


/*! @brief The MPI-IO Data class.
 */
class MPI_Binary_file : public Binary_file {
  public:
    /*! @brief The MPI-IO options structure.
     */
    struct Opt {
        /// The Type of the class this structure is nested in
        typedef MPI_Binary_file Type;

        /// Whether collective read/write operations will be used.
        /// Default value dependant on EXSEISDAT_MPIIO_COLLECTIVES definition.
        bool use_collective_operations;

        /// The info structure to use
        MPI_Info info;

        /// The maximum size to allow to be written to disk per process in one
        /// operation
        size_t max_size = exseis::utils::mpi_max_array_length<int32_t>();

        /// The MPI communicator to use for file access
        MPI_Comm file_communicator = MPI_COMM_WORLD;


        /// @brief The constructor to set default options.
        ///        Creates an MPI_Info object in `info`.
        Opt(void);

        /// @brief The destructor. Frees `info`.
        ~Opt(void);

        /// @name Implicit Members
        /// @{
        Opt(const Opt&) = delete;
        Opt& operator=(const Opt&) = delete;
        Opt(Opt&&)                 = delete;
        Opt& operator=(Opt&&) = delete;
        /// @}
    };

  private:
    /// Pointer to the PIOL object.
    std::shared_ptr<exseis::utils::Log> m_log;

    /// Store the file name for debugging purposes.
    std::string m_file_name;

    /// @copydoc MPI_Binary_file::Opt::use_collective_operations
    bool m_use_collective_operations;

    /// The MPI-IO file handle
    MPI_File m_file = MPI_FILE_NULL;

    /// @copydoc MPI_Binary_file::Opt::file_communicator
    MPI_Comm m_file_communicator;

    /// @copydoc MPI_Binary_file::Opt::info
    MPI_Info m_info;

    /// @copydoc MPI_Binary_file::Opt::max_size
    size_t m_max_size;

  public:
    /// @brief The MPI-IO class constructor.
    ///
    /// @param[in] piol      The PIOL object, used for logging.
    /// @param[in] file_name The name of the file to operate on.
    /// @param[in] mode      The file mode, e.g. read, write, delete on close
    ///                      etc.
    /// @param[in] opt       The MPI-IO options
    ///
    MPI_Binary_file(
      std::shared_ptr<ExSeisPIOL> piol,
      std::string file_name,
      FileMode mode,
      const MPI_Binary_file::Opt& opt = MPI_Binary_file::Opt());

    /// @brief The MPI-IO class constructor.
    ///
    /// @param[in] log       The Log object to use for logging.
    /// @param[in] file_name The name of the file to operate on.
    /// @param[in] mode      The file mode, e.g. read, write, delete on close
    ///                      etc.
    /// @param[in] opt       The MPI-IO options
    ///
    MPI_Binary_file(
      std::shared_ptr<exseis::utils::Log> log,
      std::string file_name,
      FileMode mode,
      const MPI_Binary_file::Opt& opt = MPI_Binary_file::Opt());

    /// @brief Destructor.
    ~MPI_Binary_file();


    /// @name Implicit members
    /// @{
    MPI_Binary_file(const MPI_Binary_file&) = delete;
    MPI_Binary_file& operator=(const MPI_Binary_file&) = delete;
    MPI_Binary_file(MPI_Binary_file&&)                 = delete;
    MPI_Binary_file& operator=(MPI_Binary_file&&) = delete;
    /// @}


    /// @brief Test if the file is open.
    ///
    /// @return Returns \c true if the file is open, \c false otherwise.
    ///
    bool is_open() const override;


    /// @brief Get the size of the file.
    ///
    /// This function is collective over `file_communicator`.
    ///
    /// @return The size of the file
    ///
    size_t get_file_size() const override;


    /// @brief Set the size of the file, either by truncating or expanding it.
    ///
    /// @param[in] size The new size of the file.
    ///
    void set_file_size(size_t size) const override;


    /// @brief Read a contiguous chunk of size \c size beginning a position \c
    ///        offset from the file into the buffer \c buffer.
    ///
    /// @param[in]  offset The file offset to start reading at
    /// @param[in]  size   The amount to read
    /// @param[out] buffer The buffer to read into
    ///                    (pointer to array of size \c size)
    ///
    void read(size_t offset, size_t size, unsigned char* buffer) const override;


    /// @brief Write a contiguous chunk of size \c size beginning a position \c
    ///        offset from the buffer \c buffer into the file.
    ///
    /// @param[in]  offset The file offset to start writing at
    /// @param[in]  size   The amount to write
    /// @param[out] buffer The buffer to write from
    ///                    (pointer to array of size \c size)
    ///
    void write(
      size_t offset, size_t size, const unsigned char* buffer) const override;


    /// @brief Read a file in regularly spaced, non-contiguous blocks.
    ///
    /// @param[in]  offset           The position in the file to start reading
    ///                              from
    /// @param[in]  block_size       The block size to read in bytes
    /// @param[in]  stride_size      The stride size in bytes, i.e. the total
    ///                              size from the start of one block to the
    ///                              next
    /// @param[in]  number_of_blocks The number of blocks to be read
    /// @param[out] buffer           Pointer to the buffer to read the data into
    ///                              (pointer to array of size
    ///                                  `block_size * number_of_blocks`)
    ///
    void read_noncontiguous(
      size_t offset,
      size_t block_size,
      size_t stride_size,
      size_t number_of_blocks,
      unsigned char* buffer) const override;


    /// @brief Write to a file in regularly spaced, non-contiguous blocks.
    ///
    /// @param[in] offset           The position in the file to start writing to
    /// @param[in] block_size       The block size to write in bytes
    /// @param[in] stride_size      The stride size in bytes, i.e. the total
    ///                             size from the start of one block to the next
    /// @param[in] number_of_blocks The number of blocks to be written
    /// @param[in] buffer           Pointer to the buffer to write the data from
    ///                             (pointer to array of size
    ///                                 `block_size*number_of_blocks`)
    ///
    void write_noncontiguous(
      size_t offset,
      size_t block_size,
      size_t stride_size,
      size_t number_of_blocks,
      const unsigned char* buffer) const override;


    /// @brief Read a file in irregularly spaced, non-contiguous chunks.
    ///
    /// @param[in]  block_size       The block size to read in bytes
    /// @param[in]  number_of_blocks The number of blocks to read
    /// @param[in]  offsets          Pointer to array of block offsets
    ///                              (size `number_of_blocks`)
    /// @param[out] buffer           Pointer to the buffer to read the data into
    ///                              (pointer to array of size
    ///                                  `block_size*number_of_blocks`)
    ///
    void read_noncontiguous_irregular(
      size_t block_size,
      size_t number_of_blocks,
      const size_t* offsets,
      unsigned char* buffer) const override;


    /// @brief Write to a file in irregularly spaced, non-contiguous chunks.
    ///
    /// @param[in] block_size       The block size to write in bytes
    /// @param[in] number_of_blocks The number of blocks to write
    /// @param[in] offsets          Pointer to array of block offsets
    ///                             (size `number_of_blocks`)
    /// @param[in] buffer           Pointer to the buffer to write the data from
    ///                              (pointer to array of size
    ///                                  `block_size*number_of_blocks`)
    ///
    void write_noncontiguous_irregular(
      size_t block_size,
      size_t number_of_blocks,
      const size_t* offsets,
      const unsigned char* buffer) const override;
};

}  // namespace mpi
}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_MPI_MPI_BINARY_FILE_HH
