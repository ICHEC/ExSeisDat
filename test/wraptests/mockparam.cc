#include "mockparam.hh"

namespace exseis {
namespace piol {

::testing::StrictMock<Mock_Trace_metadata>& mock_trace_metadata()
{
    static ::testing::StrictMock<Mock_Trace_metadata> mock_trace_metadata;
    return mock_trace_metadata;
}

}  // namespace piol
}  // namespace exseis
