#ifndef EXSEIS_UTILS_MPI_UTILS_DETECTED_MPI_IMPLEMENTATION_HH
#define EXSEIS_UTILS_MPI_UTILS_DETECTED_MPI_IMPLEMENTATION_HH

#include <mpi.h>

namespace exseis {
inline namespace utils {
inline namespace mpi_utils {

enum class mpi_implementation { mpich, open_mpi, intel, unknown };

constexpr mpi_implementation detected_mpi_implementation =
#if defined MPICH
    mpi_implementation::mpich;
#elif defined OPEN_MPI
    mpi_implementation::open_mpi;
#else
    mpi_implementation::unknown;
#endif


}  // namespace mpi_utils
}  // namespace utils
}  // namespace exseis

#endif  // EXSEIS_UTILS_MPI_UTILS_DETECTED_MPI_IMPLEMENTATION_HH
