#include "cppfileapi.hh"
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

void ExSeis::isErr(std::string msg) const
{
    piol->isErr(msg);
}

namespace File {
Direct::Direct(const Piol piol_, const std::string name_, FileMode mode) : Interface(piol_, name_, nullptr)
{
    const File::SEGY::Opt f;
    const Obj::SEGY::Opt o;
    const Data::MPIIO::Opt d;
    auto data = std::make_shared<Data::MPIIO>(piol, name, d, mode);
    auto obj = std::make_shared<Obj::SEGY>(piol, name, o, data, mode);
    file = std::make_shared<File::SEGY>(piol, name, f, obj, mode);
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

void Direct::writeTraceParam(csize_t offset, csize_t sz, const TraceParam * prm)
{
    file->writeTraceParam(offset, sz, prm);
}

void Direct::readTraceParam(csize_t offset, csize_t sz, TraceParam * prm) const
{
    file->readTraceParam(offset, sz, prm);
}

void Direct::readTrace(csize_t offset, csize_t sz, trace_t * trace, TraceParam * prm) const
{
    file->readTrace(offset, sz, trace, prm);
}

void Direct::writeTrace(csize_t offset, csize_t sz, trace_t * trace, const TraceParam * prm)
{
    file->writeTrace(offset, sz, trace, prm);
}

void Direct::readTrace(csize_t sz, csize_t * offset, trace_t * trace, TraceParam * prm) const
{
    file->readTrace(sz, offset, trace, prm);
}

void Direct::writeTrace(csize_t sz, csize_t * offset, trace_t * trace, const TraceParam * prm)
{
    file->writeTrace(sz, offset, trace, prm);
}

void Direct::readTraceParam(csize_t sz, csize_t * offset, TraceParam * prm) const
{
    file->readTraceParam(sz, offset, prm);
}

void Direct::writeTraceParam(csize_t sz, csize_t * offset, const TraceParam * prm)
{
    file->writeTraceParam(sz, offset, prm);
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
