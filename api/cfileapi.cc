#include "global.hh"
#include "cfileapi.h"
#include <iostream>
#include <cstddef>
#include <assert.h>
#include "anc/cmpi.hh"
#include "file/filesegy.hh"
#include "object/objsegy.hh"
#include "data/datampiio.hh"
#include "share/segy.hh"
#include "ops/ops.hh"
#include "builder.hh"

using namespace PIOL;

extern "C"
{
struct ExSeisFileWrapper
{
    std::shared_ptr<PIOL::File::Interface> file;
};

struct PIOLWrapper
{
    std::shared_ptr<PIOL::ExSeisPIOL> piol;
};

ExSeisHandle initPIOL(size_t logLevel, MPIOptions * mpiOpt)
{
//TODO: Test the cast of C structures to C++ types here.
    assert(sizeof(File::TraceParam) == sizeof(TraceParam));
    assert(sizeof(File::coord_t) == sizeof(ccoord_t));
    assert(sizeof(File::grid_t) == sizeof(cgrid_t));

    Comm::MPI::Opt mpi;
    if (mpiOpt != NULL)
    {
        mpi.comm = mpiOpt->comm;
        mpi.initMPI = mpiOpt->initMPI;
    }

    auto wrap = new PIOLWrapper;
    wrap->piol = makePIOL<Comm::MPI>(Log::Verb(logLevel), &mpi);
    return wrap;
}

ExSeisHandle initMPIOL(void)
{
//TODO: Test the cast of C structures to C++ types here.
    assert(sizeof(File::TraceParam) == sizeof(TraceParam));
    assert(sizeof(File::coord_t) == sizeof(ccoord_t));
    assert(sizeof(File::grid_t) == sizeof(cgrid_t));
    assert(sizeof(File::CoordElem) == sizeof(CoordElem));

    Comm::MPI::Opt mpi;
    auto wrap = new PIOLWrapper;
    wrap->piol = makePIOL<Comm::MPI>(Log::Verb::Minimal, &mpi);
    return wrap;
}

void isErr(ExSeisHandle piol)
{
    piol->piol->isErr();
}

size_t getRank(ExSeisHandle piol)
{
    return piol->piol->comm->getRank();
}

size_t getNumRank(ExSeisHandle piol)
{
    return piol->piol->comm->getNumRank();
}

/*ExSeisFile openFile(ExSeisHandle piol, const char * name, SEGYOptions * opt, MPIIOOptions * ioOpt)
{
    PIOL::Data::MPIIOOpt mpiio;
    if (ioOpt != NULL)
    {
        mpiio.mode = static_cast<FileMode>(ioOpt->mode);
        mpiio.info = ioOpt->info;
        mpiio.maxSize = ioOpt->maxSize;
        mpiio.fcomm = ioOpt->fcomm;
    }
    PIOL::Obj::SEGYOpt objOpt;
    PIOL::File::SEGYOpt fileOpt;
    if (opt != NULL)
        fileOpt.incFactor = opt->incFactor;

    std::string name_(name);

    auto filewrap = new ExSeisFileWrapper;
    filewrap->file = new File::SEGY(piol->piol, name_, fileOpt, objOpt, mpiio);
//TODO:    piol->piol->log

    return filewrap;
}*/

ExSeisFile openWriteFile(ExSeisHandle piol, const char * name)
{
    Data::MPIIO::Opt data;
    Obj::SEGY::Opt obj;
    File::SEGY::Opt file;

    auto filewrap = new ExSeisFileWrapper;
    filewrap->file = makeFile<File::SEGY, Obj::SEGY, Data::MPIIO>(piol->piol, name, file, obj, data, FileMode::Write);
    return filewrap;
}

ExSeisFile openReadFile(ExSeisHandle piol, const char * name)
{
    Data::MPIIO::Opt data;
    Obj::SEGY::Opt obj;
    File::SEGY::Opt file;

    auto filewrap = new ExSeisFileWrapper;
    filewrap->file = makeFile<File::SEGY, Obj::SEGY, Data::MPIIO>(piol->piol, name, file, obj, data, FileMode::Read);
    return filewrap;
}

void closePIOL(ExSeisHandle piol)
{
    if (piol != NULL)
    {
        if (piol->piol != NULL)
            piol->piol.reset();
        delete piol;
    }
    else
        std::cerr << "Invalid free of ExSeisPIOL NULL.\n";
}

void barrier(ExSeisHandle piol)
{
    piol->piol->comm->barrier();
}

//TODO: Inconsistent naming of ExSeisFile file/fh/f variable
void closeFile(ExSeisFile file)
{
    if (file != NULL)
    {
        if (file->file != NULL)
            file->file.reset();
        delete file;
    }
    else
        std::cerr << "Invalid free of ExSeisFile NULL.\n";
}

const char * readText(ExSeisFile f)
{
    return f->file->readText().c_str();
}

size_t readNs(ExSeisFile f)
{
    return f->file->readNs();
}

size_t readNt(ExSeisFile f)
{
    return f->file->readNt();
}

double readInc(ExSeisFile f)
{
   return f->file->readInc();
}

void writeText(ExSeisFile f, const char * text)
{
    std::string text_(text);
    f->file->writeText(text_);
}

void writeNs(ExSeisFile f, size_t ns)
{
    f->file->writeNs(ns);
}

void writeNt(ExSeisFile f, size_t nt)
{
    f->file->writeNt(nt);
}

void writeInc(ExSeisFile f, const double inc)
{
    f->file->writeInc(inc);
}

void readCoordPoint(ExSeisFile f, const CCoord item, size_t offset, size_t sz, ccoord_t * buf)
{
    f->file->readCoordPoint(static_cast<File::Coord>(item), offset, sz, (File::coord_t *)(buf));
}

void readGridPoint(ExSeisFile f, const CGrid item, size_t offset, size_t sz, cgrid_t * buf)
{
    f->file->readGridPoint(static_cast<File::Grid>(item), offset, sz, (File::grid_t *)(buf));
}

void readTrace(ExSeisFile f, size_t offset, size_t sz, trace_t * trace)
{
    f->file->readTrace(offset, sz, trace);
}

void writeTrace(ExSeisFile f, size_t offset, size_t sz, trace_t * trace)
{
    f->file->writeTrace(offset, sz, trace);
}

void writeTraceParam(ExSeisFile f, size_t offset, size_t sz, const TraceParam * prm)
{
    f->file->writeTraceParam(offset, sz, reinterpret_cast<const File::TraceParam *>(prm));
}

void readTraceParam(ExSeisFile f, size_t offset, size_t sz, TraceParam * prm)
{
    f->file->readTraceParam(offset, sz, reinterpret_cast<File::TraceParam *>(prm));
}

/////////////////////////////////////Operations///////////////////////////////

void getMinMax(ExSeisHandle piol, size_t offset, size_t sz, const ccoord_t * coord, CoordElem * minmax)
{
    getMinMax(piol->piol, offset, sz, reinterpret_cast<const File::coord_t *>(coord), reinterpret_cast<File::CoordElem * >(minmax));
}

//////////////////////////////////////SEGSZ///////////////////////////////////
size_t getSEGYTextSz()
{
    return SEGSz::getTextSz();
}

size_t getSEGYTraceLen(size_t ns)
{
    return SEGSz::getDFSz<float>(ns);
}

size_t getSEGYFileSz(size_t nt, size_t ns)
{
    return SEGSz::getFileSz<float>(nt, ns);
}

size_t getSEGYParamSz(void)
{
    return sizeof(TraceParam) + SEGSz::getMDSz();
}
}
