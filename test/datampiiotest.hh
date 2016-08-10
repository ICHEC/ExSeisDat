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
    std::shared_ptr<ExSeisPIOL> piol;
    Comm::MPIOpt opt;
    Data::MPIIOOpt ioopt;
    Data::Interface * data;
    MPIIOTest()
    {
        data = nullptr;
        opt.initMPI = false;
        piol = std::make_shared<ExSeisPIOL>(opt);
    }
    ~MPIIOTest()
    {
        if (data != nullptr)
            delete data;
    }

    template <bool writeTest = false>
    void makeMPIIO(std::string name)
    {
        if (data != nullptr)
            delete data;
        if (writeTest)
            ioopt.mode = MPI_MODE_UNIQUE_OPEN | MPI_MODE_CREATE | MPI_MODE_RDWR | MPI_MODE_DELETE_ON_CLOSE | MPI_MODE_EXCL;
        data = new Data::MPIIO(piol, name, ioopt);
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
            for (size_t k = 0; k < ns*sizeof(float); k++)
                buf[k] = (offset+i+k) % 0x100;
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
            for (size_t k = 0; k < ns*sizeof(float); k++)
                ASSERT_EQ((offset+i+k) % 0x100, buf[k]) << i << " " << k;
        }
    }
};
