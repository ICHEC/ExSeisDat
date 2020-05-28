////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date December 2016
/// @brief
/// @details Functions etc for C11 API
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/piol.h"
#include "exseisdat/piol/configuration/ExSeis.hh"
#include "exseisdat/piol/file/Input_file.hh"
#include "exseisdat/piol/file/Input_file_segy.hh"
#include "exseisdat/piol/file/Output_file.hh"
#include "exseisdat/piol/metadata/Trace_metadata.hh"
#include "exseisdat/piol/operations/minmax.hh"
#include "exseisdat/utils/types/typedefs.hh"

#include "exseisdat/not_null.hh"

#include <assert.h>
#include <cstddef>


namespace exseis {
namespace piol {
extern "C" {

using namespace exseis::utils::types;

piol_file_rule* piol_file_rule_new(bool def)
{
    return new Rule(true, def);
}

piol_file_rule* piol_file_rule_new_from_list(
    const exseis_Trace_metadata_key* m, size_t n)
{
    assert(not_null(m));

    std::vector<Trace_metadata_key> metas;
    metas.reserve(n);
    for (size_t i = 0; i < n; i++) {
        metas.push_back(static_cast<Trace_metadata_key>(m[i]));
    }

    return new Rule(std::move(metas), true, false, false);
}

void piol_file_rule_delete(piol_file_rule* rule)
{
    delete rule;
}

void piol_file_rule_add_long(
    piol_file_rule* rule,
    exseis_Trace_metadata_key m,
    exseis_Trace_header_offsets loc)
{
    assert(not_null(rule));

    (*rule).add_long(
        static_cast<Trace_metadata_key>(m),
        static_cast<Trace_header_offsets>(loc));
}

void piol_file_rule_add_short(
    piol_file_rule* rule,
    exseis_Trace_metadata_key m,
    exseis_Trace_header_offsets loc)
{
    assert(not_null(rule));

    (*rule).add_short(
        static_cast<Trace_metadata_key>(m),
        static_cast<Trace_header_offsets>(loc));
}

void piol_file_rule_add_segy_float(
    piol_file_rule* rule,
    exseis_Trace_metadata_key m,
    exseis_Trace_header_offsets loc,
    exseis_Trace_header_offsets scalar_location)
{
    assert(not_null(rule));

    (*rule).add_segy_float(
        static_cast<Trace_metadata_key>(m),
        static_cast<Trace_header_offsets>(loc),
        static_cast<Trace_header_offsets>(scalar_location));
}

void piol_file_rule_add_index(piol_file_rule* rule, exseis_Trace_metadata_key m)
{
    assert(not_null(rule));

    (*rule).add_index(static_cast<Trace_metadata_key>(m));
}

void piol_file_rule_add_copy(piol_file_rule* rule)
{
    assert(not_null(rule));

    (*rule).add_copy();
}

void piol_file_rule_rm_rule(piol_file_rule* rule, exseis_Trace_metadata_key m)
{
    assert(not_null(rule));

    (*rule).rm_rule(static_cast<Trace_metadata_key>(m));
}

size_t piol_file_rule_extent(piol_file_rule* rule)
{
    assert(not_null(rule));

    return (*rule).extent();
}

size_t piol_file_rule_memory_usage(const piol_file_rule* rule)
{
    assert(not_null(rule));

    return (*rule).memory_usage();
}

size_t piol_file_rule_memory_usage_per_header(const piol_file_rule* rule)
{
    assert(not_null(rule));

    return (*rule).memory_usage_per_header();
}

piol_file_trace_metadata* piol_file_trace_metadata_new(
    piol_file_rule* rule, size_t sz)
{
    if (not_null(rule)) {
        return new Trace_metadata(*rule, sz);
    }
    else {
        return new Trace_metadata(sz);
    }
}

void piol_file_trace_metadata_delete(piol_file_trace_metadata* param)
{
    delete param;
}

size_t piol_file_trace_metadata_size(const piol_file_trace_metadata* param)
{
    assert(not_null(param));

    return param->size();
}

size_t piol_file_trace_metadata_memory_usage(
    const piol_file_trace_metadata* param)
{
    assert(not_null(param));

    return param->memory_usage();
}

bool piol_file_rule_add_rule_meta(
    piol_file_rule* rule, exseis_Trace_metadata_key m)
{
    assert(not_null(rule));

    return (*rule).add_rule(static_cast<Trace_metadata_key>(m));
}

bool piol_file_rule_add_rule_rule(
    piol_file_rule* rule, const piol_file_rule* rule_to_copy)
{
    assert(not_null(rule));
    assert(not_null(rule_to_copy));

    return (*rule).add_rule(*rule_to_copy);
}

size_t piol_file_get_prm_index(
    size_t i,
    exseis_Trace_metadata_key entry,
    const piol_file_trace_metadata* param)
{
    assert(not_null(param));

    return param->get_index(i, static_cast<Trace_metadata_key>(entry));
}

exseis_Integer piol_file_get_prm_integer(
    size_t i,
    exseis_Trace_metadata_key entry,
    const piol_file_trace_metadata* param)
{
    assert(not_null(param));

    return param->get_integer(i, static_cast<Trace_metadata_key>(entry));
}

exseis_Floating_point piol_file_get_prm_double(
    size_t i,
    exseis_Trace_metadata_key entry,
    const piol_file_trace_metadata* param)
{
    assert(not_null(param));

    return param->get_floating_point(i, static_cast<Trace_metadata_key>(entry));
}

void piol_file_set_prm_index(
    size_t i,
    exseis_Trace_metadata_key entry,
    size_t ret,
    piol_file_trace_metadata* param)
{
    assert(not_null(param));

    param->set_index(i, static_cast<Trace_metadata_key>(entry), ret);
}

void piol_file_set_prm_integer(
    size_t i,
    exseis_Trace_metadata_key entry,
    exseis_Integer ret,
    piol_file_trace_metadata* param)
{
    assert(not_null(param));

    param->set_integer(i, static_cast<Trace_metadata_key>(entry), ret);
}

void piol_file_set_prm_double(
    size_t i,
    exseis_Trace_metadata_key entry,
    exseis_Floating_point ret,
    piol_file_trace_metadata* param)
{
    assert(not_null(param));

    param->set_floating_point(i, static_cast<Trace_metadata_key>(entry), ret);
}

void piol_file_cpy_prm(
    size_t i,
    const piol_file_trace_metadata* src,
    size_t j,
    piol_file_trace_metadata* dst)
{
    assert(not_null(src));
    assert(not_null(dst));

    dst->copy_entries(j, *src, i);
}

//////////////////PIOL////////////////////////////
piol_exseis* piol_exseis_new(exseis_Verbosity verbosity)
{
    return new std::shared_ptr<ExSeis>(
        ExSeis::make(static_cast<exseis::utils::Verbosity>(verbosity)));
}

void piol_exseis_delete(piol_exseis* piol)
{
    delete piol;
}

void piol_exseis_barrier(const piol_exseis* piol)
{
    assert(not_null(piol));

    (**piol).barrier();
}

void piol_exseis_assert_ok(const piol_exseis* piol, const char* msg)
{
    assert(not_null(piol));

    if (msg != nullptr) {
        (**piol).assert_ok(msg);
    }
    else {
        (**piol).assert_ok();
    }
}

size_t piol_exseis_get_rank(const piol_exseis* piol)
{
    assert(not_null(piol));

    return (**piol).get_rank();
}

size_t piol_exseis_get_num_rank(const piol_exseis* piol)
{
    assert(not_null(piol));

    return (**piol).get_num_rank();
}

size_t piol_exseis_max(const piol_exseis* piol, size_t n)
{
    assert(not_null(piol));

    return (**piol).max(n);
}

////////////////// File Layer ////////////////////////////

piol_file_write_interface* piol_file_write_segy_new(
    const piol_exseis* piol, const char* name)
{
    assert(not_null(piol));
    assert(not_null(name));

    return new Output_file_segy(*piol, name);
}

piol_file_read_interface* piol_file_read_segy_new(
    const piol_exseis* piol, const char* name)
{
    assert(not_null(piol));
    assert(not_null(name));

    return new Input_file_segy(*piol, name);
}

void piol_file_read_interface_delete(piol_file_read_interface* read_direct)
{
    delete read_direct;
}

void piol_file_write_interface_delete(piol_file_write_interface* write_direct)
{
    delete write_direct;
}

const char* piol_file_read_interface_read_text(
    const piol_file_read_interface* read_direct)
{
    assert(not_null(read_direct));

    return read_direct->read_text().c_str();
}

size_t piol_file_read_interface_read_ns(
    const piol_file_read_interface* read_direct)
{
    assert(not_null(read_direct));

    return read_direct->read_ns();
}

size_t piol_file_read_interface_read_nt(
    const piol_file_read_interface* read_direct)
{
    assert(not_null(read_direct));

    return read_direct->read_nt();
}

double piol_file_read_interface_read_sample_interval(
    const piol_file_read_interface* read_direct)
{
    assert(not_null(read_direct));

    return read_direct->read_sample_interval();
}

void piol_file_write_interface_write_text(
    piol_file_write_interface* write_direct, const char* text)
{
    assert(not_null(write_direct));
    assert(not_null(text));

    write_direct->write_text(text);
}

void piol_file_write_interface_write_ns(
    piol_file_write_interface* write_direct, size_t ns)
{
    assert(not_null(write_direct));

    write_direct->write_ns(ns);
}

void piol_file_write_interface_write_nt(
    piol_file_write_interface* write_direct, size_t nt)
{
    assert(not_null(write_direct));

    write_direct->write_nt(nt);
}

void piol_file_write_interface_write_sample_interval(
    piol_file_write_interface* write_direct,
    const exseis_Floating_point sample_interval)
{
    assert(not_null(write_direct));

    write_direct->write_sample_interval(sample_interval);
}

// Contiguous traces
void piol_file_read_interface_read_trace(
    const piol_file_read_interface* read_direct,
    size_t offset,
    size_t sz,
    exseis_Trace_value* trace,
    piol_file_trace_metadata* param)
{
    assert(not_null(read_direct));
    assert(not_null(trace));

    if (param == nullptr) {
        read_direct->read_trace(offset, sz, trace);
    }
    else {
        read_direct->read_trace(offset, sz, trace, param);
    }
}

void piol_file_write_interface_write_trace(
    piol_file_write_interface* write_direct,
    size_t offset,
    size_t sz,
    exseis_Trace_value* trace,
    const piol_file_trace_metadata* param)
{
    assert(not_null(write_direct));
    assert(not_null(trace));

    if (param == nullptr) {
        write_direct->write_trace(offset, sz, trace);
    }
    else {
        write_direct->write_trace(offset, sz, trace, param);
    }
}

void piol_file_write_interface_write_param(
    piol_file_write_interface* write_direct,
    size_t offset,
    size_t sz,
    const piol_file_trace_metadata* param)
{
    assert(not_null(write_direct));
    assert(not_null(param));

    write_direct->write_param(offset, sz, param);
}

void piol_file_read_interface_read_param(
    const piol_file_read_interface* read_direct,
    size_t offset,
    size_t sz,
    piol_file_trace_metadata* param)
{
    assert(not_null(read_direct));
    assert(not_null(param));

    read_direct->read_param(offset, sz, param);
}

// List traces
void piol_file_read_interface_read_trace_non_contiguous(
    piol_file_read_interface* read_direct,
    size_t sz,
    const size_t* offset,
    exseis_Trace_value* trace,
    piol_file_trace_metadata* param)
{
    if (param == nullptr) {
        read_direct->read_trace_non_contiguous(sz, offset, trace);
    }
    else {
        read_direct->read_trace_non_contiguous(sz, offset, trace, param);
    }
}

void piol_file_read_interface_read_trace_non_monotonic(
    piol_file_read_interface* read_direct,
    size_t sz,
    const size_t* offset,
    exseis_Trace_value* trace,
    piol_file_trace_metadata* param)
{
    if (param == nullptr) {
        read_direct->read_trace_non_monotonic(sz, offset, trace);
    }
    else {
        read_direct->read_trace_non_monotonic(sz, offset, trace, param);
    }
}

void piol_file_write_interface_write_trace_non_contiguous(
    piol_file_write_interface* write_direct,
    size_t sz,
    const size_t* offset,
    exseis_Trace_value* trace,
    piol_file_trace_metadata* param)
{
    if (param == nullptr) {
        write_direct->write_trace_non_contiguous(sz, offset, trace);
    }
    else {
        write_direct->write_trace_non_contiguous(sz, offset, trace, param);
    }
}

void piol_file_write_interface_write_param_non_contiguous(
    piol_file_write_interface* write_direct,
    size_t sz,
    const size_t* offset,
    piol_file_trace_metadata* param)
{
    write_direct->write_param_non_contiguous(sz, offset, param);
}

void piol_file_read_interface_read_param_non_contiguous(
    piol_file_read_interface* read_direct,
    size_t sz,
    const size_t* offset,
    piol_file_trace_metadata* param)
{
    read_direct->read_param_non_contiguous(sz, offset, param);
}

/////////////////////////////////////Operations///////////////////////////////

void piol_file_get_min_max(
    const piol_exseis* piol,
    size_t offset,
    size_t sz,
    exseis_Trace_metadata_key m1,
    exseis_Trace_metadata_key m2,
    const piol_file_trace_metadata* param,
    struct PIOL_CoordElem* minmax)
{
    assert(not_null(piol));
    assert(not_null(param));
    assert(not_null(minmax));

    get_min_max(
        (*piol).get(), offset, sz, static_cast<Trace_metadata_key>(m1),
        static_cast<Trace_metadata_key>(m2), *param, minmax);
}

//////////////////////////////////////SEGSZ///////////////////////////////////
size_t piol_segy_segy_text_header_size()
{
    return segy::segy_text_header_size();
}

size_t piol_segy_segy_trace_data_size(size_t ns)
{
    return segy::segy_trace_data_size<float>(ns);
}

size_t piol_segy_get_file_size(size_t nt, size_t ns)
{
    return segy::get_file_size<float>(nt, ns);
}

// TODO UPDATE
size_t piol_segy_segy_trace_header_size()
{
    return segy::segy_trace_header_size();
}

}  // extern "C"
}  // namespace piol
}  // namespace exseis
