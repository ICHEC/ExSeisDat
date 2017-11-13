#include "global.hh"
#include "cfileapi.h"
#include "cppfileapi.hh"
#include "flow.h"
#include "flow.hh"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <functional>

const PIOL::File::Param * PIOL::File::PARAM_NULL = (PIOL::File::Param *)1;

void PIOL::File::cpyPrm(unsigned long, PIOL::File::Param const*, unsigned long, PIOL::File::Param*) {}

void PIOL::File::getMinMax(PIOL::ExSeisPIOL*, unsigned long, unsigned long, PIOL::Meta, PIOL::Meta, PIOL::File::Param const*, PIOL::CoordElem*) {}
