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

/// @brief File mode options for MPI-IO.
///
enum class File_mode_mpi : int {
    /// @brief Read-only mode
    Read = MPI_MODE_RDONLY | MPI_MODE_UNIQUE_OPEN,

    /// @brief Write-only mode
    Write = MPI_MODE_CREATE | MPI_MODE_WRONLY | MPI_MODE_UNIQUE_OPEN,

    /// @brief Read or write
    ReadWrite = MPI_MODE_CREATE | MPI_MODE_RDWR | MPI_MODE_UNIQUE_OPEN
};


/// @brief An MPI-IO implementation of the IO_driver.
///
/// @details This class implements a number of workarounds to perform I/O
///          with data over 2GB with MPI-IO.
///
///          Because the MPI-2 interface uses an int for the read and write
///          sizes for MPI-IO routines, it is limited to 2GB operations at a
///          time.  Many implementations also make assumptions or contain bugs
///          around this number, so the actual maximum I/O chunk size per
///          operation will be different for different MPI implementations.
///
///          The typical approach is for every process to determine how many 2GB
///          I/O operations they need to perform to complete the transfer, then
///          communicate to determine how many collective operations every
///          process would need to participate in to avoid a deadlock. Every
///          process will then perform in that many MPI-IO calls.
///
class IO_driver_mpi : public IO_driver {
  public:
    /// @brief Detailed options for IO_driver_mpi
    ///
    struct Options {
        /// @brief Whether collective read/write operations will be used.
        ///
        /// @details Default value dependant on EXSEISDAT_MPIIO_COLLECTIVES
        ///          definition.
        bool use_collective_operations;

        /// @brief The info structure to use
        MPI_Info info = MPI_INFO_NULL;

        /// @brief The maximum size to allow to be written to disk per process
        ///        in one operation.
        ///
        /// Defaults to a safe value determined by auto-detecting the MPI
        /// implementation.
        size_t max_io_chunk_size =
            exseis::utils::mpi_max_array_length<int32_t>();

        /// @name @special_member_functions
        /// @{

        /// @brief Construct Options, and create an instance of MPI_Info.
        Options();

        /// @brief Delete Options, and free the instance of MPI_Info.
        ~Options();

        /// @copy_constructor{delete}
        Options(const Options&) = delete;
        /// @copy_assignment{delete}
        Options& operator=(const Options&) = delete;
        /// @move_constructor{delete}
        Options(Options&&) = delete;
        /// @move_assignment{delete}
        Options& operator=(Options&&) = delete;

        /// @}
    };

  protected:
    /// @brief MPI-IO implementation of IO_driver
    class Implementation : public IO_driver::Implementation {
      public:
        /// Pointer to the PIOL object.
        std::shared_ptr<exseis::utils::Log> m_log;

        /// Store the file name for debugging purposes.
        std::string m_file_name;

        /// The MPI communicator used for MPI operations
        MPI_Comm m_file_communicator;

        /// @copydoc IO_driver_mpi::Options::use_collective_operations
        bool m_use_collective_operations;

        /// The MPI-IO file handle
        MPI_File m_file = MPI_FILE_NULL;

        /// @copydoc IO_driver_mpi::Options::info
        MPI_Info m_info;

        /// @copydoc IO_driver_mpi::Options::max_io_chunk_size
        size_t m_max_io_chunk_size;

        /// @copydoc IO_driver_mpi::IO_driver_mpi(std::string, File_mode_mpi, MPI_Comm, std::shared_ptr<exseis::utils::Log>, const IO_driver_mpi::Options&)
        Implementation(
            std::string file_name,
            File_mode_mpi mode,
            MPI_Comm communicator,
            std::shared_ptr<exseis::utils::Log> log,
            const IO_driver_mpi::Options& options);

        /// @virtual_destructor
        ~Implementation() override;

        /// @copydoc IO_driver_mpi::close
        virtual void close();

        bool is_open() const override;

        /// @copydoc IO_driver::Implementation::get_file_size
        /// @details This function is collective over `file_communicator`.
        size_t get_file_size() const override;

        void set_file_size(size_t size) override;

        void read(size_t offset, size_t size, void* buffer) const override;

        void write(size_t offset, size_t size, const void* buffer) override;

        void read_strided(
            size_t offset,
            size_t block_size,
            size_t stride_size,
            size_t number_of_blocks,
            void* buffer) const override;

        void write_strided(
            size_t offset,
            size_t block_size,
            size_t stride_size,
            size_t number_of_blocks,
            const void* buffer) override;

        void read_offsets(
            size_t block_size,
            size_t number_of_blocks,
            const size_t* offsets,
            void* buffer) const override;

        void write_offsets(
            size_t block_size,
            size_t number_of_blocks,
            const size_t* offsets,
            const void* buffer) override;

        void sync() override;
    };

  public:
    /// @brief The MPI-IO class constructor.
    ///
    /// @param[in] file_name    The name of the file to operate on.
    /// @param[in] mode         The file mode, e.g. read, write, delete on close
    ///                         etc.
    /// @param[in] log          The Log object to use for logging.
    /// @param[in] communicator The MPI Communicator to use for communication.
    /// @param[in] options      The MPI-IO options
    ///
    IO_driver_mpi(
        std::string file_name,
        File_mode_mpi mode,
        MPI_Comm communicator,
        std::shared_ptr<exseis::utils::Log> log,
        const IO_driver_mpi::Options& options = IO_driver_mpi::Options());

    /// @brief Explicit destructor, closes file and frees info
    void close()
    {
        static_cast<IO_driver_mpi::Implementation*>(m_implementation.get())
            ->close();
    }

    /// @brief Get the underlying MPI file handle.
    ///
    /// @return Returns the underlying MPI file handle.
    ///
    MPI_File file_handle() const
    {
        return static_cast<const IO_driver_mpi::Implementation*>(
                   m_implementation.get())
            ->m_file;
    }
};

}  // namespace io_driver
}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_IO_DRIVER_IO_DRIVER_MPI_HH
