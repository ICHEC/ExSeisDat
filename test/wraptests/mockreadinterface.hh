#ifndef EXSEISDAT_TEST_WRAPTESTS_MOCKREADINTERFACE_HH
#define EXSEISDAT_TEST_WRAPTESTS_MOCKREADINTERFACE_HH

#include "googletest_variable_instances.hh"
#include "gmock/gmock.h"

#include "exseisdat/piol/ReadInterface.hh"

namespace exseis {
namespace piol {

class MockReadInterface;
::testing::StrictMock<MockReadInterface>& mock_read_interface();

class MockReadInterface {
  public:
    MOCK_METHOD3(
      ctor,
      void(
        ReadInterface*,
        std::shared_ptr<ExSeisPIOL> piol,
        const std::string name));

    MOCK_METHOD1(dtor, void(ReadInterface*));

    MOCK_CONST_METHOD0(file_name, const std::string&());

    MOCK_METHOD0(read_file_headers, void(void));

    MOCK_CONST_METHOD1(read_text, const std::string&(const ReadInterface*));

    MOCK_CONST_METHOD1(read_ns, size_t(const ReadInterface*));

    MOCK_CONST_METHOD1(read_nt, size_t(const ReadInterface*));

    MOCK_CONST_METHOD1(
      read_sample_interval,
      exseis::utils::Floating_point(const ReadInterface*));

    MOCK_CONST_METHOD6(
      read_trace,
      void(
        const ReadInterface*,
        const size_t offset,
        const size_t sz,
        exseis::utils::Trace_value* trace,
        Trace_metadata* prm,
        size_t skip));

    MOCK_CONST_METHOD5(
      read_param,
      void(
        const ReadInterface*,
        const size_t offset,
        const size_t sz,
        Trace_metadata* prm,
        size_t skip));

    MOCK_CONST_METHOD6(
      read_trace_non_contiguous,
      void(
        const ReadInterface*,
        const size_t sz,
        const size_t* offset,
        exseis::utils::Trace_value* trace,
        Trace_metadata* prm,
        size_t skip));

    MOCK_CONST_METHOD6(
      read_trace_non_monotonic,
      void(
        const ReadInterface*,
        const size_t sz,
        const size_t* offset,
        exseis::utils::Trace_value* trace,
        Trace_metadata* prm,
        size_t skip));

    MOCK_CONST_METHOD5(
      read_param_non_contiguous,
      void(
        const ReadInterface*,
        const size_t sz,
        const size_t* offset,
        Trace_metadata* prm,
        size_t skip));
};

}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_TEST_WRAPTESTS_MOCKREADINTERFACE_HH