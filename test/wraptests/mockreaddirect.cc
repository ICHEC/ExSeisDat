#include "mockreaddirect.hh"

namespace PIOL {
namespace File {

::testing::StrictMock<MockReadDirect>& mockReadDirect()
{
    static ::testing::StrictMock<MockReadDirect> mockReadDirect;
    return mockReadDirect;
}

}  // namespace File
}  // namespace PIOL
