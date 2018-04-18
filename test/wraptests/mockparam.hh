#ifndef PIOLWRAPTESTMOCKPARAM_HEADER_GUARD
#define PIOLWRAPTESTMOCKPARAM_HEADER_GUARD

#include "ExSeisDat/PIOL/Param.h"
#include "ExSeisDat/PIOL/param_utils.hh"

#include "printers.hh"

#include "googletest_variable_instances.hh"
#include "gmock/gmock.h"

namespace exseis {
namespace PIOL {

using namespace exseis::utils::typedefs;

namespace param_utils {
// Specify extern templates for templated functions so we can capture them
template<>
int16_t getPrm<int16_t>(size_t i, Meta entry, const Param* prm);

template<>
exseis::utils::Integer getPrm<exseis::utils::Integer>(
  size_t i, Meta entry, const Param* prm);

template<>
exseis::utils::Floating_point getPrm<exseis::utils::Floating_point>(
  size_t i, Meta entry, const Param* prm);


template<>
void setPrm<int16_t>(const size_t i, const Meta entry, int16_t ret, Param* prm);

template<>
void setPrm<exseis::utils::Integer>(
  const size_t i, const Meta entry, exseis::utils::Integer ret, Param* prm);

template<>
void setPrm<exseis::utils::Floating_point>(
  const size_t i,
  const Meta entry,
  exseis::utils::Floating_point ret,
  Param* prm);

}  // namespace param_utils


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

    MOCK_METHOD3(
      getPrm_Integer,
      exseis::utils::Integer(size_t i, Meta entry, const Param* prm));

    MOCK_METHOD3(
      getPrm_Floating_point,
      exseis::utils::Floating_point(size_t i, Meta entry, const Param* prm));

    MOCK_METHOD4(
      setPrm_int16_t,
      void(const size_t i, const Meta entry, int16_t ret, Param* prm));

    MOCK_METHOD4(
      setPrm_Integer,
      void(
        const size_t i,
        const Meta entry,
        exseis::utils::Integer ret,
        Param* prm));

    MOCK_METHOD4(
      setPrm_Floating_point,
      void(
        const size_t i,
        const Meta entry,
        exseis::utils::Floating_point ret,
        Param* prm));
};

}  // namespace PIOL
}  // namespace exseis

#endif  // PIOLWRAPTESTMOCKPARAM_HEADER_GUARD
