#include "mockwritedirect.hh"

namespace PIOL {

MockWriteDirect& mockWriteDirect()
{
    static MockWriteDirect mockWriteDirect;
    return mockWriteDirect;
}

}
