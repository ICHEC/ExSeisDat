////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
/// @details Functions etc for C Flow API
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/Flow.h"
#include "ExSeisDat/Flow/set.hh"

#include "not_null.hh"

#include <assert.h>
#include <cstddef>

extern "C" {

PIOL_Set* PIOL_Set_new(const PIOL_ExSeis* piol, const char* ptrn)
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
  PIOL_Set* set, PIOL_Meta m1, PIOL_Meta m2, struct PIOL_CoordElem* minmax)
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
  bool (*const func)(const PIOL_File_Param* param, size_t i, size_t j))
{
    assert(not_null(set));
    assert(func != nullptr);

    set->sort(func);
}

void PIOL_Set_taper(
  PIOL_Set* set, PIOL_TaperType type, size_t ntpstr, size_t ntpend)
{
    assert(not_null(set));

    set->taper(type, ntpstr, ntpend);
}

void PIOL_Set_AGC(
  PIOL_Set* set, PIOL_AGCType type, size_t window, PIOL_trace_t normR)
{
    assert(not_null(set));

    set->AGC(type, window, normR);
}

void PIOL_Set_output(PIOL_Set* set, const char* oname)
{
    assert(not_null(set));
    assert(not_null(oname));

    set->output(oname);
}

void PIOL_Set_text(PIOL_Set* set, const char* outmsg)
{
    assert(not_null(set));
    assert(not_null(outmsg));

    set->text(outmsg);
}

void PIOL_Set_summary(const PIOL_Set* set)
{
    assert(not_null(set));

    set->summary();
}

void PIOL_Set_add(PIOL_Set* set, const char* name)
{
    assert(not_null(set));
    assert(not_null(name));

    set->add(name);
}

}  // extern "C"
