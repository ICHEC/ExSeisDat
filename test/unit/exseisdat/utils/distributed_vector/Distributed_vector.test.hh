#include "exseisdat/utils/communicator/Communicator.hh"
#include "exseisdat/utils/distributed_vector/Distributed_vector.hh"

#include "exseisdat/test/Buffer.hh"
#include "exseisdat/test/catch2.hh"

#include <algorithm>
#include <array>
#include <limits>
#include <memory>
#include <numeric>
#include <vector>

#include <cstdint>
#include <cstring>

inline void test_distributed_vector(
    exseis::utils::Distributed_vector<size_t>& distributed_vector,
    size_t global_size,
    const exseis::utils::Communicator& communicator)
{
    SECTION ("The size() method should return the global size.") {
        REQUIRE(distributed_vector.size() == global_size);
    }

    SECTION ("The vector should be zero-initialized") {
        for (size_t i = 0; i < distributed_vector.size(); i++) {
            REQUIRE(distributed_vector[i] == 0);
            REQUIRE(distributed_vector.get(i) == 0);
        }
    }

    SECTION ("The get_n and set_n functions should set contiguous chunks") {
        size_t offset_percent     = GENERATE(0, 10, 50, 90, 100);
        size_t block_size_percent = GENERATE_COPY(filter(
            [=](size_t block_size_percent) {
                return offset_percent + block_size_percent <= 100;
            },
            values<size_t>({0, 10, 50, 90, 100})));

        const size_t offset = offset_percent * distributed_vector.size() / 100;
        const size_t block_size =
            block_size_percent * distributed_vector.size() / 100;

        auto buffer = exseis::test::Buffer<size_t>(communicator, block_size);

        for (size_t rank = 0; rank < communicator.get_num_rank(); rank++) {

            // Make sure any reads from previous iterations are complete
            distributed_vector.sync();

            if (communicator.get_rank() == rank) {
                // Set pattern on one rank
                for (size_t i = 0; i < buffer.size(); i++) {
                    buffer[i] = distributed_vector.size() * rank + offset + i;
                }

                // Set the data on one rank
                distributed_vector.set_n(offset, buffer.size(), buffer.data());
            }

            distributed_vector.sync();

            // Zero the buffer and get the data
            std::memset(buffer.data(), 0, buffer.size() * sizeof(size_t));

            distributed_vector.get_n(offset, buffer.size(), buffer.data());

            // Check the pattern on all ranks
            REQUIRE(([&] {
                for (size_t i = 0; i < buffer.size(); i++) {
                    if (buffer[i]
                        != distributed_vector.size() * rank + offset + i) {
                        return false;
                    }
                }
                return true;
            }()));
        }
    }


    SECTION ("The get/set methods should get/set data at indices, "
             "and make it accessible to all processes after synchronization") {
        // Batch operations in chunks for reasonable debug performance
        const size_t max_chunk_size =
            std::max<size_t>(1, distributed_vector.size() / 10);

        for (size_t chunk_start = 0; chunk_start < distributed_vector.size();
             chunk_start += max_chunk_size) {

            const size_t chunk_end = std::min<size_t>(
                chunk_start + max_chunk_size, distributed_vector.size());

            for (size_t global_index = chunk_start; global_index < chunk_end;
                 global_index++) {
                REQUIRE(distributed_vector[global_index] == 0);
            }

            for (size_t rank = 0; rank < communicator.get_num_rank(); rank++) {
                distributed_vector.sync();

                if (communicator.get_rank() == rank) {
                    for (size_t global_index = chunk_start;
                         global_index < chunk_end; global_index++) {

                        const size_t test_value =
                            global_index * communicator.get_num_rank() + rank;

                        distributed_vector.set(global_index, test_value);
                    }
                }

                distributed_vector.sync();

                for (size_t global_index = chunk_start;
                     global_index < chunk_end; global_index++) {

                    const size_t test_value =
                        global_index * communicator.get_num_rank() + rank;

                    REQUIRE(distributed_vector.get(global_index) == test_value);
                    REQUIRE(distributed_vector[global_index] == test_value);
                }
            }
        }
    }

    SECTION (
        "The set methods should not need to be serialized if each process calls a unique index.") {
        distributed_vector.sync();

        for (size_t i = 0; i < distributed_vector.size(); i++) {
            if (i % communicator.get_num_rank() == communicator.get_rank()) {
                distributed_vector.set(i, i);
            }
        }

        distributed_vector.sync();

        auto buffer = exseis::test::Buffer<size_t>(
            communicator, distributed_vector.size());
        distributed_vector.get_n(0, buffer.size(), buffer.data());
        for (size_t i = 0; i < distributed_vector.size(); i++) {
            REQUIRE(buffer[i] == i);
        }
    }

    SECTION ("The subscript operator should provide a \"setter\" interface.") {
        distributed_vector.sync();

        for (size_t i = 0; i < distributed_vector.size(); i++) {
            if (i % communicator.get_num_rank() == communicator.get_rank()) {
                distributed_vector[i] = i;
            }
        }

        distributed_vector.sync();

        auto buffer = exseis::test::Buffer<size_t>(
            communicator, distributed_vector.size());
        distributed_vector.get_n(0, buffer.size(), buffer.data());
        for (size_t i = 0; i < distributed_vector.size(); i++) {
            REQUIRE(buffer[i] == i);
        }
    }

    SECTION ("The resize() method should resize the Distributed_vector.") {
        size_t original_size = distributed_vector.size();

        // A unique, non-zero value for every index.
        auto value_at = [](size_t global_index) { return global_index + 1; };

        auto buffer = exseis::test::Buffer<size_t>(
            communicator, distributed_vector.size());

        distributed_vector.sync();
        if (communicator.get_rank() == 0) {
            for (size_t i = 0; i < distributed_vector.size(); i++) {
                buffer[i] = value_at(i);
            }
            distributed_vector.set_n(0, buffer.size(), buffer.data());
        }

        distributed_vector.sync();

        SECTION (
            "Scaling up: Old values should remain, and newly added values should be zero-initialized.") {
            distributed_vector.resize(2 * original_size);
            REQUIRE(distributed_vector.size() == 2 * original_size);

            buffer = exseis::test::Buffer<size_t>(
                communicator, distributed_vector.size());
            std::memset(buffer.data(), 0, buffer.size() * sizeof(size_t));
            distributed_vector.get_n(0, buffer.size(), buffer.data());

            for (size_t i = 0; i < buffer.size(); i++) {
                if (i < original_size) {
                    REQUIRE(buffer[i] == value_at(i));
                }
                else {
                    REQUIRE(buffer[i] == 0);
                }
            }
        }

        SECTION ("Scaling down: Old values, still in range, should remain.") {
            distributed_vector.resize(original_size / 2);
            REQUIRE(distributed_vector.size() == original_size / 2);

            buffer = exseis::test::Buffer<size_t>(
                communicator, distributed_vector.size());
            std::memset(buffer.data(), 0, buffer.size() * sizeof(size_t));
            distributed_vector.get_n(0, buffer.size(), buffer.data());
            for (size_t i = 0; i < buffer.size(); i++) {
                REQUIRE(buffer[i] == value_at(i));
            }
        }
    }
}

template<typename Communicator>
void test_distributed_vector_2gb_limit(
    exseis::utils::Distributed_vector<unsigned char>& distributed_vector,
    const Communicator& communicator)
{
    REQUIRE(distributed_vector.size() >= size_t(2) * 1024 * 1024 * 1024);

    // Allocate buffer on rank 0 only
    size_t buffer_size = ([&]() {
        if (communicator.get_rank() == 0) return distributed_vector.size();
        return size_t(0);
    }());

    using Buffer = exseis::test::Buffer<unsigned char>;
    Buffer buffer(communicator, buffer_size);

    // Buffer pattern stuff
    constexpr size_t pattern_size =
        std::numeric_limits<Buffer::value_type>::max() + size_t(1);
    std::array<Buffer::value_type, pattern_size> pattern;
    std::iota(pattern.begin(), pattern.end(), 0);

    const auto set_pattern = [&](Buffer& buffer) {
        for (size_t i = 0; i < buffer.size(); i += pattern.size()) {
            const size_t start = i;
            const size_t end =
                std::min<size_t>(i + pattern.size(), buffer.size());
            const size_t copy_size = end - start;

            std::memcpy(
                buffer.data() + i, pattern.data(),
                copy_size * sizeof(Buffer::value_type));
        }
    };

    const auto zero_buffer = [](Buffer& buffer) {
        std::memset(
            buffer.data(), 0, buffer.size() * sizeof(Buffer::value_type));
    };

    const auto check_pattern = [&](const Buffer& buffer) {
        for (size_t i = 0; i < buffer.size(); i += pattern.size()) {
            const size_t start = i;
            const size_t end =
                std::min<size_t>(i + pattern.size(), buffer.size());
            const size_t cmp_size = end - start;

            const int cmp = std::memcmp(
                buffer.data() + i, pattern.data(),
                cmp_size * sizeof(Buffer::value_type));
            if (cmp != 0) return false;
        }
        return true;
    };

    SECTION ("set_n and get_n") {
        set_pattern(buffer);
        distributed_vector.set_n(0, buffer.size(), buffer.data());

        zero_buffer(buffer);
        distributed_vector.get_n(0, buffer.size(), buffer.data());

        REQUIRE(check_pattern(buffer));
    }
}
