#include "cfileapi.h"
#include <iostream>
#include "global.hh"
#include "anc/cmpi.hh"
#include "file/filesegy.hh"
#include "object/objsegy.hh"
#include "data/datampiio.hh"

using namespace PIOL;

struct ExSeisFileWrapper
{
    PIOL::File::Interface * file;
};

struct PIOLWrapper
{
    std::shared_ptr<PIOL::ExSeisPIOL> piol;
};

ExSeisHandle * initPIOL(size_t logLevel, MPIOptions * mpiOpt)
{
    Comm::MPIOpt mpi;
    if (mpiOpt != nullptr)
    {
        mpi.comm = mpiOpt->comm;
        mpi.initMPI = mpiOpt->initMPI;
    }

    auto wrap = new ExSeisHandle;
    wrap->piol = std::make_shared<ExSeisPIOL>(static_cast<Log::Verb>(logLevel), *dynamic_cast<Comm::Opt *>(&mpi));
    return wrap;
}

ExSeisFile * makeFile(ExSeisHandle * piol, const char * name, SEGYOptions * opt, MPIIOOptions * mpiOpt)
{
    PIOL::Data::MPIIOOpt mpiio;
    if (mpiOpt != nullptr)
    {
        mpiio.mode = mpiOpt->mode;
        mpiio.info = mpiOpt->info;
        mpiio.maxSize = mpiOpt->maxSize;
        mpiio.fcomm = mpiOpt->fcomm;
    }
    PIOL::Obj::SEGYOpt objOpt;
    PIOL::File::SEGYOpt fileOpt;
    if (opt != nullptr)
        fileOpt.incFactor = opt->incFactor;

    std::string name_(name);

    auto filewrap = new ExSeisFile;
    filewrap->file = new PIOL::File::SEGY(piol->piol, name_, fileOpt, objOpt, mpiio);
    return filewrap;
}

void deletePIOL(ExSeisHandle * piol)
{
    if (piol != nullptr)
    {
        if (piol->piol != nullptr)
            piol->piol.reset();
        delete piol;
    }
    else
        std::cerr << "Invalid free of ExSeisPIOL nullptr.\n";
}

void deleteFile(ExSeisFile * file)
{
    if (file != nullptr)
    {
        if (file->file != nullptr)
            delete file->file;
        delete file;
    }
    else
        std::cerr << "Invalid free of ExSeisFile nullptr.\n";
}

void writeText(ExSeisFile * f, const char * text)
{
    std::string text_(text);
    f->file->writeText(text_);
}

void writeNs(ExSeisFile * f, size_t ns)
{
    f->file->writeNs(ns);
}

void writeNt(ExSeisFile * f, size_t nt)
{
    f->file->writeNt(nt);
}

void writeInc(ExSeisFile * f, const double inc)
{
    f->file->writeInc(inc);
}

void readCoordPoint(ExSeisFile * f, const CCoord item, size_t offset, size_t sz, ccoord_t * buf)
{
    f->file->readCoordPoint(static_cast<File::Coord>(item), offset, sz, (File::coord_t *)(buf));
}

void readGridPoint(ExSeisFile * f, const CGrid item, size_t offset, size_t sz, cgrid_t * buf)
{
    f->file->readGridPoint(static_cast<File::Grid>(item), offset, sz, (File::grid_t *)(buf));
}

void readTrace(ExSeisFile * f, size_t offset, size_t sz, trace_t * trace)
{
    f->file->readTrace(offset, sz, trace);
}

void writeTrace(ExSeisFile * f, size_t offset, size_t sz, const trace_t * trace)
{
    f->file->writeTrace(offset, sz, trace);
}

/*void writeTraceParam(ExSeisFile * f, size_t offset, size_t sz, const TraceParam * prm)
{
    f->file->writeTraceParam(offset, sz, prm);
}*/

