////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation for \c ExSeis
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/piol/ExSeis.hh"

using namespace exseis::utils::logging;

namespace exseis {
namespace piol {

ExSeis::ExSeis(const Verbosity max_level, MPI_Comm comm) :
    ExSeisPIOL(max_level, CommunicatorMPI::Opt{comm})
{
}

ExSeis::~ExSeis() = default;

size_t ExSeis::get_rank(void) const
{
    return comm->get_rank();
}

size_t ExSeis::get_num_rank(void) const
{
    return comm->get_num_rank();
}

void ExSeis::barrier(void) const
{
    comm->barrier();
}

size_t ExSeis::max(size_t n) const
{
    return comm->max(n);
}

void ExSeis::assert_ok(const std::string& msg) const
{
    ExSeisPIOL::assert_ok(msg);
}

}  // namespace piol
}  // namespace exseis
