#ifndef PIOLWRAPTESTMOCKPARAM_HEADER_GUARD
#define PIOLWRAPTESTMOCKPARAM_HEADER_GUARD

#include "ExSeisDat/PIOL/Param.hh"
#include "ExSeisDat/PIOL/param_utils.hh"

#include "printers.hh"

#include "googletest_variable_instances.hh"
#include "gmock/gmock.h"

namespace PIOL {

// Specify extern templates for templated functions so we can capture them
template<>
int16_t getPrm<int16_t>(size_t i, Meta entry, const Param* prm);

template<>
PIOL::llint getPrm<PIOL::llint>(size_t i, Meta entry, const Param* prm);

template<>
geom_t getPrm<geom_t>(size_t i, Meta entry, const Param* prm);


template<>
void setPrm<int16_t>(const size_t i, const Meta entry, int16_t ret, Param* prm);

template<>
void setPrm<llint>(const size_t i, const Meta entry, llint ret, Param* prm);

template<>
void setPrm<geom_t>(const size_t i, const Meta entry, geom_t ret, Param* prm);


class MockParam;
::testing::StrictMock<MockParam>& mockParam();

class MockParamFreeFunctions;
::testing::StrictMock<MockParamFreeFunctions>& mockParamFreeFunctions();

class MockParam {
  public:
    MOCK_METHOD3(ctor, void(Param*, std::shared_ptr<Rule> r_, const size_t sz));
    MOCK_METHOD2(ctor, void(Param*, const size_t sz));
    MOCK_METHOD1(dtor, void(Param*));

    MOCK_CONST_METHOD1(size, size_t(const Param*));

    MOCK_CONST_METHOD1(memUsage, size_t(const Param*));
};

class MockParamFreeFunctions {
  public:
    MOCK_METHOD4(
      cpyPrm,
      void(const size_t j, const Param* src, const size_t k, Param* dst));

    MOCK_METHOD3(
      getPrm_int16_t, int16_t(size_t i, Meta entry, const Param* prm));

    MOCK_METHOD3(getPrm_llint, llint(size_t i, Meta entry, const Param* prm));

    MOCK_METHOD3(getPrm_geom_t, geom_t(size_t i, Meta entry, const Param* prm));

    MOCK_METHOD4(
      setPrm_int16_t,
      void(const size_t i, const Meta entry, int16_t ret, Param* prm));

    MOCK_METHOD4(
      setPrm_llint,
      void(const size_t i, const Meta entry, llint ret, Param* prm));

    MOCK_METHOD4(
      setPrm_geom_t,
      void(const size_t i, const Meta entry, geom_t ret, Param* prm));
};

}  // namespace PIOL

#endif  // PIOLWRAPTESTMOCKPARAM_HEADER_GUARD
