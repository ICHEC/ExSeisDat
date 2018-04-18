#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "tglobal.hh"

#include "ExSeisDat/PIOL/DataMPIIO.hh"
#include "ExSeisDat/PIOL/ExSeis.hh"
#include "ExSeisDat/PIOL/segy_utils.hh"

#include "ExSeisDat/utils/encoding/number_encoding.hh"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <memory>
#include <unistd.h>

using namespace testing;
using namespace exseis::utils;
using namespace exseis::PIOL;

size_t modifyNt(
  const size_t fs, const size_t offset, const size_t nt, const size_t ns);

class MPIIOTest : public Test {
  protected:
    std::shared_ptr<ExSeis> piol = ExSeis::New();
    CommunicatorMPI::Opt opt;
    DataMPIIO::Opt ioopt;
    std::shared_ptr<DataInterface> data = nullptr;

    template<bool WRITE = false>
    void makeMPIIO(std::string name)
    {
        if (data != nullptr) data.reset();
        FileMode mode = (WRITE ? FileMode::Test : FileMode::Read);
        data          = std::make_shared<DataMPIIO>(piol, name, mode);
    }

    void makeTestSz(const size_t sz)
    {
        makeMPIIO<true>(tempFile);
        data->setFileSz(sz);

        struct stat info;
        stat(tempFile.c_str(), &info);
        ASSERT_EQ(sz, static_cast<size_t>(info.st_size));
    }

    template<bool block>
    void writeSmallBlocks(size_t nt, const size_t ns, const size_t offset = 0)
    {
        size_t step = (block ? SEGY_utils::getMDSz() : SEGY_utils::getDOSz(ns));
        std::vector<unsigned char> tr(step * nt);

        for (size_t i = 0; i < nt; i++) {
            unsigned char* md = &tr[step * i];

            const auto be_ilNum = to_big_endian(ilNum(i + offset));
            const auto be_xlNum = to_big_endian(xlNum(i + offset));

            std::copy(std::begin(be_ilNum), std::end(be_ilNum), &md[188]);
            std::copy(std::begin(be_xlNum), std::end(be_xlNum), &md[192]);
        }

        if (block) {
            data->write(
              SEGY_utils::getHOSz() + offset * SEGY_utils::getDOSz(ns),
              SEGY_utils::getMDSz(), SEGY_utils::getDOSz(ns), nt, tr.data());
        }
        else {
            data->write(
              SEGY_utils::getHOSz() + offset * SEGY_utils::getDOSz(ns),
              SEGY_utils::getDOSz(ns) * nt, tr.data());
        }

        readSmallBlocks<block>(nt, ns, offset);
    }

    template<bool block>
    void writeBigBlocks(size_t nt, const size_t ns, const size_t offset = 0)
    {
        size_t step =
          (block ? SEGY_utils::getDFSz(ns) : SEGY_utils::getDOSz(ns));
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

        if (block)
            data->write(
              SEGY_utils::getDODFLoc<float>(offset, ns),
              SEGY_utils::getDFSz(ns), SEGY_utils::getDOSz(ns), nt, tr.data());
        else
            data->write(
              SEGY_utils::getDODFLoc<float>(offset, ns),
              SEGY_utils::getDOSz(ns) * nt, tr.data());

        readBigBlocks<block>(nt, ns, offset);
    }

    template<bool block>
    void readSmallBlocks(size_t nt, const size_t ns, const size_t offset = 0)
    {
        size_t step = (block ? SEGY_utils::getMDSz() : SEGY_utils::getDOSz(ns));
        std::vector<unsigned char> tr(step * nt);

        if (block)
            data->read(
              SEGY_utils::getHOSz() + offset * SEGY_utils::getDOSz(ns),
              SEGY_utils::getMDSz(), SEGY_utils::getDOSz(ns), nt, tr.data());
        else
            data->read(
              SEGY_utils::getHOSz() + offset * SEGY_utils::getDOSz(ns),
              SEGY_utils::getDOSz(ns) * nt, tr.data());

        nt = modifyNt(data->getFileSz(), offset, nt, ns);
        for (size_t i = 0; i < nt; i++) {
            unsigned char* md = &tr[step * i];
            ASSERT_EQ(ilNum(i + offset), getHost<int32_t>(&md[188])) << i;
            ASSERT_EQ(xlNum(i + offset), getHost<int32_t>(&md[192])) << i;
        }
    }

    template<bool block>
    void readBigBlocks(size_t nt, const size_t ns, const size_t offset = 0)
    {
        size_t step =
          (block ? SEGY_utils::getDFSz(ns) : SEGY_utils::getDOSz(ns));
        std::vector<unsigned char> tr(step * nt);

        if (block) {
            data->read(
              SEGY_utils::getDODFLoc<float>(offset, ns),
              SEGY_utils::getDFSz(ns), SEGY_utils::getDOSz(ns), nt, tr.data());
        }
        else {
            data->read(
              SEGY_utils::getDODFLoc<float>(offset, ns),
              SEGY_utils::getDOSz(ns) * nt, tr.data());
        }

        nt = modifyNt(data->getFileSz(), offset, nt, ns);
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

    void writeList(const size_t sz, const size_t ns)
    {
        auto offset = getRandomVec(sz, 1337);
        size_t bsz  = SEGY_utils::getDFSz(ns);
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
        for (size_t i = 0; i < sz; i++)
            boffset[i] = SEGY_utils::getDODFLoc<float>(offset[i], ns);
        data->write(bsz, sz, boffset.data(), d.data());
        piol->isErr();
        readList(sz, ns, offset.data());
    }

    void readList(const size_t sz, const size_t ns, const size_t* offset)
    {
        size_t bsz = SEGY_utils::getDFSz(ns);
        std::vector<unsigned char> d(bsz * sz);
        std::vector<size_t> boffset(sz);
        for (size_t i = 0; i < sz; i++)
            boffset[i] = SEGY_utils::getDODFLoc<float>(offset[i], ns);
        data->read(bsz, sz, boffset.data(), d.data());
        piol->isErr();

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
