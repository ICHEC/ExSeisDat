#ifndef PIOLSTUBEXSEISSTUB_HEADER_GUARD
#define PIOLSTUBEXSEISSTUB_HEADER_GUARD

#include "global.hh"
#include "cfileapi.h"
#include "cppfileapi.hh"
#include "flow.h"
#include "flow.hh"
#include <functional>

#include "stubtools.hh"

MAKE_STUB_1_CTOR(PIOL_ExSeis_new_cb, PIOL::ExSeis::ExSeis,, void(PIOL::Verbosity))
MAKE_STUB_0_CTOR(PIOL_ExSeis_delete_cb, PIOL::ExSeis::~ExSeis,)

MAKE_STUB_0(PIOL_ExSeis_getRank_cb,    PIOL::ExSeis::getRank,,    size_t(void))

MAKE_STUB_0(PIOL_ExSeis_getNumRank_cb, PIOL::ExSeis::getNumRank,, size_t(void))

MAKE_STUB_0(PIOL_ExSeis_barrier_cb, PIOL::ExSeis::barrier, const, void(void))

MAKE_STUB_1(PIOL_ExSeis_max_cb,     PIOL::ExSeis::max, const,   size_t(size_t))

MAKE_STUB_1(PIOL_ExSeis_isErr_cb,   PIOL::ExSeis::isErr, const, void(std::string))

#endif
