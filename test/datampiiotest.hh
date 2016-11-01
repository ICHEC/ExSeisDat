#include <memory>
#include <fstream>
#include <cstdio>
#include <unistd.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#define UNIT_TEST
#define private public
#define protected public
#include "tglobal.hh"
#include "share/segy.hh"
#include "share/datatype.hh"
#include "data/datampiio.hh"
#undef private
#undef protected

using namespace testing;
using namespace PIOL;
extern size_t modifyNt(csize_t fs, csize_t offset, csize_t nt, csize_t ns);

class MPIIOTest : public Test
{
    protected :
    Piol piol;
    Comm::MPI::Opt opt;
    Data::MPIIO::Opt ioopt;
    std::shared_ptr<Data::Interface> data;
    MPIIOTest()
    {
        data = nullptr;
        opt.initMPI = false;
        piol = std::make_shared<ExSeisPIOL>(opt);
    }
    ~MPIIOTest()
    {
        if (data != nullptr)
            data.reset();
    }

    template <bool WRITE = false>
    void makeMPIIO(std::string name)
    {
        if (data != nullptr)
            data.reset();
        FileMode mode = (WRITE ? FileMode::Test : FileMode::Read);
        data = std::make_shared<Data::MPIIO>(piol, name, mode);
    }

    void makeTestSz(csize_t sz)
    {
        makeMPIIO<true>(tempFile);
        data->setFileSz(sz);

        struct stat info;
        stat(tempFile.c_str(), &info);
        ASSERT_EQ(sz, info.st_size);
    }

    template <bool block>
    void writeSmallBlocks(size_t nt, csize_t ns, csize_t offset = 0)
    {
        size_t step = (block ? SEGSz::getMDSz() : SEGSz::getDOSz(ns));
        std::vector<uchar> tr(step*nt);

        for (size_t i = 0; i < nt; i++)
        {
            uchar * md = &tr[step*i];
            getBigEndian(ilNum(i+offset), &md[188]);
            getBigEndian(xlNum(i+offset), &md[192]);
        }

        if (block)
            data->write(SEGSz::getHOSz() + offset*SEGSz::getDOSz(ns), SEGSz::getMDSz(), SEGSz::getDOSz(ns), nt, tr.data());
        else
            data->write(SEGSz::getHOSz() + offset*SEGSz::getDOSz(ns), SEGSz::getDOSz(ns)*nt, tr.data());

        readSmallBlocks<block>(nt, ns, offset);
    }

    template <bool block>
    void writeBigBlocks(size_t nt, csize_t ns, csize_t offset = 0)
    {
        size_t step = (block ? SEGSz::getDFSz(ns) : SEGSz::getDOSz(ns));
        std::vector<uchar> tr(step*nt);

        for (size_t i = 0; i < nt; i++)
        {
            uchar * buf = &tr[step*i];
            for (size_t k = 0; k < ns; k++)
            {
                union { float f; uint32_t i; } n;
                n.f = i+k;
                buf[4*k + 0] = n.i >> 24 & 0xFF;
                buf[4*k + 1] = n.i >> 16 & 0xFF;
                buf[4*k + 2] = n.i >> 8  & 0xFF;
                buf[4*k + 3] = n.i       & 0xFF;
            }

        }

        if (block)
            data->write(SEGSz::getDODFLoc<float>(offset, ns), SEGSz::getDFSz(ns), SEGSz::getDOSz(ns), nt, tr.data());
        else
            data->write(SEGSz::getDODFLoc<float>(offset, ns), SEGSz::getDOSz(ns)*nt, tr.data());

        readBigBlocks<block>(nt, ns, offset);
    }

    template <bool block>
    void readSmallBlocks(size_t nt, csize_t ns, csize_t offset = 0)
    {
        size_t step = (block ? SEGSz::getMDSz() : SEGSz::getDOSz(ns));
        std::vector<uchar> tr(step*nt);

        if (block)
            data->read(SEGSz::getHOSz() + offset*SEGSz::getDOSz(ns), SEGSz::getMDSz(), SEGSz::getDOSz(ns), nt, tr.data());
        else
            data->read(SEGSz::getHOSz() + offset*SEGSz::getDOSz(ns), SEGSz::getDOSz(ns)*nt, tr.data());

        nt = modifyNt(data->getFileSz(), offset, nt, ns);
        for (size_t i = 0; i < nt; i++)
        {
            uchar * md = &tr[step*i];
            ASSERT_EQ(ilNum(i+offset), getHost<int32_t>(&md[188])) << i;
            ASSERT_EQ(xlNum(i+offset), getHost<int32_t>(&md[192])) << i;
        }
    }

    template <bool block>
    void readBigBlocks(size_t nt, csize_t ns, csize_t offset = 0)
    {
        size_t step = (block ? SEGSz::getDFSz(ns) : SEGSz::getDOSz(ns));
        std::vector<uchar> tr(step*nt);

        if (block)
            data->read(SEGSz::getDODFLoc<float>(offset, ns), SEGSz::getDFSz(ns), SEGSz::getDOSz(ns), nt, tr.data());
        else
            data->read(SEGSz::getDODFLoc<float>(offset, ns), SEGSz::getDOSz(ns)*nt, tr.data());

        nt = modifyNt(data->getFileSz(), offset, nt, ns);
        for (size_t i = 0; i < nt; i++)
        {
            uchar * buf = &tr[step*i];
            for (size_t k = 0; k < ns; k++)
            {
                union { float f; uint32_t i; } n;
                n.f = i+k;
                ASSERT_EQ(buf[4*k + 0], n.i >> 24 & 0xFF);
                ASSERT_EQ(buf[4*k + 1], n.i >> 16 & 0xFF);
                ASSERT_EQ(buf[4*k + 2], n.i >> 8  & 0xFF);
                ASSERT_EQ(buf[4*k + 3], n.i       & 0xFF);
            }
        }
    }

    void writeList(csize_t sz, csize_t ns)
    {
        auto offset = getRandomVec(sz, 1337);
        size_t bsz = SEGSz::getDFSz(ns);
        std::vector<uchar> d(bsz*sz);
        for (size_t i = 0; i < sz; i++)
        {
            uchar * buf = &d[bsz*i];
            for (size_t k = 0; k < ns; k++)
            {
                union { float f; uint32_t i; } n;
                n.f = offset[i]+k;
                buf[4*k + 0] = n.i >> 24 & 0xFF;
                buf[4*k + 1] = n.i >> 16 & 0xFF;
                buf[4*k + 2] = n.i >> 8  & 0xFF;
                buf[4*k + 3] = n.i       & 0xFF;
            }
        }

        std::vector<size_t> boffset(sz);
        for (size_t i = 0; i < sz; i++)
            boffset[i] = SEGSz::getDODFLoc<float>(offset[i], ns);
        data->write(bsz, sz, boffset.data(), d.data());
        piol->isErr();
        readList(sz, ns, offset.data());
    }

    void readList(csize_t sz, csize_t ns, csize_t * offset)
    {
        size_t bsz = SEGSz::getDFSz(ns);
        std::vector<uchar> d(bsz*sz);
        std::vector<size_t> boffset(sz);
        for (size_t i = 0; i < sz; i++)
            boffset[i] = SEGSz::getDODFLoc<float>(offset[i], ns);
        data->read(bsz, sz, boffset.data(), d.data());
        piol->isErr();

        for (size_t i = 0; i < sz; i++)
        {
            uchar * buf = &d[bsz*i];
            for (size_t k = 0; k < ns; k++)
            {
                union { float f; uint32_t i; } n;
                n.f = offset[i] + k;
                ASSERT_EQ(buf[4*k + 0], n.i >> 24 & 0xFF) << i << " " << k;
                ASSERT_EQ(buf[4*k + 1], n.i >> 16 & 0xFF) << i << " " << k;
                ASSERT_EQ(buf[4*k + 2], n.i >> 8  & 0xFF) << i << " " << k;
                ASSERT_EQ(buf[4*k + 3], n.i       & 0xFF) << i << " " << k;
            }
        }
    }
};
