#ifndef PIOLWRAPTESTSMOCKREADDIRECT_HEADER_GUARD
#define PIOLWRAPTESTSMOCKREADDIRECT_HEADER_GUARD

#include "gmock/gmock.h"
#include "cppfileapi.hh"

namespace PIOL {
using namespace File;

class MockReadDirect;
::testing::StrictMock<MockReadDirect>& mockReadDirect();

class MockReadDirect
{
public:

    MOCK_METHOD3(ctor, void(ReadDirect*, const Piol piol, const std::string name));

    MOCK_METHOD2(ctor, void(ReadDirect*, std::shared_ptr<ReadInterface> file));

    MOCK_METHOD1(dtor, void(ReadDirect*));

    MOCK_CONST_METHOD1(readText, const std::string& (const ReadDirect*));

    MOCK_CONST_METHOD1(readNs, size_t(const ReadDirect*));

    MOCK_METHOD1(readNt, size_t(ReadDirect*));

    MOCK_CONST_METHOD1(readInc, geom_t(const ReadDirect*));

    MOCK_CONST_METHOD5(
        readTrace,
        void(
            const ReadDirect*,
            csize_t offset, csize_t sz, trace_t * trace, Param * prm
        )
    );

    MOCK_CONST_METHOD4(
        readParam,
        void(const ReadDirect*, csize_t offset, csize_t sz, Param * prm)
    );

    MOCK_CONST_METHOD5(
        readTrace,
        void(
            const ReadDirect*,
            csize_t sz, csize_t * offset, trace_t * trace, Param * prm
        )
    );

    MOCK_CONST_METHOD5(
        readTraceNonMono,
        void(
            const ReadDirect*,
            csize_t sz, csize_t * offset, trace_t * trace, Param * prm
        )
    );

    MOCK_CONST_METHOD4(
        readParam,
        void(const ReadDirect*, csize_t sz, csize_t * offset, Param * prm)
    );
};

} // namespace PIOL

#endif
