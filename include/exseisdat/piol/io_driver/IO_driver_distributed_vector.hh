////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief   An in-memory IO_driver
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_IO_DRIVER_IO_DRIVER_DISTRIBUTED_VECTOR_HH
#define EXSEISDAT_PIOL_IO_DRIVER_IO_DRIVER_DISTRIBUTED_VECTOR_HH

#include "exseisdat/piol/io_driver/IO_driver.hh"
#include "exseisdat/utils/distributed_vector/Distributed_vector.hh"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <numeric>

namespace exseis {
namespace piol {
inline namespace io_driver {

/// @brief A specialization of a \ref IO_driver for storage using a
///        Distributed_vector.
///
class IO_driver_distributed_vector : public IO_driver {
  public:
    /// The type to use for the underlying storage.
    using Distributed_vector = exseis::utils::Distributed_vector<unsigned char>;

  private:
    class Implementation : public IO_driver::Implementation {
      public:
        /// An instance of the underlying storage.
        Distributed_vector m_distributed_vector;

        Implementation(Distributed_vector distributed_vector) :
            m_distributed_vector(std::move(distributed_vector))
        {
        }


        bool is_open() const override
        {
            return m_distributed_vector.size() != 0;
        }


        size_t get_file_size() const override
        {
            return m_distributed_vector.size();
        }


        void set_file_size(size_t size) override
        {
            m_distributed_vector.resize(size);
        }


        void read(size_t offset, size_t size, void* buffer) const override
        {
            assert(offset + size <= get_file_size());

            m_distributed_vector.get_n(
                offset, size, static_cast<unsigned char*>(buffer));
        }


        void write(size_t offset, size_t size, const void* buffer) override
        {
            assert(offset + size <= get_file_size());

            m_distributed_vector.set_n(
                offset, size, static_cast<const unsigned char*>(buffer));
        }


        void read_strided(
            size_t offset,
            size_t block_size,
            size_t stride_size,
            size_t number_of_blocks,
            void* buffer) const override
        {
            assert(offset + number_of_blocks * stride_size <= get_file_size());

            for (size_t block_i = 0; block_i < number_of_blocks; block_i++) {
                m_distributed_vector.get_n(
                    offset + block_i * stride_size, block_size,
                    static_cast<unsigned char*>(buffer) + block_i * block_size);
            }
        }


        void write_strided(
            size_t offset,
            size_t block_size,
            size_t stride_size,
            size_t number_of_blocks,
            const void* buffer) override
        {
            assert(offset + number_of_blocks * stride_size <= get_file_size());

            for (size_t block_i = 0; block_i < number_of_blocks; block_i++) {
                m_distributed_vector.set_n(
                    offset + block_i * stride_size, block_size,
                    static_cast<const unsigned char*>(buffer)
                        + block_i * block_size);
            }
        }


        void read_offsets(
            size_t block_size,
            size_t number_of_blocks,
            const size_t* offsets,
            void* buffer) const override
        {
            assert(
                number_of_blocks == 0
                || (*std::max_element(offsets, offsets + number_of_blocks)
                        + block_size
                    <= get_file_size()));

            for (size_t offset_i = 0; offset_i < number_of_blocks; offset_i++) {
                m_distributed_vector.get_n(
                    offsets[offset_i], block_size,
                    static_cast<unsigned char*>(buffer)
                        + offset_i * block_size);
            }
        }


        void write_offsets(
            size_t block_size,
            size_t number_of_blocks,
            const size_t* offsets,
            const void* buffer) override
        {
            assert(
                number_of_blocks == 0
                || (*std::max_element(offsets, offsets + number_of_blocks)
                        + block_size
                    <= get_file_size()));

            for (size_t offset_i = 0; offset_i < number_of_blocks; offset_i++) {
                m_distributed_vector.set_n(
                    offsets[offset_i], block_size,
                    static_cast<const unsigned char*>(buffer)
                        + offset_i * block_size);
            }
        }

        void sync() override { m_distributed_vector.sync(); }
    };


  public:
    /// @brief Build an  from an instance of Distributed_vector.
    ///
    /// @param[in] distributed_vector The underlying Distributed_vector
    ///
    IO_driver_distributed_vector(Distributed_vector distributed_vector) :
        IO_driver(std::unique_ptr<IO_driver::Implementation>{
            std::make_unique<IO_driver_distributed_vector::Implementation>(
                std::move(distributed_vector))})
    {
    }

    /// @}


    /// @brief Get the underlying Distributed_vector.
    /// @returns The underlying Distributed_vector.
    const Distributed_vector& distributed_vector() const&
    {
        return static_cast<const IO_driver_distributed_vector::Implementation*>(
                   m_implementation.get())
            ->m_distributed_vector;
    }

    /// @brief   Take the underlying Distributed_vector destructively
    /// @returns The underlying Distributed_vector
    Distributed_vector distributed_vector() &&
    {
        auto& distributed_vector =
            static_cast<IO_driver_distributed_vector::Implementation*>(
                m_implementation.get())
                ->m_distributed_vector;
        return std::move(distributed_vector);
    }
};

}  // namespace io_driver
}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_IO_DRIVER_IO_DRIVER_DISTRIBUTED_VECTOR_HH
