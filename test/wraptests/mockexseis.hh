#ifndef PIOLWRAPTESTSMOCKEXSEIS_HEADER_GUARD
#define PIOLWRAPTESTSMOCKEXSEIS_HEADER_GUARD

#include "cppfileapi.hh"
#include "gmock/gmock.h"

namespace PIOL {

class MockExSeis;
class MockExSeis& mockExSeis();

class MockExSeis
{
public:
    MOCK_METHOD1(ctor, void(const PIOL::Verbosity maxLevel));
    MOCK_METHOD2(ctor, void(bool initComm, const PIOL::Verbosity maxLevel));
    MOCK_METHOD2(ctor, void(MPI_Comm comm, const PIOL::Verbosity maxLevel));
    MOCK_METHOD0(dtor, void());
    MOCK_METHOD0(getRank, size_t());
    MOCK_METHOD0(getNumRank, size_t());
    MOCK_CONST_METHOD0(barrier, void());
    MOCK_CONST_METHOD1(max, size_t(size_t n));
    MOCK_CONST_METHOD1(isErr, void(const std::string msg));
};

} // namespace PIOL

#endif
