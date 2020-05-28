///
/// @file
/// @brief Implementation for finding the maximum array length for the MPI
///        implementation.
///

#include "exseisdat/utils/mpi_utils/MPI_max_array_length.hh"

#include "detected_mpi_implementation.hh"

#include <limits>
#include <numeric>

#include <cassert>
#include <cstdint>
#include <cstring>

namespace exseis {
namespace utils {
inline namespace mpi_utils {

size_t mpi_max_array_length(size_t type_size)
{
    switch (detected_mpi_implementation) {
        case (mpi_implementation::intel):
            // If you aren't (4096 - Chunk)/Chunk from the limit, intel mpi
            // breaks on Fionn.  Probably something to do with pages.

            return (std::numeric_limits<int32_t>::max() - (4096U - type_size))
                   / type_size;

        case (mpi_implementation::open_mpi):

            // For open-mpi 3.1.0 on macOS, I/O on arrays of size larger than
            // around (INT_MAX / 256) are very slow.

            return std::numeric_limits<int32_t>::max() / (1UL << 8);

        case (mpi_implementation::mpich):
        case (mpi_implementation::unknown):
            // Default limit is integer max

            /// @todo Should the limit be uint64_t max?
            ///       It's int max for sizes and indices in a number of MPI
            ///       functions, but should internally be uint64_t max for well
            ///       written implementations.
            return std::numeric_limits<int32_t>::max();
    }

    assert(false && "Unknown MPI Implementation!");
    return 0;
}

}  // namespace mpi_utils
}  // namespace utils
}  // namespace exseis
