////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief   The specfic MPI implementation of the Data layer interface
/// @details MPI implementation of data layer features such as reading.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_IO_DRIVER_IO_DRIVER_MPI_HH
#define EXSEISDAT_PIOL_IO_DRIVER_IO_DRIVER_MPI_HH

#include "exseisdat/piol/io_driver/IO_driver.hh"
#include "exseisdat/utils/mpi_utils/MPI_max_array_length.hh"
#include "exseisdat/utils/types/typedefs.hh"

namespace exseis {
namespace piol {
inline namespace io_driver {

/// @brief The file modes possible for files.
///
enum FileMode : int {
    /// @brief Read-only mode
    Read = MPI_MODE_RDONLY | MPI_MODE_UNIQUE_OPEN,

    /// @brief Write-only mode
    Write = MPI_MODE_CREATE | MPI_MODE_WRONLY | MPI_MODE_UNIQUE_OPEN,

    /// @brief Read or write
    ReadWrite = MPI_MODE_CREATE | MPI_MODE_RDWR | MPI_MODE_UNIQUE_OPEN
};


/// @brief The MPI-IO Data class.
///
class IO_driver_mpi : public IO_driver {
  public:
    /// @brief The MPI-IO options structure.
    ///
    /// @details This class also manages the lifetime of an MPI_Info object.
    ///
    struct Opt {
        /// @brief The Type of the class this structure is nested in
        typedef IO_driver_mpi Type;

        /// @brief Whether collective read/write operations will be used.
        ///
        /// @details Default value dependant on EXSEISDAT_MPIIO_COLLECTIVES
        ///          definition.
        bool use_collective_operations;

        /// @brief The info structure to use
        MPI_Info info;

        /// @brief The maximum size to allow to be written to disk per process
        ///        in one operation
        size_t max_size = exseis::utils::mpi_max_array_length<int32_t>();

        /// @brief The MPI communicator to use for file access
        MPI_Comm file_communicator = MPI_COMM_WORLD;


        /// @name @special_member_functions
        /// @{

        /// @brief Construct Opt, and create an instance of MPI_Info.
        Opt();

        /// @brief Delete Opt, and free the instance of MPI_Info.
        void free();

        /// @brief Delete Opt, and free the instance of MPI_Info.
        ~Opt();

        /// @copy_constructor{delete}
        Opt(const Opt&) = delete;
        /// @copy_assignment{delete}
        Opt& operator=(const Opt&) = delete;
        /// @move_constructor{delete}
        Opt(Opt&&) = delete;
        /// @move_assignment{delete}
        Opt& operator=(Opt&&) = delete;

        /// @}
    };

  private:
    /// Pointer to the PIOL object.
    std::shared_ptr<exseis::utils::Log> m_log;

    /// Store the file name for debugging purposes.
    std::string m_file_name;

    /// @copydoc IO_driver_mpi::Opt::use_collective_operations
    bool m_use_collective_operations;

    /// The MPI-IO file handle
    MPI_File m_file = MPI_FILE_NULL;

    /// @copydoc IO_driver_mpi::Opt::file_communicator
    MPI_Comm m_file_communicator;

    /// @copydoc IO_driver_mpi::Opt::info
    MPI_Info m_info;

    /// @copydoc IO_driver_mpi::Opt::max_size
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
    IO_driver_mpi(
        std::shared_ptr<ExSeisPIOL> piol,
        std::string file_name,
        FileMode mode,
        const IO_driver_mpi::Opt& opt = IO_driver_mpi::Opt());

    /// @brief The MPI-IO class constructor.
    ///
    /// @param[in] log       The Log object to use for logging.
    /// @param[in] file_name The name of the file to operate on.
    /// @param[in] mode      The file mode, e.g. read, write, delete on close
    ///                      etc.
    /// @param[in] opt       The MPI-IO options
    ///
    IO_driver_mpi(
        std::shared_ptr<exseis::utils::Log> log,
        std::string file_name,
        FileMode mode,
        const IO_driver_mpi::Opt& opt = IO_driver_mpi::Opt());

    /// @brief Explicit destructor, closes file and frees info
    void close();

    /// @brief Destructor.
    ~IO_driver_mpi();


    /// @name @special_member_functions
    /// @{

    /// @copy_constructor{delete}
    IO_driver_mpi(const IO_driver_mpi&) = delete;
    /// @copy_assignment{delete}
    IO_driver_mpi& operator=(const IO_driver_mpi&) = delete;

    /// @move_constructor{delete}
    IO_driver_mpi(IO_driver_mpi&&) = delete;
    /// @move_assignment{delete}
    IO_driver_mpi& operator=(IO_driver_mpi&&) = delete;

    /// @}


    /// @brief Test if the file is open.
    ///
    /// @return Returns \c true if the file is open, \c false otherwise.
    ///
    bool is_open() const override;


    /// @brief Get the size of the file.
    ///
    /// @return The size of the file
    ///
    /// @details This function is collective over `file_communicator`.
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
    void read(size_t offset, size_t size, void* buffer) const override;


    /// @brief Write a contiguous chunk of size \c size beginning a position \c
    ///        offset from the buffer \c buffer into the file.
    ///
    /// @param[in]  offset The file offset to start writing at
    /// @param[in]  size   The amount to write
    /// @param[out] buffer The buffer to write from
    ///                    (pointer to array of size \c size)
    ///
    void write(size_t offset, size_t size, const void* buffer) const override;


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
        void* buffer) const override;


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
        const void* buffer) const override;


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
        void* buffer) const override;


    /// @brief Write to a file in irregularly spaced, non-contiguous chunks.
    ///
    /// @param[in] block_size       The block size to write in bytes
    /// @param[in] number_of_blocks The number of blocks to write
    /// @param[in] offsets          Pointer to array of block offsets
    ///                             (size `number_of_blocks`)
    /// @param[in] buffer           Pointer to the buffer to write the data from
    ///                             (pointer to array of size
    ///                                 `block_size*number_of_blocks`)
    ///
    void write_noncontiguous_irregular(
        size_t block_size,
        size_t number_of_blocks,
        const size_t* offsets,
        const void* buffer) const override;
};

}  // namespace io_driver
}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_IO_DRIVER_IO_DRIVER_MPI_HH
