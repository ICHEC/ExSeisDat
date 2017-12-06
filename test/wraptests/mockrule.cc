#include "mockrule.hh"

namespace PIOL {

MockRule& mockRule()
{
    static MockRule mockRule;
    return mockRule;
}

} // namespace PIOL
