////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation for \c ExSeis
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/piol/configuration/ExSeis.hh"

using namespace exseis::utils::logging;

namespace exseis {
namespace piol {
inline namespace configuration {

ExSeis::ExSeis(const Verbosity max_level, MPI_Comm comm) :
    ExSeisPIOL(max_level, Communicator_mpi::Opt{comm})
{
}

ExSeis::~ExSeis() = default;

size_t ExSeis::get_rank() const
{
    return comm->get_rank();
}

size_t ExSeis::get_num_rank() const
{
    return comm->get_num_rank();
}

void ExSeis::barrier() const
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

}  // namespace configuration
}  // namespace piol
}  // namespace exseis
