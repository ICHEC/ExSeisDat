////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief   The IO_driver interface
/// @details The IO_driver interface defines the minimal I/O operations
///          implementations should provide.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_IO_DRIVER_IO_DRIVER_HH
#define EXSEISDAT_PIOL_IO_DRIVER_IO_DRIVER_HH

#include "exseisdat/piol/configuration/ExSeisPIOL.hh"
#include "exseisdat/utils/types/typedefs.hh"

#include <memory>
#include <utility>

namespace exseis {
namespace piol {
inline namespace io_driver {

/// @brief Perform I/O on an underlying resource.
///
/// Specific I/O implementations, e.g. using MPI-IO for parallel IO, will
/// inherit from this class and implement the specified operations, e.g.
/// reading & writing contiguous blocks, reading & writing strided blocks,
/// and reading & writing blocks from a list of offsets.
///
class IO_driver {
  protected:
    class Implementation;

    /// @brief     A pointer to the underlying polymorphic implementation.
    ///
    /// @invariant Any parent or child in the inheritance chain of IO_driver
    ///            can safely static_cast m_implementation.get() to their
    ///            locally defined Implementation class.
    std::unique_ptr<IO_driver::Implementation> m_implementation = nullptr;

  public:
    /// @brief Test if the file is open.
    ///
    /// @return Returns \c true if the file is open, \c false otherwise.
    ///
    bool is_open() const { return m_implementation->is_open(); }


    /// @brief Get the size of the file.
    ///
    /// @return The size of the file
    ///
    size_t get_file_size() const { return m_implementation->get_file_size(); }


    /// @brief Set the size of the file, either by truncating or expanding it.
    ///
    /// @param[in] size The new size of the file.
    ///
    void set_file_size(size_t size) { m_implementation->set_file_size(size); }


    /// @brief Read a contiguous chunk of size \c size beginning a position \c
    ///        offset from the file into the buffer \c buffer.
    ///
    /// @param[in]  offset The file offset to start reading at
    /// @param[in]  size   The amount to read
    /// @param[out] buffer The buffer to read into
    ///                    (pointer to array of size \c size)
    ///
    /// @pre `offset + size <= get_file_size()`
    ///
    void read(size_t offset, size_t size, void* buffer) const
    {
        m_implementation->read(offset, size, buffer);
    }


    /// @brief Write a contiguous chunk of size \c size beginning a position \c
    ///        offset from the buffer \c buffer into the file.
    ///
    /// @param[in]  offset The file offset to start writing at
    /// @param[in]  size   The amount to write
    /// @param[out] buffer The buffer to write from
    ///                    (pointer to array of size \c size)
    ///
    /// @pre `offset + size <= get_file_size()`
    ///
    void write(size_t offset, size_t size, const void* buffer)
    {
        m_implementation->write(offset, size, buffer);
    }


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
    /// @pre `offset + number_of_blocks * stride_size <= get_file_size()`
    ///
    void read_strided(
        size_t offset,
        size_t block_size,
        size_t stride_size,
        size_t number_of_blocks,
        void* buffer) const
    {
        m_implementation->read_strided(
            offset, block_size, stride_size, number_of_blocks, buffer);
    }


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
    /// @pre `offset + number_of_blocks * stride_size <= get_file_size()`
    ///
    void write_strided(
        size_t offset,
        size_t block_size,
        size_t stride_size,
        size_t number_of_blocks,
        const void* buffer)
    {
        m_implementation->write_strided(
            offset, block_size, stride_size, number_of_blocks, buffer);
    }


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
    /// @pre `max(offsets) + block_size <= get_file_size()`
    ///
    void read_offsets(
        size_t block_size,
        size_t number_of_blocks,
        const size_t* offsets,
        void* buffer) const
    {
        m_implementation->read_offsets(
            block_size, number_of_blocks, offsets, buffer);
    }


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
    /// @pre `max(offsets) + block_size <= get_file_size()`
    ///
    void write_offsets(
        size_t block_size,
        size_t number_of_blocks,
        const size_t* offsets,
        const void* buffer)
    {
        m_implementation->write_offsets(
            block_size, number_of_blocks, offsets, buffer);
    }

    /// @brief Make writes visible to all processes
    void sync() { m_implementation->sync(); }

    /// @brief  Destructively returns the underlying implementation
    /// @return The underlying implementation
    std::unique_ptr<Implementation> implementation() &&
    {
        return std::move(m_implementation);
    }

  protected:
    /// @brief Polymorphic Implementation interface for IO_driver.
    ///
    /// The IO_driver class follows a bridge pattern style to
    /// allow developers to use polymorphic inheritance for implementation,
    /// while allowing users to use IO_driver as a regular type.
    ///
    /// @note To fulfill the invariant on IO_driver::m_implementation, any
    ///       classes deriving from IO_driver or a child of IO_driver should
    ///       also have their Implementation class inherit from their direct
    ///       parent's Implementation class.
    ///
    class Implementation {
      public:
        /// @virtual_destructor
        virtual ~Implementation() = default;

        /// @copydoc IO_driver::is_open
        virtual bool is_open() const = 0;

        /// @copydoc IO_driver::get_file_size
        virtual size_t get_file_size() const = 0;

        /// @copydoc IO_driver::set_file_size
        virtual void set_file_size(size_t size) = 0;

        /// @copydoc IO_driver::read
        virtual void read(size_t offset, size_t size, void* buffer) const = 0;

        /// @copydoc IO_driver::write
        virtual void write(size_t offset, size_t size, const void* buffer) = 0;

        /// @copydoc IO_driver::read_strided
        virtual void read_strided(
            size_t offset,
            size_t block_size,
            size_t stride_size,
            size_t number_of_blocks,
            void* buffer) const = 0;

        /// @copydoc IO_driver::write_strided
        virtual void write_strided(
            size_t offset,
            size_t block_size,
            size_t stride_size,
            size_t number_of_blocks,
            const void* buffer) = 0;

        /// @copydoc IO_driver::read_offsets
        virtual void read_offsets(
            size_t block_size,
            size_t number_of_blocks,
            const size_t* offsets,
            void* buffer) const = 0;

        /// @copydoc IO_driver::read_offsets
        virtual void write_offsets(
            size_t block_size,
            size_t number_of_blocks,
            const size_t* offsets,
            const void* buffer) = 0;

        /// @copydoc IO_driver::sync
        virtual void sync() = 0;
    };

    /// @brief For Derived classes to initialize the implementation
    ///
    /// @param implementation The derived instance of IO_driver::Implementation
    ///
    IO_driver(std::unique_ptr<IO_driver::Implementation> implementation) :
        m_implementation{std::move(implementation)}
    {
    }
};

}  // namespace io_driver
}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_IO_DRIVER_IO_DRIVER_HH
