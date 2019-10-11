////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
/// @details Functions etc for C Flow API
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/flow.h"
#include "exseisdat/flow/Set.hh"

#include "exseisdat/not_null.hh"

#include <assert.h>
#include <cstddef>

namespace exseis {
namespace piol {
extern "C" {

PIOL_Set* piol_set_new(const piol_exseis* piol, const char* ptrn)
{
    assert(not_null(piol));
    assert(not_null(ptrn));

    return new exseis::flow::Set(*piol, ptrn);
}

void piol_set_delete(PIOL_Set* set)
{
    delete set;
}

void piol_set_get_min_max(
    PIOL_Set* set,
    exseis_Trace_metadata_key m1,
    exseis_Trace_metadata_key m2,
    struct PIOL_CoordElem* minmax)
{
    assert(not_null(set));
    assert(not_null(minmax));

    set->get_min_max(
        static_cast<exseis::piol::Trace_metadata_key>(m1),
        static_cast<exseis::piol::Trace_metadata_key>(m2), minmax);
}

void piol_set_sort(PIOL_Set* set, exseis_SortType type)
{
    assert(not_null(set));

    set->sort(static_cast<exseis::piol::Sort_type>(type));
}

void piol_set_sort_fn(
    PIOL_Set* set,
    bool (*const func)(
        const piol_file_trace_metadata* param, size_t i, size_t j))
{
    assert(not_null(set));
    assert(func != nullptr);

    const auto wrapped_func = [=](const exseis::piol::Trace_metadata& param,
                                  size_t i,
                                  size_t j) { return func(&param, i, j); };

    set->sort(wrapped_func);
}

void piol_set_taper(
    PIOL_Set* set,
    exseis_Taper_function taper_function,
    size_t ntpstr,
    size_t ntpend)
{
    assert(not_null(set));
    assert(taper_function != nullptr);

    set->taper(taper_function, ntpstr, ntpend);
}

void piol_set_agc(
    PIOL_Set* set,
    exseis_Gain_function type,
    size_t window,
    exseis_Trace_value target_amplitude)
{
    assert(not_null(set));

    set->agc(type, window, target_amplitude);
}

void piol_set_output(PIOL_Set* set, const char* oname)
{
    assert(not_null(set));
    assert(not_null(oname));

    set->output(oname);
}

void piol_set_text(PIOL_Set* set, const char* outmsg)
{
    assert(not_null(set));
    assert(not_null(outmsg));

    set->text(outmsg);
}

void piol_set_summary(const PIOL_Set* set)
{
    assert(not_null(set));

    set->summary();
}

void piol_set_add(PIOL_Set* set, const char* name)
{
    assert(not_null(set));
    assert(not_null(name));

    set->add(name);
}

}  // extern "C"
}  // namespace piol
}  // namespace exseis
