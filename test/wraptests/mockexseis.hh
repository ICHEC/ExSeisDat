#ifndef EXSEISDAT_TEST_WRAPTESTS_MOCKEXSEIS_HH
#define EXSEISDAT_TEST_WRAPTESTS_MOCKEXSEIS_HH

#include "googletest_variable_instances.hh"
#include "gmock/gmock.h"

#include "exseisdat/piol/ExSeis.hh"


namespace exseis {
namespace piol {

class MockExSeis;
class ::testing::StrictMock<MockExSeis>& mock_exseis();

class MockExSeis {
  public:
    MOCK_METHOD3(
      ctor,
      void(ExSeis*, const exseis::utils::Verbosity maxLevel, MPI_Comm comm));

    MOCK_METHOD1(dtor, void(ExSeis*));

    MOCK_CONST_METHOD1(get_rank, size_t(const ExSeis*));

    MOCK_CONST_METHOD1(get_num_rank, size_t(const ExSeis*));

    MOCK_CONST_METHOD1(barrier, void(const ExSeis*));

    MOCK_CONST_METHOD2(max, size_t(const ExSeis*, size_t n));

    MOCK_CONST_METHOD2(assert_ok, void(const ExSeis*, const std::string& msg));
};

}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_TEST_WRAPTESTS_MOCKEXSEIS_HH
