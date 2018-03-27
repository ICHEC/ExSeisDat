#ifndef PIOLWRAPTESTSMOCKEXSEIS_HEADER_GUARD
#define PIOLWRAPTESTSMOCKEXSEIS_HEADER_GUARD

#include "googletest_variable_instances.hh"
#include "gmock/gmock.h"

#include "ExSeisDat/PIOL/ExSeis.hh"


namespace PIOL {

class MockExSeis;
class ::testing::StrictMock<MockExSeis>& mockExSeis();

class MockExSeis {
  public:
    MOCK_METHOD3(
      ctor, void(ExSeis*, const PIOL::Verbosity maxLevel, MPI_Comm comm));

    MOCK_METHOD1(dtor, void(ExSeis*));

    MOCK_CONST_METHOD1(getRank, size_t(const ExSeis*));

    MOCK_CONST_METHOD1(getNumRank, size_t(const ExSeis*));

    MOCK_CONST_METHOD1(barrier, void(const ExSeis*));

    MOCK_CONST_METHOD2(max, size_t(const ExSeis*, size_t n));

    MOCK_CONST_METHOD2(isErr, void(const ExSeis*, const std::string& msg));
};

}  // namespace PIOL

#endif  // PIOLWRAPTESTSMOCKEXSEIS_HEADER_GUARD
