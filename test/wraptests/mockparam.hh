#ifndef PIOLWRAPTESTMOCKPARAM_HEADER_GUARD
#define PIOLWRAPTESTMOCKPARAM_HEADER_GUARD

#include "share/param.hh"
#include "file/dynsegymd.hh"
#include "gmock/gmock.h"
#include "printers.hh"

namespace PIOL {
namespace File {

class MockParam;
::testing::StrictMock<MockParam>& mockParam();

class MockParamFreeFunctions;
::testing::StrictMock<MockParamFreeFunctions>& mockParamFreeFunctions();

// size_t return types for mocks hang during construction with
// -fsanitize=address on g++ 7.2.0 , but not for const size_t return type.
// WTF?
#ifdef __SANITIZE_ADDRESS__
#define EXSEISDAT_MOCK_PARAM_CONST const
#else
#define EXSEISDAT_MOCK_PARAM_CONST
#endif

class MockParam
{
public:
    MOCK_METHOD3(ctor, void(Param*, std::shared_ptr<Rule> r_, csize_t sz));
    MOCK_METHOD2(ctor, void(Param*, csize_t sz));

    MOCK_CONST_METHOD1(ctor, EXSEISDAT_MOCK_PARAM_CONST size_t (const Param*));
    MOCK_CONST_METHOD1(memUsage, EXSEISDAT_MOCK_PARAM_CONST size_t (const Param*));
};

class MockParamFreeFunctions
{
public:
    MOCK_METHOD4(
        cpyPrm,
        void(csize_t j, const Param * src, csize_t k, Param * dst)
    );
};

} // namespace File
} // namespace PIOL

#endif
