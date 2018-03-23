#ifndef PIOLWRAPTESTSMOCKWRITEDIRECT_HEADER_GUARD
#define PIOLWRAPTESTSMOCKWRITEDIRECT_HEADER_GUARD

#include "gmock/gmock.h"

#include "ExSeisDat/PIOL/WriteDirect.hh"

namespace PIOL {
namespace File {

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

    MOCK_METHOD2(writeInc, void(WriteDirect*, const geom_t inc_));

    MOCK_METHOD5(
      writeTrace,
      void(
        WriteDirect*,
        const size_t offset,
        const size_t sz,
        trace_t* trace,
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
        trace_t* trace,
        const Param* prm));

    MOCK_METHOD4(
      writeParamNonContiguous,
      void(
        WriteDirect*, const size_t sz, const size_t* offset, const Param* prm));
};

}  // namespace File
}  // namespace PIOL

#endif  // PIOLWRAPTESTSMOCKWRITEDIRECT_HEADER_GUARD
