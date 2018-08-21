#include "mockset.hh"

namespace exseis {
namespace Flow {

::testing::StrictMock<MockSet>& mockSet()
{
    static ::testing::StrictMock<MockSet> mockSet;
    return mockSet;
}

}  // namespace Flow
}  // namespace exseis
