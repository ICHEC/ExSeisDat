#include "global.hh"
#include "cfileapi.h"
#include "cppfileapi.hh"
#include "flow.h"
#include "flow.hh"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <functional>

void PIOL::File::Rule::addSEGYFloat(PIOL::Meta, PIOL::File::Tr, PIOL::File::Tr) {}

void PIOL::File::Rule::rmRule(PIOL::Meta) {}

void PIOL::File::Rule::addLong(PIOL::Meta, PIOL::File::Tr) {}

void PIOL::File::Rule::addShort(PIOL::Meta, PIOL::File::Tr) {}

PIOL::File::RuleEntry * PIOL::File::Rule::getEntry(PIOL::Meta) { return NULL; }

PIOL::File::Rule::Rule(std::initializer_list<PIOL::Meta>, bool) {}

PIOL::File::Rule::Rule(bool, bool, bool) {}

PIOL::File::Rule::~Rule() {}
