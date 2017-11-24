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


extern "C"
{

PIOL_File_RuleHandle PIOL_File_Rule_new(bool def)
{
    return new PIOL_File_RulePointer(new PIOL::File::Rule(true, def));
}

PIOL_File_RuleHandle PIOL_File_Rule_new_from_list(size_t n, const PIOL_Meta * m)
{
    return new PIOL_File_RulePointer(
        new PIOL::File::Rule({m, m+n}, true, false, false)
    );
}

void PIOL_File_Rule_delete(PIOL_File_RuleHandle rule)
{
    delete rule;
}

void PIOL_File_Rule_addLong(PIOL_File_RuleHandle rule, PIOL_Meta m, size_t loc)
{
    (*rule)->addLong(m, static_cast<PIOL::File::Tr>(loc));
}

void PIOL_File_Rule_addShort(PIOL_File_RuleHandle rule, PIOL_Meta m, size_t loc)
{
    (*rule)->addShort(m, static_cast<PIOL::File::Tr>(loc));
}

void PIOL_File_Rule_addSEGYFloat(
    PIOL_File_RuleHandle rule, PIOL_Meta m, size_t loc, size_t scalLoc
) {
    (*rule)->addSEGYFloat(
        m, static_cast<PIOL::File::Tr>(loc),
        static_cast<PIOL::File::Tr>(scalLoc)
    );
}

void PIOL_File_Rule_rmRule(PIOL_File_RuleHandle rule, PIOL_Meta m)
{
    (*rule)->rmRule(m);
}

PIOL_File_ParamHandle PIOL_File_Param_new(PIOL_File_RuleHandle rule, size_t sz)
{
    if(rule == NULL) rule = PIOL_File_Rule_new(true);
    return new PIOL_File_ParamPointer(new PIOL::File::Param(*rule, sz));
}

void PIOL_File_Param_delete(PIOL_File_ParamHandle param)
{
    delete param;
}

short PIOL_File_getPrm_short(size_t i, PIOL_Meta entry, PIOL_File_ParamHandle param)
{
    return PIOL::File::getPrm<short>(i, entry, param->get());
}

int64_t PIOL_File_getPrm_llint(
    size_t i, PIOL_Meta entry, PIOL_File_ParamHandle param
)
{
    return PIOL::File::getPrm<PIOL::llint>(i, entry, param->get());
}

double PIOL_File_getPrm_double(
    size_t i, PIOL_Meta entry, PIOL_File_ParamHandle param
)
{
    return PIOL::File::getPrm<geom_t>(i, entry, param->get());
}

void PIOL_File_setPrm_short(
    size_t i, PIOL_Meta entry, short ret, PIOL_File_ParamHandle param
)
{
    PIOL::File::setPrm(i, entry, ret, param->get());
}

void PIOL_File_setPrm_llint(
    size_t i, PIOL_Meta entry, int64_t ret, PIOL_File_ParamHandle param
)
{
    PIOL::File::setPrm(i, entry, ret, param->get());
}

void PIOL_File_setPrm_double(
    size_t i, PIOL_Meta entry, double ret, PIOL_File_ParamHandle param
)
{
    PIOL::File::setPrm(i, entry, ret, param->get());
}

void PIOL_File_cpyPrm(
    size_t i, const PIOL_File_ParamHandle src,
    size_t j, PIOL_File_ParamHandle dst
)
{
    PIOL::File::cpyPrm(i, src->get(), j, dst->get());
}

//////////////////PIOL////////////////////////////
PIOL_ExSeisHandle PIOL_ExSeis_new(PIOL_Verbosity verbosity)
{
    return new PIOL_ExSeisPointer(new PIOL::ExSeis(verbosity));
}

void PIOL_ExSeis_delete(PIOL_ExSeisHandle piol)
{
    delete piol;
}

void PIOL_ExSeis_barrier(PIOL_ExSeisHandle piol)
{
    (*piol)->barrier();
}

void PIOL_ExSeis_isErr(PIOL_ExSeisHandle piol, const char* msg)
{
    if(msg != NULL) {
        (*piol)->isErr(msg);
    } else {
        (*piol)->isErr();
    }
}

size_t PIOL_ExSeis_getRank(PIOL_ExSeisHandle piol)
{
    return (*piol)->getRank();
}

size_t PIOL_ExSeis_getNumRank(PIOL_ExSeisHandle piol)
{
    return (*piol)->getNumRank();
}

size_t PIOL_ExSeis_max(PIOL_ExSeisHandle piol, size_t n) {
    return (*piol)->max(n);
}

////////////////// File Layer ////////////////////////////

PIOL_File_WriteDirectHandle PIOL_File_WriteDirect_new(
    PIOL_ExSeisHandle piol, const char * name
)
{
    return new PIOL_File_WriteDirectPointer(
        new PIOL::File::WriteDirect(**piol, name)
    );
}

PIOL_File_ReadDirectHandle PIOL_File_ReadDirect_new(
    PIOL_ExSeisHandle piol, const char * name
)
{
    return new PIOL_File_ReadDirectPointer(
        new PIOL::File::ReadDirect(**piol, name)
    );
}

void PIOL_File_ReadDirect_delete(PIOL_File_ReadDirectHandle readDirect)
{
    delete readDirect;
}

void PIOL_File_WriteDirect_delete(PIOL_File_WriteDirectHandle writeDirect)
{
    delete writeDirect;
}

const char * PIOL_File_ReadDirect_readText(
    PIOL_File_ReadDirectHandle readDirect
)
{
    return (*readDirect)->readText().c_str();
}

size_t PIOL_File_ReadDirect_readNs(PIOL_File_ReadDirectHandle readDirect)
{
    return (*readDirect)->readNs();
}

size_t PIOL_File_ReadDirect_readNt(PIOL_File_ReadDirectHandle readDirect)
{
    return (*readDirect)->readNt();
}

double PIOL_File_ReadDirect_readInc(PIOL_File_ReadDirectHandle readDirect)
{
   return (*readDirect)->readInc();
}

void PIOL_File_WriteDirect_writeText(
    PIOL_File_WriteDirectHandle writeDirect, const char * text
)
{
    (*writeDirect)->writeText(text);
}

void PIOL_File_WriteDirect_writeNs(
    PIOL_File_WriteDirectHandle writeDirect, size_t ns
)
{
    (*writeDirect)->writeNs(ns);
}

void PIOL_File_WriteDirect_writeNt(
    PIOL_File_WriteDirectHandle writeDirect, size_t nt
)
{
    (*writeDirect)->writeNt(nt);
}

void PIOL_File_WriteDirect_writeInc(
    PIOL_File_WriteDirectHandle writeDirect, const geom_t inc
)
{
    (*writeDirect)->writeInc(inc);
}

//Contiguous traces
void PIOL_File_ReadDirect_readTrace(
    PIOL_File_ReadDirectHandle readDirect,
    size_t offset, size_t sz, PIOL::trace_t * trace,
    PIOL_File_ParamHandle param
)
{
    if(param != NULL) {
        (*readDirect)->readTrace(offset, sz, trace);
    } else {
        (*readDirect)->readTrace(offset, sz, trace, param->get());
    }
}

void PIOL_File_WriteDirect_writeTrace(
    PIOL_File_WriteDirectHandle writeDirect,
    size_t offset, size_t sz, PIOL::trace_t * trace,
    PIOL_File_ParamHandle param
)
{
    if(param == NULL) {
        (*writeDirect)->writeTrace(offset, sz, trace);
    } else {
        (*writeDirect)->writeTrace(offset, sz, trace, param->get());
    }
}

void PIOL_File_WriteDirect_writeParam(
    PIOL_File_WriteDirectHandle writeDirect,
    size_t offset, size_t sz, PIOL_File_ParamHandle param
)
{
    (*writeDirect)->writeParam(offset, sz, param->get());
}

void PIOL_File_ReadDirect_readParam(
    PIOL_File_ReadDirectHandle readDirect,
    size_t offset, size_t sz, PIOL_File_ParamHandle param
)
{
    (*readDirect)->readParam(offset, sz, param->get());
}

//List traces
#warning TODO: Add readTrace non-contiguous
//void PIOL_File_ReadDirect_readTrace(
//    PIOL_File_ReadDirectHandle readDirect,
//    size_t sz, size_t * offset, trace_t * trace,
//    PIOL_File_ParamHandle param
//)
//{
//    if(param == NULL) {
//        (*readDirect)->readTrace(sz, offset, trace);
//    } else {
//        (*readDirect)->readTrace(sz, offset, trace, *param);
//    }
//        
//}

#warning TODO: Add writeTrace non-contiguous
//void PIOL_File_WriteDirect_writeTrace(
//    PIOL_File_WriteDirectHandle writeDirect,
//    size_t sz, size_t * offset, trace_t * trace,
//    PIOL_File_ParamHandle param
//)
//{
//    if(param == NULL) {
//        (*writeDirect)->writeTrace(sz, offset, trace);
//    } else {
//        (*writeDirect)->writeTrace(sz, offset, trace, param);
//    }
//}

#warning TODO: add writeParam non-contiguous
//void PIOL_File_WriteDirect_writeParam(
//    PIOL_File_WriteDirectHandle writeDirect,
//    size_t sz, size_t * offset, PIOL_File_ParamHandle param
//)
//{
//    (*writeDirect)->writeParam( sz, offset, *param);
//}

#warning TODO: add readParam non-contiguous
//void PIOL_File_ReadDirect_readParam(
//    PIOL_File_ReadDirectHandle readDirect,
//    size_t sz, size_t * offset, PIOL_File_ParamHandle param
//)
//{
//    (*readDirect)->readParam(sz, offset, *param);
//}

/////////////////////////////////////Operations///////////////////////////////

void PIOL_File_getMinMax(
    PIOL_ExSeisHandle piol,
    size_t offset, size_t sz, PIOL_Meta m1, PIOL_Meta m2, PIOL_File_ParamHandle param,
    CoordElem * minmax
)
{
    PIOL::File::getMinMax(**piol, offset, sz, m1, m2, param->get(), minmax);
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
    //return sizeof(Param) + SEGSz::getMDSz();
}

////////////////////////////////////SET/////////////////////////////////////////
PIOL_SetHandle PIOL_Set_new(PIOL_ExSeisHandle piol, const char * ptrn)
{
    return new PIOL_SetPointer(new PIOL::Set(**piol, ptrn));
}

void PIOL_Set_delete(PIOL_SetHandle set)
{
    delete set;
}

void PIOL_Set_getMinMax(
    PIOL_SetHandle set, PIOL_Meta m1, PIOL_Meta m2, CoordElem * minmax
)
{
    (*set)->getMinMax(m1, m2, minmax);
}

void PIOL_Set_sort(PIOL_SetHandle set, PIOL_SortType type)
{
    (*set)->sort(type);
}

void PIOL_Set_sort_fn(
    PIOL_SetHandle set,
    bool (* func)(const PIOL_File_ParamHandle param, PIOL::csize_t i, PIOL::csize_t j)
)
{
    auto lam = [func] (
        const PIOL::File::Param * param, PIOL::csize_t i, PIOL::csize_t j
    ) -> bool
    {
        // Create a shared_ptr with no deleter to satisfy types for
        // PIOL_File_ParamHandle.
        // WARNING: func must not store a copy of PIOL_File_ParamHandle!
        //
        auto param_ptr = PIOL_File_ParamPointer(
            const_cast<PIOL::File::Param*>(param), [](PIOL::File::Param*){}
        );

        return func(&param_ptr, i, j);
    };
    (*set)->sort(lam);
}

void PIOL_Set_taper(
    PIOL_SetHandle set, TaperType type, size_t ntpstr, size_t ntpend)
{
    (*set)->taper(type, ntpstr, ntpend);
}

void PIOL_Set_AGC(PIOL_SetHandle set, AGCType type, size_t window, float normR)
{
    (*set)->AGC(type, window, normR);
}

void PIOL_Set_output(PIOL_SetHandle set, const char * oname)
{
    (*set)->output(oname);
}

void PIOL_Set_text(PIOL_SetHandle set, const char * outmsg)
{
    (*set)->text(outmsg);
}

void PIOL_Set_summary(PIOL_SetHandle set)
{
    (*set)->summary();
}

void PIOL_Set_add(PIOL_SetHandle set, const char * name)
{
    (*set)->add(name);
}

} // extern "C"
