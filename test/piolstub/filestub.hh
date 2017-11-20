#ifndef PIOLSTUB_FILESTUB_HEADER_GUARD
#define PIOLSTUB_FILESTUB_HEADER_GUARD

#include "global.hh"
#include "cfileapi.h"
#include "cppfileapi.hh"
#include "flow.h"
#include "flow.hh"
#include <functional>

#include "stubtools.hh"

MAKE_STUB_4(PIOL_File_cpyPrm_cb,    PIOL::File::cpyPrm,,    void(size_t, const PIOL::File::Param *, size_t, PIOL::File::Param *))

MAKE_STUB_7(PIOL_File_getMinMax_cb, PIOL::File::getMinMax,, void(PIOL::ExSeisPIOL *, size_t, size_t, PIOL::Meta, PIOL::Meta, const PIOL::File::Param *, PIOL::CoordElem *))

#endif
