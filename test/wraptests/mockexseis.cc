#include "mockexseis.hh"

namespace exseis {
namespace PIOL {

::testing::StrictMock<MockExSeis>& mockExSeis()
{
    static ::testing::StrictMock<MockExSeis> mockExSeis;
    return mockExSeis;
}

}  // namespace PIOL
}  // namespace exseis
