#ifndef EXSEISDAT_TEST_SPECTESTS_DATAMPIIOTEST_HH
#define EXSEISDAT_TEST_SPECTESTS_DATAMPIIOTEST_HH

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "tglobal.hh"

#include "exseisdat/piol/ExSeis.hh"
#include "exseisdat/piol/mpi/MPI_Binary_file.hh"
#include "exseisdat/piol/segy/utils.hh"

#include "exseisdat/utils/encoding/number_encoding.hh"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <memory>
#include <unistd.h>

using namespace testing;
using namespace exseis::utils;
using namespace exseis::piol;

size_t modify_nt(size_t fs, size_t offset, size_t nt, size_t ns);

class MPIIOTest : public Test {
  protected:
    std::shared_ptr<ExSeis> m_piol      = ExSeis::make();
    std::shared_ptr<Binary_file> m_data = nullptr;

    template<bool WRITE = false>
    void make_mpiio(
      std::string name,
      const MPI_Binary_file::Opt& ioopt = MPI_Binary_file::Opt())
    {
        if (m_data != nullptr) {
            m_data.reset();
        }

        FileMode mode = (WRITE ? FileMode::ReadWrite : FileMode::Read);
        m_data = std::make_shared<MPI_Binary_file>(m_piol, name, mode, ioopt);
    }

    void make_test_sz(size_t sz)
    {
        make_mpiio<true>(temp_file());
        m_data->set_file_size(sz);

        struct stat info;
        stat(temp_file().c_str(), &info);
        ASSERT_EQ(sz, static_cast<size_t>(info.st_size));
    }

    template<bool Block>
    void write_small_blocks(size_t nt, size_t ns, size_t offset = 0)
    {
        size_t step =
          (Block ? segy::segy_trace_header_size() : segy::segy_trace_size(ns));
        std::vector<unsigned char> tr(step * nt);

        for (size_t i = 0; i < nt; i++) {
            unsigned char* md = &tr[step * i];

            const auto be_il_num = to_big_endian(il_num(i + offset));
            const auto be_xl_num = to_big_endian(xl_num(i + offset));

            std::copy(std::begin(be_il_num), std::end(be_il_num), &md[188]);
            std::copy(std::begin(be_xl_num), std::end(be_xl_num), &md[192]);
        }

        if (Block) {
            m_data->write_noncontiguous(
              segy::segy_binary_file_header_size()
                + offset * segy::segy_trace_size(ns),
              segy::segy_trace_header_size(), segy::segy_trace_size(ns), nt,
              tr.data());
        }
        else {
            m_data->write(
              segy::segy_binary_file_header_size()
                + offset * segy::segy_trace_size(ns),
              segy::segy_trace_size(ns) * nt, tr.data());
        }

        read_small_blocks<Block>(nt, ns, offset);
    }

    template<bool Block>
    void write_big_blocks(size_t nt, size_t ns, size_t offset = 0)
    {
        size_t step =
          (Block ? segy::segy_trace_data_size(ns) : segy::segy_trace_size(ns));
        std::vector<unsigned char> tr(step * nt);

        for (size_t i = 0; i < nt; i++) {
            unsigned char* buf = &tr[step * i];
            for (size_t k = 0; k < ns; k++) {

                const float f = i + k;
                uint32_t n    = 0;
                std::memcpy(&n, &f, sizeof(uint32_t));

                buf[4 * k + 0] = n >> 24 & 0xFF;
                buf[4 * k + 1] = n >> 16 & 0xFF;
                buf[4 * k + 2] = n >> 8 & 0xFF;
                buf[4 * k + 3] = n & 0xFF;
            }
        }

        if (Block) {
            m_data->write_noncontiguous(
              segy::segy_trace_data_location<float>(offset, ns),
              segy::segy_trace_data_size(ns), segy::segy_trace_size(ns), nt,
              tr.data());
        }
        else {
            m_data->write(
              segy::segy_trace_data_location<float>(offset, ns),
              segy::segy_trace_size(ns) * nt, tr.data());
        }

        read_big_blocks<Block>(nt, ns, offset);
    }

    template<bool Block>
    void read_small_blocks(size_t nt, size_t ns, size_t offset = 0)
    {
        size_t step =
          (Block ? segy::segy_trace_header_size() : segy::segy_trace_size(ns));
        std::vector<unsigned char> tr;
        tr.resize(step * nt);

        if (Block) {
            m_data->read_noncontiguous(
              segy::segy_binary_file_header_size()
                + offset * segy::segy_trace_size(ns),
              segy::segy_trace_header_size(), segy::segy_trace_size(ns), nt,
              tr.data());
        }
        else {
            m_data->read(
              segy::segy_binary_file_header_size()
                + offset * segy::segy_trace_size(ns),
              segy::segy_trace_size(ns) * nt, tr.data());
        }

        nt = modify_nt(m_data->get_file_size(), offset, nt, ns);
        for (size_t i = 0; i < nt; i++) {

            unsigned char* md = &tr[step * i];

            ASSERT_EQ(
              il_num(i + offset),
              from_big_endian<int32_t>(
                md[188 + 0], md[188 + 1], md[188 + 2], md[188 + 3]))
              << i;
            ASSERT_EQ(
              xl_num(i + offset),
              from_big_endian<int32_t>(
                md[192 + 0], md[192 + 1], md[192 + 2], md[192 + 3]))
              << i;
        }
    }

    template<bool Block>
    void read_big_blocks(size_t nt, size_t ns, const size_t offset = 0)
    {
        size_t step =
          (Block ? segy::segy_trace_data_size(ns) : segy::segy_trace_size(ns));
        std::vector<unsigned char> tr(step * nt);

        if (Block) {
            m_data->read_noncontiguous(
              segy::segy_trace_data_location<float>(offset, ns),
              segy::segy_trace_data_size(ns), segy::segy_trace_size(ns), nt,
              tr.data());
        }
        else {
            m_data->read(
              segy::segy_trace_data_location<float>(offset, ns),
              segy::segy_trace_size(ns) * nt, tr.data());
        }

        nt = modify_nt(m_data->get_file_size(), offset, nt, ns);
        for (size_t i = 0; i < nt; i++) {
            unsigned char* buf = &tr[step * i];
            for (size_t k = 0; k < ns; k++) {

                const float f = i + k;
                uint32_t n    = 0;
                std::memcpy(&n, &f, sizeof(uint32_t));

                ASSERT_EQ(buf[4 * k + 0], n >> 24 & 0xFF);
                ASSERT_EQ(buf[4 * k + 1], n >> 16 & 0xFF);
                ASSERT_EQ(buf[4 * k + 2], n >> 8 & 0xFF);
                ASSERT_EQ(buf[4 * k + 3], n & 0xFF);
            }
        }
    }

    void write_list(const size_t sz, const size_t ns)
    {
        auto offset = get_random_vec(sz, 1337);
        size_t bsz  = segy::segy_trace_data_size(ns);
        std::vector<unsigned char> d(bsz * sz);
        for (size_t i = 0; i < sz; i++) {
            unsigned char* buf = &d[bsz * i];
            for (size_t k = 0; k < ns; k++) {

                const float f = offset[i] + k;
                uint32_t n    = 0;
                std::memcpy(&n, &f, sizeof(uint32_t));

                buf[4 * k + 0] = n >> 24 & 0xFF;
                buf[4 * k + 1] = n >> 16 & 0xFF;
                buf[4 * k + 2] = n >> 8 & 0xFF;
                buf[4 * k + 3] = n & 0xFF;
            }
        }

        std::vector<size_t> boffset(sz);
        for (size_t i = 0; i < sz; i++) {
            boffset[i] = segy::segy_trace_data_location<float>(offset[i], ns);
        }
        m_data->write_noncontiguous_irregular(
          bsz, sz, boffset.data(), d.data());
        m_piol->assert_ok();

        read_list(sz, ns, offset.data());
    }

    void read_list(const size_t sz, const size_t ns, const size_t* offset)
    {
        const size_t bsz = segy::segy_trace_data_size(ns);
        std::vector<unsigned char> d(bsz * sz);

        std::vector<size_t> boffset(sz);
        for (size_t i = 0; i < sz; i++) {
            boffset[i] = segy::segy_trace_data_location<float>(offset[i], ns);
        }
        m_data->read_noncontiguous_irregular(bsz, sz, boffset.data(), d.data());
        m_piol->assert_ok();

        for (size_t i = 0; i < sz; i++) {
            unsigned char* buf = &d[bsz * i];
            for (size_t k = 0; k < ns; k++) {

                const float f = offset[i] + k;
                uint32_t n    = 0;
                std::memcpy(&n, &f, sizeof(uint32_t));

                ASSERT_EQ(buf[4 * k + 0], n >> 24 & 0xFF) << i << " " << k;
                ASSERT_EQ(buf[4 * k + 1], n >> 16 & 0xFF) << i << " " << k;
                ASSERT_EQ(buf[4 * k + 2], n >> 8 & 0xFF) << i << " " << k;
                ASSERT_EQ(buf[4 * k + 3], n & 0xFF) << i << " " << k;
            }
        }
    }
};

#endif  // EXSEISDAT_TEST_SPECTESTS_DATAMPIIOTEST_HH
