#include "mockminmax.hh"

namespace PIOL {

MockGetMinMax& mockGetMinMax()
{
    static MockGetMinMax mockGetMinMax;
    return mockGetMinMax;
}

} // namespace PIOL
