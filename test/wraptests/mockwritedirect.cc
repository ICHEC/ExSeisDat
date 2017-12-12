#include "mockwritedirect.hh"

namespace PIOL {

::testing::StrictMock<MockWriteDirect>& mockWriteDirect()
{
    static ::testing::StrictMock<MockWriteDirect> mockWriteDirect;
    return mockWriteDirect;
}

}
