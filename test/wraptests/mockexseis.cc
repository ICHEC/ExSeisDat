#include "mockexseis.hh"

namespace PIOL {

::testing::StrictMock<MockExSeis>& mockExSeis()
{
    static ::testing::StrictMock<MockExSeis> mockExSeis;
    return mockExSeis;
}

}  // namespace PIOL
