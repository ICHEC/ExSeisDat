#ifndef PIOLWRAPTESTSMOCKWRITEDIRECT_HEADER_GUARD
#define PIOLWRAPTESTSMOCKWRITEDIRECT_HEADER_GUARD

#include "googletest_variable_instances.hh"
#include "gmock/gmock.h"

#include "ExSeisDat/PIOL/WriteDirect.hh"

namespace exseis {
namespace PIOL {

class MockWriteDirect;
::testing::StrictMock<MockWriteDirect>& mockWriteDirect();

class MockWriteDirect {
  public:
    MOCK_METHOD3(
      ctor,
      void(
        WriteDirect*,
        std::shared_ptr<ExSeisPIOL> piol,
        const std::string name));

    MOCK_METHOD2(
      ctor, void(WriteDirect*, std::shared_ptr<WriteInterface> file));

    MOCK_METHOD1(dtor, void(WriteDirect*));

    MOCK_METHOD2(writeText, void(WriteDirect*, const std::string text_));

    MOCK_METHOD2(writeNs, void(WriteDirect*, const size_t ns_));

    MOCK_METHOD2(writeNt, void(WriteDirect*, const size_t nt_));

    MOCK_METHOD2(
      writeInc, void(WriteDirect*, const exseis::utils::Floating_point inc_));

    MOCK_METHOD5(
      writeTrace,
      void(
        WriteDirect*,
        const size_t offset,
        const size_t sz,
        exseis::utils::Trace_value* trace,
        const Param* prm));

    MOCK_METHOD4(
      writeParam,
      void(
        WriteDirect*, const size_t offset, const size_t sz, const Param* prm));

    MOCK_METHOD5(
      writeTraceNonContiguous,
      void(
        WriteDirect*,
        const size_t sz,
        const size_t* offset,
        exseis::utils::Trace_value* trace,
        const Param* prm));

    MOCK_METHOD4(
      writeParamNonContiguous,
      void(
        WriteDirect*, const size_t sz, const size_t* offset, const Param* prm));
};

}  // namespace PIOL
}  // namespace exseis

#endif  // PIOLWRAPTESTSMOCKWRITEDIRECT_HEADER_GUARD
