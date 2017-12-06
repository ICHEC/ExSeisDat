#include "mockexseis.hh"

namespace PIOL {

ExSeis::ExSeis(const PIOL::Verbosity maxLevel)
{
    mockExSeis().ctor(maxLevel);
}

ExSeis::ExSeis(bool initComm, const PIOL::Verbosity maxLevel)
{
    mockExSeis().ctor(initComm, maxLevel);
}

ExSeis::ExSeis(MPI_Comm comm, const PIOL::Verbosity maxLevel)
{
    mockExSeis().ctor(comm, maxLevel);
}

ExSeis::~ExSeis()
{
    mockExSeis().dtor();
}

size_t ExSeis::getRank()
{
    return mockExSeis().getRank();
}

size_t ExSeis::getNumRank()
{
    return mockExSeis().getNumRank();
}

void ExSeis::barrier() const
{
    mockExSeis().barrier();
}

size_t ExSeis::max(size_t n) const
{
    return mockExSeis().max(n);
}

void ExSeis::isErr(const std::string msg) const
{
    mockExSeis().isErr(msg);
}

} // namespace PIOL
