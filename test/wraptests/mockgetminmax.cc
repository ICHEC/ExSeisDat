#include "mockgetminmax.hh"

namespace exseis {
namespace piol {

::testing::StrictMock<MockGetMinMax>& mock_get_min_max()
{
    static ::testing::StrictMock<MockGetMinMax> mock_get_min_max;
    return mock_get_min_max;
}

}  // namespace piol
}  // namespace exseis
