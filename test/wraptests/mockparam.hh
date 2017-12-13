#ifndef PIOLWRAPTESTMOCKPARAM_HEADER_GUARD
#define PIOLWRAPTESTMOCKPARAM_HEADER_GUARD

#include "share/param.hh"
#include "file/dynsegymd.hh"
#include "gmock/gmock.h"
#include "printers.hh"

namespace PIOL {
namespace File {

// Specify extern templates for templated functions so we can capture them
template<>
int16_t getPrm<int16_t>(size_t i, Meta entry, const Param * prm);

template<>
PIOL::llint getPrm<PIOL::llint>(size_t i, Meta entry, const Param * prm);

template<>
geom_t getPrm<geom_t>(size_t i, Meta entry, const Param * prm);


template<>
void setPrm<int16_t>(csize_t i, const Meta entry, int16_t ret, Param * prm);

template<>
void setPrm<llint>(csize_t i, const Meta entry, llint ret, Param * prm);

template<>
void setPrm<geom_t>(csize_t i, const Meta entry, geom_t ret, Param * prm);


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
    MOCK_METHOD1(dtor, void(Param*));

    MOCK_CONST_METHOD1(size, EXSEISDAT_MOCK_PARAM_CONST size_t (const Param*));
    MOCK_CONST_METHOD1(memUsage, EXSEISDAT_MOCK_PARAM_CONST size_t (const Param*));
};

class MockParamFreeFunctions
{
public:
    MOCK_METHOD4(
        cpyPrm,
        void(csize_t j, const Param * src, csize_t k, Param * dst)
    );

    MOCK_METHOD3(
        getPrm_int16_t, const int16_t(size_t i, Meta entry, const Param* prm)
    );
    MOCK_METHOD3(
        getPrm_llint,   const llint(size_t i, Meta entry, const Param* prm)
    );
    MOCK_METHOD3(
        getPrm_geom_t,  const geom_t(size_t i, Meta entry, const Param* prm)
    );

    MOCK_METHOD4(
        setPrm_int16_t,
        void(csize_t i, const Meta entry, int16_t ret, Param * prm)
    );
    MOCK_METHOD4(
        setPrm_llint,
        void(csize_t i, const Meta entry, llint ret, Param * prm)
    );
    MOCK_METHOD4(
        setPrm_geom_t,
        void(csize_t i, const Meta entry, geom_t ret, Param * prm)
    );
};

} // namespace File
} // namespace PIOL

#endif
