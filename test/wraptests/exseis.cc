#include "mockexseis.hh"

namespace PIOL {

ExSeis::ExSeis(const PIOL::Verbosity maxLevel)
{
    mockExSeis().ctor(this, maxLevel);
}

ExSeis::ExSeis(bool initComm, const PIOL::Verbosity maxLevel)
{
    mockExSeis().ctor(this, initComm, maxLevel);
}

ExSeis::ExSeis(MPI_Comm comm, const PIOL::Verbosity maxLevel)
{
    mockExSeis().ctor(this, comm, maxLevel);
}

ExSeis::~ExSeis()
{
    mockExSeis().dtor(this);
}

size_t ExSeis::getRank()
{
    return mockExSeis().getRank(this);
}

size_t ExSeis::getNumRank()
{
    return mockExSeis().getNumRank(this);
}

void ExSeis::barrier() const
{
    mockExSeis().barrier(this);
}

size_t ExSeis::max(size_t n) const
{
    return mockExSeis().max(this, n);
}

void ExSeis::isErr(const std::string msg) const
{
    mockExSeis().isErr(this, msg);
}

} // namespace PIOL
