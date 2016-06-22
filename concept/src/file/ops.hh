#ifndef PIOLFILEOPS_INCLUDE_GUARD
#define PIOLFILEOPS_INCLUDE_GUARD
#include <array>
#include <functional>
#include "global.hh"
#include "comm/mpi.hh"
#include "file/file.hh"
namespace PIOL { namespace Op {
//    typedef std::pair<real, real> CoordData;
//    typedef std::pair<BlockMd, BlockMd> CoordPair;
//    typedef std::array<CoordData, static_cast<size_t>(Coord::Len)> CoordArray;
real calcMin(Comms::MPI &, const File::CoordData &, const std::vector<File::CoordData> &);
real calcMax(Comms::MPI &, const File::CoordData &, const std::vector<File::CoordData> &);
size_t findMax(Comms::MPI &, size_t, const File::CoordData &, const std::vector<File::CoordData> &);
size_t findMin(Comms::MPI &, size_t, const File::CoordData &, const std::vector<File::CoordData> &);
}}
#endif
