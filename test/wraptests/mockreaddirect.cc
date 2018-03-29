#include "mockreaddirect.hh"

namespace PIOL {

::testing::StrictMock<MockReadDirect>& mockReadDirect()
{
    static ::testing::StrictMock<MockReadDirect> mockReadDirect;
    return mockReadDirect;
}

}  // namespace PIOL
