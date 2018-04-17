#include "mockwritedirect.hh"

namespace exseis {
namespace PIOL {

::testing::StrictMock<MockWriteDirect>& mockWriteDirect()
{
    static ::testing::StrictMock<MockWriteDirect> mockWriteDirect;
    return mockWriteDirect;
}

}  // namespace PIOL
}  // namespace exseis
