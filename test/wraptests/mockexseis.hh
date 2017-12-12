#ifndef PIOLWRAPTESTSMOCKEXSEIS_HEADER_GUARD
#define PIOLWRAPTESTSMOCKEXSEIS_HEADER_GUARD

#include "cppfileapi.hh"
#include "gmock/gmock.h"

namespace PIOL {

class MockExSeis;
class ::testing::StrictMock<MockExSeis>& mockExSeis();

class MockExSeis
{
public:
    MOCK_METHOD2(ctor, void(ExSeis*, const PIOL::Verbosity maxLevel));
    MOCK_METHOD3(ctor, void(ExSeis*, bool initComm, const PIOL::Verbosity maxLevel));
    MOCK_METHOD3(ctor, void(ExSeis*, MPI_Comm comm, const PIOL::Verbosity maxLevel));
    MOCK_METHOD1(dtor, void(ExSeis*));
    MOCK_METHOD1(getRank, size_t(ExSeis*));
    MOCK_METHOD1(getNumRank, size_t(ExSeis*));
    MOCK_CONST_METHOD1(barrier, void(const ExSeis*));
    MOCK_CONST_METHOD2(max, size_t(const ExSeis*, size_t n));
    MOCK_CONST_METHOD2(isErr, void(const ExSeis*, const std::string msg));
};

} // namespace PIOL

#endif
