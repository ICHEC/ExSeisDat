#include "cppfileapi.hh"
#include "global.hh"
#include "anc/cmpi.hh"
#include "file/filesegy.hh"
#include "object/objsegy.hh"
#include "data/datampiio.hh"
#include "file/dynsegymd.hh"
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
const TraceParam * PRM_NULL = (TraceParam *)1;
void toTraceParam(Rule * r, size_t sz, const Param * p, TraceParam * prm)
{
    for (size_t i = 0; i < sz; i++)
    {
        prm[i].src.x = getPrm(r, i, Meta::xSrc, p);
        prm[i].src.y = getPrm(r, i, Meta::ySrc, p);
        prm[i].rcv.x = getPrm(r, i, Meta::xRcv, p);
        prm[i].rcv.y = getPrm(r, i, Meta::yRcv, p);
        prm[i].cmp.x = getPrm(r, i, Meta::xCmp, p);
        prm[i].cmp.y = getPrm(r, i, Meta::yCmp, p);
        prm[i].line.il = getPrm(r, i, Meta::il, p);
        prm[i].line.xl = getPrm(r, i, Meta::xl, p);
        prm[i].tn = getPrm(r, i, Meta::tn, p);
    }
}

void fromTraceParam(Rule * r, size_t sz, const TraceParam * prm, Param * p)
{
    for (size_t i = 0; i < sz; i++)
    {
        setPrm(r, i, Meta::xSrc, prm[i].src.x, p);
        setPrm(r, i, Meta::ySrc, prm[i].src.y, p);
        setPrm(r, i, Meta::xRcv, prm[i].rcv.x, p);
        setPrm(r, i, Meta::yRcv, prm[i].rcv.y, p);
        setPrm(r, i, Meta::xCmp, prm[i].cmp.x, p);
        setPrm(r, i, Meta::yCmp, prm[i].cmp.y, p);
        setPrm(r, i, Meta::il, prm[i].line.il, p);
        setPrm(r, i, Meta::xl, prm[i].line.xl, p);
        setPrm(r, i, Meta::tn, llint(prm[i].tn), p);
    }
}


Direct::Direct(const Piol piol, const std::string name, FileMode mode)
{
    const File::SEGY::Opt f;
    const Obj::SEGY::Opt o;
    const Data::MPIIO::Opt d;
    auto data = std::make_shared<Data::MPIIO>(piol, name, d, mode);
    auto obj = std::make_shared<Obj::SEGY>(piol, name, o, data, mode);
    file = std::make_shared<File::SEGY>(piol, name, f, obj, mode);
    rule = f.rule;
}

const std::string & Direct::readText(void) const
{
    return file->readText();
}

size_t Direct::readNs(void) const
{
    return file->readNs();
}

size_t Direct::readNt(void)
{
    return file->readNt();
}

geom_t Direct::readInc(void) const
{
    return file->readInc();
}

void Direct::readTraceParam(csize_t offset, csize_t sz, TraceParam * prm) const
{
    Param p(rule.get(), sz);
    file->readParam(offset, sz, &p);
    toTraceParam(rule.get(), sz, &p, prm);
}

void Direct::writeTraceParam(csize_t offset, csize_t sz, const TraceParam * prm)
{
    Param p(rule.get(), sz);
    fromTraceParam(rule.get(), sz, prm, &p);
    file->writeParam(offset, sz, &p);
}

void Direct::readTrace(csize_t offset, csize_t sz, trace_t * trace, TraceParam * prm) const
{
    if (prm != PRM_NULL)
    {
        Param p(rule.get(), sz);
        file->readTrace(offset, sz, trace, &p);
        toTraceParam(rule.get(), sz, &p, prm);
    }
    else
        file->readTrace(offset, sz, trace, const_cast<Param *>(PARAM_NULL));
}

void Direct::writeTrace(csize_t offset, csize_t sz, trace_t * trace, const TraceParam * prm)
{
    if (prm != PRM_NULL)
    {
        Param p(rule.get(), sz);
        fromTraceParam(rule.get(), sz, prm, &p);
        file->writeTrace(offset, sz, trace, &p);
    }
    else
        file->writeTrace(offset, sz, trace, PARAM_NULL);
}

void Direct::readTrace(csize_t sz, csize_t * offset, trace_t * trace, TraceParam * prm) const
{
    if (prm != PRM_NULL)
    {
        Param p(rule.get(), sz);
        file->readTrace(sz, offset, trace, &p);
        toTraceParam(rule.get(), sz, &p, prm);
    }
    else
        file->readTrace(sz, offset, trace, const_cast<Param *>(PARAM_NULL));
}

void Direct::writeTrace(csize_t sz, csize_t * offset, trace_t * trace, const TraceParam * prm)
{
    if (prm != PRM_NULL)
    {
        Param p(rule.get(), sz);
        fromTraceParam(rule.get(), sz, prm, &p);
        file->writeTrace(sz, offset, trace, &p);
    }
    else
        file->writeTrace(sz, offset, trace, PARAM_NULL);
}

void Direct::readTraceParam(csize_t sz, csize_t * offset, TraceParam * prm) const
{
    Param p(rule.get(), sz);
    file->readParam(sz, offset, &p);
    toTraceParam(rule.get(), sz, &p, prm);
}

void Direct::writeTraceParam(csize_t sz, csize_t * offset, const TraceParam * prm)
{
    Param p(rule.get(), sz);
    fromTraceParam(rule.get(), sz, prm, &p);
    file->writeParam(sz, offset, &p);
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
