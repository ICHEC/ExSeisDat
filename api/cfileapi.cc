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
#include "set.hh"
#include "set.h"

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

void addFloat(RuleHdl rule, Meta m, size_t loc, size_t scalLoc)
{
    rule->rule->addFloat(m, static_cast<File::Tr>(loc), static_cast<File::Tr>(scalLoc));
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

void readFullTrace(ExSeisFile f, size_t offset, size_t sz, trace_t * trace, CParam prm)
{
    f->file->readTrace(offset, sz, trace, prm->param);
}

void writeTrace(ExSeisFile f, size_t offset, size_t sz, trace_t * trace)
{
    f->file->writeTrace(offset, sz, trace);
}

void writeFullTrace(ExSeisFile f, size_t offset, size_t sz, trace_t * trace, const CParam prm)
{
    f->file->writeTrace(offset, sz, trace, static_cast<const File::Param *>(prm->param));
}

void writeParam(ExSeisFile f, size_t offset, size_t sz, const CParam prm)
{
    f->file->writeParam(offset, sz, static_cast<const File::Param *>(prm->param));
}

void readParam(ExSeisFile f, size_t offset, size_t sz, CParam prm)
{
    f->file->readParam(offset, sz, prm->param);
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
void readFullListTrace(ExSeisFile f, size_t sz, size_t * offset, trace_t * trace, CParam prm)
{
    f->file->readTrace(sz, offset, trace, prm->param);
}

void writeFullListTrace(ExSeisFile f, size_t sz, size_t * offset, trace_t * trace, const CParam prm)
{
    f->file->writeTrace(sz, offset, trace, static_cast<const File::Param *>(prm->param));
}

void writeListParam(ExSeisFile f, size_t sz, size_t * offset, const CParam prm)
{
    f->file->writeParam(sz, offset, static_cast<const File::Param *>(prm->param));
}

void readListParam(ExSeisFile f, size_t sz, size_t * offset, CParam prm)
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

#warning UPDATE
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

/*void sortSet(ExSeisSet s, bool (* func)(const Param *, const Param *))
{
    s->set->sort([func] (const Param & a, const Param & b) -> bool { return func(&a, &b); });
}*/

void sortSet(ExSeisSet s, SortType type)
{
    s->set->sort(type);
}

size_t getInNt(ExSeisSet s)
{
    return s->set->getInNt();
}

size_t getLNtSet(ExSeisSet s)
{
    return s->set->getLNt();
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
}
