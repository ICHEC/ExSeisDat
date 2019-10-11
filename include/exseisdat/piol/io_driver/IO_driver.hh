////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief   The Data layer interface
/// @details The Data layer interface is a base class which specific data I/O
///          implementations work off.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_IO_DRIVER_IO_DRIVER_HH
#define EXSEISDAT_PIOL_IO_DRIVER_IO_DRIVER_HH

#include "exseisdat/piol/configuration/ExSeisPIOL.hh"
#include "exseisdat/utils/types/typedefs.hh"

namespace exseis {
namespace piol {
inline namespace io_driver {

/// @brief The Data layer interface. Specific data I/O implementations
///        work off this base class.
///
class IO_driver {
  public:
    /// @name @special_member_functions
    /// @{

    /// @default_constructor{default}
    IO_driver() = default;
    /// @virtual_destructor
    virtual ~IO_driver() = default;

    /// @copy_constructor{delete}
    IO_driver(const IO_driver&) = delete;
    /// @copy_assignment{delete}
    IO_driver& operator=(const IO_driver&) = delete;

    /// @move_constructor{delete}
    IO_driver(IO_driver&&) = delete;
    /// @move_assignment{delete}
    IO_driver& operator=(IO_driver&&) = delete;

    /// @}


    /// @brief Test if the file is open.
    ///
    /// @return Returns \c true if the file is open, \c false otherwise.
    ///
    virtual bool is_open() const = 0;


    /// @brief Get the size of the file.
    ///
    /// @return The size of the file
    ///
    virtual size_t get_file_size() const = 0;


    /// @brief Set the size of the file, either by truncating or expanding it.
    ///
    /// @param[in] size The new size of the file.
    ///
    virtual void set_file_size(size_t size) const = 0;


    /// @brief Read a contiguous chunk of size \c size beginning a position \c
    ///        offset from the file into the buffer \c buffer.
    ///
    /// @param[in]  offset The file offset to start reading at
    /// @param[in]  size   The amount to read
    /// @param[out] buffer The buffer to read into
    ///                    (pointer to array of size \c size)
    ///
    virtual void read(size_t offset, size_t size, void* buffer) const = 0;


    /// @brief Write a contiguous chunk of size \c size beginning a position \c
    ///        offset from the buffer \c buffer into the file.
    ///
    /// @param[in]  offset The file offset to start writing at
    /// @param[in]  size   The amount to write
    /// @param[out] buffer The buffer to write from
    ///                    (pointer to array of size \c size)
    ///
    virtual void write(
        size_t offset, size_t size, const void* buffer) const = 0;


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
    virtual void read_noncontiguous(
        size_t offset,
        size_t block_size,
        size_t stride_size,
        size_t number_of_blocks,
        void* buffer) const = 0;


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
    virtual void write_noncontiguous(
        size_t offset,
        size_t block_size,
        size_t stride_size,
        size_t number_of_blocks,
        const void* buffer) const = 0;


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
    virtual void read_noncontiguous_irregular(
        size_t block_size,
        size_t number_of_blocks,
        const size_t* offsets,
        void* buffer) const = 0;


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
    virtual void write_noncontiguous_irregular(
        size_t block_size,
        size_t number_of_blocks,
        const size_t* offsets,
        const void* buffer) const = 0;
};

}  // namespace io_driver
}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_IO_DRIVER_IO_DRIVER_HH
