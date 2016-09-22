#include "cppfile.hh"
#include "global.hh"
#include "anc/cmpi.hh"
#include "file/filesegy.hh"
#include "object/objsegy.hh"
#include "data/datampiio.hh"
namespace PIOL {
ExSeis::ExSeis(const Log::Verb maxLevel)
{
    piol = std::make_shared<ExSeisPIOL>(maxLevel);
}

ExSeis::ExSeis(bool initComm, const Log::Verb maxLevel)
{
    piol = std::make_shared<ExSeisPIOL>(initComm, maxLevel);
}

void ExSeis::record(const std::string file, const Log::Layer layer, const Log::Status stat,
                    const std::string msg, const Log::Verb verbosity) const
{
    piol->log->record(file, layer, stat, msg, verbosity);
}

void ExSeis::isErr(std::string msg) const
{
    piol->isErr(msg);
}

namespace File {
Direct::Direct(const Piol piol, const std::string name_, FileMode mode)
{
    const File::SEGY::Opt f;
    const Obj::SEGY::Opt o;
    const Data::MPIIO::Opt d;
    auto data = std::make_shared<Data::MPIIO>(piol, name_, d, mode);
    auto obj = std::make_shared<Obj::SEGY>(piol, name_, o, data, mode);
    file = std::make_shared<File::SEGY>(piol, name_, f, obj, mode);
}

const std::string & Direct::readText(void) const
{
 return file->readText();
}

size_t Direct::readNs(void) const
{
    return file->readNs();
}

size_t Direct::readNt(void) const
{
    return file->readNt();
}

geom_t Direct::readInc(void) const
{
    return file->readInc();
}

void Direct::readCoordPoint(const Coord item, csize_t i, csize_t sz, coord_t * buf) const
{
    file->readCoordPoint(item, i, sz, buf);
}

void Direct::readGridPoint(const Grid item, csize_t i, csize_t sz, grid_t * buf) const
{
    file->readGridPoint(item, i, sz, buf);
}

void Direct::writeTraceParam(csize_t offset, csize_t sz, const TraceParam * prm)
{
    file->writeTraceParam(offset, sz, prm);
}

void Direct::readTraceParam(csize_t offset, csize_t sz, TraceParam * prm) const
{
    file->readTraceParam(offset, sz, prm);
}

void Direct::readTrace(csize_t offset, csize_t sz, trace_t * trace) const
{
    file->readTrace(offset, sz, trace);
}

void Direct::writeTrace(csize_t offset, csize_t sz, trace_t * trace)
{
    file->writeTrace(offset, sz, trace);
}

void Direct::writeText(const std::string text_)
{
    file->writeText(text_);
}

void Direct::writeNs(const size_t ns_)
{
    file->writeNs(ns_);
}

void Direct::writeNt(const size_t nt_)
{
    file->writeNt(nt_);
}

void Direct::writeInc(const geom_t inc_)
{
    file->writeInc(inc_);
}
}}
