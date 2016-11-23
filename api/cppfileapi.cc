#include "cppfileapi.hh"
#include "global.hh"
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
const TraceParam * PRM_NULL = (TraceParam *)1;
void toTraceParam(size_t sz, const Param * p, TraceParam * prm)
{
    for (size_t i = 0; i < sz; i++)
    {
        prm[i].src.x = getPrm<geom_t>(i, Meta::xSrc, p);
        prm[i].src.y = getPrm<geom_t>(i, Meta::ySrc, p);
        prm[i].rcv.x = getPrm<geom_t>(i, Meta::xRcv, p);
        prm[i].rcv.y = getPrm<geom_t>(i, Meta::yRcv, p);
        prm[i].cmp.x = getPrm<geom_t>(i, Meta::xCmp, p);
        prm[i].cmp.y = getPrm<geom_t>(i, Meta::yCmp, p);
        prm[i].line.il = getPrm<llint>(i, Meta::il, p);
        prm[i].line.xl = getPrm<llint>(i, Meta::xl, p);
        prm[i].tn = getPrm<llint>(i, Meta::tn, p);
    }
}

void fromTraceParam(size_t sz, const TraceParam * prm, Param * p)
{
    for (size_t i = 0; i < sz; i++)
    {
        setPrm(i, Meta::xSrc, prm[i].src.x, p);
        setPrm(i, Meta::ySrc, prm[i].src.y, p);
        setPrm(i, Meta::xRcv, prm[i].rcv.x, p);
        setPrm(i, Meta::yRcv, prm[i].rcv.y, p);
        setPrm(i, Meta::xCmp, prm[i].cmp.x, p);
        setPrm(i, Meta::yCmp, prm[i].cmp.y, p);
        setPrm(i, Meta::il, prm[i].line.il, p);
        setPrm(i, Meta::xl, prm[i].line.xl, p);
        setPrm(i, Meta::tn, llint(prm[i].tn), p);
    }
}

Direct::Direct(const Piol piol, const std::string name, FileMode mode, std::shared_ptr<Rule> rule_)
{
    const File::SEGY::Opt f;
    const Obj::SEGY::Opt o;
    const Data::MPIIO::Opt d;
    auto data = std::make_shared<Data::MPIIO>(piol, name, d, mode);
    auto obj = std::make_shared<Obj::SEGY>(piol, name, o, data, mode);
    file = std::make_shared<File::SEGY>(piol, name, f, obj, mode);
    rule = rule_;
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
    Param p(rule, sz);
    file->readParam(offset, sz, &p);
    toTraceParam(sz, &p, prm);
}

void Direct::readTraceParam(csize_t offset, csize_t sz, Param * prm) const
{
    file->readParam(offset, sz, prm);
}

void Direct::writeTraceParam(csize_t offset, csize_t sz, const TraceParam * prm)
{
    Param p(rule, sz);
    fromTraceParam(sz, prm, &p);
    file->writeParam(offset, sz, &p);
}

void Direct::writeTraceParam(csize_t offset, csize_t sz, const Param * prm)
{
    file->writeParam(offset, sz, prm);
}

void Direct::readTrace(csize_t offset, csize_t sz, trace_t * trace, TraceParam * prm) const
{
    if (prm != PRM_NULL)
    {
        Param p(rule, sz);
        file->readTrace(offset, sz, trace, &p);
        toTraceParam(sz, &p, prm);
    }
    else
        file->readTrace(offset, sz, trace, const_cast<Param *>(PARAM_NULL));
}

void Direct::readTrace(csize_t offset, csize_t sz, trace_t * trace, Param * prm) const
{
    file->readTrace(offset, sz, trace, prm);
}

void Direct::writeTrace(csize_t offset, csize_t sz, trace_t * trace, const TraceParam * prm)
{
    if (prm != PRM_NULL)
    {
        Param p(rule, sz);
        fromTraceParam(sz, prm, &p);
        file->writeTrace(offset, sz, trace, &p);
    }
    else
        file->writeTrace(offset, sz, trace, PARAM_NULL);
}

void Direct::writeTrace(csize_t offset, csize_t sz, trace_t * trace, const Param * prm)
{
    file->writeTrace(offset, sz, trace, prm);
}

void Direct::readTrace(csize_t sz, csize_t * offset, trace_t * trace, TraceParam * prm) const
{
    if (prm != PRM_NULL)
    {
        Param p(rule, sz);
        file->readTrace(sz, offset, trace, &p);
        toTraceParam(sz, &p, prm);
    }
    else
        file->readTrace(sz, offset, trace, const_cast<Param *>(PARAM_NULL));
}

void Direct::readTrace(csize_t sz, csize_t * offset, trace_t * trace, Param * prm) const
{
    file->readTrace(sz, offset, trace, prm);
}

void Direct::writeTrace(csize_t sz, csize_t * offset, trace_t * trace, const TraceParam * prm)
{
    if (prm != PRM_NULL)
    {
        Param p(rule, sz);
        fromTraceParam(sz, prm, &p);
        file->writeTrace(sz, offset, trace, &p);
    }
    else
        file->writeTrace(sz, offset, trace, PARAM_NULL);
}

void Direct::writeTrace(csize_t sz, csize_t * offset, trace_t * trace, const Param * prm)
{
    file->writeTrace(sz, offset, trace, prm);
}

void Direct::readTraceParam(csize_t sz, csize_t * offset, TraceParam * prm) const
{
    Param p(rule, sz);
    file->readParam(sz, offset, &p);
    toTraceParam(sz, &p, prm);
}

void Direct::readTraceParam(csize_t sz, csize_t * offset, Param * prm) const
{
    file->readParam(sz, offset, prm);
}

void Direct::writeTraceParam(csize_t sz, csize_t * offset, const TraceParam * prm)
{
    Param p(rule, sz);
    fromTraceParam(sz, prm, &p);
    file->writeParam(sz, offset, &p);
}

void Direct::writeTraceParam(csize_t sz, csize_t * offset, const Param * prm)
{
    file->writeParam(sz, offset, prm);
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
