#include "mockexseis.hh"

namespace PIOL {

MockExSeis& MockExSeis::instance() {
    static MockExSeis instance;
    return instance;
}


ExSeis::ExSeis(const PIOL::Verbosity maxLevel)
{
    MockExSeis::instance().ctor(maxLevel);
}

ExSeis::ExSeis(bool initComm, const PIOL::Verbosity maxLevel)
{
    MockExSeis::instance().ctor(initComm, maxLevel);
}

ExSeis::ExSeis(MPI_Comm comm, const PIOL::Verbosity maxLevel)
{
    MockExSeis::instance().ctor(comm, maxLevel);
}

ExSeis::~ExSeis()
{
    MockExSeis::instance().dtor();
}

size_t ExSeis::getRank()
{
    return MockExSeis::instance().getRank();
}

size_t ExSeis::getNumRank()
{
    return MockExSeis::instance().getNumRank();
}

void ExSeis::barrier() const
{
    MockExSeis::instance().barrier();
}

size_t ExSeis::max(size_t n) const
{
    return MockExSeis::instance().max(n);
}

void ExSeis::isErr(const std::string msg) const
{
    MockExSeis::instance().isErr(msg);
}

} // namespace PIOL
