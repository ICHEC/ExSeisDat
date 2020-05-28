#ifndef EXSEISDAT_TEST_PIOL_BINARY_FILE_TEST_HH
#define EXSEISDAT_TEST_PIOL_BINARY_FILE_TEST_HH

#include "exseisdat/piol/io_driver/IO_driver.hh"
#include "exseisdat/utils/decomposition/block_decomposition.hh"

#include "exseisdat/test/Buffer.hh"
#include "exseisdat/test/catch2.hh"

#include <algorithm>
#include <array>
#include <limits>
#include <memory>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

#include <cstdint>
#include <cstring>


template<typename Inspector, typename IODriverDerived, typename Communicator>
void test_io_driver(
    IODriverDerived& io_driver_derived, Communicator& communicator)
{
    // Perform operations on IO_driver.
    // Perform inspections on IO_driver_derived.
    auto& io_driver = io_driver_derived;

    size_t rank            = communicator.get_rank();
    size_t number_of_ranks = communicator.get_num_rank();

    // A generator for new values
    const auto new_value_at = [&](size_t global_index) {
        return Inspector::alt_pattern(global_index);
    };

    REQUIRE(io_driver.get_file_size() <= exseis::test::global_max_buffer_size);

    REQUIRE(
        io_driver.get_file_size() == Inspector::file_size(io_driver_derived));

    SECTION ("is_open() should be true for a properly constructed IO_driver.") {
        REQUIRE(io_driver.is_open() == true);
    }

    SECTION ("get_file_size() should return the file size.") {
        REQUIRE(
            io_driver.get_file_size()
            == Inspector::file_size(io_driver_derived));
    }

    SECTION ("set_file_size() should set the file size.") {
        io_driver_derived =
            Inspector::set_pattern(std::move(io_driver_derived));

        size_t original_size = io_driver.get_file_size();

        io_driver.set_file_size(original_size + 1);

        REQUIRE(io_driver.get_file_size() == original_size + 1);

        REQUIRE(Inspector::file_size(io_driver_derived) == original_size + 1);

        SECTION ("Any data within the old size should still be there.") {
            REQUIRE(Inspector::all_of(
                io_driver_derived,
                [=](size_t global_i, unsigned char original_value,
                    unsigned char current_value) {
                    // Data within original_size should be unchanged
                    if (global_i < original_size) {
                        return original_value == current_value;
                    }
                    // Data outside original_size is unchanged
                    return true;
                }));
        }
    }

    SECTION ("Contiguous access") {
        enum class Access { parallel, single };

        Access access = GENERATE_COPY(filter(
            [=](Access access) {
                // Use only Access::single for single-process
                if (number_of_ranks == 1) {
                    return access == Access::single;
                }
                return true;
            },
            values({Access::parallel, Access::single})));

        size_t offset_percent = GENERATE(0, 10, 50, 90, 100);
        size_t size_percent   = GENERATE_COPY(filter(
            [=](size_t size_percent) {
                return size_percent + offset_percent <= 100;
            },
            values({0, 10, 50, 90, 100})));

        size_t offset = offset_percent * io_driver.get_file_size() / 100;
        size_t size   = size_percent * io_driver.get_file_size() / 100;

        auto decomposition = ([=] {
            switch (access) {
                // parallel access: all processes allocate and read a buffer.
                case Access::parallel:
                    return exseis::utils::block_decomposition(
                        size, number_of_ranks, rank);

                // single access: only rank 0 allocates and reads a buffer.
                case Access::single:
                    if (rank == 0) {
                        return exseis::utils::Contiguous_decomposition(0, size);
                    }
                    return exseis::utils::Contiguous_decomposition(0, 0);
            }

            assert(false && "Unreachable");
        }());

        size_t local_offset = offset + decomposition.global_offset;
        size_t local_size   = decomposition.local_size;

        io_driver_derived =
            Inspector::set_pattern(std::move(io_driver_derived));

        {
            INFO("read() should read a contiguous chunk of the file.");

            exseis::test::Buffer<unsigned char> buffer(
                communicator, local_size);

            io_driver.read(local_offset, local_size, buffer.data());

            REQUIRE(Inspector::all_of(
                io_driver_derived,
                [&](size_t global_i, unsigned char original_value,
                    unsigned char current_value) -> bool {
                    // The underlying values should be unchanged
                    if (current_value != original_value) return false;

                    // The buffer should be equal to a contiguous chunk of
                    // the file starting from `offset`.
                    if (global_i >= local_offset
                        && global_i < local_offset + local_size) {

                        if (buffer[global_i - local_offset] != original_value) {
                            return false;
                        }
                    }

                    return true;
                }));
        }

        {
            INFO("write() should write a contiguous chunk of the file");

            // Set the buffer to the new values
            exseis::test::Buffer<unsigned char> buffer = ([=] {
                exseis::test::Buffer<unsigned char> buffer(
                    communicator, local_size);
                for (size_t i = 0; i < buffer.size(); i++) {
                    buffer[i] = new_value_at(local_offset + i);
                }
                return buffer;
            }());

            io_driver.write(local_offset, local_size, buffer.data());

            io_driver.sync();

            REQUIRE(Inspector::all_of(
                io_driver_derived,
                [&](size_t global_i, unsigned char original_value,
                    unsigned char current_value) -> bool {
                    if (global_i >= offset && global_i < offset + size) {
                        if (current_value != new_value_at(global_i)) {
                            return false;
                        }
                    }
                    else {
                        if (current_value != original_value) {
                            return false;
                        }
                    }

                    return true;
                }));
        }
    }

    SECTION ("Strided access") {
        enum class Access { parallel, single };

        Access access = GENERATE_COPY(filter(
            [=](Access access) {
                // Use only Access::single for single-process
                if (number_of_ranks == 1) {
                    return access == Access::single;
                }
                return true;
            },
            values({Access::parallel, Access::single})));

        size_t offset_percent = GENERATE(0, 10, 90, 100);

        size_t block_size_percent = GENERATE_COPY(filter(
            [=](size_t block_size_percent) {
                // Offset + block size shouldn't pass the end of the file
                return offset_percent + block_size_percent <= 100;
            },
            values({0, 10, 90, 100})));

        size_t stride_size_percent = GENERATE_COPY(filter(
            [=](size_t stride_size_percent) {
                // Offset + stride size shouldn't pass the end of the file.
                // The stride size shouldn't be smaller than the block size.
                return offset_percent + stride_size_percent <= 100
                       && stride_size_percent >= block_size_percent;
            },
            values({0, 10, 20, 90, 100})));

        size_t number_of_blocks = GENERATE_COPY(filter(
            [=](size_t number_of_blocks) {
                // Stride size * number of blocks shouldn't be larger than
                // the file.
                if (offset_percent + number_of_blocks * stride_size_percent
                    > 100) {
                    return false;
                }

                // Block size * number of blocks shouldn't be larger than the
                // global buffer max
                if (number_of_blocks * block_size_percent > 100) {
                    return false;
                }

                return true;
            },
            values({0, 1, 2, 10})));


        //
        // Set strided read/write parameters
        //

        size_t offset = offset_percent * io_driver.get_file_size() / 100;
        size_t block_size =
            block_size_percent * io_driver.get_file_size() / 100;
        size_t stride_size =
            stride_size_percent * io_driver.get_file_size() / 100;


        //
        // Set process-local read/write parameters
        //

        auto block_index_decomposition = ([=] {
            switch (access) {
                case Access::parallel:
                    return exseis::utils::block_decomposition(
                        number_of_blocks, number_of_ranks, rank);

                case Access::single:
                    if (rank == 0) {
                        return exseis::utils::Contiguous_decomposition(
                            0, number_of_blocks);
                    }
                    return exseis::utils::Contiguous_decomposition(0, 0);
            }

            assert(false && "Unreachable");
        }());

        size_t local_block_index_start =
            block_index_decomposition.global_offset;
        size_t local_number_of_blocks = block_index_decomposition.local_size;

        auto offset_to_local_block_start =
            offset + local_block_index_start * stride_size;

        const auto get_stride_index = [=](size_t global_index) {
            size_t offset_index = global_index - offset;
            return stride_size == 0 ? 0 : offset_index / stride_size;
        };

        const auto get_block_data_index = [=](size_t global_index) {
            size_t offset_index = global_index - offset;
            return stride_size == 0 ? 0 : offset_index % stride_size;
        };

        // test if a global_index falls within a block in the strided range
        const auto is_in_global_range = [=](size_t global_index) {
            if (global_index < offset) {
                return false;
            }
            if (get_stride_index(global_index) >= number_of_blocks) {
                return false;
            }
            if (get_block_data_index(global_index) >= block_size) {
                return false;
            }

            return true;
        };

        const auto is_in_local_range = [=](size_t global_index) {
            if (global_index < offset_to_local_block_start) {
                return false;
            }
            if (get_stride_index(global_index)
                >= local_block_index_start + local_number_of_blocks) {
                return false;
            }
            if (get_block_data_index(global_index) >= block_size) {
                return false;
            }

            return true;
        };

        io_driver_derived =
            Inspector::set_pattern(std::move(io_driver_derived));

        {
            INFO("read_strided() should read strided data from the file.");

            exseis::test::Buffer<unsigned char> buffer(
                communicator, local_number_of_blocks * block_size);

            io_driver.read_strided(
                offset_to_local_block_start, block_size, stride_size,
                local_number_of_blocks, buffer.data());

            REQUIRE(Inspector::all_of(
                io_driver_derived,
                [&](size_t global_index, unsigned char original_value,
                    unsigned char current_value) -> bool {
                    // The underlying values should be unchanged
                    if (original_value != current_value) {
                        return false;
                    }

                    if (is_in_local_range(global_index)) {
                        size_t stride_index = get_stride_index(global_index);
                        size_t local_stride_index =
                            stride_index - local_block_index_start;
                        size_t block_data_index =
                            get_block_data_index(global_index);

                        size_t buffer_index =
                            local_stride_index * block_size + block_data_index;

                        if (buffer[buffer_index] != current_value) {
                            return false;
                        }
                    }
                    return true;
                }));
        }

        {
            INFO("write_strided() should write strided data to the file.");

            // Set the buffer to the new values
            exseis::test::Buffer<unsigned char> buffer = ([=] {
                exseis::test::Buffer<unsigned char> buffer(
                    communicator, local_number_of_blocks * block_size);

                for (size_t local_block_index = 0;
                     local_block_index < local_number_of_blocks;
                     local_block_index++) {
                    for (size_t block_data_index = 0;
                         block_data_index < block_size; block_data_index++) {

                        size_t buffer_index =
                            local_block_index * block_size + block_data_index;

                        size_t file_index = offset_to_local_block_start
                                            + local_block_index * stride_size
                                            + block_data_index;

                        buffer[buffer_index] = new_value_at(file_index);
                    }
                }
                return buffer;
            }());

            io_driver.write_strided(
                offset_to_local_block_start, block_size, stride_size,
                local_number_of_blocks, buffer.data());

            io_driver.sync();

            REQUIRE(Inspector::all_of(
                io_driver_derived,
                [&](size_t global_index, unsigned char original_value,
                    unsigned char current_value) -> bool {
                    // The values inside the global range should be set to
                    // `new_value_at(global_index)`
                    if (is_in_local_range(global_index)) {
                        if (current_value != new_value_at(global_index)) {
                            return false;
                        }
                    }

                    // The underlying values outside the globally written range
                    // should be unchanged.
                    if (!is_in_global_range(global_index)) {
                        if (original_value != current_value) {
                            return false;
                        }
                    }

                    return true;
                }));
        }
    }

    SECTION ("Unstructured access") {
        //
        // Set unstructured read/write parameters
        //

        enum class Access { parallel, single };

        Access access = GENERATE_COPY(filter(
            [=](Access access) {
                // Use only Access::single for single-process
                if (number_of_ranks == 1) {
                    return access == Access::single;
                }
                return true;
            },
            values({Access::parallel, Access::single})));

        size_t block_size_percent = GENERATE(0, 10, 50, 90, 100);

        size_t number_of_blocks = GENERATE_COPY(filter(
            [=](size_t number_of_blocks) {
                // Block size * number of blocks must be less than or
                // equal to the file size.
                return block_size_percent * number_of_blocks <= 100;
            },
            values({0, 1, 5, 10})));

        CAPTURE(access, block_size_percent, number_of_blocks);

        size_t block_size =
            block_size_percent * io_driver.get_file_size() / 100;

        //
        // Generate a list of non-overlapping global offsets, roughtly scattered
        // throughout the file.
        //
        auto global_offsets = ([&] {
            exseis::test::Buffer<size_t> offsets(
                communicator, number_of_blocks);

            // First, generate a contiguous set of offset blocks
            std::iota(offsets.begin(), offsets.end(), size_t(0));
            for (auto& offset : offsets) {
                offset *= block_size;
            }

            // Next, move each block along a random offset into the file
            // between its current position, and the start of the next offset
            std::mt19937_64 rand(0);

            for (size_t i = 0; i < offsets.size(); i++) {
                size_t previous_index = offsets.size() - i;
                size_t current_index  = previous_index - 1;

                size_t current_offset = offsets[current_index];
                size_t max_offset     = ([&] {
                    if (previous_index == offsets.size()) {
                        return io_driver.get_file_size() - block_size;
                    }
                    return offsets[previous_index] - block_size;
                }());

                if (max_offset == current_offset) continue;

                size_t new_offset =
                    current_offset + (rand() % (max_offset - current_offset));

                offsets[current_index] = new_offset;
            }

            // Ensure the offsets don't overlap
            for (size_t i = 0; i < offsets.size(); i++) {
                if (i + 1 < offsets.size()) {
                    assert(offsets[i] + block_size <= offsets[i + 1]);
                }
                else {
                    assert(
                        offsets[i] + block_size <= io_driver.get_file_size());
                }
            }

            // Finally, shuffle the offsets and pull out the local chunk
            // std::shuffle(offsets.begin(), offsets.end(), rand);

            return offsets;
        }());

        auto offsets_decomposition = ([&] {
            switch (access) {
                case Access::parallel:
                    // Get a local chunk of the offsets
                    return exseis::utils::block_decomposition(
                        number_of_blocks, number_of_ranks, rank);

                case Access::single:
                    if (rank == 0) {
                        return exseis::utils::Contiguous_decomposition(
                            0, number_of_blocks);
                    }
                    return exseis::utils::Contiguous_decomposition(0, 0);
            }

            assert(false && "Unreachable");
        }());

        size_t local_offsets_begin    = offsets_decomposition.global_offset;
        size_t local_number_of_blocks = offsets_decomposition.local_size;

        exseis::test::Buffer<size_t> local_offsets(
            communicator, local_number_of_blocks);
        std::copy(
            global_offsets.begin() + local_offsets_begin,
            global_offsets.begin() + local_offsets_begin
                + local_number_of_blocks,
            local_offsets.begin());

        io_driver_derived =
            Inspector::set_pattern(std::move(io_driver_derived));

        {
            INFO(
                "read_offsets() should read data from a file at arbitrary offsets");

            exseis::test::Buffer<unsigned char> buffer(
                communicator, local_offsets.size() * block_size);

            io_driver.read_offsets(
                block_size, local_offsets.size(), local_offsets.data(),
                buffer.data());

            REQUIRE(Inspector::all_of(
                io_driver_derived,
                [&](size_t global_index, unsigned char original_value,
                    unsigned char current_value) -> bool {
                    // The underlying data shouldn't have changed
                    if (current_value != original_value) {
                        return false;
                    }

                    // loop through the local offsets to find the block index
                    // into the local buffer
                    for (size_t local_offset_i = 0;
                         local_offset_i < local_offsets.size();
                         local_offset_i++) {

                        size_t local_offset = local_offsets[local_offset_i];

                        if (global_index >= local_offset
                            && global_index < local_offset + block_size) {

                            size_t block_data_index =
                                global_index - local_offset;
                            size_t buffer_index =
                                local_offset_i * block_size + block_data_index;

                            if (current_value != buffer[buffer_index]) {
                                return false;
                            }

                            // Index was found, break early
                            break;
                        }
                    }

                    return true;
                }));
        }

        {
            INFO(
                "write_offsets() should write data to a file at arbitrary offsets")

            // Set the buffer to the new values
            exseis::test::Buffer<unsigned char> buffer = ([&] {
                exseis::test::Buffer<unsigned char> buffer(
                    communicator, local_offsets.size() * block_size);

                for (size_t local_offset_index = 0;
                     local_offset_index < local_offsets.size();
                     local_offset_index++) {
                    for (size_t block_data_index = 0;
                         block_data_index < block_size; block_data_index++) {

                        size_t buffer_index =
                            local_offset_index * block_size + block_data_index;

                        size_t file_index = local_offsets[local_offset_index]
                                            + block_data_index;

                        buffer[buffer_index] = new_value_at(file_index);
                    }
                }
                return buffer;
            }());

            io_driver.write_offsets(
                block_size, local_offsets.size(), local_offsets.data(),
                buffer.data());

            io_driver.sync();

            REQUIRE(Inspector::all_of(
                io_driver_derived,
                [&](size_t global_index, unsigned char original_value,
                    unsigned char current_value) -> bool {
                    // loop through the local offsets to find the block index
                    // into the local buffer
                    for (size_t global_offset_index = 0;
                         global_offset_index < global_offsets.size();
                         global_offset_index++) {

                        size_t offset = global_offsets[global_offset_index];

                        if (global_index >= offset
                            && global_index < offset + block_size) {

                            if (current_value != new_value_at(global_index)) {
                                return false;
                            }

                            // index found, return early
                            return true;
                        }
                    }

                    // If the index hasn't been found, the underlying value
                    // should remain unchanged
                    if (current_value != original_value) {
                        return false;
                    }

                    return true;
                }));
        }
    }
}

template<typename Communicator>
void test_io_driver_2gb_limit(
    exseis::piol::IO_driver& io_driver, const Communicator& communicator)
{
    REQUIRE(io_driver.get_file_size() >= size_t(2) * 1024 * 1024 * 1024);

    // Allocate buffer on rank 0 only
    size_t buffer_size = ([&]() {
        const size_t file_size = io_driver.get_file_size();
        if (communicator.get_rank() == 0) return file_size;
        return size_t(0);
    }());

    using Buffer = exseis::test::Buffer<unsigned char>;
    Buffer buffer(communicator, buffer_size);


    constexpr size_t pattern_size =
        std::numeric_limits<Buffer::value_type>::max() + size_t(1);
    std::array<Buffer::value_type, pattern_size> pattern;
    std::iota(pattern.begin(), pattern.end(), 0);

    // Set buffer to predetermined pattern
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

    // Set buffer to all 0s
    const auto zero_buffer = [&](Buffer& buffer) {
        std::memset(
            buffer.data(), 0, buffer.size() * sizeof(Buffer::value_type));
    };

    // Check buffer matches predetermined pattern
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


    SECTION ("write and read") {
        set_pattern(buffer);
        io_driver.write(0, buffer.size(), buffer.data());

        io_driver.sync();

        zero_buffer(buffer);
        io_driver.read(0, buffer.size(), buffer.data());

        REQUIRE(check_pattern(buffer));
    }

    SECTION ("write_strided and read_strided") {
        set_pattern(buffer);
        io_driver.write_strided(
            0, buffer.size(), buffer.size(), 1, buffer.data());

        io_driver.sync();

        zero_buffer(buffer);
        io_driver.read_strided(
            0, buffer.size(), buffer.size(), 1, buffer.data());

        REQUIRE(check_pattern(buffer));
    }

    SECTION ("write_offsets and read_offsets") {
        set_pattern(buffer);
        const size_t offsets[] = {0};
        io_driver.write_offsets(buffer.size(), 1, offsets, buffer.data());

        io_driver.sync();

        zero_buffer(buffer);
        io_driver.read_offsets(buffer.size(), 1, offsets, buffer.data());

        REQUIRE(check_pattern(buffer));
    }
}

#endif  // EXSEISDAT_TEST_PIOL_BINARY_FILE_TEST_HH
