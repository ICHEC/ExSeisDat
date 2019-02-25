#ifndef EXSEISDAT_SRC_UTILS_MPI_DETECTED_MPI_IMPLEMENTATION_HH
#define EXSEISDAT_SRC_UTILS_MPI_DETECTED_MPI_IMPLEMENTATION_HH

namespace exseis {
namespace utils {
inline namespace mpi {

enum class mpi_implementation { mpich, open_mpi, intel, unknown };

constexpr mpi_implementation detected_mpi_implementation =
#if defined MPICH
    mpi_implementation::mpich;
#elif defined OPEN_MPI
    mpi_implementation::open_mpi;
#else
    mpi_implementation::unknown;
#endif


}  // namespace mpi
}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_SRC_UTILS_MPI_DETECTED_MPI_IMPLEMENTATION_HH
