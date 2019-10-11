#ifndef EXSEISDAT_TEST_WRAPTESTS_MOCKWRITEINTERFACE_HH
#define EXSEISDAT_TEST_WRAPTESTS_MOCKWRITEINTERFACE_HH

#include "googletest_variable_instances.hh"
#include "gmock/gmock.h"

#include "exseisdat/piol/file/Output_file.hh"

namespace exseis {
namespace piol {

class MockWriteInterface;
::testing::StrictMock<MockWriteInterface>& mock_write_interface();

class MockWriteInterface {
  public:
    MOCK_METHOD3(
        ctor,
        void(
            Output_file*,
            std::shared_ptr<ExSeisPIOL> piol,
            const std::string name));

    MOCK_METHOD1(dtor, void(Output_file*));

    MOCK_CONST_METHOD1(file_name, const std::string&(const Output_file*));
    MOCK_METHOD2(write_text, void(Output_file*, const std::string text_));

    MOCK_METHOD2(write_ns, void(Output_file*, const size_t ns_));

    MOCK_METHOD2(write_nt, void(Output_file*, const size_t nt_));

    MOCK_METHOD2(
        write_sample_interval,
        void(
            Output_file*,
            const exseis::utils::Floating_point sample_interval_));

    MOCK_METHOD6(
        write_trace,
        void(
            Output_file*,
            const size_t offset,
            const size_t sz,
            exseis::utils::Trace_value* trace,
            const Trace_metadata* prm,
            size_t skip));

    MOCK_METHOD5(
        write_param,
        void(
            Output_file*,
            const size_t offset,
            const size_t sz,
            const Trace_metadata* prm,
            size_t skip));

    MOCK_METHOD6(
        write_trace_non_contiguous,
        void(
            Output_file*,
            const size_t sz,
            const size_t* offset,
            exseis::utils::Trace_value* trace,
            const Trace_metadata* prm,
            size_t skip));

    MOCK_METHOD5(
        write_param_non_contiguous,
        void(
            Output_file*,
            const size_t sz,
            const size_t* offset,
            const Trace_metadata* prm,
            size_t skip));
};

}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_TEST_WRAPTESTS_MOCKWRITEINTERFACE_HH
