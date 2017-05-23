/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date December 2016
 *   \brief
 *   \details Functions etc for C11 API
 *//*******************************************************************************************/
#include "global.hh"
#include "cfileapi.h"
#include <iostream>
#include <cstddef>
#include <assert.h>
#include "cppfileapi.hh"
#include "file/dynsegymd.hh"
#include "share/segy.hh"
#include "share/api.hh"
#include "fileops.hh"
#include "flow.hh"
#include "flow.h"

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

struct ExSeisReadWrapper
{
    PIOL::File::ReadDirect * file;
};

struct ExSeisWriteWrapper
{
    PIOL::File::WriteDirect * file;
};

struct ExSeisSetWrapper
{
    PIOL::Set * set;
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

void addLongRule(RuleHdl rule, Meta m, size_t loc)
{
    rule->rule->addLong(m, static_cast<File::Tr>(loc));
}

void addShortRule(RuleHdl rule, Meta m, size_t loc)
{
    rule->rule->addShort(m, static_cast<File::Tr>(loc));
}

void addSEGYFloat(RuleHdl rule, Meta m, size_t loc, size_t scalLoc)
{
    rule->rule->addSEGYFloat(m, static_cast<File::Tr>(loc), static_cast<File::Tr>(scalLoc));
}

void rmRule(RuleHdl rule, Meta m)
{
    rule->rule->rmRule(m);
}

CParam initDefParam(size_t sz)
{
    auto rule = std::make_shared<File::Rule>(true, true);
    auto wrap = new ParamWrapper;
    wrap->param = new File::Param(rule, sz);
    return wrap;
}

CParam initParam(RuleHdl rule, size_t sz)
{
    auto wrap = new ParamWrapper;
    wrap->param = new File::Param(rule->rule, sz);
    return wrap;
}

void freeParam(CParam prm)
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

short getShortPrm(size_t i, Meta entry, const CParam prm)
{
    return File::getPrm<short>(i, entry, prm->param);
}

int64_t getLongPrm(size_t i, Meta entry, const CParam prm)
{
    return File::getPrm<llint>(i, entry, prm->param);
}

double getFloatPrm(size_t i, Meta entry, const CParam prm)
{
    return File::getPrm<geom_t>(i, entry, prm->param);
}

void setShortPrm(size_t i, Meta entry, short ret, CParam prm)
{
    File::setPrm(i, entry, ret, prm->param);
}

void setLongPrm(size_t i, Meta entry, int64_t ret, CParam prm)
{
    File::setPrm(i, entry, ret, prm->param);
}

void setFloatPrm(size_t i, Meta entry, double ret, CParam prm)
{
    File::setPrm(i, entry, ret, prm->param);
}

void cpyPrm(size_t i, const CParam src, size_t j, CParam dst)
{
    File::cpyPrm(i, src->param, j, dst->param);
}

//////////////////PIOL////////////////////////////
ExSeisHandle initMPIOL(void)
{
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

ExSeisWrite openWriteFile(ExSeisHandle piol, const char * name)
{
    auto wrap = new ExSeisWriteWrapper;
    wrap->file = new File::WriteDirect(*piol->piol, name);
    return wrap;
}

ExSeisRead openReadFile(ExSeisHandle piol, const char * name)
{
    auto wrap = new ExSeisReadWrapper;
    wrap->file = new File::ReadDirect(*piol->piol, name);
    return wrap;
}

void closeReadFile(ExSeisRead f)
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

void closeWriteFile(ExSeisWrite f)
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

const char * readText(ExSeisRead f)
{
    return f->file->readText().c_str();
}

size_t readNs(ExSeisRead f)
{
    return f->file->readNs();
}

size_t readNt(ExSeisRead f)
{
    return f->file->readNt();
}

double readInc(ExSeisRead f)
{
   return f->file->readInc();
}

void writeText(ExSeisWrite f, const char * text)
{
    std::string text_(text);
    f->file->writeText(text_);
}

void writeNs(ExSeisWrite f, size_t ns)
{
    f->file->writeNs(ns);
}

void writeNt(ExSeisWrite f, size_t nt)
{
    f->file->writeNt(nt);
}

void writeInc(ExSeisWrite f, const geom_t inc)
{
    f->file->writeInc(inc);
}

//Contiguous traces
void readTrace(ExSeisRead f, size_t offset, size_t sz, trace_t * trace)
{
    f->file->readTrace(offset, sz, trace);
}

void readFullTrace(ExSeisRead f, size_t offset, size_t sz, trace_t * trace, CParam prm)
{
    f->file->readTrace(offset, sz, trace, prm->param);
}

void writeTrace(ExSeisWrite f, size_t offset, size_t sz, trace_t * trace)
{
    f->file->writeTrace(offset, sz, trace);
}

void writeFullTrace(ExSeisWrite f, size_t offset, size_t sz, trace_t * trace, const CParam prm)
{
    f->file->writeTrace(offset, sz, trace, static_cast<const File::Param *>(prm->param));
}

void writeParam(ExSeisWrite f, size_t offset, size_t sz, const CParam prm)
{
    f->file->writeParam(offset, sz, static_cast<const File::Param *>(prm->param));
}

void readParam(ExSeisRead f, size_t offset, size_t sz, CParam prm)
{
    f->file->readParam(offset, sz, prm->param);
}

//List traces
void readListTrace(ExSeisRead f, size_t sz, size_t * offset, trace_t * trace)
{
    f->file->readTrace(sz, offset, trace);
}

void writeListTrace(ExSeisWrite f, size_t sz, size_t * offset, trace_t * trace)
{
    f->file->writeTrace(sz, offset, trace);
}

void readFullListTrace(ExSeisRead f, size_t sz, size_t * offset, trace_t * trace, CParam prm)
{
    f->file->readTrace(sz, offset, trace, prm->param);
}

void writeFullListTrace(ExSeisWrite f, size_t sz, size_t * offset, trace_t * trace, const CParam prm)
{
    f->file->writeTrace(sz, offset, trace, static_cast<const File::Param *>(prm->param));
}

void writeListParam(ExSeisWrite f, size_t sz, size_t * offset, const CParam prm)
{
    f->file->writeParam(sz, offset, static_cast<const File::Param *>(prm->param));
}

void readListParam(ExSeisRead f, size_t sz, size_t * offset, CParam prm)
{
    f->file->readParam(sz, offset, prm->param);
}

/////////////////////////////////////Operations///////////////////////////////

void getMinMax(ExSeisHandle piol, size_t offset, size_t sz, Meta m1, Meta m2, const CParam prm, CoordElem * minmax)
{
    getMinMax(*piol->piol, offset, sz, m1, m2, static_cast<const File::Param *>(prm->param), minmax);
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

//TODO UPDATE
size_t getSEGYParamSz(void)
{
    return SEGSz::getMDSz();
    //return sizeof(Param) + SEGSz::getMDSz();
}

////////////////////////////////////SET/////////////////////////////////////////
ExSeisSet initSet(ExSeisHandle piol, const char * ptrn)
{
    auto wrap = new ExSeisSetWrapper;
    wrap->set = new Set(*piol->piol, ptrn);
    return wrap;
}

void freeSet(ExSeisSet s)
{
    if (s != NULL)
    {
        if (s->set != NULL)
            delete s->set;
        delete s;
    }
    else
        std::cerr << "Invalid free of ExSeisSet NULL.\n";
}

void getMinMaxSet(ExSeisSet s, Meta m1, Meta m2, CoordElem * minmax)
{
    s->set->getMinMax(m1, m2, minmax);
}

void sortSet(ExSeisSet s, SortType type)
{
    s->set->sort(type);
}

void outputSet(ExSeisSet s, const char * oname)
{
    s->set->output(oname);
}

void textSet(ExSeisSet s, const char * outmsg)
{
    s->set->text(outmsg);
}

void summarySet(ExSeisSet s)
{
    s->set->summary();
}

void addSet(ExSeisSet s, const char * name)
{
    s->set->add(name);
}

void sortCustomSet(ExSeisSet s, bool (* func)(const CParam a, const CParam b))
{
    auto lam = [func] (const File::Param & a, const File::Param & b) -> bool
        {
            ParamWrapper awrap = { const_cast<File::Param *>(&a) };
            ParamWrapper bwrap = { const_cast<File::Param *>(&b) };
            return func(&awrap, &bwrap);
    };
    s->set->sort(lam);
}
}
