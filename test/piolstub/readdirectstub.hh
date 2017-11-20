#ifndef PIOLSTUBREADDIRECTSTUB_HEADER_GUARD
#define PIOLSTUBREADDIRECTSTUB_HEADER_GUARD

#include "global.hh"
#include "cfileapi.h"
#include "cppfileapi.hh"
#include "flow.h"
#include "flow.hh"
#include <functional>

#include "stubtools.hh"


MAKE_STUB_2_CTOR(PIOL_File_ReadDirect_new_cb,  PIOL::File::ReadDirect::ReadDirect,, void(std::shared_ptr<PIOL::ExSeisPIOL>, std::string))

MAKE_STUB_0(PIOL_File_ReadDirect_readNt_cb,    PIOL::File::ReadDirect::readNt,,          size_t(void))

MAKE_STUB_0(PIOL_File_ReadDirect_readNs_cb,    PIOL::File::ReadDirect::readNs,    const, size_t(void))

MAKE_STUB_0(PIOL_File_ReadDirect_readInc_cb,   PIOL::File::ReadDirect::readInc,   const, geom_t(void))

MAKE_STUB_0(PIOL_File_ReadDirect_readText_cb,  PIOL::File::ReadDirect::readText,  const, const std::string&(void))

MAKE_STUB_3(PIOL_File_ReadDirect_readParam_cb, PIOL::File::ReadDirect::readParam, const, void(size_t, size_t, PIOL::File::Param *))

MAKE_STUB_4(PIOL_File_ReadDirect_readTract_cb, PIOL::File::ReadDirect::readTrace, const, void(size_t, size_t, float *, PIOL::File::Param *))

#endif
