#include "global.hh"
#include "cfileapi.h"
#include "cppfileapi.hh"
#include "flow.h"
#include "flow.hh"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <functional>

size_t PIOL::File::ReadDirect::readNt() { return 0; }

PIOL::File::ReadDirect::ReadDirect(std::shared_ptr<PIOL::ExSeisPIOL>, std::string) {}

size_t PIOL::File::ReadDirect::readNs() const { return 0; }

geom_t PIOL::File::ReadDirect::readInc() const { return 0; }

const std::string& PIOL::File::ReadDirect::readText() const
{
    static std::string s;
    return s;
}

void PIOL::File::ReadDirect::readParam(csize_t, csize_t, PIOL::File::Param*) const {}

void PIOL::File::ReadDirect::readTrace(csize_t, csize_t, float*, PIOL::File::Param*) const {}
