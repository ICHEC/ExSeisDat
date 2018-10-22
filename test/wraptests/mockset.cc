#include "mockset.hh"

namespace exseis {
namespace flow {

::testing::StrictMock<MockSet>& mock_set()
{
    static ::testing::StrictMock<MockSet> mock_set;
    return mock_set;
}

}  // namespace flow
}  // namespace exseis
