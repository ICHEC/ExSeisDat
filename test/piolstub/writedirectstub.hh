#ifndef PIOLSTUBWRITEDIRECTSTUB_HEADER_GUARD
#define PIOLSTUBWRITEDIRECTSTUB_HEADER_GUARD

#include "global.hh"
#include "cfileapi.h"
#include "cppfileapi.hh"
#include "flow.h"
#include "flow.hh"

#include "stubtools.hh"

MAKE_STUB_3(PIOL_File_WriteDirect_writeParam_cb, PIOL::File::WriteDirect::writeParam,,  void(size_t, size_t, const PIOL::File::Param *))

MAKE_STUB_4(PIOL_File_WriteDirect_writeTrace_cb, PIOL::File::WriteDirect::writeTrace,,  void(size_t, size_t, float *, const PIOL::File::Param *))

MAKE_STUB_1(PIOL_File_WriteDirect_writeNs_cb,    PIOL::File::WriteDirect::writeNs,,     void(size_t))

MAKE_STUB_1(PIOL_File_WriteDirect_writeNt_cb,    PIOL::File::WriteDirect::writeNt,,     void(size_t))

MAKE_STUB_1(PIOL_File_WriteDirect_writeInc_cb,   PIOL::File::WriteDirect::writeInc,,    void(double))

MAKE_STUB_1(PIOL_File_WriteDirect_writeText_cb,  PIOL::File::WriteDirect::writeText,,   void(std::string))

MAKE_STUB_2_CTOR(PIOL_File_WriteDirect_new_cb,   PIOL::File::WriteDirect::WriteDirect,, void(std::shared_ptr<PIOL::ExSeisPIOL>, std::string))

#endif
