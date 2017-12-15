#include "mockgetminmax.hh"

namespace PIOL {
namespace File {

::testing::StrictMock<MockGetMinMax>& mockGetMinMax()
{
    static ::testing::StrictMock<MockGetMinMax> mockGetMinMax;
    return mockGetMinMax;
}

} // namespace File
} // namespace PIOL
