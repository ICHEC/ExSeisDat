#include "mockreaddirect.hh"

namespace PIOL {

MockReadDirect& mockReadDirect()
{
    static MockReadDirect mockReadDirect;
    return mockReadDirect;
}

} // namespace PIOL
