////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of \c MPI_Binary_file
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

/// @brief Create an error Log_entry.
///
/// This is an implementation detail of exseis::piol::MPI_Binary_file, to
/// provide a clear interface for logging.
///
/// @param[in] log        The Log object to add the error entry to.
/// @param[in] message    The error message to present to the user.
/// @param[in] file_name  The file being read/written when the error occurred.
/// @param[in] mpi_error  The error returned by MPI for the failed operation.
/// @param[in] mpi_status The status object set by MPI when the operation
///                       failed.
///                       (Pass MPI_STATUS_IGNORE if there is no status object)
/// @param[in] source_position A formatted string of the position in the source
///                            code to identify with the log.
///
void log_error(
    const std::shared_ptr<exseis::utils::Log>& log,
    std::string message,
    std::string file_name,
    int mpi_error,
    MPI_Status* mpi_status,
    exseis::utils::Source_position source_position)
{
    std::string formatted_message = std::move(message);

    if (!file_name.empty()) {
        formatted_message += " for file \"" + file_name + "\"";
    }

    formatted_message +=
        ": " + exseis::utils::mpi_error_to_string(mpi_error, mpi_status);

    using namespace exseis::utils::logging;

    log->add_entry(Log_entry{Status::Error, std::move(formatted_message),
                             exseis::utils::Verbosity::none,
                             std::move(source_position)});
}


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
bool is_castable_to(const From& from)
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
            // The magnitude of the smallest negative number in 2s complement
            // is larger than the largest number.
            // i.e. INT_MAX: 2147483647
            //      INT_MIN: -2147483648
            // => abs(INT_MIN) > INT_MAX.
            //
            // Therefore, we need to first make the magnitude of the negative
            // number representable (i.e. add 1), then negate it (i.e. `-`),
            // then cast it to size_t before we undo the original addition of
            // 1 (which, after negation etc. means we add 1 again.)
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


/// @brief Reset the MPI I/O view of a file and free the datatype used to
///        create it.
///
/// @param[in]     file      The MPI_File handle to the file to reset the view for.
/// @param[in]     info      The info object to use when resetting the view.
/// @param[in,out] view      The view object to destroy.
/// @param[in]     log       The logging object to log to on error.
/// @param[in]     file_name The file name to report to the logger.
///
void free_and_reset_view(
    MPI_File file,
    MPI_Info info,
    MPI_Datatype& view,
    const std::shared_ptr<exseis::utils::Log>& log,
    const std::string& file_name)
{
    static const char* function_name = "exseis::piol::mpi::free_and_reset_view";

    // Reset the view.
    int err = MPI_File_set_view(file, 0, MPI_CHAR, MPI_CHAR, "native", info);
    if (err != MPI_SUCCESS) {
        log_error(
            log, "MPI_File_set_view error", file_name, err, MPI_STATUS_IGNORE,
            EXSEISDAT_SOURCE_POSITION(function_name));
    }

    err = MPI_Type_free(&view);
    if (err != MPI_SUCCESS) {
        log_error(
            log, "MPI_Type_free error", file_name, err, MPI_STATUS_IGNORE,
            EXSEISDAT_SOURCE_POSITION(function_name));
    }

    view = MPI_DATATYPE_NULL;
}


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


IO_driver_mpi::Opt::Opt() : use_collective_operations(default_collectiveness())
{
    MPI_Info_create(&info);
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

void IO_driver_mpi::Opt::free()
{
    if (info != MPI_INFO_NULL) {
        MPI_Info_free(&info);
    }
}

IO_driver_mpi::Opt::~Opt()
{
    free();
}


IO_driver_mpi::IO_driver_mpi(
    std::shared_ptr<ExSeisPIOL> piol,
    std::string file_name,
    FileMode mode,
    const IO_driver_mpi::Opt& opt) :
    IO_driver_mpi(piol->log, file_name, mode, opt)
{
}

IO_driver_mpi::IO_driver_mpi(
    std::shared_ptr<exseis::utils::Log> log,
    std::string file_name,
    FileMode mode,
    const IO_driver_mpi::Opt& opt) :
    m_log(log),
    m_file_name(file_name),
    m_use_collective_operations(opt.use_collective_operations),
    m_file_communicator(opt.file_communicator),
    m_max_size(opt.max_size)
{
    static const char* function_name =
        "exseis::piol::mpi::MPI_Binary_file::MPI_Binary_file";

    MPI_Aint lb  = 0;
    MPI_Aint esz = 0;

    int err = MPI_SUCCESS;

    /// @todo WTF is this for? Should it be in a test program instead?
    err = MPI_Type_get_true_extent(MPI_CHAR, &lb, &esz);
    if (err != MPI_SUCCESS) {
        log_error(
            m_log, "Getting MPI extent error", "", err, MPI_STATUS_IGNORE,
            EXSEISDAT_SOURCE_POSITION(function_name));
    }

    if (esz != 1) {
        log_error(
            m_log, "MPI_CHAR extent is bigger than one", "", MPI_ERR_TYPE,
            MPI_STATUS_IGNORE, EXSEISDAT_SOURCE_POSITION(function_name));
    }

    if (opt.info != MPI_INFO_NULL) {
        err = MPI_Info_dup(opt.info, &m_info);

        if (err != MPI_SUCCESS) {
            log_error(
                m_log, "MPI_Info_dup error", m_file_name, err,
                MPI_STATUS_IGNORE, EXSEISDAT_SOURCE_POSITION(function_name));
        }
    }

    err = MPI_File_open(
        m_file_communicator, m_file_name.c_str(), mode, m_info, &m_file);
    if (err != MPI_SUCCESS) {
        log_error(
            m_log, "MPI_File_open error", m_file_name, err, MPI_STATUS_IGNORE,
            EXSEISDAT_SOURCE_POSITION(function_name));
    }

    err = MPI_File_set_view(m_file, 0, MPI_CHAR, MPI_CHAR, "native", m_info);
    if (err != MPI_SUCCESS) {
        log_error(
            m_log, "MPI_File_set_view error", m_file_name, err,
            MPI_STATUS_IGNORE, EXSEISDAT_SOURCE_POSITION(function_name));
    }
}

void IO_driver_mpi::close()
{
    static const char* function_name =
        "exseis::piol::mpi::MPI_Binary_file::close";

    if (m_file != MPI_FILE_NULL) {
        int err = MPI_File_close(&m_file);
        if (err != MPI_SUCCESS) {
            log_error(
                m_log, "MPI_File_close error", m_file_name, err,
                MPI_STATUS_IGNORE, EXSEISDAT_SOURCE_POSITION(function_name));
        }
    }

    if (m_info != MPI_INFO_NULL) {
        int err = MPI_Info_free(&m_info);
        if (err != MPI_SUCCESS) {
            log_error(
                m_log, "MPI_Info_free error", m_file_name, err,
                MPI_STATUS_IGNORE, EXSEISDAT_SOURCE_POSITION(function_name));
        }
    }
}

IO_driver_mpi::~IO_driver_mpi()
{
    close();
}


bool IO_driver_mpi::is_open() const
{
    return m_file == MPI_FILE_NULL;
}

size_t IO_driver_mpi::get_file_size() const
{
    static const char* function_name =
        "exseis::piol::mpi::MPI_Binary_file::get_file_size";

    MPI_Offset fsz = 0;
    int err        = MPI_File_get_size(m_file, &fsz);

    if (err != MPI_SUCCESS) {
        log_error(
            m_log, "MPI_File_get_size error", m_file_name, err,
            MPI_STATUS_IGNORE, EXSEISDAT_SOURCE_POSITION(function_name));
    }

    MPI_Offset max_fsz = 0;
    MPI_Allreduce(
        &fsz, &max_fsz, 1, exseis::utils::mpi_type<MPI_Offset>(), MPI_MAX,
        m_file_communicator);


    assert(is_castable_to<size_t>(max_fsz));

    return static_cast<size_t>(max_fsz);
}

void IO_driver_mpi::set_file_size(size_t sz) const
{
    static const char* function_name =
        "exseis::piol::mpi::MPI_Binary_file::set_file_size";

    assert(is_castable_to<MPI_Offset>(sz));

    int err = MPI_File_set_size(m_file, MPI_Offset(sz));

    if (err != MPI_SUCCESS) {
        log_error(
            m_log, "MPI_File_set_size error", m_file_name, err,
            MPI_STATUS_IGNORE, EXSEISDAT_SOURCE_POSITION(function_name));
    }
}


namespace {

/// Implementation for MPI_Binary_file::read and MPI_Binary_file::write
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
/// @param[in]     m_max_size          MPI_Binary_file::m_max_size
/// @param[in]     m_file_communicator MPI_Binary_file::m_file_communicator
/// @param[in]     m_file              MPI_Binary_file::m_file
/// @param[in]     m_log               MPI_Binary_file::m_log
/// @param[in]     m_file_name         MPI_Binary_file::m_file_name
/// @param[in]     function_name The name of the function to report during
///                              logging.
template<typename T, typename MPIReadWrite>
void read_write_impl(
    size_t offset,
    size_t size,
    T* buffer,
    MPIReadWrite mpi_read_write,
    size_t m_max_size,
    MPI_Comm m_file_communicator,
    MPI_File m_file,
    const std::shared_ptr<exseis::utils::Log>& m_log,
    const std::string& m_file_name,
    const char* function_name)
{

    const size_t stride_size = 1;
    const size_t block_size  = 1;

    const auto block_chunks =
        exseis::utils::mpi_utils::Safe_collective_block_chunks(
            stride_size, size, m_max_size, m_file_communicator);

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
                m_log, "MPI_File_set_size error", m_file_name, err,
                MPI_STATUS_IGNORE, EXSEISDAT_SOURCE_POSITION(function_name));
        }
    }
}

}  // namespace


void IO_driver_mpi::read(size_t offset, size_t size, void* buffer) const
{
    static const char* function_name =
        "exseis::piol::mpi::MPI_Binary_file::read";

    const auto mpi_read =
        m_use_collective_operations ? MPI_File_read_at_all : MPI_File_read_at;

    read_write_impl(
        offset, size, static_cast<unsigned char*>(buffer), mpi_read, m_max_size,
        m_file_communicator, m_file, m_log, m_file_name, function_name);
}

void IO_driver_mpi::write(size_t offset, size_t size, const void* buffer) const
{
    static const char* function_name =
        "exseis::piol::mpi::MPI_Binary_file::write";

    const auto mpi_write =
        m_use_collective_operations ? MPI_File_write_at_all : MPI_File_write_at;

    read_write_impl(
        offset, size, static_cast<const unsigned char*>(buffer), mpi_write,
        m_max_size, m_file_communicator, m_file, m_log, m_file_name,
        function_name);
}


namespace {

/// @brief Set a view on a file so that a read of blocks separated by
///        `(stride - block)` bytes appears contiguous.
///
/// This uses MPI views to read / write data in a strided fashion.
/// Used with MPI_Binary_file::read_noncontiguous and
/// MPI_Binary_file::write_noncontiguous
///
/// @param[in] file              The MPI-IO file handle
/// @param[in] info              The info structure to use
/// @param[in] offset            The offset in bytes from the start of the file
/// @param[in] block_size        The block_size size in bytes
/// @param[in] stride_size       The stride size in bytes block_size start to
///                              block_size start
/// @param[in] number_of_blocks  The number of blocks
/// @param[in] log               The logging object
/// @param[in] file_name         The name of the file (for logging)
///
/// @return The datatype which was used to create a view
///
MPI_Datatype create_strided_view(
    MPI_File file,
    MPI_Info info,
    size_t offset,
    size_t block_size,
    size_t stride_size,
    size_t number_of_blocks,
    const std::shared_ptr<exseis::utils::Log>& log,
    const std::string& file_name)
{
    static const char* function_name = "exseis::piol::mpi::create_strided_view";

    // Check conversions are ok
    assert(is_castable_to<int>(block_size));
    assert(is_castable_to<int>(number_of_blocks));
    assert(is_castable_to<MPI_Offset>(offset));
    assert(is_castable_to<MPI_Aint>(stride_size));

    int err = 0;

    MPI_Datatype view = MPI_DATATYPE_NULL;

    err = MPI_Type_create_hvector(
        static_cast<int>(number_of_blocks), static_cast<int>(block_size),
        static_cast<MPI_Aint>(stride_size), MPI_CHAR, &view);
    if (err != MPI_SUCCESS) {
        log_error(
            log, "MPI_Type_create_hvector failure", file_name, err,
            MPI_STATUS_IGNORE, EXSEISDAT_SOURCE_POSITION(function_name));
    }

    err = MPI_Type_commit(&view);
    if (err != MPI_SUCCESS) {
        log_error(
            log, "MPI_Type_commit failure", file_name, err, MPI_STATUS_IGNORE,
            EXSEISDAT_SOURCE_POSITION(function_name));
    }

    err = MPI_File_set_view(
        file, static_cast<MPI_Offset>(offset), MPI_CHAR, view, "native", info);
    if (err != MPI_SUCCESS) {
        log_error(
            log, "MPI_File_set_view failure", file_name, err, MPI_STATUS_IGNORE,
            EXSEISDAT_SOURCE_POSITION(function_name));
    }

    return view;
}

/// Implementation for MPI_Binary_file::read_noncontiguous and
/// MPI_Binary_file::write_noncontiguous.
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
///                                 one block to the start of the next in the
///                                 file.
/// @param[in]     number_of_blocks The number of blocks to read from, or write
///                                 to the file.
/// @param[in,out] buffer           The buffer to read into or write from.
/// @param[in]     mpi_read_write   The MPI I/O function to use for reading or
///                                 writing.
/// @param[in]     m_max_size          MPI_Binary_file::m_max_size
/// @param[in]     m_file_communicator MPI_Binary_file::m_file_communicator
/// @param[in]     m_file              MPI_Binary_file::m_file
/// @param[in]     m_info              MPI_Binary_file::m_info
/// @param[in]     m_log               MPI_Binary_file::m_log
/// @param[in]     m_file_name         MPI_Binary_file::m_file_name
/// @param[in]     function_name The name of the function to report during
///                              logging.
template<typename T, typename MPIReadWrite>
void read_write_noncontiguous_impl(
    size_t offset,
    size_t block_size,
    size_t stride_size,
    size_t number_of_blocks,
    T* buffer,
    MPIReadWrite mpi_read_write,
    size_t m_max_size,
    MPI_Comm m_file_communicator,
    MPI_File m_file,
    MPI_Info m_info,
    const std::shared_ptr<exseis::utils::Log>& m_log,
    const std::string& m_file_name,
    const char* function_name)
{
    const auto block_chunks =
        exseis::utils::mpi_utils::Safe_collective_block_chunks(
            stride_size, number_of_blocks, m_max_size, m_file_communicator);

    for (const auto block_chunk : block_chunks) {
        size_t offset_to_block_start = offset + block_chunk.start * stride_size;

        // Set a view so that MPI_File_read... functions only see contiguous
        // data.
        MPI_Datatype view = create_strided_view(
            m_file, m_info, offset_to_block_start, block_size, stride_size,
            block_chunk.size, m_log, m_file_name);

        MPI_Status status;
        int err = mpi_read_write(
            m_file, 0, buffer, block_chunk.size * block_size, MPI_CHAR,
            &status);
        if (err != MPI_SUCCESS) {
            log_error(
                m_log, "MPI_File_read_at* error", m_file_name, err, &status,
                EXSEISDAT_SOURCE_POSITION(function_name));
        }

        // Reset the view.
        free_and_reset_view(m_file, m_info, view, m_log, m_file_name);
    }
}

}  // namespace


void IO_driver_mpi::read_noncontiguous(
    size_t offset,
    size_t block_size,
    size_t stride_size,
    size_t number_of_blocks,
    void* buffer) const
{
    static const char* function_name =
        "exseis::piol::mpi::MPI_Binary_file::read_noncontiguous";

    const auto mpi_read =
        m_use_collective_operations ? MPI_File_read_at_all : MPI_File_read_at;

    read_write_noncontiguous_impl(
        offset, block_size, stride_size, number_of_blocks, buffer, mpi_read,
        m_max_size, m_file_communicator, m_file, m_info, m_log, m_file_name,
        function_name);
}

void IO_driver_mpi::write_noncontiguous(
    size_t offset,
    size_t block_size,
    size_t stride_size,
    size_t number_of_blocks,
    const void* buffer) const
{
    static const char* function_name =
        "exseis::piol::mpi::MPI_Binary_file::write_noncontiguous";

    const auto mpi_write =
        m_use_collective_operations ? MPI_File_write_at_all : MPI_File_write_at;

    read_write_noncontiguous_impl(
        offset, block_size, stride_size, number_of_blocks, buffer, mpi_write,
        m_max_size, m_file_communicator, m_file, m_info, m_log, m_file_name,
        function_name);
}


namespace {

/// @brief Set a view on a file so that a read from a given list offsets appears
///        contiguous.
///
/// This uses MPI views to read / write data in a strided fashion.
/// Used with MPI_Binary_file::read_noncontiguous_irregular and
/// MPI_Binary_file::write_noncontiguous_irregular
///
/// @param[in] file              The MPI-IO file handle
/// @param[in] info              The info structure to use
/// @param[in] offsets           An array of offsets in bytes from the start of
///                              the file.
///                              (pointer to array of size number_of_blocks)
/// @param[in] block_size        The block_size size in bytes
///                              block_size start
/// @param[in] number_of_blocks  The number of blocks
/// @param[in] log               The logging object
/// @param[in] file_name         The name of the file (for logging)
///
/// @return The datatype which was used to create a view
///
MPI_Datatype create_listed_view(
    MPI_File file,
    MPI_Info info,
    size_t number_of_blocks,
    size_t block_size,
    const MPI_Aint* offsets,
    const std::shared_ptr<exseis::utils::Log>& log,
    const std::string& file_name)
{
    static const char* function_name = "exseis::piol::mpi::create_listed_view";

    // Check conversion to unsigned ok
    assert(is_castable_to<int>(number_of_blocks));
    assert(is_castable_to<int>(block_size));

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
        err = MPI_Type_create_hindexed_block(
            static_cast<int>(number_of_blocks), static_cast<int>(block_size),
            offsets, MPI_CHAR, &view);
        if (err != MPI_SUCCESS) {
            log_error(
                log, "MPI_Type_create_hindexed_block error", file_name, err,
                MPI_STATUS_IGNORE, EXSEISDAT_SOURCE_POSITION(function_name));
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
                log, "MPI_Type_create_hindexed error", file_name, err,
                MPI_STATUS_IGNORE, EXSEISDAT_SOURCE_POSITION(function_name));
        }
    }

    err = MPI_Type_commit(&view);
    if (err != MPI_SUCCESS) {
        log_error(
            log, "MPI_Type_commit error", file_name, err, MPI_STATUS_IGNORE,
            EXSEISDAT_SOURCE_POSITION(function_name));
    }

    err = MPI_File_set_view(file, 0, MPI_BYTE, view, "native", info);
    if (err != MPI_SUCCESS) {
        log_error(
            log, "MPI_File_set_view error", file_name, err, MPI_STATUS_IGNORE,
            EXSEISDAT_SOURCE_POSITION(function_name));
    }

    return view;
}

/// Implementation for MPI_Binary_file::read_noncontiguous_irregular and
/// MPI_Binary_file::write_noncontiguous_irregular.
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
/// @param[in]     m_max_size          MPI_Binary_file::m_max_size
/// @param[in]     m_file_communicator MPI_Binary_file::m_file_communicator
/// @param[in]     m_file              MPI_Binary_file::m_file
/// @param[in]     m_info              MPI_Binary_file::m_info
/// @param[in]     m_log               MPI_Binary_file::m_log
/// @param[in]     m_file_name         MPI_Binary_file::m_file_name
/// @param[in]     function_name The name of the function to report during
///                              logging.
template<typename T, typename MPIReadWrite>
void read_write_noncontiguous_irregular_impl(
    size_t block_size,
    size_t number_of_blocks,
    const size_t* offsets,
    T* buffer,
    MPIReadWrite mpi_read_write,
    size_t m_max_size,
    MPI_Comm m_file_communicator,
    MPI_File m_file,
    MPI_Info m_info,
    const std::shared_ptr<exseis::utils::Log>& m_log,
    const std::string& m_file_name,
    const char* function_name)
{
    // An array for storing the offsets as an MPI_Aint.
    std::vector<MPI_Aint> mpi_offsets;

    const auto block_chunks =
        exseis::utils::mpi_utils::Safe_collective_block_chunks(
            (block_size != 0 ? block_size * 2LU : 1LU), number_of_blocks,
            m_max_size, m_file_communicator);

    for (const auto block_chunk : block_chunks) {
        mpi_offsets.assign(
            offsets + block_chunk.start,
            offsets + (block_chunk.start + block_chunk.size));

        // Set a view so that MPI_File_read... functions only see contiguous
        // data.
        MPI_Datatype view = create_listed_view(
            m_file, m_info, block_chunk.size, block_size, mpi_offsets.data(),
            m_log, m_file_name);

        // read the data
        MPI_Status status;
        int err = mpi_read_write(
            m_file, 0, buffer, block_chunk.size * block_size, MPI_CHAR,
            &status);
        if (err != MPI_SUCCESS) {
            log_error(
                m_log, "MPI_File_read_at* error", m_file_name, err, &status,
                EXSEISDAT_SOURCE_POSITION(function_name));
        }

        // Reset the view.
        free_and_reset_view(m_file, m_info, view, m_log, m_file_name);
    }
}

}  // namespace


void IO_driver_mpi::read_noncontiguous_irregular(
    size_t block_size,
    size_t number_of_blocks,
    const size_t* offsets,
    void* buffer) const
{
    static const char* function_name =
        "exseis::piol::mpi::MPI_Binary_file::read_noncontiguous_irregular";

    const auto mpi_read =
        m_use_collective_operations ? MPI_File_read_at_all : MPI_File_read_at;

    read_write_noncontiguous_irregular_impl(
        block_size, number_of_blocks, offsets, buffer, mpi_read, m_max_size,
        m_file_communicator, m_file, m_info, m_log, m_file_name, function_name);
}

void IO_driver_mpi::write_noncontiguous_irregular(
    size_t block_size,
    size_t number_of_blocks,
    const size_t* offsets,
    const void* buffer) const
{
    static const char* function_name =
        "exseis::piol::mpi::MPI_Binary_file::write_noncontiguous_irregular";

    const auto mpi_write =
        m_use_collective_operations ? MPI_File_write_at_all : MPI_File_write_at;

    read_write_noncontiguous_irregular_impl(
        block_size, number_of_blocks, offsets, buffer, mpi_write, m_max_size,
        m_file_communicator, m_file, m_info, m_log, m_file_name, function_name);
}

}  // namespace io_driver
}  // namespace piol
}  // namespace exseis
