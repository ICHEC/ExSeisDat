#include "mockexseis.hh"

namespace exseis {
namespace piol {

ExSeis::ExSeis(const exseis::utils::Verbosity max_level, MPI_Comm comm)
{
    mock_exseis().ctor(this, max_level, comm);
}

ExSeis::~ExSeis()
{
    mock_exseis().dtor(this);
}

size_t ExSeis::get_rank() const
{
    return mock_exseis().get_rank(this);
}

size_t ExSeis::get_num_rank() const
{
    return mock_exseis().get_num_rank(this);
}

void ExSeis::barrier() const
{
    mock_exseis().barrier(this);
}

size_t ExSeis::max(size_t n) const
{
    return mock_exseis().max(this, n);
}

void ExSeis::assert_ok(const std::string& msg) const
{
    mock_exseis().assert_ok(this, msg);
}

}  // namespace piol
}  // namespace exseis
