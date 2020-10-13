///
/// @file
/// @brief Routines for finding the maximum array length supported by Intel MPI.
///
#ifndef EXSEIS_UTILS_MPI_UTILS_MPI_MAX_ARRAY_LENGTH_HH
#define EXSEIS_UTILS_MPI_UTILS_MPI_MAX_ARRAY_LENGTH_HH

#include <cstddef>

namespace exseis {
inline namespace utils {
inline namespace mpi_utils {

/// @brief Return the known limit for the array length of a given type for the
///        MPI implementation.
///
/// @param[in] type_size The size of the type to check the limit for.
///
/// @return The maximum number of items in the array.
///
size_t mpi_max_array_length(size_t type_size);

/// @brief Return the known limit for the array length of a given type for the
///        MPI implementation.
///
/// @tparam T The type to find the array length limit for.
///
/// @return The maximum number of items in the array.
///
template<typename T>
size_t mpi_max_array_length()
{
    return mpi_max_array_length(sizeof(T));
}

}  // namespace mpi_utils
}  // namespace utils
}  // namespace exseis

#endif  // EXSEIS_UTILS_MPI_UTILS_MPI_MAX_ARRAY_LENGTH_HH