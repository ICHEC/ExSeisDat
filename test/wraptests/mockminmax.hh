#ifndef PIOLWRAPTESTSMOCKMINMAX_HEADER_GUARD
#define PIOLWRAPTESTSMOCKMINMAX_HEADER_GUARD

#include "gmock/gmock.h"
#include "ops/minmax.hh"

namespace PIOL {
using namespace File;

class MockGetMinMax;
MockGetMinMax& mockGetMinMax();

class MockGetMinMax
{
public:
    MOCK_METHOD7(
        getMinMax,
        void(
            ExSeisPIOL * piol, size_t offset, size_t sz, Meta m1, Meta m2,
            const Param * prm, CoordElem * minmax
        )
    );
};

}

#endif
