#include "global.hh"
#include "cfileapi.h"
#include "cppfileapi.hh"
#include "flow.h"
#include "flow.hh"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <functional>

void PIOL::File::WriteDirect::writeParam(csize_t, csize_t, PIOL::File::Param const*) {}

void PIOL::File::WriteDirect::writeTrace(csize_t, csize_t, float*, PIOL::File::Param const*) {}

void PIOL::File::WriteDirect::writeNs(csize_t) {}

void PIOL::File::WriteDirect::writeNt(csize_t) {}

void PIOL::File::WriteDirect::writeInc(double) {}

void PIOL::File::WriteDirect::writeText(std::string) {}

PIOL::File::WriteDirect::WriteDirect(std::shared_ptr<PIOL::ExSeisPIOL>, std::string) {}
