#include "mockexseis.hh"

namespace exseis {
namespace piol {

::testing::StrictMock<MockExSeis>& mock_exseis()
{
    static ::testing::StrictMock<MockExSeis> mock_exseis;
    return mock_exseis;
}

}  // namespace piol
}  // namespace exseis
