#include "mockwritedirect.hh"

namespace PIOL {
namespace File {

::testing::StrictMock<MockWriteDirect>& mockWriteDirect()
{
    static ::testing::StrictMock<MockWriteDirect> mockWriteDirect;
    return mockWriteDirect;
}

} // namespace File
} // namespace PIOL
