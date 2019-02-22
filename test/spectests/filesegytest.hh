#ifndef EXSEISDAT_TEST_SPECTESTS_FILESEGYTEST_HH
#define EXSEISDAT_TEST_SPECTESTS_FILESEGYTEST_HH

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "tglobal.hh"

#include "segymdextra.hh"

#include "exseisdat/piol/CommunicatorMPI.hh"
#include "exseisdat/piol/ExSeis.hh"
#include "exseisdat/piol/ObjectSEGY.hh"
#include "exseisdat/piol/ReadInterface.hh"
#include "exseisdat/piol/ReadSEGY.hh"
#include "exseisdat/piol/WriteInterface.hh"
#include "exseisdat/piol/WriteSEGY.hh"
#include "exseisdat/piol/mpi/MPI_Binary_file.hh"
#include "exseisdat/piol/segy/utils.hh"
#include "exseisdat/utils/encoding/character_encoding.hh"
#include "exseisdat/utils/encoding/number_encoding.hh"

#include <algorithm>
#include <memory>
#include <random>
#include <string.h>
#include <typeinfo>


using namespace testing;
using namespace exseis::utils;
using namespace exseis::piol;

struct ReadSEGY_public : public ReadSEGY {
    using ReadSEGY::ReadSEGY;

    using ReadSEGY::m_name;
    using ReadSEGY::m_obj;
    using ReadSEGY::m_piol;

    using ReadSEGY::m_ns;
    using ReadSEGY::m_nt;
    using ReadSEGY::m_sample_interval;
    using ReadSEGY::m_text;

    ReadSEGY_public(
      std::shared_ptr<ExSeisPIOL> piol,
      std::string name,
      std::shared_ptr<ObjectInterface> obj) :
        ReadSEGY(piol, name, {}, obj)
    {
    }

    static ReadSEGY_public* get(ReadInterface* read_interface)
    {
        auto* read_segy_public = dynamic_cast<ReadSEGY_public*>(read_interface);

        assert(read_segy_public != nullptr);

        return read_segy_public;
    }
};

struct WriteSEGY_public : public WriteSEGY {
    using WriteSEGY::WriteSEGY;

    using WriteSEGY::m_name;
    using WriteSEGY::m_obj;
    using WriteSEGY::m_piol;

    using WriteSEGY::m_ns;
    using WriteSEGY::m_nt;
    using WriteSEGY::m_sample_interval;
    using WriteSEGY::m_text;

    WriteSEGY_public(
      std::shared_ptr<ExSeisPIOL> piol,
      std::string name,
      std::shared_ptr<ObjectInterface> obj) :
        WriteSEGY(piol, name, {}, obj)
    {
    }

    static WriteSEGY_public* get(WriteInterface* write_interface)
    {
        auto* write_segy_public =
          dynamic_cast<WriteSEGY_public*>(write_interface);

        assert(write_segy_public != nullptr);

        return write_segy_public;
    }
};


enum Hdr : size_t {
    sample_interval = 3216U,
    NumSample       = 3220U,
    Type            = 3224U,
    Sort            = 3228U,
    Units           = 3254U,
    SEGYFormat      = 3500U,
    FixedTrace      = 3502U,
    Extensions      = 3504U,
};

enum TrHdr : size_t {
    SeqNum      = 0U,
    SeqFNum     = 4U,
    ORF         = 8U,
    TORF        = 12U,
    RcvElv      = 40U,
    SurfElvSrc  = 44U,
    SrcDpthSurf = 48U,
    DtmElvRcv   = 52U,
    DtmElvSrc   = 56U,
    WtrDepSrc   = 60U,
    WtrDepRcv   = 64U,
    ScaleElev   = 68U,
    ScaleCoord  = 70U,
    x_src       = 72U,
    y_src       = 76U,
    x_rcv       = 80U,
    y_rcv       = 84U,
    xCMP        = 180U,
    yCMP        = 184U,
    il          = 188U,
    xl          = 192U
};

class Mock_Binary_file : public Binary_file {
    MOCK_CONST_METHOD0(is_open, bool());
    MOCK_CONST_METHOD0(get_file_size, size_t());
    MOCK_CONST_METHOD1(set_file_size, void(size_t size));
    MOCK_CONST_METHOD3(read, void(size_t offset, size_t size, void* buffer));
    MOCK_CONST_METHOD3(
      write, void(size_t offset, size_t size, const void* buffer));
    MOCK_CONST_METHOD5(
      read_noncontiguous,
      void(
        size_t offset,
        size_t block_size,
        size_t stride_size,
        size_t number_of_blocks,
        void* buffer));
    MOCK_CONST_METHOD5(
      write_noncontiguous,
      void(
        size_t offset,
        size_t block_size,
        size_t stride_size,
        size_t number_of_blocks,
        const void* buffer));
    MOCK_CONST_METHOD4(
      read_noncontiguous_irregular,
      void(
        size_t block_size,
        size_t number_of_blocks,
        const size_t* offsets,
        void* buffer));
    MOCK_CONST_METHOD4(
      write_noncontiguous_irregular,
      void(
        size_t block_size,
        size_t number_of_blocks,
        const size_t* offsets,
        const void* buffer));
};

class Mock_Object : public ObjectInterface {
  private:
    std::shared_ptr<ExSeisPIOL> m_piol;
    std::string m_name;

  public:
    Mock_Object(std::shared_ptr<ExSeisPIOL> piol, const std::string name) :
        m_piol(piol),
        m_name(name)
    {
    }

    MOCK_CONST_METHOD0(piol, std::shared_ptr<ExSeisPIOL>());
    MOCK_CONST_METHOD0(name, std::string());
    MOCK_CONST_METHOD0(data, std::shared_ptr<Binary_file>());


    MOCK_CONST_METHOD0(get_file_size, size_t(void));
    MOCK_CONST_METHOD1(read_ho, void(unsigned char*));
    MOCK_CONST_METHOD1(set_file_size, void(const size_t));
    MOCK_CONST_METHOD1(should_write_file_header, void(const unsigned char*));
    MOCK_CONST_METHOD4(
      read_trace_metadata,
      void(const size_t, const size_t, const size_t, unsigned char*));
    MOCK_CONST_METHOD4(
      write_trace_metadata,
      void(const size_t, const size_t, const size_t, const unsigned char*));

    MOCK_CONST_METHOD4(
      read_trace_data,
      void(const size_t, const size_t, const size_t, unsigned char*));
    MOCK_CONST_METHOD4(
      write_trace_data,
      void(const size_t, const size_t, const size_t, const unsigned char*));
    MOCK_CONST_METHOD4(
      read_trace,
      void(const size_t, const size_t, const size_t, unsigned char*));
    MOCK_CONST_METHOD4(
      write_trace,
      void(const size_t, const size_t, const size_t, const unsigned char*));

    MOCK_CONST_METHOD4(
      read_trace,
      void(const size_t*, const size_t, const size_t, unsigned char*));
    MOCK_CONST_METHOD4(
      write_trace,
      void(const size_t*, const size_t, const size_t, const unsigned char*));
    MOCK_CONST_METHOD4(
      read_trace_data,
      void(const size_t*, const size_t, const size_t, unsigned char*));
    MOCK_CONST_METHOD4(
      write_trace_data,
      void(const size_t*, const size_t, const size_t, const unsigned char*));
    MOCK_CONST_METHOD4(
      read_trace_metadata,
      void(const size_t*, const size_t, const size_t, unsigned char*));
    MOCK_CONST_METHOD4(
      write_trace_metadata,
      void(const size_t*, const size_t, const size_t, const unsigned char*));
};

struct FileReadSEGYTest : public Test {
    std::shared_ptr<ExSeis> piol = ExSeis::make();

    bool test_ebcdic = false;

    std::string test_string = {
      "This is a string for testing EBCDIC conversion etc.\n"
      "The quick brown fox jumps over the lazy dog."};

    // The test_string in EBCDIC encoding.
    std::string ebcdic_test_string = {
      // This is a string for testing EBCDIC conversion etc.\n
      "\xE3\x88\x89\xA2\x40\x89\xA2\x40\x81\x40\xA2\xA3\x99\x89\x95\x87\x40"
      "\x86\x96\x99\x40\xA3\x85\xA2\xA3\x89\x95\x87\x40\xC5\xC2\xC3\xC4\xC9"
      "\xC3\x40\x83\x96\x95\xA5\x85\x99\xA2\x89\x96\x95\x40\x85\xA3\x83\x4B"
      "\x25"

      // The quick brown fox jumps over the lazy dog.
      "\xE3\x88\x85\x40\x98\xA4\x89\x83\x92\x40\x82\x99\x96\xA6\x95\x40\x86"
      "\x96\xA7\x40\x91\xA4\x94\x97\xA2\x40\x96\xA5\x85\x99\x40\xA3\x88\x85"
      "\x40\x93\x81\xA9\xA8\x40\x84\x96\x87\x4B"};

    std::unique_ptr<ReadInterface> file = nullptr;

    std::vector<unsigned char> tr;

    size_t nt           = 40U;
    size_t ns           = 200U;
    int sample_interval = 10;
    const size_t format = 5;

    std::vector<unsigned char> ho =
      std::vector<unsigned char>(segy::segy_binary_file_header_size());

    std::shared_ptr<Mock_Object> mock_object;

    ~FileReadSEGYTest() { Mock::VerifyAndClearExpectations(&mock_object); }

    // Make a ReadInterface with SEGY object layer.
    template<bool OPTS = false>
    void make_segy(std::string name)
    {
        if (file.get() != nullptr) {
            file.reset();
        }

        // Make a ReadInterface reader with options if OPTS = true.
        if (OPTS) {
            ReadSEGY::Options opt;
            file = std::make_unique<ReadSEGY>(piol, name, opt);
        }
        else {
            file = std::make_unique<ReadSEGY>(piol, name);
        }

        piol->assert_ok();
    }

    // Make a ReadInterface with mock Object layer instance. Set this.file to
    // it.
    std::string make_mock_segy()
    {
        if (file.get() != nullptr) {
            file.reset();
        }
        if (mock_object != nullptr) {
            mock_object.reset();
        }

        auto filename = nonexistant_filename();
        mock_object   = std::make_shared<Mock_Object>(piol, filename);
        piol->assert_ok();
        Mock::AllowLeak(mock_object.get());

        // Add EBCIDIC or ASCII string to start of header.
        if (test_ebcdic) {
            // Create an EBCDID string to convert back to ASCII in the test
            std::copy(
              std::begin(ebcdic_test_string), std::end(ebcdic_test_string),
              std::begin(ho));
        }
        else {
            for (size_t i = 0; i < test_string.size(); i++) {
                ho[i] = test_string[i];
            }
        }

        // Fill the header to segy::segy_text_header_size().
        if (!test_string.empty()) {
            for (size_t i = test_string.size();
                 i < segy::segy_text_header_size(); i++) {
                ho[i] = ho[i % test_string.size()];
            }
        }

        // Set the number of samples (in Big Endian)
        ho[NumSample]     = ns >> 8 & 0xFF;
        ho[NumSample + 1] = ns & 0xFF;

        // Set the sample increment (in Big Endian)
        ho[Hdr::sample_interval]     = sample_interval >> 8 & 0xFF;
        ho[Hdr::sample_interval + 1] = sample_interval & 0xFF;

        // Set the number type
        ho[Hdr::Type + 1] = format;

        EXPECT_CALL(*mock_object, get_file_size())
          .Times(Exactly(1))
          .WillOnce(Return(
            segy::segy_binary_file_header_size()
            + nt * segy::segy_trace_size(ns)));

        EXPECT_CALL(*mock_object, read_ho(_))
          .Times(Exactly(1))
          .WillOnce(SetArrayArgument<0>(ho.begin(), ho.end()));

        // Use ReadSEGY_public so test functions can access the ReadSEGY
        // internals.
        file = std::make_unique<ReadSEGY_public>(piol, filename, mock_object);

        return filename;
    }

    // Initialize the trace data array and the trace headers in that array.
    void init_tr_block()
    {
        // Set the right size
        tr.resize(nt * segy::segy_trace_header_size());

        // Loop over the traces
        for (size_t i = 0; i < nt; i++) {
            // Set md to the start of the current trace
            unsigned char* md = &tr[i * segy::segy_trace_header_size()];

            // Set the inline, crossline values for the current trace
            const auto be_il_num = to_big_endian(il_num(i));
            const auto be_xl_num = to_big_endian(xl_num(i));

            std::copy(std::begin(be_il_num), std::end(be_il_num), &md[il]);
            std::copy(std::begin(be_xl_num), std::end(be_xl_num), &md[xl]);


            // Set the scale for the current trace
            int16_t scale;
            int16_t scal1 = segy::find_scalar(x_num(i));
            int16_t scal2 = segy::find_scalar(y_num(i));

            if (scal1 > 1 || scal2 > 1) {
                scale = std::max(scal1, scal2);
            }
            else {
                scale = std::min(scal1, scal2);
            }

            const auto be_scale = to_big_endian(scale);
            std::copy(
              std::begin(be_scale), std::end(be_scale), &md[ScaleCoord]);


            // Set the x and y source header.
            const auto be_x_num =
              to_big_endian(int32_t(std::lround(x_num(i) / scale)));
            const auto be_y_num =
              to_big_endian(int32_t(std::lround(y_num(i) / scale)));

            std::copy(std::begin(be_x_num), std::end(be_x_num), &md[x_src]);
            std::copy(std::begin(be_y_num), std::end(be_y_num), &md[y_src]);
        }
    }

    void init_read_tr_mock(size_t expected_ns, size_t offset)
    {
        std::vector<unsigned char>::iterator iter =
          tr.begin() + offset * segy::segy_trace_header_size();

        EXPECT_CALL(
          *mock_object.get(), read_trace_metadata(offset, expected_ns, 1U, _))
          .Times(Exactly(1))
          .WillRepeatedly(
            SetArrayArgument<3>(iter, iter + segy::segy_trace_header_size()));

        Trace_metadata prm(1U);
        file->read_param(offset, 1U, &prm);
        ASSERT_EQ(il_num(offset), prm.get_integer(0U, Meta::il));
        ASSERT_EQ(xl_num(offset), prm.get_integer(0U, Meta::xl));

        if (sizeof(exseis::utils::Floating_point) == sizeof(double)) {
            ASSERT_DOUBLE_EQ(
              x_num(offset), prm.get_floating_point(0U, Meta::x_src));
            ASSERT_DOUBLE_EQ(
              y_num(offset), prm.get_floating_point(0U, Meta::y_src));
        }
        else {
            ASSERT_FLOAT_EQ(
              x_num(offset), prm.get_floating_point(0U, Meta::x_src));
            ASSERT_FLOAT_EQ(
              y_num(offset), prm.get_floating_point(0U, Meta::y_src));
        }
    }

    void init_read_tr_hdrs_mock(size_t expected_ns, size_t tn)
    {
        size_t zero = 0U;
        EXPECT_CALL(
          *mock_object.get(), read_trace_metadata(zero, expected_ns, tn, _))
          .Times(Exactly(1))
          .WillRepeatedly(SetArrayArgument<3>(tr.begin(), tr.end()));

        const auto rule = Rule(std::initializer_list<Meta>{
          Meta::il, Meta::xl, Meta::Copy, Meta::x_src, Meta::y_src});
        Trace_metadata prm(std::move(rule), tn);
        file->read_param(0, tn, &prm);

        for (size_t i = 0; i < tn; i++) {
            ASSERT_EQ(il_num(i), prm.get_integer(i, Meta::il));
            ASSERT_EQ(xl_num(i), prm.get_integer(i, Meta::xl));

            if (sizeof(exseis::utils::Floating_point) == sizeof(double)) {
                ASSERT_DOUBLE_EQ(
                  x_num(i), prm.get_floating_point(i, Meta::x_src));
                ASSERT_DOUBLE_EQ(
                  y_num(i), prm.get_floating_point(i, Meta::y_src));
            }
            else {
                ASSERT_FLOAT_EQ(
                  x_num(i), prm.get_floating_point(i, Meta::x_src));
                ASSERT_FLOAT_EQ(
                  y_num(i), prm.get_floating_point(i, Meta::y_src));
            }
        }
        ASSERT_TRUE(tr.size());
        ASSERT_THAT(prm.raw_metadata, ContainerEq(tr));
    }

    void init_rand_read_tr_hdrs_mock(size_t expected_ns, size_t tn)
    {
        Trace_metadata prm(tn);
        std::vector<size_t> offset(tn);
        std::iota(offset.begin(), offset.end(), 0);

        std::random_device rand;
        std::mt19937 mt(rand());
        std::shuffle(offset.begin(), offset.end(), mt);

        EXPECT_CALL(
          *mock_object.get(),
          read_trace_metadata(A<const size_t*>(), expected_ns, tn, _))
          .Times(Exactly(1))
          .WillRepeatedly(SetArrayArgument<3>(tr.begin(), tr.end()));

        file->read_trace_non_monotonic(tn, offset.data(), nullptr, &prm);

        for (size_t i = 0; i < tn; i++) {
            ASSERT_EQ(il_num(offset[i]), prm.get_integer(i, Meta::il));
            ASSERT_EQ(xl_num(offset[i]), prm.get_integer(i, Meta::xl));

            if (sizeof(exseis::utils::Floating_point) == sizeof(double)) {
                ASSERT_DOUBLE_EQ(
                  x_num(offset[i]), prm.get_floating_point(i, Meta::x_src));
                ASSERT_DOUBLE_EQ(
                  y_num(offset[i]), prm.get_floating_point(i, Meta::y_src));
            }
            else {
                ASSERT_FLOAT_EQ(
                  x_num(offset[i]), prm.get_floating_point(i, Meta::x_src));
                ASSERT_FLOAT_EQ(
                  y_num(offset[i]), prm.get_floating_point(i, Meta::y_src));
            }
        }
    }

    /// Test reading the trace data. The data read should match the data
    /// previously defined.
    /// @param[in] offset The trace to start reading from
    /// @param[in] tn     The number of traces to read
    template<bool ReadPrm = false, bool UseMock = true, bool RmRule = false>
    void read_trace_test(const size_t offset, size_t tn)
    {
        size_t tn_read = tn;
        if (nt > offset) {
            tn_read = std::min(nt - offset, tn);
        }

        std::vector<unsigned char> buf;

        bool mock_read_trace_called = true;
        if (UseMock) {
            mock_read_trace_called = false;

            if (mock_object == nullptr) {
                std::cerr << "Using Mock when not initialised: LOC: "
                          << __LINE__ << std::endl;
                return;
            }
            if (ReadPrm) {
                // Reading Trace_metadatas, need to read whole data obect
                buf.resize(tn_read * segy::segy_trace_size(ns));
            }
            else {
                // Not reading params, only need to read the trace data
                buf.resize(tn_read * segy::segy_trace_data_size(ns));
            }

            for (size_t i = 0U; i < tn_read; i++) {
                if (ReadPrm) {
                    std::copy(
                      tr.begin()
                        + (offset + i) * segy::segy_trace_header_size(),
                      tr.begin()
                        + (offset + i + 1) * segy::segy_trace_header_size(),
                      buf.begin() + i * segy::segy_trace_size(ns));
                }

                for (size_t j = 0U; j < ns; j++) {
                    const float val = offset + i + j;

                    size_t addr = ReadPrm ? (i * segy::segy_trace_size(ns)
                                             + segy::segy_trace_header_size()
                                             + j * sizeof(float)) :
                                            (i * ns + j) * sizeof(float);

                    const auto be_val = to_big_endian<float>(val);
                    std::copy(std::begin(be_val), std::end(be_val), &buf[addr]);
                }
            }


            // Set up the mock functions to return the test data

            EXPECT_CALL(*mock_object, read_trace(offset, ns, tn_read, _))
              .Times(AnyNumber())
              .WillOnce(DoAll(
                SetArrayArgument<3>(buf.begin(), buf.end()),
                InvokeWithoutArgs([&] { mock_read_trace_called = true; })));

            EXPECT_CALL(
              *mock_object, read_trace_metadata(offset, ns, tn_read, _))
              .Times(AnyNumber())
              .WillOnce(DoAll(
                SetArrayArgument<3>(buf.begin(), buf.end()),
                InvokeWithoutArgs([&] { mock_read_trace_called = true; })));

            EXPECT_CALL(*mock_object, read_trace_data(offset, ns, tn_read, _))
              .Times(AnyNumber())
              .WillOnce(DoAll(
                SetArrayArgument<3>(buf.begin(), buf.end()),
                InvokeWithoutArgs([&] { mock_read_trace_called = true; })));
        }

        std::vector<exseis::utils::Trace_value> bufnew(tn * ns);
        auto rule = Rule(true, true);
        if (RmRule) {
            rule.rm_rule(Meta::x_src);
            rule.add_segy_float(Meta::ShotNum, Tr::UpSrc, Tr::UpRcv);
            rule.add_long(Meta::Misc1, Tr::TORF);
            rule.add_short(Meta::Misc2, Tr::ShotNum);
            rule.add_short(Meta::Misc3, Tr::ShotScal);
            rule.rm_rule(Meta::ShotNum);
            rule.rm_rule(Meta::Misc1);
            rule.rm_rule(Meta::Misc2);
            rule.rm_rule(Meta::Misc3);
            rule.rm_rule(Meta::y_src);
            rule.add_segy_float(Meta::x_src, Tr::x_src, Tr::ScaleCoord);
            rule.add_segy_float(Meta::y_src, Tr::y_src, Tr::ScaleCoord);
        }

        Trace_metadata prm(std::move(rule), tn);
        file->read_trace(offset, tn, bufnew.data(), (ReadPrm ? &prm : nullptr));

        ASSERT_TRUE(mock_read_trace_called);

        for (size_t i = 0U; i < tn_read; i++) {
            if (ReadPrm && tn_read && ns) {

                ASSERT_EQ(il_num(i + offset), prm.get_integer(i, Meta::il))
                  << "Trace Number " << i << " offset " << offset;
                ASSERT_EQ(xl_num(i + offset), prm.get_integer(i, Meta::xl))
                  << "Trace Number " << i << " offset " << offset;

                if (sizeof(exseis::utils::Floating_point) == sizeof(double)) {
                    ASSERT_DOUBLE_EQ(
                      x_num(i + offset),
                      prm.get_floating_point(i, Meta::x_src));

                    ASSERT_DOUBLE_EQ(
                      y_num(i + offset),
                      prm.get_floating_point(i, Meta::y_src));
                }
                else {
                    ASSERT_FLOAT_EQ(
                      x_num(i + offset),
                      prm.get_floating_point(i, Meta::x_src));

                    ASSERT_FLOAT_EQ(
                      y_num(i + offset),
                      prm.get_floating_point(i, Meta::y_src));
                }
            }

            for (size_t j = 0U; j < ns; j++) {
                ASSERT_EQ(bufnew[i * ns + j], float(offset + i + j))
                  << "Trace Number: " << i << " " << j;
            }
        }
    }

    template<bool ReadPrm = false, bool UseMock = true>
    void read_random_trace_test(size_t tn, const std::vector<size_t> offset)
    {
        ASSERT_EQ(tn, offset.size());
        std::vector<unsigned char> buf;

        bool mock_read_trace_called = true;
        if (UseMock) {
            mock_read_trace_called = false;

            if (mock_object == nullptr) {
                std::cerr << "Using Mock when not initialised: LOC: "
                          << __LINE__ << std::endl;
                return;
            }

            if (ReadPrm) {
                buf.resize(tn * segy::segy_trace_size(ns));
            }
            else {
                buf.resize(tn * segy::segy_trace_data_size(ns));
            }

            for (size_t i = 0U; i < tn; i++) {
                if (ReadPrm && ns && tn) {
                    std::copy(
                      tr.begin() + offset[i] * segy::segy_trace_header_size(),
                      tr.begin()
                        + (offset[i] + 1) * segy::segy_trace_header_size(),
                      buf.begin() + i * segy::segy_trace_size(ns));
                }

                for (size_t j = 0U; j < ns; j++) {
                    size_t addr = ReadPrm ? (i * segy::segy_trace_size(ns)
                                             + segy::segy_trace_header_size()
                                             + j * sizeof(float)) :
                                            (i * ns + j) * sizeof(float);

                    const float val = offset[i] + j;

                    const auto be_val = to_big_endian<float>(val);
                    std::copy(std::begin(be_val), std::end(be_val), &buf[addr]);
                }
            }


            // Setup mock calls to return the test data

            EXPECT_CALL(*mock_object, read_trace(offset.data(), ns, tn, _))
              .Times(AnyNumber())
              .WillOnce(DoAll(
                SetArrayArgument<3>(buf.begin(), buf.end()),
                InvokeWithoutArgs([&] { mock_read_trace_called = true; })));

            EXPECT_CALL(
              *mock_object, read_trace_metadata(offset.data(), ns, tn, _))
              .Times(AnyNumber())
              .WillOnce(DoAll(
                SetArrayArgument<3>(buf.begin(), buf.end()),
                InvokeWithoutArgs([&] { mock_read_trace_called = true; })));

            EXPECT_CALL(*mock_object, read_trace_data(offset.data(), ns, tn, _))
              .Times(AnyNumber())
              .WillOnce(DoAll(
                SetArrayArgument<3>(buf.begin(), buf.end()),
                InvokeWithoutArgs([&] { mock_read_trace_called = true; })));
        }

        std::vector<float> bufnew(tn * ns);
        Trace_metadata prm(tn);
        if (ReadPrm) {
            file->read_trace_non_contiguous(
              tn, offset.data(), bufnew.data(), &prm);
        }
        else {
            file->read_trace_non_contiguous(tn, offset.data(), bufnew.data());
        }

        ASSERT_TRUE(mock_read_trace_called);

        for (size_t i = 0U; i < tn; i++) {
            if (ReadPrm && tn && ns) {
                ASSERT_EQ(il_num(offset[i]), prm.get_integer(i, Meta::il))
                  << "Trace Number " << i << " offset " << offset[i];
                ASSERT_EQ(xl_num(offset[i]), prm.get_integer(i, Meta::xl))
                  << "Trace Number " << i << " offset " << offset[i];

                if (sizeof(exseis::utils::Floating_point) == sizeof(double)) {
                    ASSERT_DOUBLE_EQ(
                      x_num(offset[i]), prm.get_floating_point(i, Meta::x_src));
                    ASSERT_DOUBLE_EQ(
                      y_num(offset[i]), prm.get_floating_point(i, Meta::y_src));
                }
                else {
                    ASSERT_FLOAT_EQ(
                      x_num(offset[i]), prm.get_floating_point(i, Meta::x_src));
                    ASSERT_FLOAT_EQ(
                      y_num(offset[i]), prm.get_floating_point(i, Meta::y_src));
                }
            }

            for (size_t j = 0U; j < ns; j++) {
                ASSERT_EQ(bufnew[i * ns + j], float(offset[i] + j))
                  << "Trace Number: " << offset[i] << " " << j;
            }
        }
    }
};

struct FileWriteSEGYTest : public Test {
    std::shared_ptr<Mock_Object> mock_object;

    std::shared_ptr<ExSeis> piol = ExSeis::make();
    bool test_ebcdic             = false;
    std::string test_string      = {
      "This is a string for testing EBCDIC conversion etc."};
    std::string file_name;
    std::vector<unsigned char> tr;
    size_t nt           = 40U;
    size_t ns           = 200U;
    int sample_interval = 10;
    const size_t format = 5;
    std::vector<unsigned char> ho =
      std::vector<unsigned char>(segy::segy_binary_file_header_size());
    std::unique_ptr<WriteInterface> file = nullptr;
    std::shared_ptr<MPI_Binary_file> output_binary_file;
    std::unique_ptr<ReadInterface> readfile;

    ~FileWriteSEGYTest() { Mock::VerifyAndClearExpectations(&mock_object); }

    void make_segy()
    {
        file_name = temp_file();
        if (file.get() != nullptr) {
            file.reset();
        }
        piol->assert_ok();

        file = std::make_unique<WriteSEGY_public>(piol, file_name);

        should_write_file_header<false>();


        readfile = std::make_unique<ReadSEGY_public>(piol, file_name);

        ReadSEGY_public::get(readfile.get())->m_nt = nt;
        ReadSEGY_public::get(readfile.get())->m_ns = ns;
        ReadSEGY_public::get(readfile.get())->m_sample_interval =
          sample_interval;
        ReadSEGY_public::get(readfile.get())->m_text = test_string;
    }

    template<bool CallHo = true>
    void make_mock_segy()
    {
        if (file.get() != nullptr) {
            file.reset();
        }
        if (mock_object != nullptr) {
            mock_object.reset();
        }
        auto filename = nonexistant_filename();
        mock_object   = std::make_shared<Mock_Object>(piol, filename);
        piol->assert_ok();
        Mock::AllowLeak(mock_object.get());

        auto sfile =
          std::make_unique<WriteSEGY_public>(piol, filename, mock_object);
        if (!CallHo) {
            sfile->m_nt = nt;
            sfile->write_ns(ns);
        }

        file               = std::move(sfile);
        output_binary_file = nullptr;

        if (CallHo) {
            piol->assert_ok();
            should_write_file_header<true>();
        }
    }

    void init_tr_block()
    {
        tr.resize(nt * segy::segy_trace_header_size());
        for (size_t i = 0; i < nt; i++) {
            unsigned char* md = &tr[i * segy::segy_trace_header_size()];

            const auto be_il_num = to_big_endian(il_num(i));
            const auto be_xl_num = to_big_endian(xl_num(i));

            std::copy(std::begin(be_il_num), std::end(be_il_num), &md[il]);
            std::copy(std::begin(be_xl_num), std::end(be_xl_num), &md[xl]);


            int16_t scale;
            int16_t scal1 = segy::find_scalar(x_num(i));
            int16_t scal2 = segy::find_scalar(y_num(i));

            if (scal1 > 1 || scal2 > 1) {
                scale = std::max(scal1, scal2);
            }
            else {
                scale = std::min(scal1, scal2);
            }

            const auto be_scale = to_big_endian(scale);
            std::copy(
              std::begin(be_scale), std::end(be_scale), &md[ScaleCoord]);


            const auto be_x_num =
              to_big_endian(int32_t(std::lround(x_num(i) / scale)));
            const auto be_y_num =
              to_big_endian(int32_t(std::lround(y_num(i) / scale)));

            std::copy(std::begin(be_x_num), std::end(be_x_num), &md[x_src]);
            std::copy(std::begin(be_y_num), std::end(be_y_num), &md[y_src]);
        }
    }

    template<bool UseMock = true>
    void should_write_file_header()
    {
        if (UseMock) {
            size_t fsz = segy::segy_binary_file_header_size()
                         + nt * segy::segy_trace_size(ns);
            EXPECT_CALL(*mock_object, set_file_size(fsz)).Times(Exactly(1));

            for (size_t i = 0U;
                 i
                 < std::min(test_string.size(), segy::segy_text_header_size());
                 i++) {
                ho[i] = test_string[i];
            }

            ho[Hdr::NumSample + 1]       = ns & 0xFF;
            ho[Hdr::NumSample]           = ns >> 8 & 0xFF;
            ho[Hdr::sample_interval + 1] = sample_interval & 0xFF;
            ho[Hdr::sample_interval]     = sample_interval >> 8 & 0xFF;
            ho[Hdr::Type + 1]            = format;
            ho[3255U]                    = 1;
            ho[3500U]                    = 1;
            ho[3503U]                    = 1;
            ho[3505U]                    = 0;

            EXPECT_CALL(*mock_object, should_write_file_header(_))
              .Times(Exactly(1))
              .WillOnce(DoAll(
                Invoke([=](const unsigned char* ho_to_test) {
                    // Make sure only one process calls this with a
                    // buffer to write
                    int is_not_null    = (ho_to_test != nullptr) ? 1 : 0;
                    int total_not_null = piol->comm->sum(is_not_null);
                    ASSERT_TRUE(total_not_null == 1);
                }),
                Invoke([ho = ho](const unsigned char* ho_to_test) {
                    if (ho_to_test != nullptr) {

                        for (size_t i = 0;
                             i < segy::segy_binary_file_header_size(); i++) {
                            ASSERT_EQ(ho[i], ho_to_test[i])
                              << "Error with byte: " << i << "\n";
                        }
                    }
                })));
        }

        file->write_nt(nt);
        piol->assert_ok();

        file->write_ns(ns);
        piol->assert_ok();

        const double microsecond = 1e-6;
        file->write_sample_interval(sample_interval * microsecond);
        piol->assert_ok();

        file->write_text(test_string);
        piol->assert_ok();
    }

    void write_tr_hdr_grid_test(size_t offset)
    {
        std::vector<unsigned char> test_trace_metadata(
          segy::segy_trace_header_size());

        const auto be_il_num = to_big_endian(il_num(offset));
        const auto be_xl_num = to_big_endian(xl_num(offset));

        std::copy(
          std::begin(be_il_num), std::end(be_il_num),
          test_trace_metadata.data() + il);
        std::copy(
          std::begin(be_xl_num), std::end(be_xl_num),
          test_trace_metadata.data() + xl);


        const auto be_scale_coord = to_big_endian<int16_t>(1);
        std::copy(
          std::begin(be_scale_coord), std::end(be_scale_coord),
          &test_trace_metadata[ScaleCoord]);

        const auto be_seqfnum = to_big_endian(int32_t(offset));
        std::copy(
          std::begin(be_seqfnum), std::end(be_seqfnum),
          &test_trace_metadata[SeqFNum]);

        EXPECT_CALL(*mock_object, write_trace_metadata(offset, ns, 1U, _))
          .Times(Exactly(1))
          .WillOnce(
            check3(test_trace_metadata.data(), segy::segy_trace_header_size()));

        Trace_metadata prm(1U);
        prm.set_integer(0, Meta::il, il_num(offset));
        prm.set_integer(0, Meta::xl, xl_num(offset));
        prm.set_integer(0, Meta::tn, offset);
        file->write_param(offset, 1U, &prm);
    }

    void init_write_tr_hdr_coord(
      std::pair<size_t, size_t> item,
      std::pair<int32_t, int32_t> val,
      int16_t scal,
      size_t offset,
      std::vector<unsigned char>* trace_metadata_buffer)
    {
        const auto be_scal = to_big_endian(scal);
        std::copy(
          std::begin(be_scal), std::end(be_scal),
          &trace_metadata_buffer->at(ScaleCoord));

        const auto be_val_first = to_big_endian(val.first);
        std::copy(
          std::begin(be_val_first), std::end(be_val_first),
          &trace_metadata_buffer->at(item.first));

        const auto be_val_second = to_big_endian(val.second);
        std::copy(
          std::begin(be_val_second), std::end(be_val_second),
          &trace_metadata_buffer->at(item.second));

        const auto be_offset = to_big_endian(int32_t(offset));
        std::copy(
          std::begin(be_offset), std::end(be_offset),
          &trace_metadata_buffer->at(SeqFNum));

        EXPECT_CALL(*mock_object, write_trace_metadata(offset, ns, 1U, _))
          .Times(Exactly(1))
          .WillOnce(check3(
            trace_metadata_buffer->data(), segy::segy_trace_header_size()));
    }

    void init_write_headers(size_t file_pos, unsigned char* md)
    {
        coord_t src = coord_t(x_num(file_pos), y_num(file_pos));
        coord_t rcv = coord_t(x_num(file_pos), y_num(file_pos));
        coord_t cmp = coord_t(x_num(file_pos), y_num(file_pos));
        grid_t line = grid_t(il_num(file_pos), xl_num(file_pos));

        int16_t scale = 1;

        scale = scal_comp(1, calc_scale(src));
        scale = scal_comp(scale, calc_scale(rcv));
        scale = scal_comp(scale, calc_scale(cmp));

        const auto be_scale = to_big_endian(scale);
        std::copy(std::begin(be_scale), std::end(be_scale), &md[ScaleCoord]);

        set_coord(Coord::Src, src, scale, md);
        set_coord(Coord::Rcv, rcv, scale, md);
        set_coord(Coord::CMP, cmp, scale, md);

        set_grid(Grid::Line, line, md);

        const auto be_file_pos = to_big_endian(int32_t(file_pos));
        std::copy(std::begin(be_file_pos), std::end(be_file_pos), &md[SeqFNum]);
    }

    template<bool WritePrm = false, bool UseMock = true>
    void write_trace_test(const size_t offset, const size_t tn)
    {
        std::vector<unsigned char> buf;

        bool mock_write_trace_called = true;
        if (UseMock) {
            mock_write_trace_called = false;

            if (mock_object == nullptr) {
                std::cerr << "Using Mock when not initialised: LOC: "
                          << __LINE__ << std::endl;
                return;
            }

            EXPECT_CALL(*mock_object, should_write_file_header(_))
              .Times(Exactly(1));
            EXPECT_CALL(*mock_object, set_file_size(_)).Times(Exactly(1));


            buf.resize(
              tn
              * (WritePrm ? segy::segy_trace_size(ns) :
                            segy::segy_trace_data_size(ns)));

            for (size_t i = 0U; i < tn; i++) {
                if (WritePrm) {
                    init_write_headers(
                      offset + i, &buf[i * segy::segy_trace_size(ns)]);
                }

                for (size_t j = 0U; j < ns; j++) {
                    const size_t addr =
                      WritePrm ?
                        (i * segy::segy_trace_size(ns)
                         + segy::segy_trace_header_size() + j * sizeof(float)) :
                        (i * ns + j) * sizeof(float);

                    const float val = offset + i + j;

                    const auto be_val = to_big_endian<float>(val);
                    std::copy(std::begin(be_val), std::end(be_val), &buf[addr]);
                }
            }

            EXPECT_CALL(*mock_object, write_trace(offset, ns, tn, _))
              .Times(AnyNumber())
              .WillOnce(DoAll(
                check3(buf.data(), buf.size()),
                InvokeWithoutArgs([&] { mock_write_trace_called = true; })));

            EXPECT_CALL(*mock_object, write_trace_metadata(offset, ns, tn, _))
              .Times(AnyNumber())
              .WillOnce(DoAll(
                check3(buf.data(), buf.size()),
                InvokeWithoutArgs([&] { mock_write_trace_called = true; })));

            EXPECT_CALL(*mock_object, write_trace_data(offset, ns, tn, _))
              .Times(AnyNumber())
              .WillOnce(DoAll(
                check3(buf.data(), buf.size()),
                InvokeWithoutArgs([&] { mock_write_trace_called = true; })));
        }
        std::vector<float> bufnew(tn * ns);
        if (WritePrm) {
            Trace_metadata prm(tn);
            for (size_t i = 0U; i < tn; i++) {
                prm.set_floating_point(i, Meta::x_src, x_num(offset + i));
                prm.set_floating_point(i, Meta::x_rcv, x_num(offset + i));
                prm.set_floating_point(i, Meta::xCmp, x_num(offset + i));
                prm.set_floating_point(i, Meta::y_src, y_num(offset + i));
                prm.set_floating_point(i, Meta::y_rcv, y_num(offset + i));
                prm.set_floating_point(i, Meta::yCmp, y_num(offset + i));
                prm.set_integer(i, Meta::il, il_num(offset + i));
                prm.set_integer(i, Meta::xl, xl_num(offset + i));
                prm.set_integer(i, Meta::tn, offset + i);

                for (size_t j = 0U; j < ns; j++) {
                    bufnew[i * ns + j] = float(offset + i + j);
                }
            }

            file->write_trace(offset, tn, bufnew.data(), &prm);
        }
        else {
            for (size_t i = 0U; i < tn; i++) {
                for (size_t j = 0U; j < ns; j++) {
                    bufnew[i * ns + j] = float(offset + i + j);
                }
            }

            file->write_trace(offset, tn, bufnew.data());
        }

        ASSERT_TRUE(mock_write_trace_called);

        if (UseMock == false) {
            ReadSEGY_public::get(readfile.get())->m_nt =
              std::max(offset + tn, ReadSEGY_public::get(readfile.get())->m_nt);
            read_trace_test<WritePrm>(offset, tn);
        }
    }

    template<bool ReadPrm = false>
    void read_trace_test(const size_t offset, const size_t tn)
    {
        size_t tn_read = (offset + tn > nt && nt > offset ? nt - offset : tn);
        std::vector<exseis::utils::Trace_value> bufnew(tn * ns);
        Trace_metadata prm(tn);
        if (ReadPrm) {
            readfile->read_trace(offset, tn, bufnew.data(), &prm);
        }
        else {
            readfile->read_trace(offset, tn, bufnew.data());
        }

        for (size_t i = 0U; i < tn_read; i++) {
            if (ReadPrm && tn_read && ns) {
                ASSERT_EQ(il_num(i + offset), prm.get_integer(i, Meta::il))
                  << "Trace Number " << i << " offset " << offset;
                ASSERT_EQ(xl_num(i + offset), prm.get_integer(i, Meta::xl))
                  << "Trace Number " << i << " offset " << offset;

                if (sizeof(exseis::utils::Floating_point) == sizeof(double)) {
                    ASSERT_DOUBLE_EQ(
                      x_num(i + offset),
                      prm.get_floating_point(i, Meta::x_src));
                    ASSERT_DOUBLE_EQ(
                      y_num(i + offset),
                      prm.get_floating_point(i, Meta::y_src));
                }
                else {
                    ASSERT_FLOAT_EQ(
                      x_num(i + offset),
                      prm.get_floating_point(i, Meta::x_src));
                    ASSERT_FLOAT_EQ(
                      y_num(i + offset),
                      prm.get_floating_point(i, Meta::y_src));
                }
            }

            for (size_t j = 0U; j < ns; j++) {
                ASSERT_EQ(
                  bufnew[i * ns + j],
                  exseis::utils::Trace_value(offset + i + j))
                  << "Trace Number: " << i << " " << j;
            }
        }
    }

    template<bool WritePrm = false, bool UseMock = true>
    void write_random_trace_test(size_t tn, const std::vector<size_t> offset)
    {
        ASSERT_EQ(tn, offset.size());
        std::vector<unsigned char> buf;

        bool mock_write_trace_called = true;
        if (UseMock) {
            mock_write_trace_called = false;

            if (mock_object == nullptr) {
                std::cerr << "Using Mock when not initialised: LOC: "
                          << __LINE__ << std::endl;
                return;
            }

            EXPECT_CALL(*mock_object, should_write_file_header(_))
              .Times(Exactly(1));
            EXPECT_CALL(*mock_object, set_file_size(_)).Times(Exactly(1));


            if (WritePrm) {
                buf.resize(tn * segy::segy_trace_size(ns));
            }
            else {
                buf.resize(tn * segy::segy_trace_data_size(ns));
            }

            for (size_t i = 0U; i < tn; i++) {
                if (WritePrm) {
                    init_write_headers(
                      offset[i], &buf[i * segy::segy_trace_size(ns)]);
                }

                for (size_t j = 0U; j < ns; j++) {
                    const size_t addr =
                      WritePrm ?
                        (i * segy::segy_trace_size(ns)
                         + segy::segy_trace_header_size() + j * sizeof(float)) :
                        (i * ns + j) * sizeof(float);

                    const float val = offset[i] + j;

                    const auto be_val = to_big_endian<float>(val);
                    std::copy(std::begin(be_val), std::end(be_val), &buf[addr]);
                }
            }

            EXPECT_CALL(*mock_object, write_trace(offset.data(), ns, tn, _))
              .Times(AnyNumber())
              .WillOnce(DoAll(
                check3(buf.data(), buf.size()),
                InvokeWithoutArgs([&] { mock_write_trace_called = true; })));

            EXPECT_CALL(
              *mock_object, write_trace_metadata(offset.data(), ns, tn, _))
              .Times(AnyNumber())
              .WillOnce(DoAll(
                check3(buf.data(), buf.size()),
                InvokeWithoutArgs([&] { mock_write_trace_called = true; })));

            EXPECT_CALL(
              *mock_object, write_trace_data(offset.data(), ns, tn, _))
              .Times(AnyNumber())
              .WillOnce(DoAll(
                check3(buf.data(), buf.size()),
                InvokeWithoutArgs([&] { mock_write_trace_called = true; })));
        }

        Trace_metadata prm(tn);
        std::vector<float> bufnew(tn * ns);
        for (size_t i = 0U; i < tn; i++) {
            if (WritePrm) {
                prm.set_floating_point(i, Meta::x_src, x_num(offset[i]));
                prm.set_floating_point(i, Meta::x_rcv, x_num(offset[i]));
                prm.set_floating_point(i, Meta::xCmp, x_num(offset[i]));
                prm.set_floating_point(i, Meta::y_src, y_num(offset[i]));
                prm.set_floating_point(i, Meta::y_rcv, y_num(offset[i]));
                prm.set_floating_point(i, Meta::yCmp, y_num(offset[i]));
                prm.set_integer(i, Meta::il, il_num(offset[i]));
                prm.set_integer(i, Meta::xl, xl_num(offset[i]));
                prm.set_integer(i, Meta::tn, offset[i]);
            }

            for (size_t j = 0U; j < ns; j++) {
                bufnew[i * ns + j] = float(offset[i] + j);
            }
        }

        if (WritePrm) {
            file->write_trace_non_contiguous(
              tn, offset.data(), bufnew.data(), &prm);
        }
        else {
            file->write_trace_non_contiguous(tn, offset.data(), bufnew.data());
        }

        ASSERT_TRUE(mock_write_trace_called);

        if (UseMock == false) {
            for (size_t i = 0U; i < tn; i++) {
                ReadSEGY_public::get(readfile.get())->m_nt = std::max(
                  offset[i], ReadSEGY_public::get(readfile.get())->m_nt);
            }
            read_random_trace_test<WritePrm>(tn, offset);
        }
    }

    template<bool ReadPrm = false>
    void read_random_trace_test(size_t tn, const std::vector<size_t> offset)
    {
        ASSERT_EQ(tn, offset.size());
        std::vector<unsigned char> buf;
        std::vector<float> bufnew(tn * ns);
        Trace_metadata prm(tn);
        if (ReadPrm) {
            readfile->read_trace_non_contiguous(
              tn, offset.data(), bufnew.data(), &prm);
        }
        else {
            readfile->read_trace_non_contiguous(
              tn, offset.data(), bufnew.data());
        }

        for (size_t i = 0U; i < tn; i++) {
            if (ReadPrm && tn && ns) {
                ASSERT_EQ(il_num(offset[i]), prm.get_integer(i, Meta::il))
                  << "Trace Number " << i << " offset " << offset[i];
                ASSERT_EQ(xl_num(offset[i]), prm.get_integer(i, Meta::xl))
                  << "Trace Number " << i << " offset " << offset[i];

                if (sizeof(exseis::utils::Floating_point) == sizeof(double)) {
                    ASSERT_DOUBLE_EQ(
                      x_num(offset[i]), prm.get_floating_point(i, Meta::x_src));
                    ASSERT_DOUBLE_EQ(
                      y_num(offset[i]), prm.get_floating_point(i, Meta::y_src));
                }
                else {
                    ASSERT_FLOAT_EQ(
                      x_num(offset[i]), prm.get_floating_point(i, Meta::x_src));
                    ASSERT_FLOAT_EQ(
                      y_num(offset[i]), prm.get_floating_point(i, Meta::y_src));
                }
            }

            for (size_t j = 0U; j < ns; j++) {
                ASSERT_EQ(bufnew[i * ns + j], float(offset[i] + j))
                  << "Trace Number: " << offset[i] << " " << j;
            }
        }
    }

    template<bool Copy>
    void write_trace_header_test(const size_t offset, const size_t tn)
    {
        const bool mock = true;
        std::vector<unsigned char> buf;
        if (mock) {
            buf.resize(tn * segy::segy_trace_header_size());
            for (size_t i = 0; i < tn; i++) {
                coord_t src = coord_t(il_num(i + 1), xl_num(i + 5));
                coord_t rcv = coord_t(il_num(i + 2), xl_num(i + 6));
                coord_t cmp = coord_t(il_num(i + 3), xl_num(i + 7));
                grid_t line = grid_t(il_num(i + 4), xl_num(i + 8));

                int16_t scale = scal_comp(1, calc_scale(src));
                scale         = scal_comp(scale, calc_scale(rcv));
                scale         = scal_comp(scale, calc_scale(cmp));

                unsigned char* md = &buf[i * segy::segy_trace_header_size()];

                const auto be_scale = to_big_endian(scale);
                std::copy(
                  std::begin(be_scale), std::end(be_scale), &md[ScaleCoord]);

                set_coord(Coord::Src, src, scale, md);
                set_coord(Coord::Rcv, rcv, scale, md);
                set_coord(Coord::CMP, cmp, scale, md);

                set_grid(Grid::Line, line, md);

                const auto be_seqfnum = to_big_endian(int32_t(offset + i));
                std::copy(
                  std::begin(be_seqfnum), std::end(be_seqfnum), &md[SeqFNum]);
            }
            EXPECT_CALL(
              *mock_object.get(), write_trace_metadata(offset, ns, tn, _))
              .Times(Exactly(1))
              .WillOnce(check3(buf.data(), buf.size()));
        }

        if (Copy) {
            const auto rule = Rule(std::initializer_list<Meta>{Meta::Copy});
            Trace_metadata prm(std::move(rule), tn);
            ASSERT_TRUE(prm.size());
            prm.raw_metadata = buf;
            file->write_param(offset, prm.size(), &prm);
        }
        else {
            Trace_metadata prm(tn);
            for (size_t i = 0; i < tn; i++) {
                prm.set_floating_point(i, Meta::x_src, il_num(i + 1));
                prm.set_floating_point(i, Meta::x_rcv, il_num(i + 2));
                prm.set_floating_point(i, Meta::xCmp, il_num(i + 3));
                prm.set_integer(i, Meta::il, il_num(i + 4));
                prm.set_floating_point(i, Meta::y_src, xl_num(i + 5));
                prm.set_floating_point(i, Meta::y_rcv, xl_num(i + 6));
                prm.set_floating_point(i, Meta::yCmp, xl_num(i + 7));
                prm.set_integer(i, Meta::xl, xl_num(i + 8));
                prm.set_integer(i, Meta::tn, offset + i);
            }
            file->write_param(offset, prm.size(), &prm);
        }
    }
};

typedef FileWriteSEGYTest FileSEGYWrite;
typedef FileReadSEGYTest FileSEGYRead;
typedef FileReadSEGYTest FileSEGYIntegRead;
typedef FileWriteSEGYTest FileSEGYIntegWrite;

#endif  // EXSEISDAT_TEST_SPECTESTS_FILESEGYTEST_HH
