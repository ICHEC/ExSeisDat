#include "datampiiotest.hh"

size_t modify_nt(
  const size_t fs,
  const size_t offset,
  const size_t requested_nt,
  const size_t ns)
{
    // We shouldn't have our ASSERT_EQ test beyond the actual number of traces
    // we've written to the file, because we don't know what those values
    // will be.
    //
    // We allow this, rather than throwing an error, because this is allowed
    // behaviour.
    //
    // We find how many traces are in the actual file, and if the requested
    // number of traces would go beyond that, we truncate the requested number
    // to the number remaining in the file.

    // Total number of traces in the file
    const size_t total_nt = segy::get_nt(fs, ns);

    // Number of traces remaining after the offset
    const size_t remaining_nt = ([&]() -> size_t {
        if (total_nt < offset) {
            return 0;
        }

        return total_nt - offset;
    }());

    return std::min(requested_nt, remaining_nt);
}


// Test fixture preparing an ExSeis and a Binary_file using a MPI_Binary_file
// instance.
class MPI_Binary_file_Read : public Test {
  public:
    std::shared_ptr<exseis::piol::ExSeis> piol      = nullptr;
    std::shared_ptr<exseis::piol::Binary_file> file = nullptr;

    void make_mpiio(
      std::string filename,
      const MPI_Binary_file::Opt& opt = MPI_Binary_file::Opt())
    {
        piol = exseis::piol::ExSeis::make();
        file = std::make_shared<MPI_Binary_file>(
          piol, filename, FileMode::Read, opt);
    }
};


TEST_F(MPI_Binary_file_Read, Death_FailedConstructor)
{
    make_mpiio(nonexistant_filename());

    EXPECT_EXIT(
      piol->assert_ok(), ExitedWithCode(EXIT_FAILURE),
      ".*Fatal Error in PIOL\\. Dumping Log\\..*");
}

///////////////////////// MPI-IO getting the file size /////////////////////////
TEST_F(MPI_Binary_file_Read, Constructor)
{
    make_mpiio(zero_file());
    piol->assert_ok();
    piol->log->process_entries();

    EXPECT_NE(nullptr, file) << "file is null";
    auto mio = std::dynamic_pointer_cast<MPI_Binary_file>(file);
    EXPECT_NE(nullptr, mio) << "MPI-IO file cast failed";
    EXPECT_FALSE(mio->is_open()) << "File was not opened";

    piol->assert_ok();
    EXPECT_EQ(static_cast<size_t>(0), file->get_file_size());
}

TEST_F(MPI_Binary_file_Read, SmallFileSize)
{
    make_mpiio(small_file());
    piol->assert_ok();
    EXPECT_EQ(small_size, file->get_file_size());
}

TEST_F(MPI_Binary_file_Read, LargeFileSize)
{
    make_mpiio(large_file());
    piol->assert_ok();
    EXPECT_EQ(large_size, file->get_file_size());
}

//////////////////////// MPI-IO reading contiguous data ////////////////////////

TEST_F(MPI_Binary_file_Read, BlockingReadSmall)
{
    make_mpiio(small_file());

    std::vector<unsigned char> d(small_size);
    d.back() = get_pattern(d.size() - 2);

    file->read(0, d.size() - 1, d.data());
    piol->assert_ok();

    EXPECT_EQ(get_pattern(d.size() - 2), d.back());

    // Set the last element to zero
    d.back() = 0U;
    std::vector<unsigned char> test(small_size);
    ASSERT_THAT(d, ElementsAreArray(test));
}

TEST_F(MPI_Binary_file_Read, ZeroSizeReadOnLarge)
{
    make_mpiio(plarge_file());

    std::vector<unsigned char> d = {get_pattern(1U)};
    file->read(0, 0, d.data());
    piol->assert_ok();

    EXPECT_EQ(get_pattern(1U), d[0]);
    EXPECT_NE(get_pattern(0U), d[0]);
}

TEST_F(MPI_Binary_file_Read, OffsetsBlockingReadLarge)
{
    MPI_Binary_file::Opt opt;
    opt.max_size = magic_num1;
    make_mpiio(plarge_file(), opt);

    // Test looping logic for big files, various offsets
    for (size_t j = 0; j < magic_num1; j += 10U) {
        size_t sz     = 16U * magic_num1 + j;
        size_t offset = (large_size / magic_num1) * j;
        std::vector<unsigned char> d(sz);

        file->read(offset, d.size(), d.data());
        piol->assert_ok();

        for (size_t i = 0; i < d.size(); i++) {
            ASSERT_EQ(d[i], get_pattern(offset + i));
        }
    }
}

TEST_F(MPI_Binary_file_Read, BlockingOneByteReadLarge)
{
    make_mpiio(plarge_file());
    // Test single value reads mid file
    for (size_t i = 0; i < magic_num1; i++) {
        size_t offset         = large_size / 2U + i;
        unsigned char test[2] = {get_pattern(offset - 2),
                                 get_pattern(offset - 1)};

        file->read(offset, 1, test);
        piol->assert_ok();
        EXPECT_EQ(test[0], get_pattern(offset));
        EXPECT_EQ(test[1], get_pattern(offset - 1));
    }
}

///////////////// MPI-IO reading non-contiguous blocks of data /////////////////

/// Read a contiguous block of `nt` traces starting from the `offset`th block,
/// test that the in-line and cross-line values match the expected test values.
///
/// This function implementing the ReadContiguous* tests
///
std::vector<unsigned char> read_contiguous(
  const std::shared_ptr<Binary_file>& file, size_t nt, size_t ns, size_t offset)
{
    const auto trace_packet_size = segy::segy_trace_size(ns);
    std::vector<unsigned char> trace_buffer(trace_packet_size * nt);

    // Read a contiguous block of `nt` traces starting from the `offset`th block
    const auto trace_data_start = segy::segy_binary_file_header_size();
    file->read(
      trace_data_start + offset * trace_packet_size, trace_packet_size * nt,
      trace_buffer.data());

    return trace_buffer;
}

/// Test the inline and crossline metadata held by trace_buffer matches the
/// expected test values.
void test_il_xl_metadata(
  const std::shared_ptr<Binary_file>& file,
  size_t nt,
  size_t ns,
  size_t offset,
  const std::vector<unsigned char>& trace_buffer)
{
    const auto trace_packet_size = segy::segy_trace_size(ns);

    nt = modify_nt(file->get_file_size(), offset, nt, ns);
    for (size_t i = 0; i < nt; i++) {

        // The trace buffer starting at the `ith` read trace
        const unsigned char* trace_buffer_i =
          &trace_buffer[trace_packet_size * i];

        // The buffer position starting at the in-line and cross-line metadata
        // values.
        const unsigned char* il_bytes =
          &trace_buffer_i[static_cast<size_t>(Tr::il) - 1];
        const unsigned char* xl_bytes =
          &trace_buffer_i[static_cast<size_t>(Tr::xl) - 1];

        ASSERT_EQ(
          il_num(i + offset),
          from_big_endian<int32_t>(
            il_bytes[0], il_bytes[1], il_bytes[2], il_bytes[3]))
          << "i: " << i << ", nt: " << nt << ", ns: " << ns
          << ", offset: " << offset;

        ASSERT_EQ(
          xl_num(i + offset),
          from_big_endian<int32_t>(
            xl_bytes[0], xl_bytes[1], xl_bytes[2], xl_bytes[3]))
          << i;
    }
}


TEST_F(MPI_Binary_file_Read, ReadContiguousZero)
{
    make_mpiio(small_segy_file());
    const size_t nt        = 0;
    const size_t ns        = 0;
    const auto trace_bufer = read_contiguous(file, nt, ns, 0);
    piol->assert_ok();
    test_il_xl_metadata(file, nt, ns, 0, trace_bufer);
}

TEST_F(MPI_Binary_file_Read, ReadContiguousSSS)
{
    make_mpiio(small_segy_file());
    const size_t nt        = 400;
    const size_t ns        = 261;
    const auto trace_bufer = read_contiguous(file, nt, ns, 0);
    piol->assert_ok();
    test_il_xl_metadata(file, nt, ns, 0, trace_bufer);
}

// Intentionally read much beyond the end of the file to make sure that MPI-IO
// doesn't abort/fails.  MPI 3.1 spec says (or at least strongly implies) it
// should work.
TEST_F(MPI_Binary_file_Read, Farm_ReadContiguousEnd)
{
    make_mpiio(small_segy_file());
    size_t nt       = 400;
    const size_t ns = 261;

    // Read extra
    nt *= 1024;
    const auto trace_bufer = read_contiguous(file, nt, ns, 200);
    piol->assert_ok();
    test_il_xl_metadata(file, nt, ns, 200, trace_bufer);
}

TEST_F(MPI_Binary_file_Read, ReadContiguousSLS)
{
    make_mpiio(big_trace_segy_file());

    const size_t nt = 40U;
    const size_t ns = 32000U;

    const auto trace_bufer = read_contiguous(file, nt, ns, 1000U);
    piol->assert_ok();
    test_il_xl_metadata(file, nt, ns, 1000U, trace_bufer);
}

TEST_F(MPI_Binary_file_Read, Farm_ReadContiguousSLM)
{
    make_mpiio(big_trace_segy_file());

    const size_t nt = 40000U;
    const size_t ns = 32000U;

    const auto trace_bufer = read_contiguous(file, nt, ns, 0);
    piol->assert_ok();
    test_il_xl_metadata(file, nt, ns, 0, trace_bufer);
}

TEST_F(MPI_Binary_file_Read, Farm_ReadContiguousSLL)
{
    make_mpiio(large_segy_file());

    const size_t nt = 2000000U;
    const size_t ns = 1000U;

    const auto trace_bufer = read_contiguous(file, nt, ns, 0);
    piol->assert_ok();
    test_il_xl_metadata(file, nt, ns, 0, trace_bufer);
}


/// Test the trace data held by the trace_buffer matches the expected test
/// values.
void test_trace_data(
  const std::shared_ptr<Binary_file>& file,
  size_t nt,
  size_t ns,
  size_t offset,
  const std::vector<unsigned char>& trace_buffer)
{
    const auto trace_packet_size = segy::segy_trace_size(ns);

    nt = modify_nt(file->get_file_size(), offset, nt, ns);
    for (size_t i = 0; i < nt; i++) {

        const unsigned char* md =
          &trace_buffer[trace_packet_size * i + segy::segy_trace_header_size()];

        for (size_t k = 0; k < ns; k++) {

            const float f = i + k;
            uint32_t n    = 0;
            std::memcpy(&n, &f, sizeof(uint32_t));

            ASSERT_EQ(md[4 * k + 0], n >> 24 & 0xFF);
            ASSERT_EQ(md[4 * k + 1], n >> 16 & 0xFF);
            ASSERT_EQ(md[4 * k + 2], n >> 8 & 0xFF);
            ASSERT_EQ(md[4 * k + 3], n & 0xFF);
        }
    }
}

TEST_F(MPI_Binary_file_Read, Farm_ReadContiguousLLM)
{
    make_mpiio(big_trace_segy_file());

    const size_t nt = 40000U;
    const size_t ns = 32000U;

    const auto trace_bufer = read_contiguous(file, nt, ns, 0);
    piol->assert_ok();
    test_trace_data(file, nt, ns, 0, trace_bufer);
}

TEST_F(MPI_Binary_file_Read, Farm_ReadContiguousMLL)
{
    make_mpiio(large_segy_file());

    const size_t nt = 2000000U;
    const size_t ns = 1000U;

    const auto trace_bufer = read_contiguous(file, nt, ns, 0);
    piol->assert_ok();
    test_trace_data(file, nt, ns, 0, trace_bufer);
}


TEST_F(MPIIOTest, ReadBlocksZero)
{
    make_mpiio(small_segy_file());

    const size_t nt = 0;
    const size_t ns = 0;

    read_small_blocks<true>(nt, ns);
    m_piol->assert_ok();
}

TEST_F(MPIIOTest, ReadBlocksSSS)
{
    make_mpiio(small_segy_file());
    const size_t nt = 400;
    const size_t ns = 261;
    read_small_blocks<true>(nt, ns);
    m_piol->assert_ok();
}

TEST_F(MPIIOTest, ReadBlocksEnd)
{
    make_mpiio(small_segy_file());
    size_t nt       = 400;
    const size_t ns = 261;

    // Read extra
    nt *= 1024;
    read_small_blocks<true>(nt, ns, 200);
    m_piol->assert_ok();
}

TEST_F(MPIIOTest, ReadBlocksSLS)
{
    make_mpiio(big_trace_segy_file());
    const size_t nt = 40U;
    const size_t ns = 32000U;
    read_small_blocks<true>(nt, ns, 1000U);
    m_piol->assert_ok();
}

TEST_F(MPIIOTest, Farm_ReadBlocksSLM)
{
    make_mpiio(big_trace_segy_file());
    const size_t nt = 40000U;
    const size_t ns = 32000U;
    read_small_blocks<true>(nt, ns);
    m_piol->assert_ok();
}

TEST_F(MPIIOTest, Farm_ReadBlocksSLL)
{
    make_mpiio(large_segy_file());
    const size_t nt = 2000000U;
    const size_t ns = 1000U;
    read_small_blocks<true>(nt, ns);
    m_piol->assert_ok();
}

TEST_F(MPIIOTest, Farm_ReadBlocksLLM)
{
    make_mpiio(big_trace_segy_file());
    const size_t nt = 40000U;
    const size_t ns = 32000U;
    read_big_blocks<true>(nt, ns);
    m_piol->assert_ok();
}

TEST_F(MPIIOTest, Farm_ReadBlocksMLL)
{
    make_mpiio(large_segy_file());
    const size_t nt = 2000000U;
    const size_t ns = 1000U;
    read_big_blocks<true>(nt, ns);
    m_piol->assert_ok();
}

///////Lists//////////
const size_t largens = 1000U;
const size_t largent = 2000000U;
const size_t smallns = 261U;
const size_t smallnt = 400U;

TEST_F(MPIIOTest, ReadListZero)
{
    make_mpiio(small_segy_file());
    read_list(0, 0, NULL);
    m_piol->assert_ok();
}

TEST_F(MPIIOTest, ReadListSmall)
{
    make_mpiio(small_segy_file());
    auto vec = get_random_vec(smallnt / 2, smallnt, 1337);
    read_list(smallnt / 2, smallns, vec.data());
    m_piol->assert_ok();
}


TEST_F(MPIIOTest, Farm_ReadListLarge)
{
    make_mpiio(large_segy_file());
    auto vec = get_random_vec(largent / 2, largent, 1337);

    read_list(largent / 2, largens, vec.data());
    m_piol->assert_ok();
}
