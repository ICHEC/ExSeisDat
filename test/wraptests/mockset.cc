#include "mockset.hh"

namespace PIOL {

::testing::StrictMock<MockSet>& mockSet()
{
    static ::testing::StrictMock<MockSet> mockSet;
    return mockSet;
}

}  // namespace PIOL
