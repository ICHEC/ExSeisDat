#include "mockexseis.hh"

namespace exseis {
namespace PIOL {

ExSeis::ExSeis(const PIOL::Verbosity maxLevel, MPI_Comm comm)
{
    mockExSeis().ctor(this, maxLevel, comm);
}

ExSeis::~ExSeis()
{
    mockExSeis().dtor(this);
}

size_t ExSeis::getRank() const
{
    return mockExSeis().getRank(this);
}

size_t ExSeis::getNumRank() const
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

void ExSeis::isErr(const std::string& msg) const
{
    mockExSeis().isErr(this, msg);
}

}  // namespace PIOL
}  // namespace exseis
