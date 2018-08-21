////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation for \c ExSeis
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/ExSeis.hh"

namespace exseis {
namespace PIOL {

ExSeis::ExSeis(const Verbosity maxLevel, MPI_Comm comm) :
    ExSeisPIOL(maxLevel, CommunicatorMPI::Opt{comm})
{
}

ExSeis::~ExSeis() = default;

size_t ExSeis::getRank(void) const
{
    return comm->getRank();
}

size_t ExSeis::getNumRank(void) const
{
    return comm->getNumRank();
}

void ExSeis::barrier(void) const
{
    comm->barrier();
}

size_t ExSeis::max(size_t n) const
{
    return comm->max(n);
}

void ExSeis::isErr(const std::string& msg) const
{
    ExSeisPIOL::isErr(msg);
}

}  // namespace PIOL
}  // namespace exseis
