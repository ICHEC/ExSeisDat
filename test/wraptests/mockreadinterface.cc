#include "mockreadinterface.hh"

namespace exseis {
namespace piol {

::testing::StrictMock<MockReadInterface>& mock_read_interface()
{
    static ::testing::StrictMock<MockReadInterface> mock_read_interface;
    return mock_read_interface;
}

}  // namespace piol
}  // namespace exseis
