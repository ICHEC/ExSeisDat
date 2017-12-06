#ifndef PIOLWRAPTESTMOCKPARAM_HEADER_GUARD
#define PIOLWRAPTESTMOCKPARAM_HEADER_GUARD

#include "share/param.hh"
#include "file/dynsegymd.hh"
#include "gmock/gmock.h"

namespace PIOL {
using namespace File;

class MockParam;
MockParam& mockParam();

class MockParamFreeFunctions;
MockParamFreeFunctions& mockParamFreeFunctions();

class MockParam
{
public:
    MOCK_METHOD3(ctor, void(Param*, std::shared_ptr<Rule> r_, csize_t sz));
    MOCK_METHOD2(ctor, void(Param*, csize_t sz));

    // This hangs during construction with -fsanitize=address on g++ 7.2.0
    // for size_t return type, but works fine with const size_t return type.
    // WTF?
#ifdef __SANITIZE_ADDRESS__
    MOCK_CONST_METHOD1(ctor, const size_t (const Param*));
    MOCK_CONST_METHOD1(memUsage, const size_t (const Param*));
#else
    MOCK_CONST_METHOD1(ctor, size_t (const Param*));
    MOCK_CONST_METHOD1(memUsage, size_t (const Param*));
#endif
};

class MockParamFreeFunctions
{
public:
    MOCK_METHOD4(
        cpyPrm,
        void(csize_t j, const Param * src, csize_t k, Param * dst)
    );
};

}

#endif
