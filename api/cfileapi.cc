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
#include "flow.h"

#include <iostream>
#include <cstddef>
#include <assert.h>

#include "cppfileapi.hh"
#include "flow.hh"
#include "file/dynsegymd.hh"
#include "share/segy.hh"
#include "share/api.hh"
#include "flow/set.hh"


// Test for pointer null, empty shared_ptr, or shared_ptr to null
template<typename T>
static inline bool not_null(const T* t)
{
    return t != nullptr;
}

template<typename T>
static inline bool not_null(const std::shared_ptr<T>* t)
{
    return t != nullptr && *t;
}

extern "C"
{

PIOL_File_Rule* PIOL_File_Rule_new(bool def)
{
    return new std::shared_ptr<PIOL::File::Rule>(
        new PIOL::File::Rule(true, def)
    );
}

PIOL_File_Rule* PIOL_File_Rule_new_from_list(const PIOL_Meta * m, size_t n)
{
    assert(not_null(m));

    return new std::shared_ptr<PIOL::File::Rule>(
        new PIOL::File::Rule({m, m+n}, true, false, false)
    );
}

void PIOL_File_Rule_delete(PIOL_File_Rule* rule)
{
    delete rule;
}

void PIOL_File_Rule_addLong(PIOL_File_Rule* rule, PIOL_Meta m, PIOL_Tr loc)
{
    assert(not_null(rule));

    (**rule).addLong(m, loc);
}

void PIOL_File_Rule_addShort(PIOL_File_Rule* rule, PIOL_Meta m, PIOL_Tr loc)
{
    assert(not_null(rule));

    (**rule).addShort(m, loc);
}

void PIOL_File_Rule_addSEGYFloat(
    PIOL_File_Rule* rule, PIOL_Meta m, PIOL_Tr loc, PIOL_Tr scalLoc
) {
    assert(not_null(rule));

    (**rule).addSEGYFloat(m, loc, scalLoc);
}

void PIOL_File_Rule_addIndex(PIOL_File_Rule* rule, PIOL_Meta m)
{
    assert(not_null(rule));

    (**rule).addIndex(m);
}

void PIOL_File_Rule_addCopy(PIOL_File_Rule* rule)
{
    assert(not_null(rule));

    (**rule).addCopy();
}

void PIOL_File_Rule_rmRule(PIOL_File_Rule* rule, PIOL_Meta m)
{
    assert(not_null(rule));

    (**rule).rmRule(m);
}

size_t PIOL_File_Rule_extent(PIOL_File_Rule* rule)
{
    assert(not_null(rule));

    return (**rule).extent();
}

size_t PIOL_File_Rule_memUsage(const PIOL_File_Rule* rule)
{
    assert(not_null(rule));

    return (**rule).memUsage();
}

size_t PIOL_File_Rule_paramMem(const PIOL_File_Rule* rule)
{
    assert(not_null(rule));

    return (**rule).paramMem();
}

PIOL_File_Param* PIOL_File_Param_new(PIOL_File_Rule* rule, size_t sz)
{
    if(not_null(rule))
    {
        return new PIOL::File::Param(*rule, sz);
    }
    else
    {
        return new PIOL::File::Param(sz);
    }
}

void PIOL_File_Param_delete(PIOL_File_Param* param)
{
    delete param;
}

size_t PIOL_File_Param_size(const PIOL_File_Param* param)
{
    assert(not_null(param));

    return param->size();
}

size_t PIOL_File_Param_memUsage(const PIOL_File_Param* param) {
    assert(not_null(param));

    return param->memUsage();
}

bool PIOL_File_Rule_addRule_Meta(PIOL_File_Rule* rule, PIOL_Meta m)
{
    assert(not_null(rule));

    return (**rule).addRule(m);
}

bool PIOL_File_Rule_addRule_Rule(PIOL_File_Rule* rule, const PIOL_File_Rule* ruleToCopy)
{
    assert(not_null(rule));
    assert(not_null(ruleToCopy));

    return (**rule).addRule(**ruleToCopy);
}

int16_t PIOL_File_getPrm_short(size_t i, PIOL_Meta entry, PIOL_File_Param* param)
{
    assert(not_null(param));

    return PIOL::File::getPrm<int16_t>(i, entry, param);
}

PIOL_llint PIOL_File_getPrm_llint(
    size_t i, PIOL_Meta entry, PIOL_File_Param* param
)
{
    assert(not_null(param));

    return PIOL::File::getPrm<PIOL::llint>(i, entry, param);
}

PIOL_geom_t PIOL_File_getPrm_double(
    size_t i, PIOL_Meta entry, PIOL_File_Param* param
)
{
    assert(not_null(param));

    return PIOL::File::getPrm<PIOL::geom_t>(i, entry, param);
}

void PIOL_File_setPrm_short(
    size_t i, PIOL_Meta entry, int16_t ret, PIOL_File_Param* param
)
{
    assert(not_null(param));

    PIOL::File::setPrm(i, entry, ret, param);
}

void PIOL_File_setPrm_llint(
    size_t i, PIOL_Meta entry, PIOL_llint ret, PIOL_File_Param* param
)
{
    assert(not_null(param));

    PIOL::File::setPrm(i, entry, ret, param);
}

void PIOL_File_setPrm_double(
    size_t i, PIOL_Meta entry, PIOL_geom_t ret, PIOL_File_Param* param
)
{
    assert(not_null(param));

    PIOL::File::setPrm(i, entry, ret, param);
}

void PIOL_File_cpyPrm(
    size_t i, const PIOL_File_Param* src,
    size_t j, PIOL_File_Param* dst
)
{
    assert(not_null(src));
    assert(not_null(dst));

    PIOL::File::cpyPrm(i, src, j, dst);
}

//////////////////PIOL////////////////////////////
PIOL_ExSeis* PIOL_ExSeis_new(PIOL_Verbosity verbosity)
{
    return new PIOL::ExSeis(verbosity);
}

void PIOL_ExSeis_delete(PIOL_ExSeis* piol)
{
    delete piol;
}

void PIOL_ExSeis_barrier(PIOL_ExSeis* piol)
{
    assert(not_null(piol));

    piol->barrier();
}

void PIOL_ExSeis_isErr(PIOL_ExSeis* piol, const char* msg)
{
    assert(not_null(piol));

    if(msg != NULL)
    {
        piol->isErr(msg);
    }
    else
    {
        piol->isErr();
    }
}

size_t PIOL_ExSeis_getRank(PIOL_ExSeis* piol)
{
    assert(not_null(piol));

    return piol->getRank();
}

size_t PIOL_ExSeis_getNumRank(PIOL_ExSeis* piol)
{
    assert(not_null(piol));

    return piol->getNumRank();
}

size_t PIOL_ExSeis_max(PIOL_ExSeis* piol, size_t n)
{
    assert(not_null(piol));

    return piol->max(n);
}

////////////////// File Layer ////////////////////////////

PIOL_File_WriteDirect* PIOL_File_WriteDirect_new(
    PIOL_ExSeis* piol, const char * name
)
{
    assert(not_null(piol));
    assert(not_null(name));

    return new PIOL::File::WriteDirect(*piol, name);
}

PIOL_File_ReadDirect* PIOL_File_ReadDirect_new(
    PIOL_ExSeis* piol, const char * name
)
{
    assert(not_null(piol));
    assert(not_null(name));

    return new PIOL::File::ReadDirect(*piol, name);
}

void PIOL_File_ReadDirect_delete(PIOL_File_ReadDirect* readDirect)
{
    delete readDirect;
}

void PIOL_File_WriteDirect_delete(PIOL_File_WriteDirect* writeDirect)
{
    delete writeDirect;
}

const char * PIOL_File_ReadDirect_readText(
    PIOL_File_ReadDirect* readDirect
)
{
    assert(not_null(readDirect));

    return readDirect->readText().c_str();
}

size_t PIOL_File_ReadDirect_readNs(PIOL_File_ReadDirect* readDirect)
{
    assert(not_null(readDirect));

    return readDirect->readNs();
}

size_t PIOL_File_ReadDirect_readNt(PIOL_File_ReadDirect* readDirect)
{
    assert(not_null(readDirect));

    return readDirect->readNt();
}

double PIOL_File_ReadDirect_readInc(PIOL_File_ReadDirect* readDirect)
{
    assert(not_null(readDirect));

    return readDirect->readInc();
}

void PIOL_File_WriteDirect_writeText(
    PIOL_File_WriteDirect* writeDirect, const char * text
)
{
    assert(not_null(writeDirect));
    assert(not_null(text));

    writeDirect->writeText(text);
}

void PIOL_File_WriteDirect_writeNs(
    PIOL_File_WriteDirect* writeDirect, size_t ns
)
{
    assert(not_null(writeDirect));

    writeDirect->writeNs(ns);
}

void PIOL_File_WriteDirect_writeNt(
    PIOL_File_WriteDirect* writeDirect, size_t nt
)
{
    assert(not_null(writeDirect));

    writeDirect->writeNt(nt);
}

void PIOL_File_WriteDirect_writeInc(
    PIOL_File_WriteDirect* writeDirect, const PIOL_geom_t inc
)
{
    assert(not_null(writeDirect));

    writeDirect->writeInc(inc);
}

//Contiguous traces
void PIOL_File_ReadDirect_readTrace(
    PIOL_File_ReadDirect* readDirect,
    size_t offset, size_t sz, PIOL_trace_t * trace,
    PIOL_File_Param* param
)
{
    assert(not_null(readDirect));
    assert(not_null(trace));

    if(param != NULL) {
        readDirect->readTrace(offset, sz, trace);
    } else {
        readDirect->readTrace(offset, sz, trace, param);
    }
}

void PIOL_File_WriteDirect_writeTrace(
    PIOL_File_WriteDirect* writeDirect,
    size_t offset, size_t sz, PIOL_trace_t * trace,
    PIOL_File_Param* param
)
{
    assert(not_null(writeDirect));
    assert(not_null(trace));

    if(param == NULL) {
        writeDirect->writeTrace(offset, sz, trace);
    } else {
        writeDirect->writeTrace(offset, sz, trace, param);
    }
}

void PIOL_File_WriteDirect_writeParam(
    PIOL_File_WriteDirect* writeDirect,
    size_t offset, size_t sz, PIOL_File_Param* param
)
{
    assert(not_null(writeDirect));
    assert(not_null(param));

    writeDirect->writeParam(offset, sz, param);
}

void PIOL_File_ReadDirect_readParam(
    PIOL_File_ReadDirect* readDirect,
    size_t offset, size_t sz, PIOL_File_Param* param
)
{
    assert(not_null(readDirect));
    assert(not_null(param));

    readDirect->readParam(offset, sz, param);
}

//List traces
#warning TODO: Add readTrace non-contiguous
//void PIOL_File_ReadDirect_readTrace(
//    PIOL_File_ReadDirect* readDirect,
//    size_t sz, size_t * offset, trace_t * trace,
//    PIOL_File_Param* param
//)
//{
//    if(param == NULL) {
//        readDirect->readTrace(sz, offset, trace);
//    } else {
//        readDirect->readTrace(sz, offset, trace, param);
//    }
//        
//}

#warning TODO: Add writeTrace non-contiguous
//void PIOL_File_WriteDirect_writeTrace(
//    PIOL_File_WriteDirect* writeDirect,
//    size_t sz, size_t * offset, trace_t * trace,
//    PIOL_File_Param* param
//)
//{
//    if(param == NULL) {
//        writeDirect->writeTrace(sz, offset, trace);
//    } else {
//        writeDirect->writeTrace(sz, offset, trace, param);
//    }
//}

#warning TODO: add writeParam non-contiguous
//void PIOL_File_WriteDirect_writeParam(
//    PIOL_File_WriteDirect* writeDirect,
//    size_t sz, size_t * offset, PIOL_File_Param* param
//)
//{
//    writeDirect->writeParam( sz, offset, param);
//}

#warning TODO: add readParam non-contiguous
//void PIOL_File_ReadDirect_readParam(
//    PIOL_File_ReadDirect* readDirect,
//    size_t sz, size_t * offset, PIOL_File_Param* param
//)
//{
//    readDirect->readParam(sz, offset, param);
//}

/////////////////////////////////////Operations///////////////////////////////

void PIOL_File_getMinMax(
    PIOL_ExSeis* piol,
    size_t offset, size_t sz, PIOL_Meta m1, PIOL_Meta m2, PIOL_File_Param* param,
    struct PIOL_CoordElem * minmax
)
{
    assert(not_null(piol));
    assert(not_null(param));
    assert(not_null(minmax));

    PIOL::File::getMinMax(*piol, offset, sz, m1, m2, param, minmax);
}

//////////////////////////////////////SEGSZ///////////////////////////////////
size_t PIOL_SEGSz_getTextSz()
{
    return PIOL::SEGSz::getTextSz();
}

size_t PIOL_SEGSz_getDFSz(size_t ns)
{
    return PIOL::SEGSz::getDFSz<float>(ns);
}

size_t PIOL_SEGSz_getFileSz(size_t nt, size_t ns)
{
    return PIOL::SEGSz::getFileSz<float>(nt, ns);
}

//TODO UPDATE
size_t PIOL_SEGSz_getMDSz(void)
{
    return PIOL::SEGSz::getMDSz();
}

////////////////////////////////////SET/////////////////////////////////////////
PIOL_Set* PIOL_Set_new(PIOL_ExSeis* piol, const char * ptrn)
{
    assert(not_null(piol));
    assert(not_null(ptrn));

    return new PIOL::Set(*piol, ptrn);
}

void PIOL_Set_delete(PIOL_Set* set)
{
    delete set;
}

void PIOL_Set_getMinMax(
    PIOL_Set* set, PIOL_Meta m1, PIOL_Meta m2, struct PIOL_CoordElem * minmax
)
{
    assert(not_null(set));
    assert(not_null(minmax));

    set->getMinMax(m1, m2, minmax);
}

void PIOL_Set_sort(PIOL_Set* set, PIOL_SortType type)
{
    assert(not_null(set));

    set->sort(type);
}

void PIOL_Set_sort_fn(
    PIOL_Set* set,
    bool (* const func)(const PIOL_File_Param* param, size_t i, size_t j)
)
{
    assert(not_null(set));
    assert(func != nullptr);

    set->sort(func);
}

void PIOL_Set_taper(
    PIOL_Set* set, TaperType type, size_t ntpstr, size_t ntpend)
{
    assert(not_null(set));

    set->taper(type, ntpstr, ntpend);
}

void PIOL_Set_AGC(PIOL_Set* set, AGCType type, size_t window, PIOL_trace_t normR)
{
    assert(not_null(set));

    set->AGC(type, window, normR);
}

void PIOL_Set_output(PIOL_Set* set, const char * oname)
{
    assert(not_null(set));
    assert(not_null(oname));

    set->output(oname);
}

void PIOL_Set_text(PIOL_Set* set, const char * outmsg)
{
    assert(not_null(set));
    assert(not_null(outmsg));

    set->text(outmsg);
}

void PIOL_Set_summary(PIOL_Set* set)
{
    assert(not_null(set));

    set->summary();
}

void PIOL_Set_add(PIOL_Set* set, const char * name)
{
    assert(not_null(set));
    assert(not_null(name));

    set->add(name);
}

} // extern "C"
