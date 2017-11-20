#ifndef PIOLSTUBRULESTUB_HEADER_GUARD
#define PIOLSTUBRULESTUB_HEADER_GUARD

#include "global.hh"
#include "cfileapi.h"
#include "cppfileapi.hh"
#include "flow.h"
#include "flow.hh"

#include "stubtools.hh"

MAKE_STUB_3(PIOL_File_Rule_addSEGYFloat_cb, PIOL::File::Rule::addSEGYFloat,, void(PIOL::Meta, PIOL::File::Tr, PIOL::File::Tr))

MAKE_STUB_1(PIOL_File_Rule_rmRule_cb,       PIOL::File::Rule::rmRule,,   void(PIOL::Meta))

MAKE_STUB_2(PIOL_File_Rule_addLong_cb,      PIOL::File::Rule::addLong,,  void(PIOL::Meta, PIOL::File::Tr))

MAKE_STUB_2(PIOL_File_Rule_addShort_cb,     PIOL::File::Rule::addShort,, void(PIOL::Meta, PIOL::File::Tr))

MAKE_STUB_1(PIOL_File_Rule_getEntry_cb,     PIOL::File::Rule::getEntry,, PIOL::File::RuleEntry * (PIOL::Meta))

MAKE_STUB_2_CTOR(PIOL_File_Rule_new_1_cb,   PIOL::File::Rule::Rule,,     void(std::initializer_list<PIOL::Meta>, bool))

MAKE_STUB_3_CTOR(PIOL_File_Rule_new_2_cb,   PIOL::File::Rule::Rule,,     void(bool, bool, bool))

MAKE_STUB_0_CTOR(PIOL_File_Rule_delete_cb, PIOL::File::Rule::~Rule,)

#endif
