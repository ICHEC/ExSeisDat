#ifndef PIOLFILEOPS_INCLUDE_GUARD
#define PIOLFILEOPS_INCLUDE_GUARD
#include <array>
#include <functional>
#include "global.hh"
#include "comm/mpi.hh"
#include "file/file.hh"

#include "set/man.hh"
namespace PIOL { namespace Ops {
coreal calcMin(Comms::MPI &, const File::CoordData &, const std::vector<File::CoordData> &);
llint calcMin(Comms::MPI &, const File::GridData &, const std::vector<File::GridData> &);

coreal calcMax(Comms::MPI &, const File::CoordData &, const std::vector<File::CoordData> &);
llint calcMax(Comms::MPI &, const File::GridData &, const std::vector<File::GridData> &);

size_t findMax(Comms::MPI &, size_t, const File::CoordData &, const std::vector<File::CoordData> &);
size_t findMax(Comms::MPI &, size_t, const File::GridData &, const std::vector<File::GridData> &);

size_t findMin(Comms::MPI &, size_t, const File::CoordData &, const std::vector<File::CoordData> &);
size_t findMin(Comms::MPI &, size_t, const File::GridData &, const std::vector<File::GridData> &);
std::vector<size_t> Sort(Comms::MPI & comm, size_t, std::vector<File::TraceHeader> & thead);
}}
#endif
