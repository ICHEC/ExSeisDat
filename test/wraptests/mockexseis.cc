#include "mockexseis.hh"

namespace PIOL {

MockExSeis& mockExSeis() {
    static MockExSeis mockExSeis;
    return mockExSeis;
}

} // namespace PIOL
