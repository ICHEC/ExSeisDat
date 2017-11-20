#ifndef PIOLSTUBSETSTUB_HEADER_GUARD
#define PIOLSTUBSETSTUB_HEADER_GUARD

#include "global.hh"
#include "cfileapi.h"
#include "cppfileapi.hh"
#include "flow.h"
#include "flow.hh"

#include "gmock/gmock.h"

#include "stubtools.hh"

MAKE_STUB_4_CTOR(PIOL_Set_new_1_cb,  PIOL::Set::Set, :cache(arg1), void(PIOL::Piol, std::string, std::string, std::shared_ptr<PIOL::File::Rule>))

MAKE_STUB_2_CTOR(PIOL_Set_new_2_cb,  PIOL::Set::Set, :cache(arg1), void(PIOL::Piol, std::shared_ptr<PIOL::File::Rule>))

MAKE_STUB_0_CTOR(PIOL_Set_delete_cb, PIOL::Set::~Set,)

MAKE_STUB_1(PIOL_Set_sort_1_cb, PIOL::Set::sort,, void(PIOL::CompareP))

MAKE_STUB_2(PIOL_Set_sort_2_cb, PIOL::Set::sort,, void(std::shared_ptr<PIOL::File::Rule>, PIOL::CompareP))

MAKE_STUB_1(PIOL_Set_output_cb, PIOL::Set::output,, std::vector<std::string>(std::string))

MAKE_STUB_3(PIOL_Set_getMinMax_1_cb, PIOL::Set::getMinMax,, void(MinMaxFunc<PIOL::File::Param>, MinMaxFunc<PIOL::File::Param>, CoordElem *))

MAKE_STUB_3(PIOL_Set_getMinMax_2_cb, PIOL::Set::getMinMax,, void(Meta, Meta, CoordElem *))

MAKE_STUB_1(PIOL_Set_text_cb,    PIOL::Set::text,, void(std::string))

MAKE_STUB_0(PIOL_Set_summary_cb, PIOL::Set::summary, const, void(void))

MAKE_STUB_N(PIOL_Set_add_1_cb,   PIOL::Set::add,, void, (std::unique_ptr<PIOL::File::ReadInterface> arg1), (std::move(arg1)))

MAKE_STUB_1(PIOL_Set_add_2_cb,   PIOL::Set::add,, void(std::string))

MAKE_STUB_4(PIOL_Set_toAngle_cb, PIOL::Set::toAngle,, void(std::string, csize_t, csize_t, geom_t))

MAKE_STUB_1(PIOL_Set_sort_cb,    PIOL::Set::sort,, void(SortType))

MAKE_STUB_3(PIOL_Set_taper_1_cb, PIOL::Set::taper,, void(TaperType, size_t, size_t))

MAKE_STUB_3(PIOL_Set_taper_2_cb, PIOL::Set::taper,, void(TaperFunc, size_t, size_t))

MAKE_STUB_3(PIOL_Set_AGC_1_cb,   PIOL::Set::AGC,, void(AGCType, size_t, trace_t))

MAKE_STUB_3(PIOL_Set_AGC_2_cb,   PIOL::Set::AGC,, void(AGCFunc, size_t, trace_t))

MAKE_STUB_7(PIOL_Set_temporalFilter_1_cb, PIOL::Set::temporalFilter,, void(PIOL::FltrType, PIOL::FltrDmn, PIOL::PadType, trace_t, std::vector<trace_t>, size_t, size_t))

MAKE_STUB_8(PIOL_Set_temporalFilter_2_cb, PIOL::Set::temporalFilter,, void(PIOL::FltrType, PIOL::FltrDmn, PIOL::PadType, trace_t, size_t, std::vector<trace_t>, size_t, size_t))

#endif
