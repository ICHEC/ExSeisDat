#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "tglobal.hh"

#include "ExSeisDat/PIOL/ExSeis.hh"
#include "ExSeisDat/PIOL/data/datampiio.hh"
#include "ExSeisDat/PIOL/share/datatype.hh"
#include "ExSeisDat/PIOL/share/segy.hh"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <memory>
#include <unistd.h>

using namespace testing;
using namespace PIOL;

size_t modifyNt(
  const size_t fs, const size_t offset, const size_t nt, const size_t ns);

class MPIIOTest : public Test {
  protected:
    std::shared_ptr<ExSeis> piol = ExSeis::New();
    Comm::MPI::Opt opt;
    Data::MPIIO::Opt ioopt;
    std::shared_ptr<Data::Interface> data = nullptr;

    template<bool WRITE = false>
    void makeMPIIO(std::string name)
    {
        if (data != nullptr) data.reset();
        FileMode mode = (WRITE ? FileMode::Test : FileMode::Read);
        data          = std::make_shared<Data::MPIIO>(piol, name, mode);
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
        size_t step = (block ? SEGSz::getMDSz() : SEGSz::getDOSz(ns));
        std::vector<uchar> tr(step * nt);

        for (size_t i = 0; i < nt; i++) {
            uchar* md = &tr[step * i];
            getBigEndian(ilNum(i + offset), &md[188]);
            getBigEndian(xlNum(i + offset), &md[192]);
        }

        if (block) {
            data->write(
              SEGSz::getHOSz() + offset * SEGSz::getDOSz(ns), SEGSz::getMDSz(),
              SEGSz::getDOSz(ns), nt, tr.data());
        }
        else {
            data->write(
              SEGSz::getHOSz() + offset * SEGSz::getDOSz(ns),
              SEGSz::getDOSz(ns) * nt, tr.data());
        }

        readSmallBlocks<block>(nt, ns, offset);
    }

    template<bool block>
    void writeBigBlocks(size_t nt, const size_t ns, const size_t offset = 0)
    {
        size_t step = (block ? SEGSz::getDFSz(ns) : SEGSz::getDOSz(ns));
        std::vector<uchar> tr(step * nt);

        for (size_t i = 0; i < nt; i++) {
            uchar* buf = &tr[step * i];
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
              SEGSz::getDODFLoc<float>(offset, ns), SEGSz::getDFSz(ns),
              SEGSz::getDOSz(ns), nt, tr.data());
        else
            data->write(
              SEGSz::getDODFLoc<float>(offset, ns), SEGSz::getDOSz(ns) * nt,
              tr.data());

        readBigBlocks<block>(nt, ns, offset);
    }

    template<bool block>
    void readSmallBlocks(size_t nt, const size_t ns, const size_t offset = 0)
    {
        size_t step = (block ? SEGSz::getMDSz() : SEGSz::getDOSz(ns));
        std::vector<uchar> tr(step * nt);

        if (block)
            data->read(
              SEGSz::getHOSz() + offset * SEGSz::getDOSz(ns), SEGSz::getMDSz(),
              SEGSz::getDOSz(ns), nt, tr.data());
        else
            data->read(
              SEGSz::getHOSz() + offset * SEGSz::getDOSz(ns),
              SEGSz::getDOSz(ns) * nt, tr.data());

        nt = modifyNt(data->getFileSz(), offset, nt, ns);
        for (size_t i = 0; i < nt; i++) {
            uchar* md = &tr[step * i];
            ASSERT_EQ(ilNum(i + offset), getHost<int32_t>(&md[188])) << i;
            ASSERT_EQ(xlNum(i + offset), getHost<int32_t>(&md[192])) << i;
        }
    }

    template<bool block>
    void readBigBlocks(size_t nt, const size_t ns, const size_t offset = 0)
    {
        size_t step = (block ? SEGSz::getDFSz(ns) : SEGSz::getDOSz(ns));
        std::vector<uchar> tr(step * nt);

        if (block) {
            data->read(
              SEGSz::getDODFLoc<float>(offset, ns), SEGSz::getDFSz(ns),
              SEGSz::getDOSz(ns), nt, tr.data());
        }
        else {
            data->read(
              SEGSz::getDODFLoc<float>(offset, ns), SEGSz::getDOSz(ns) * nt,
              tr.data());
        }

        nt = modifyNt(data->getFileSz(), offset, nt, ns);
        for (size_t i = 0; i < nt; i++) {
            uchar* buf = &tr[step * i];
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
        size_t bsz  = SEGSz::getDFSz(ns);
        std::vector<uchar> d(bsz * sz);
        for (size_t i = 0; i < sz; i++) {
            uchar* buf = &d[bsz * i];
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
            boffset[i] = SEGSz::getDODFLoc<float>(offset[i], ns);
        data->write(bsz, sz, boffset.data(), d.data());
        piol->isErr();
        readList(sz, ns, offset.data());
    }

    void readList(const size_t sz, const size_t ns, const size_t* offset)
    {
        size_t bsz = SEGSz::getDFSz(ns);
        std::vector<uchar> d(bsz * sz);
        std::vector<size_t> boffset(sz);
        for (size_t i = 0; i < sz; i++)
            boffset[i] = SEGSz::getDODFLoc<float>(offset[i], ns);
        data->read(bsz, sz, boffset.data(), d.data());
        piol->isErr();

        for (size_t i = 0; i < sz; i++) {
            uchar* buf = &d[bsz * i];
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
