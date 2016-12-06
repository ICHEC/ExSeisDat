#include "global.hh"
#include "cfileapi.h"
#include <iostream>
#include <cstddef>
#include <assert.h>
#include "cppfileapi.hh"
#include "file/dynsegymd.hh"
#include "share/segy.hh"
#include "fileops.hh"

using namespace PIOL;

extern "C"
{
struct PIOLWrapper
{
    std::shared_ptr<ExSeis> piol;
};

struct RuleWrapper
{
    std::shared_ptr<File::Rule> rule;
};

struct ParamWrapper
{
    File::Param * param;
};

struct ExSeisFileWrapper
{
    PIOL::File::Direct * file;
};

RuleHdl intiRules(bool def)
{
    auto wrap = new RuleWrapper;
    wrap->rule = std::make_shared<File::Rule>(true, def);
    return wrap;
}

void freeRules(RuleHdl rule)
{
    if (rule != NULL)
        delete rule;
    else
        std::cerr << "Invalid free of NULL rule.\n";
}

void addLongRule(RuleHdl rule, CMeta m, size_t loc)
{
    rule->rule->addLong(static_cast<File::Meta>(m), static_cast<File::Tr>(loc));
}

void addShortRule(RuleHdl rule, CMeta m, size_t loc)
{
    rule->rule->addShort(static_cast<File::Meta>(m), static_cast<File::Tr>(loc));
}

void addFloat(RuleHdl rule, CMeta m, size_t loc, size_t scalLoc)
{
    rule->rule->addFloat(static_cast<File::Meta>(m), static_cast<File::Tr>(loc), static_cast<File::Tr>(scalLoc));
}

void rmRule(RuleHdl rule, CMeta m)
{
    rule->rule->rmRule(static_cast<File::Meta>(m));
}

Param newDefParam(size_t sz)
{
    auto rule = std::make_shared<File::Rule>(true, true);
    auto wrap = new ParamWrapper;
    wrap->param = new File::Param(rule, sz);
    return wrap;
}

Param newParam(RuleHdl rule, size_t sz)
{
    auto wrap = new ParamWrapper;
    wrap->param = new File::Param(rule->rule, sz);
    return wrap;
}

void freeParam(Param prm)
{
    if (prm != NULL)
    {
        if (prm->param)
            delete prm->param;
        delete prm;
    }
    else
        std::cerr << "Invalid free of NULL prm.\n";
}

short getShortPrm(size_t i, CMeta entry, const Param prm)
{
    return File::getPrm<short>(i, static_cast<File::Meta>(entry), prm->param);
}

int64_t getLongPrm(size_t i, CMeta entry, const Param prm)
{
    return File::getPrm<llint>(i, static_cast<File::Meta>(entry), prm->param);
}

double getFloatPrm(size_t i, CMeta entry, const Param prm)
{
    return File::getPrm<geom_t>(i, static_cast<File::Meta>(entry), prm->param);
}

void setShortPrm(size_t i, CMeta entry, short ret, Param prm)
{
    File::setPrm(i, static_cast<File::Meta>(entry), ret, prm->param);
}

void setLongPrm(size_t i, CMeta entry, int64_t ret, Param prm)
{
    File::setPrm(i, static_cast<File::Meta>(entry), ret, prm->param);
}

void setFloatPrm(size_t i, CMeta entry, double ret, Param prm)
{
    File::setPrm(i, static_cast<File::Meta>(entry), ret, prm->param);
}

void cpyPrm(size_t i, const Param src, size_t j, Param dst)
{
    File::cpyPrm(i, src->param, j, dst->param);
}

//////////////////PIOL////////////////////////////
ExSeisHandle initMPIOL(void)
{
//TODO: Test the cast of C structures to C++ types here.
    assert(sizeof(File::CoordElem) == sizeof(CoordElem));

    auto wrap = new PIOLWrapper;
    wrap->piol = std::make_shared<ExSeis>();
    return wrap;
}

void freePIOL(ExSeisHandle piol)
{
    if (piol != NULL)
        delete piol;
    else
        std::cerr << "Invalid free of ExSeisPIOL NULL.\n";
}

void barrier(ExSeisHandle piol)
{
    piol->piol->barrier();
}

void isErr(ExSeisHandle piol)
{
    piol->piol->isErr();
}

size_t getRank(ExSeisHandle piol)
{
    return piol->piol->getRank();
}

size_t getNumRank(ExSeisHandle piol)
{
    return piol->piol->getNumRank();
}

////////////////// File Layer ////////////////////////////

ExSeisFile openWriteFile(ExSeisHandle piol, const char * name)
{
    auto wrap = new ExSeisFileWrapper;
    wrap->file = new File::Direct(*piol->piol, name, FileMode::Write);
    return wrap;
}

ExSeisFile openReadFile(ExSeisHandle piol, const char * name)
{
    auto wrap = new ExSeisFileWrapper;
    wrap->file = new File::Direct(*piol->piol, name, FileMode::Read);
    return wrap;
}

void closeFile(ExSeisFile f)
{
    if (f != NULL)
    {
        if (f->file != NULL)
            delete f->file;
        delete f;
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

//Contiguous traces
void readTrace(ExSeisFile f, size_t offset, size_t sz, trace_t * trace)
{
    f->file->readTrace(offset, sz, trace);
}

void readFullTrace(ExSeisFile f, size_t offset, size_t sz, trace_t * trace, Param prm)
{
    f->file->readTrace(offset, sz, trace, static_cast<File::Param *>(prm->param));
}

void writeTrace(ExSeisFile f, size_t offset, size_t sz, trace_t * trace)
{
    f->file->writeTrace(offset, sz, trace);
}

void writeFullTrace(ExSeisFile f, size_t offset, size_t sz, trace_t * trace, const Param prm)
{
    f->file->writeTrace(offset, sz, trace, static_cast<const File::Param *>(prm->param));
}

void writeTraceParam(ExSeisFile f, size_t offset, size_t sz, const Param prm)
{
    f->file->writeTraceParam(offset, sz, static_cast<const File::Param *>(prm->param));
}

void readTraceParam(ExSeisFile f, size_t offset, size_t sz, Param prm)
{
    f->file->readTraceParam(offset, sz, static_cast<File::Param *>(prm->param));
}

//List traces
void readListTrace(ExSeisFile f, size_t sz, size_t * offset, trace_t * trace)
{
    f->file->readTrace(sz, offset, trace);
}

void writeListTrace(ExSeisFile f, size_t sz, size_t * offset, trace_t * trace)
{
    f->file->writeTrace(sz, offset, trace);
}

//
void readFullListTrace(ExSeisFile f, size_t sz, size_t * offset, trace_t * trace, Param prm)
{
    f->file->readTrace(sz, offset, trace, static_cast<File::Param *>(prm->param));
}

void writeFullListTrace(ExSeisFile f, size_t sz, size_t * offset, trace_t * trace, const Param prm)
{
    f->file->writeTrace(sz, offset, trace, static_cast<const File::Param *>(prm->param));
}

void writeListTraceParam(ExSeisFile f, size_t sz, size_t * offset, const Param prm)
{
    f->file->writeTraceParam(sz, offset, static_cast<const File::Param *>(prm->param));
}

void readListTraceParam(ExSeisFile f, size_t sz, size_t * offset, Param prm)
{
    f->file->readTraceParam(sz, offset, static_cast<File::Param *>(prm->param));
}

/////////////////////////////////////Operations///////////////////////////////

#warning Re-introduce minmax to the C API
/*void getMinMax(ExSeisHandle piol, size_t offset, size_t sz, const ccoord_t * coord, CoordElem * minmax)
{
    getMinMax(*piol->piol, offset, sz, reinterpret_cast<const File::coord_t *>(coord), reinterpret_cast<File::CoordElem * >(minmax));
}*/

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

#warning UPDATE
size_t getSEGYParamSz(void)
{
    return SEGSz::getMDSz();
    //return sizeof(TraceParam) + SEGSz::getMDSz();
}
}
