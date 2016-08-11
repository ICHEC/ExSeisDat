#include "cfileapi.h"
#include <iostream>
#include "global.hh"
#include "anc/cmpi.hh"
#include "file/filesegy.hh"
#include "object/objsegy.hh"
#include "data/datampiio.hh"

using namespace PIOL;

extern "C"
{
struct ExSeisFileWrapper
{
    PIOL::File::Interface * file;
};

struct PIOLWrapper
{
    std::shared_ptr<PIOL::ExSeisPIOL> piol;
};

ExSeisHandle initPIOL(size_t logLevel, MPIOptions * mpiOpt)
{
    Comm::MPIOpt mpi;
    if (mpiOpt != NULL)
    {
        mpi.comm = mpiOpt->comm;
        mpi.initMPI = mpiOpt->initMPI;
    }

    auto wrap = new PIOLWrapper;
    wrap->piol = std::make_shared<ExSeisPIOL>(static_cast<Log::Verb>(logLevel), *dynamic_cast<Comm::Opt *>(&mpi));
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
    return piol->piol->comm->getRank();
}

ExSeisFile openFile(ExSeisHandle piol, const char * name, SEGYOptions * opt, MPIIOOptions * ioOpt)
{
    PIOL::Data::MPIIOOpt mpiio;
    if (ioOpt != NULL)
    {
        mpiio.mode = ioOpt->mode;
        mpiio.info = ioOpt->info;
        mpiio.maxSize = ioOpt->maxSize;
        mpiio.fcomm = ioOpt->fcomm;
    }
    PIOL::Obj::SEGYOpt objOpt;
    PIOL::File::SEGYOpt fileOpt;
    if (opt != NULL)
        fileOpt.incFactor = opt->incFactor;

    std::string name_(name);

    std::cout << "Open file " << name << std::endl;
    auto filewrap = new ExSeisFileWrapper;
    filewrap->file = new File::SEGY(piol->piol, name_, fileOpt, objOpt, mpiio);
//TODO:    piol->piol->log

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

void closeFile(ExSeisFile file)
{
    if (file != NULL)
    {
        if (file->file != NULL)
            delete file->file;
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

void writeTrace(ExSeisFile f, size_t offset, size_t sz, const trace_t * trace)
{
    f->file->writeTrace(offset, sz, trace);
}

/*void writeTraceParam(ExSeisFile f, size_t offset, size_t sz, const TraceParam * prm)
{
    f->file->writeTraceParam(offset, sz, prm);
}*/
}
