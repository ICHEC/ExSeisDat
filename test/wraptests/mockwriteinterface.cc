#include "mockwriteinterface.hh"

namespace exseis {
namespace piol {

::testing::StrictMock<MockWriteInterface>& mock_write_interface()
{
    static ::testing::StrictMock<MockWriteInterface> mock_write_interface;
    return mock_write_interface;
}

}  // namespace piol
}  // namespace exseis
