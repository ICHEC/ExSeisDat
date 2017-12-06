#include "mockset.hh"

namespace PIOL {

MockSet& mockSet()
{
    static MockSet mockSet;
    return mockSet;
}

} // namespace PIOL
