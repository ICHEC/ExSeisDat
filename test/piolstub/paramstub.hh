#ifndef PIOLSTUBPARAMSTUB_HEADER_GUARD
#define PIOLSTUBPARAMSTUB_HEADER_GUARD

#include "global.hh"
#include "cfileapi.h"
#include "cppfileapi.hh"
#include "flow.h"
#include "flow.hh"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <functional>

#include "stubtools.hh"

MAKE_STUB_2_CTOR(PIOL_File_Param_new_cb, PIOL::File::Param::Param,, void(std::shared_ptr<PIOL::File::Rule>, size_t))

#endif
