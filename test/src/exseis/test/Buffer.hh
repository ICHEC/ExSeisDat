#ifndef EXSEIS_TEST_BUFFER_HH
#define EXSEIS_TEST_BUFFER_HH

#include "exseis/utils/communicator/Communicator.hh"

#include <memory>
#include <new>
#include <utility>

#include <cstdint>

namespace exseis {
namespace test {

// Define a buffer which tracks global memory usage, and doesn't exceed a
// global max.

constexpr size_t global_max_buffer_size = size_t{1024} * 1024 * 1024 * 3;

inline size_t& total_global_buffer_usage()
{
    static size_t m_total_global_buffer_usage = 0;
    return m_total_global_buffer_usage;
}

inline size_t remaining_global_buffer_space()
{
    return global_max_buffer_size - total_global_buffer_usage();
}

class Tracked_buffer_usage {
    size_t m_global_size = 0;

  public:
    Tracked_buffer_usage(
        const exseis::Communicator& communicator, size_t local_size = 0)
    {
        const size_t requested_size = communicator.sum(local_size);

        if (total_global_buffer_usage() + requested_size
            > global_max_buffer_size) {
            throw std::bad_alloc();
        }

        m_global_size = requested_size;
        total_global_buffer_usage() += m_global_size;
    }

    Tracked_buffer_usage(const Tracked_buffer_usage&) = delete;
    Tracked_buffer_usage& operator=(const Tracked_buffer_usage&) = delete;

    Tracked_buffer_usage(Tracked_buffer_usage&& other) :
        m_global_size{std::move(other.m_global_size)}
    {
        other.m_global_size = 0;
    }

    Tracked_buffer_usage& operator=(Tracked_buffer_usage&& other)
    {
        if (this != &other) {
            total_global_buffer_usage() -= m_global_size;

            m_global_size = std::move(other.m_global_size);

            other.m_global_size = 0;
        }

        return *this;
    }

    ~Tracked_buffer_usage() { total_global_buffer_usage() -= m_global_size; }
};


template<typename T>
class Buffer {
    Tracked_buffer_usage m_tracker;
    std::unique_ptr<T[]> m_buffer;
    size_t m_buffer_size;

  public:
    using value_type = T;

    Buffer(const exseis::Communicator& communicator, size_t local_count) :
        m_tracker{communicator, local_count * sizeof(T)},
        m_buffer{std::make_unique<T[]>(local_count)},
        m_buffer_size{local_count}
    {
    }

    ~Buffer() = default;

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer(Buffer&&)                 = default;
    Buffer& operator=(Buffer&&) = default;

    T& operator[](size_t index) { return m_buffer[index]; }
    const T& operator[](size_t index) const { return m_buffer[index]; }

    T* data() { return m_buffer.get(); }
    const T* data() const { return m_buffer.get(); }

    size_t size() const { return m_buffer_size; }

    auto begin() { return m_buffer.get(); }
    auto begin() const { return m_buffer.get(); }

    auto end() { return m_buffer.get() + m_buffer_size; }
    auto end() const { return m_buffer.get() + m_buffer_size; }
};

}  // namespace test
}  // namespace exseis

#endif  // EXSEIS_TEST_BUFFER_HH
