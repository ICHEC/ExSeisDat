////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of \c IO_driver_mpi
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/piol/configuration/ExSeisPIOL.hh"

#include "exseisdat/piol/io_driver/IO_driver_mpi.hh"
#include "exseisdat/utils/mpi_utils/MPI_error_to_string.hh"
#include "exseisdat/utils/mpi_utils/MPI_max_array_length.hh"
#include "exseisdat/utils/types/MPI_type.hh"

#include "exseisdat/utils/mpi_utils/Safe_collective_block_chunks.hh"
#include "exseisdat/utils/mpi_utils/detected_mpi_implementation.hh"

#include <algorithm>
#include <assert.h>
#include <string>


namespace exseis {
namespace piol {
inline namespace io_driver {
namespace {

/// @brief Returns whether collectives should be used by default or not.
///
/// @retval true  if collectives should be used by default
/// @retval false otherwise
///
bool default_collectiveness()
{
    using namespace exseis::utils;

    switch (detected_mpi_implementation) {
        case (mpi_implementation::open_mpi):
            return false;
        case (mpi_implementation::mpich):
        case (mpi_implementation::intel):
        case (mpi_implementation::unknown):
            return true;
    }

    assert(false && "Unknown MPI Implementation!");
    return true;
}

}  // namespace


IO_driver_mpi::Options::Options() :
    use_collective_operations(default_collectiveness())
{
    //    MPI_Info_create(&info);
    //    MPI_Info_set(info, "access_style", "read_once");
    //    MPI_Info_set(info, "romio_cb_read", "false");
    //    MPI_Info_set(info, "romio_cb_write", "false");
    //    MPI_Info_set(info, "ind_rd_buffer_size", "0");
    //    MPI_Info_set(info, "ind_wr_buffer_size", "0");
    //    MPI_Info_set(info, "direct_read", "true");
    //    MPI_Info_set(info, "direct_write", "true");

    //    MPI_Info_set(info, "cb_block_size", "");  see spec for more
    //    MPI_Info_set(info, "chunked", "");        see spec for more
    //    MPI_Info_set(info, "nb_proc", "");
    //    MPI_Info_set(info, "num_io_nodes", "");
    //    MPI_Info_set(info, "striping_factor", "10");
    //    MPI_Info_set(info, "striping_unit", "2097152");

    //    // ROMIO has this on by default. Annoying.
    //    MPI_Info_set(info, "panfs_concurrent_write", "false");
}

IO_driver_mpi::Options::~Options()
{
    // if (info != MPI_INFO_NULL) {
    //     MPI_Info_free(&info);
    // }
}


namespace {


/// @brief Test if a value can be casted to a given type without loss of data.
///
/// @tparam To The type to test casting to.
/// @tparam From The type to test casting from.
/// @param[in] from The value to test casting from.
///
/// @retval true  if `from` is representable by type `To`.
/// @retval false otherwise.
///
template<typename To, typename From>
static bool is_castable_to(const From& from)
{
    // Check To and From are integer types
    static_assert(
        std::is_integral<To>::value,
        "is_castable_to only supports integral To types");
    static_assert(
        std::is_integral<From>::value,
        "is_castable_to only supports integral From types");

    // Check absolute values of To and From can be represented by size_t.
    static_assert(
        sizeof(size_t) >= sizeof(To),
        "is_castable_to expects absolute values of type To to be representable as size_t.");
    static_assert(
        sizeof(size_t) >= sizeof(From),
        "is_castable_to expects absolute values of type From to be representable as size_t.");

    const auto abs = [](auto value) -> size_t {
        if (std::is_signed<decltype(value)>::value && value < 0) {
            // The magnitude of the smallest negative number in 2s
            // complement is larger than the largest number. i.e. INT_MAX:
            // 2147483647
            //      INT_MIN: -2147483648
            // => abs(INT_MIN) > INT_MAX.
            //
            // Therefore, we need to first make the magnitude of the
            // negative number representable (i.e. add 1), then negate it
            // (i.e. `-`), then cast it to size_t before we undo the
            // original addition of 1 (which, after negation etc. means we
            // add 1 again.)
            //
            return static_cast<size_t>(-(value + 1)) + 1;
        }

        return static_cast<size_t>(value);
    };

    if (from == 0) {
        return true;
    }

    if (from > 0) {
        if (abs(from) <= abs(std::numeric_limits<To>::max())) {
            return true;
        }
    }

    if (std::is_signed<decltype(from)>::value && from < 0) {
        if (abs(from) <= abs(std::numeric_limits<To>::min())) {
            return true;
        }
    }

    return false;
}


class IO_driver_mpi_detail {
  public:
    const std::shared_ptr<exseis::utils::Log>& m_log;
    const std::string& m_file_name;
    MPI_Comm m_file_communicator;
    MPI_File m_file;
    MPI_Info m_info;

    void log_error(
        std::string message,
        int mpi_error,
        MPI_Status* mpi_status,
        exseis::utils::Source_position source_position);

    void free_and_reset_view(MPI_Datatype& view);

    template<typename T, typename MPIReadWrite>
    void read_write_impl(
        size_t offset,
        size_t size,
        T&& buffer,
        MPIReadWrite&& mpi_read_write,
        size_t m_max_io_chunk_size,
        const char* function_name);

    MPI_Datatype create_strided_view(
        size_t offset,
        size_t block_size,
        size_t stride_size,
        size_t number_of_blocks);

    template<typename T, typename MPIReadWrite>
    void read_write_strided_impl(
        size_t offset,
        size_t block_size,
        size_t stride_size,
        size_t number_of_blocks,
        T&& buffer,
        MPIReadWrite&& mpi_read_write,
        size_t m_max_io_chunk_size,
        const char* function_name);

    MPI_Datatype create_listed_view(
        size_t number_of_blocks, size_t block_size, const MPI_Aint* offsets);

    template<typename T, typename MPIReadWrite>
    void read_write_offsets_impl(
        size_t block_size,
        size_t number_of_blocks,
        const size_t* offsets,
        T&& buffer,
        MPIReadWrite&& mpi_read_write,
        size_t m_max_io_chunk_size,
        const char* function_name);
};

/// @brief Create an error Log_entry.
///
/// This is an implementation detail of exseis::piol::IO_driver_mpi, to
/// provide a clear interface for logging.
///
/// @param[in] message     The error message to present to the user.
/// @param[in] mpi_error   The error returned by MPI for the failed operation.
/// @param[in] mpi_status  The status object set by MPI when the operation
///                        failed.
///                        (Pass MPI_STATUS_IGNORE if there is no status object)
/// @param[in] source_position A formatted string of the position in the source
///                            code to identify with the log.
///
void IO_driver_mpi_detail::log_error(
    std::string message,
    int mpi_error,
    MPI_Status* mpi_status,
    exseis::utils::Source_position source_position)
{
    std::string formatted_message = std::move(message);

    if (!m_file_name.empty()) {
        formatted_message += " for file \"" + m_file_name + "\"";
    }

    formatted_message +=
        ": " + exseis::utils::mpi_error_to_string(mpi_error, mpi_status);

    using namespace exseis::utils::logging;

    m_log->add_entry(Log_entry{Status::Error, std::move(formatted_message),
                               exseis::utils::Verbosity::none,
                               std::move(source_position)});
}


/// @brief Reset the MPI I/O view of a file and free the datatype used to
///        create it.
///
/// @param[in]     file      The MPI_File handle to the file to reset the view for.
/// @param[in,out] view      The view object to destroy.
/// @param[in]     file_name The file name to report to the logger.
///
void IO_driver_mpi_detail::free_and_reset_view(MPI_Datatype& view)
{
    static const char* function_name =
        "exseis::piol::io_driver::IO_driver_mpi_detail::free_and_reset_view";

    // Reset the view.
    int err =
        MPI_File_set_view(m_file, 0, MPI_CHAR, MPI_CHAR, "native", m_info);
    if (err != MPI_SUCCESS) {
        log_error(
            "MPI_File_set_view error", err, MPI_STATUS_IGNORE,
            EXSEISDAT_SOURCE_POSITION(function_name));
    }

    err = MPI_Type_free(&view);
    if (err != MPI_SUCCESS) {
        log_error(
            "MPI_Type_free error", err, MPI_STATUS_IGNORE,
            EXSEISDAT_SOURCE_POSITION(function_name));
    }

    view = MPI_DATATYPE_NULL;
}


}  // namespace


IO_driver_mpi::IO_driver_mpi(
    std::string file_name,
    File_mode_mpi mode,
    MPI_Comm communicator,
    std::shared_ptr<exseis::utils::Log> log,
    const IO_driver_mpi::Options& options) :
    IO_driver(std::unique_ptr<IO_driver::Implementation>{
        std::make_unique<IO_driver_mpi::Implementation>(
            std::move(file_name),
            std::move(mode),
            communicator,
            std::move(log),
            options)})
{
}

IO_driver_mpi::Implementation::Implementation(
    std::string file_name,
    File_mode_mpi mode,
    MPI_Comm communicator,
    std::shared_ptr<exseis::utils::Log> log,
    const IO_driver_mpi::Options& options) :
    m_log(std::move(log)),
    m_file_name(std::move(file_name)),
    m_file_communicator(communicator),
    m_use_collective_operations(options.use_collective_operations),
    m_info(MPI_INFO_NULL),
    m_max_io_chunk_size(options.max_io_chunk_size)
{
    static const char* function_name =
        "exseis::piol::io_driver::IO_driver_mpi::Implementation::Implementation";

    auto detail = IO_driver_mpi_detail{m_log, m_file_name, m_file_communicator,
                                       m_file, m_info};

    int err = MPI_SUCCESS;

    if (options.info != MPI_INFO_NULL) {
        err = MPI_Info_dup(options.info, &m_info);

        if (err != MPI_SUCCESS) {
            detail.log_error(
                "MPI_Info_dup error", err, MPI_STATUS_IGNORE,
                EXSEISDAT_SOURCE_POSITION(function_name));
        }
    }

    err = MPI_File_open(
        m_file_communicator, m_file_name.c_str(), static_cast<int>(mode),
        m_info, &m_file);
    if (err != MPI_SUCCESS) {
        detail.log_error(
            "MPI_File_open error", err, MPI_STATUS_IGNORE,
            EXSEISDAT_SOURCE_POSITION(function_name));
    }

    err = MPI_File_set_view(m_file, 0, MPI_CHAR, MPI_CHAR, "native", m_info);
    if (err != MPI_SUCCESS) {
        detail.log_error(
            "MPI_File_set_view error", err, MPI_STATUS_IGNORE,
            EXSEISDAT_SOURCE_POSITION(function_name));
    }
}


void IO_driver_mpi::Implementation::close()
{
    static const char* function_name =
        "exseis::piol::io_driver::IO_driver_mpi::Implementation::close";

    auto detail = IO_driver_mpi_detail{m_log, m_file_name, m_file_communicator,
                                       m_file, m_info};

    if (m_file != MPI_FILE_NULL) {
        int err = MPI_File_close(&m_file);
        if (err != MPI_SUCCESS) {
            detail.log_error(
                "MPI_File_close error", err, MPI_STATUS_IGNORE,
                EXSEISDAT_SOURCE_POSITION(function_name));
        }
        m_file = MPI_FILE_NULL;
    }

    if (m_info != MPI_INFO_NULL) {
        int err = MPI_Info_free(&m_info);
        if (err != MPI_SUCCESS) {
            detail.log_error(
                "MPI_Info_free error", err, MPI_STATUS_IGNORE,
                EXSEISDAT_SOURCE_POSITION(function_name));
        }
        m_info = MPI_INFO_NULL;
    }
}


IO_driver_mpi::Implementation::~Implementation()
{
    static const char* function_name =
        "exseis::piol::io_driver::IO_driver_mpi::Implementation::~Implementation";

    auto detail = IO_driver_mpi_detail{m_log, m_file_name, m_file_communicator,
                                       m_file, m_info};

    if (m_file != MPI_FILE_NULL) {
        int err = MPI_File_close(&m_file);
        if (err != MPI_SUCCESS) {
            detail.log_error(
                "MPI_File_close error", err, MPI_STATUS_IGNORE,
                EXSEISDAT_SOURCE_POSITION(function_name));
        }
        m_file = MPI_FILE_NULL;
    }

    if (m_info != MPI_INFO_NULL) {
        int err = MPI_Info_free(&m_info);
        if (err != MPI_SUCCESS) {
            detail.log_error(
                "MPI_Info_free error", err, MPI_STATUS_IGNORE,
                EXSEISDAT_SOURCE_POSITION(function_name));
        }
        m_info = MPI_INFO_NULL;
    }
}


bool IO_driver_mpi::Implementation::is_open() const
{
    return m_file != MPI_FILE_NULL;
}

size_t IO_driver_mpi::Implementation::get_file_size() const
{
    static const char* function_name =
        "exseis::piol::io_driver::IO_driver_mpi::Implementation::get_file_size";

    auto detail = IO_driver_mpi_detail{m_log, m_file_name, m_file_communicator,
                                       m_file, m_info};

    // Test if read-only for file synching
    int amode = MPI_MODE_RDONLY;
    MPI_File_get_amode(m_file, &amode);
    const bool is_read_only = (amode & MPI_MODE_RDONLY) == MPI_MODE_RDONLY;

    if (!is_read_only) {
        int err = MPI_File_sync(m_file);

        if (err != MPI_SUCCESS) {
            detail.log_error(
                "MPI_File_sync error", err, MPI_STATUS_IGNORE,
                EXSEISDAT_SOURCE_POSITION(function_name));
        }
    }


    MPI_Offset fsz = 0;
    int err        = MPI_File_get_size(m_file, &fsz);

    if (err != MPI_SUCCESS) {
        detail.log_error(
            "MPI_File_get_size error", err, MPI_STATUS_IGNORE,
            EXSEISDAT_SOURCE_POSITION(function_name));
    }

    MPI_Offset max_fsz = 0;
    MPI_Allreduce(
        &fsz, &max_fsz, 1, exseis::utils::mpi_type<MPI_Offset>(), MPI_MAX,
        m_file_communicator);


    assert(is_castable_to<size_t>(max_fsz));

    return static_cast<size_t>(max_fsz);
}

void IO_driver_mpi::Implementation::set_file_size(size_t size)
{
    static const char* function_name =
        "exseis::piol::io_driver::IO_driver_mpi::Implementation::set_file_size";

    auto detail = IO_driver_mpi_detail{m_log, m_file_name, m_file_communicator,
                                       m_file, m_info};

    assert(is_castable_to<MPI_Offset>(size));

    int err = MPI_File_set_size(m_file, MPI_Offset(size));

    if (err != MPI_SUCCESS) {
        detail.log_error(
            "MPI_File_set_size error", err, MPI_STATUS_IGNORE,
            EXSEISDAT_SOURCE_POSITION(function_name));
    }
}


namespace {

/// Implementation for IO_driver_mpi::Implementation::read and
/// IO_driver_mpi::Implementation::write
///
/// @tparam T The type of the buffer.
///           This is used to template the constness of the buffer.
/// @tparam MPIReadWrite The MPI I/O function type.
///                      The MPI I/O read and write functions have the same
///                      types, except for the constness of the buffer
///                      parameter. This is used to template that constness.
///
/// @param[in]     offset         The offset in bytes into the file.
/// @param[in]     size           The size in bytes to read/write from/to the
///                               file.
/// @param[in,out] buffer         The buffer to read into or write from.
/// @param[in]     mpi_read_write The MPI I/O function to use for reading or
///                               writing.
/// @param[in]     m_max_io_chunk_size          IO_driver_mpi::Implementation::m_max_size
/// @param[in]     function_name The name of the function to report during
///                              logging.
template<typename T, typename MPIReadWrite>
void IO_driver_mpi_detail::read_write_impl(
    size_t offset,
    size_t size,
    T&& buffer,
    MPIReadWrite&& mpi_read_write,
    size_t m_max_io_chunk_size,
    const char* function_name)
{
    int rank      = -1;
    int rank_size = -1;
    MPI_Comm_rank(m_file_communicator, &rank);
    MPI_Comm_size(m_file_communicator, &rank_size);

    const size_t stride_size = 1;
    const size_t block_size  = 1;

    const auto block_chunks =
        exseis::utils::mpi_utils::Safe_collective_block_chunks(
            stride_size, size, m_max_io_chunk_size, m_file_communicator);

    for (const auto block_chunk : block_chunks) {
        const size_t offset_to_block_start =
            offset + block_chunk.start * stride_size;

        MPI_Status status;
        int err = mpi_read_write(
            m_file, offset_to_block_start,
            buffer + (block_chunk.start * block_size), block_chunk.size,
            exseis::utils::mpi_type<unsigned char>(), &status);
        if (err != MPI_SUCCESS) {
            log_error(
                "MPI_File_set_size error", err, MPI_STATUS_IGNORE,
                EXSEISDAT_SOURCE_POSITION(function_name));
        }
    }
}

}  // namespace


void IO_driver_mpi::Implementation::read(
    size_t offset, size_t size, void* buffer) const
{
    static const char* function_name =
        "exseis::piol::io_driver::IO_driver_mpi::Implementation::read";

    auto detail = IO_driver_mpi_detail{m_log, m_file_name, m_file_communicator,
                                       m_file, m_info};

    const auto mpi_read =
        m_use_collective_operations ? MPI_File_read_at_all : MPI_File_read_at;

    detail.read_write_impl(
        offset, size, static_cast<unsigned char*>(buffer), mpi_read,
        m_max_io_chunk_size, function_name);
}

void IO_driver_mpi::Implementation::write(
    size_t offset, size_t size, const void* buffer)
{
    static const char* function_name =
        "exseis::piol::io_driver::IO_driver_mpi::Implementation::write";

    auto detail = IO_driver_mpi_detail{m_log, m_file_name, m_file_communicator,
                                       m_file, m_info};

    const auto mpi_write =
        m_use_collective_operations ? MPI_File_write_at_all : MPI_File_write_at;

    detail.read_write_impl(
        offset, size, static_cast<const unsigned char*>(buffer), mpi_write,
        m_max_io_chunk_size, function_name);
}


namespace {

MPI_Datatype create_contiguous_type(size_t block_size)
{
    int max_size = utils::mpi_max_array_length<char>();
    assert(is_castable_to<int>(block_size / static_cast<size_t>(max_size)));
    int num_max_io_chunk_sizes = block_size / max_size;
    int remaining_size         = block_size % max_size;

    MPI_Datatype max_size_type;
    MPI_Type_contiguous(max_size, MPI_CHAR, &max_size_type);
    MPI_Type_commit(&max_size_type);

    MPI_Datatype remaining_size_type;
    MPI_Type_contiguous(remaining_size, MPI_CHAR, &remaining_size_type);
    MPI_Type_commit(&remaining_size_type);

    MPI_Datatype contiguous_type;
    int block_lengths[]      = {num_max_io_chunk_sizes, 1};
    MPI_Aint displacements[] = {0, num_max_io_chunk_sizes * max_size};
    MPI_Datatype datatypes[] = {max_size_type, remaining_size_type};
    MPI_Type_create_struct(
        2, block_lengths, displacements, datatypes, &contiguous_type);
    MPI_Type_commit(&contiguous_type);

    MPI_Type_free(&remaining_size_type);
    MPI_Type_free(&max_size_type);

    return contiguous_type;
}

/// @brief Set a view on a file so that a read of blocks separated by
///        `(stride - block)` bytes appears contiguous.
///
/// This uses MPI views to read / write data in a strided fashion.
/// Used with IO_driver_mpi::Implementation::read_strided and
/// IO_driver_mpi::Implementation::write_strided
///
/// @param[in] file              The MPI-IO file handle
/// @param[in] offset            The offset in bytes from the start of the file
/// @param[in] block_size        The block_size size in bytes
/// @param[in] stride_size       The stride size in bytes block_size start to
///                              block_size start
/// @param[in] number_of_blocks  The number of blocks
/// @param[in] file_name         The name of the file (for logging)
///
/// @return The datatype which was used to create a view
///
MPI_Datatype IO_driver_mpi_detail::create_strided_view(
    size_t offset,
    size_t block_size,
    size_t stride_size,
    size_t number_of_blocks)
{
    static const char* function_name =
        "exseis::piol::io_driver::IO_driver_mpi_detail::create_strided_view";

    // Check conversions are ok
    assert(is_castable_to<int>(number_of_blocks));
    assert(is_castable_to<MPI_Offset>(offset));
    assert(is_castable_to<MPI_Aint>(stride_size));

    int err = 0;

    MPI_Datatype block_type = create_contiguous_type(block_size);

    MPI_Datatype view = MPI_DATATYPE_NULL;

    err = MPI_Type_create_hvector(
        static_cast<int>(number_of_blocks), 1,
        static_cast<MPI_Aint>(stride_size), block_type, &view);
    if (err != MPI_SUCCESS) {
        log_error(
            "MPI_Type_create_hvector failure", err, MPI_STATUS_IGNORE,
            EXSEISDAT_SOURCE_POSITION(function_name));
    }

    err = MPI_Type_commit(&view);
    if (err != MPI_SUCCESS) {
        log_error(
            "MPI_Type_commit failure", err, MPI_STATUS_IGNORE,
            EXSEISDAT_SOURCE_POSITION(function_name));
    }

    MPI_Type_free(&block_type);

    err = MPI_File_set_view(
        m_file, static_cast<MPI_Offset>(offset), MPI_CHAR, view, "native",
        m_info);
    if (err != MPI_SUCCESS) {
        log_error(
            "MPI_File_set_view failure", err, MPI_STATUS_IGNORE,
            EXSEISDAT_SOURCE_POSITION(function_name));
    }

    return view;
}

/// Implementation for IO_driver_mpi::Implementation::read_strided and
/// IO_driver_mpi::Implementation::write_strided.
///
/// @tparam T The type of the buffer.
///           This is used to template the constness of the buffer.
/// @tparam MPIReadWrite The MPI I/O function type.
///                      The MPI I/O read and write functions have the same
///                      types, except for the constness of the buffer
///                      parameter. This is used to template that constness.
///
/// @param[in]     offset           The offset in bytes into the file.
/// @param[in]     block_size       The size of the blocks, in bytes, to read
///                                 from, or write to the file.
/// @param[in]     stride_size      The distance, in bytes, from the start of
///                                 one block to the start of the next in
///                                 the file.
/// @param[in]     number_of_blocks The number of blocks to read from, or write
///                                 to the file.
/// @param[in,out] buffer           The buffer to read into or write from.
/// @param[in]     mpi_read_write   The MPI I/O function to use for reading or
///                                 writing.
/// @param[in]     m_max_io_chunk_size          IO_driver_mpi::Implementation::m_max_size
/// @param[in]     m_info              IO_driver_mpi::Implementation::m_info
/// @param[in]     function_name The name of the function to report during
///                              logging.
template<typename T, typename MPIReadWrite>
void IO_driver_mpi_detail::read_write_strided_impl(
    size_t offset,
    size_t block_size,
    size_t stride_size,
    size_t number_of_blocks,
    T&& buffer,
    MPIReadWrite&& mpi_read_write,
    size_t m_max_io_chunk_size,
    const char* function_name)
{
    assert(stride_size >= block_size);

    // Special case when block_size >= m_max_io_chunk_size,
    // access each block individually using read_write_impl
    size_t global_max_block_size = block_size;
    MPI_Allreduce(
        MPI_IN_PLACE, &global_max_block_size, 1, utils::mpi_type<size_t>(),
        MPI_MAX, m_file_communicator);
    if (global_max_block_size >= m_max_io_chunk_size) {
        const auto stride_chunks =
            exseis::utils::mpi_utils::Safe_collective_block_chunks(
                1, number_of_blocks, 1, m_file_communicator);
        for (const auto stride_chunk : stride_chunks) {
            assert(stride_chunk.size == 1 || stride_chunk.size == 0);

            read_write_impl(
                offset + stride_chunk.start * stride_size,
                stride_chunk.size * block_size,
                std::forward<T>(buffer + stride_chunk.start * block_size),
                std::forward<MPIReadWrite>(mpi_read_write), m_max_io_chunk_size,
                function_name);
        }
        return;
    }

    // When block_size < m_max_io_chunk_size, use strided file views
    const auto stride_chunks =
        exseis::utils::mpi_utils::Safe_collective_block_chunks(
            stride_size, number_of_blocks, m_max_io_chunk_size,
            m_file_communicator);

    for (const auto stride_chunk : stride_chunks) {
        size_t offset_to_block_start =
            offset + stride_chunk.start * stride_size;

        // Set a view so that MPI_File_read... functions only see contiguous
        // data.
        MPI_Datatype view = create_strided_view(
            offset_to_block_start, block_size, stride_size, stride_chunk.size);

        assert(
            stride_chunk.start * block_size + stride_chunk.size * block_size
            <= number_of_blocks * block_size);
        assert(is_castable_to<int>(stride_chunk.size * block_size));
        assert(stride_chunk.size * block_size <= m_max_io_chunk_size);

        MPI_Status status;
        int err = mpi_read_write(
            m_file, 0, buffer + stride_chunk.start * block_size,
            stride_chunk.size * block_size, MPI_CHAR, &status);

        if (err != MPI_SUCCESS) {
            log_error(
                "MPI_File_(read|write)_at* error", err, &status,
                EXSEISDAT_SOURCE_POSITION(function_name));
        }

        // Reset the view.
        free_and_reset_view(view);
    }
}

}  // namespace


void IO_driver_mpi::Implementation::read_strided(
    size_t offset,
    size_t block_size,
    size_t stride_size,
    size_t number_of_blocks,
    void* buffer) const
{
    static const char* function_name =
        "exseis::piol::io_driver::IO_driver_mpi::Implementation::read_strided";

    auto detail = IO_driver_mpi_detail{m_log, m_file_name, m_file_communicator,
                                       m_file, m_info};

    const auto mpi_read =
        m_use_collective_operations ? MPI_File_read_at_all : MPI_File_read_at;

    detail.read_write_strided_impl(
        offset, block_size, stride_size, number_of_blocks,
        static_cast<char*>(buffer), mpi_read, m_max_io_chunk_size,
        function_name);
}

void IO_driver_mpi::Implementation::write_strided(
    size_t offset,
    size_t block_size,
    size_t stride_size,
    size_t number_of_blocks,
    const void* buffer)
{
    static const char* function_name =
        "exseis::piol::io_driver::IO_driver_mpi::Implementation::write_strided";

    auto detail = IO_driver_mpi_detail{m_log, m_file_name, m_file_communicator,
                                       m_file, m_info};

    const auto mpi_write =
        m_use_collective_operations ? MPI_File_write_at_all : MPI_File_write_at;

    detail.read_write_strided_impl(
        offset, block_size, stride_size, number_of_blocks,
        static_cast<const char*>(buffer), mpi_write, m_max_io_chunk_size,
        function_name);
}


namespace {

/// @brief Set a view on a file so that a read from a given list offsets appears
///        contiguous.
///
/// This uses MPI views to read / write data in a strided fashion.
/// Used with IO_driver_mpi::Implementation::read_offsets and
/// IO_driver_mpi::Implementation::write_offsets
///
/// @param[in] file              The MPI-IO file handle
/// @param[in] offsets           An array of offsets in bytes from the start of
///                              the file.
///                              (pointer to array of size number_of_blocks)
/// @param[in] block_size        The block_size size in bytes
///                              block_size start
/// @param[in] number_of_blocks  The number of blocks
/// @param[in] file_name         The name of the file (for logging)
///
/// @return The datatype which was used to create a view
///
MPI_Datatype IO_driver_mpi_detail::create_listed_view(
    size_t number_of_blocks, size_t block_size, const MPI_Aint* offsets)
{
    static const char* function_name =
        "exseis::piol::io_driver::IO_driver_mpi_detail::create_listed_view";

    // Check offsets are sorted
    assert(std::is_sorted(offsets, offsets + number_of_blocks));

    // Check conversion to unsigned ok
    assert(is_castable_to<int>(number_of_blocks));

    // Check number_of_blocks is within the MPI view size limit
    assert(
        number_of_blocks
        < std::numeric_limits<int>::max() / (sizeof(int) + sizeof(MPI_Aint)));

    bool hindexed_block_works = ([] {
#ifndef HINDEXED_BLOCK_WORKS
        return false;
#else
        return true;
#endif
    }());


    MPI_Datatype view = MPI_DATATYPE_NULL;

    int err = 0;

    if (hindexed_block_works) {
        assert(is_castable_to<int>(block_size));
        err = MPI_Type_create_hindexed_block(
            static_cast<int>(number_of_blocks), static_cast<int>(block_size),
            offsets, MPI_CHAR, &view);
        if (err != MPI_SUCCESS) {
            log_error(
                "MPI_Type_create_hindexed_block error", err, MPI_STATUS_IGNORE,
                EXSEISDAT_SOURCE_POSITION(function_name));
        }
    }
    else {

        std::vector<int> bl(number_of_blocks);
        for (size_t i = 0; i < number_of_blocks; i++) {
            bl[i] = block_size;
        }

        err = MPI_Type_create_hindexed(
            static_cast<int>(number_of_blocks), bl.data(), offsets, MPI_CHAR,
            &view);
        if (err != MPI_SUCCESS) {
            log_error(
                "MPI_Type_create_hindexed error", err, MPI_STATUS_IGNORE,
                EXSEISDAT_SOURCE_POSITION(function_name));
        }
    }

    err = MPI_Type_commit(&view);
    if (err != MPI_SUCCESS) {
        log_error(
            "MPI_Type_commit error", err, MPI_STATUS_IGNORE,
            EXSEISDAT_SOURCE_POSITION(function_name));
    }

    err = MPI_File_set_view(m_file, 0, MPI_BYTE, view, "native", m_info);
    if (err != MPI_SUCCESS) {
        log_error(
            "MPI_File_set_view error", err, MPI_STATUS_IGNORE,
            EXSEISDAT_SOURCE_POSITION(function_name));
    }

    return view;
}

/// Implementation for IO_driver_mpi::Implementation::read_offsets and
/// IO_driver_mpi::Implementation::write_offsets.
///
/// @tparam T The type of the buffer.
///           This is used to template the constness of the buffer.
/// @tparam MPIReadWrite The MPI I/O function type.
///                      The MPI I/O read and write functions have the same
///                      types, except for the constness of the buffer
///                      parameter. This is used to template that constness.
///
/// @param[in]     block_size       The size of the blocks, in bytes, to read
///                                 from, or write to the file.
/// @param[in]     number_of_blocks The number of blocks to read from, or write
///                                 to the file.
/// @param[in]     offsets          The starting positions of the blocks to read
///                                 from or write to in the file.
/// @param[in,out] buffer           The buffer to read into or write from.
/// @param[in]     mpi_read_write   The MPI I/O function to use for reading or
///                                 writing.
/// @param[in]     m_max_io_chunk_size          IO_driver_mpi::Implementation::m_max_size
/// @param[in]     function_name The name of the function to report during
///                              logging.
template<typename T, typename MPIReadWrite>
void IO_driver_mpi_detail::read_write_offsets_impl(
    size_t block_size,
    size_t number_of_blocks,
    const size_t* offsets,
    T&& buffer,
    MPIReadWrite&& mpi_read_write,
    size_t m_max_io_chunk_size,
    const char* function_name)
{
    // Special case when block_size >= m_max_io_chunk_size,
    // access each block individually using read_write_impl
    size_t global_max_block_size = block_size;
    MPI_Allreduce(
        MPI_IN_PLACE, &global_max_block_size, 1, utils::mpi_type<size_t>(),
        MPI_MAX, m_file_communicator);
    if (global_max_block_size >= m_max_io_chunk_size) {
        const auto offset_chunks =
            exseis::utils::mpi_utils::Safe_collective_block_chunks(
                1, number_of_blocks, 1, m_file_communicator);

        for (const auto offset_chunk : offset_chunks) {
            assert(offset_chunk.size == 1 || offset_chunk.size == 0);

            const size_t offset =
                (offset_chunk.size == 0) ? 0 : offsets[offset_chunk.start];
            read_write_impl(
                offset, offset_chunk.size * block_size,
                std::forward<T>(buffer + offset_chunk.start * block_size),
                std::forward<MPIReadWrite>(mpi_read_write), m_max_io_chunk_size,
                function_name);
        }
        return;
    }

    // An array for storing the offsets as an MPI_Aint.
    std::vector<MPI_Aint> mpi_offsets;

    const auto block_chunks =
        exseis::utils::mpi_utils::Safe_collective_block_chunks(
            block_size * 2LU, number_of_blocks, m_max_io_chunk_size,
            m_file_communicator);

    for (const auto block_chunk : block_chunks) {
        mpi_offsets.assign(
            offsets + block_chunk.start,
            offsets + (block_chunk.start + block_chunk.size));

        // Set a view so that MPI_File_read... functions only see contiguous
        // data.
        MPI_Datatype view = create_listed_view(
            block_chunk.size, block_size, mpi_offsets.data());

        assert(is_castable_to<int>(block_chunk.size * block_size));
        // assert(block_chunk.size * block_size <= m_max_io_chunk_size);

        // read the data
        MPI_Status status;
        int err = mpi_read_write(
            m_file, 0, buffer + block_chunk.start * block_size,
            block_chunk.size * block_size, MPI_CHAR, &status);
        if (err != MPI_SUCCESS) {
            log_error(
                "MPI_File_(read|write)_at* error", err, &status,
                EXSEISDAT_SOURCE_POSITION(function_name));
        }

        // Reset the view.
        free_and_reset_view(view);
    }
}

}  // namespace


void IO_driver_mpi::Implementation::read_offsets(
    size_t block_size,
    size_t number_of_blocks,
    const size_t* offsets,
    void* buffer) const
{
    static const char* function_name =
        "exseis::piol::io_driver::IO_driver_mpi::Implementation::read_offsets";

    auto detail = IO_driver_mpi_detail{m_log, m_file_name, m_file_communicator,
                                       m_file, m_info};

    const auto mpi_read =
        m_use_collective_operations ? MPI_File_read_at_all : MPI_File_read_at;

    detail.read_write_offsets_impl(
        block_size, number_of_blocks, offsets, static_cast<char*>(buffer),
        mpi_read, m_max_io_chunk_size, function_name);
}

void IO_driver_mpi::Implementation::write_offsets(
    size_t block_size,
    size_t number_of_blocks,
    const size_t* offsets,
    const void* buffer)
{
    static const char* function_name =
        "exseis::piol::io_driver::IO_driver_mpi::Implementation::write_offsets";

    auto detail = IO_driver_mpi_detail{m_log, m_file_name, m_file_communicator,
                                       m_file, m_info};

    const auto mpi_write =
        m_use_collective_operations ? MPI_File_write_at_all : MPI_File_write_at;

    detail.read_write_offsets_impl(
        block_size, number_of_blocks, offsets, static_cast<const char*>(buffer),
        mpi_write, m_max_io_chunk_size, function_name);
}

void IO_driver_mpi::Implementation::sync()
{
    static const char* function_name =
        "exseis::piol::io_driver::IO_driver_mpi::Implementation::sync";

    auto detail = IO_driver_mpi_detail{m_log, m_file_name, m_file_communicator,
                                       m_file, m_info};

    int err = MPI_File_sync(m_file);

    if (err != MPI_SUCCESS) {
        detail.log_error(
            "MPI_File_(read|write)_at* error", err, MPI_STATUS_IGNORE,
            EXSEISDAT_SOURCE_POSITION(function_name));
    }
}

}  // namespace io_driver
}  // namespace piol
}  // namespace exseis
