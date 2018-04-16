///
/// @file
/// @brief Routines for finding the maximum array length supported by Intel MPI.
///
#ifndef EXSEISDAT_UTILS_MPI_MPI_MAX_ARRAY_LENGTH_HH
#define EXSEISDAT_UTILS_MPI_MPI_MAX_ARRAY_LENGTH_HH

namespace exseis {
namespace utils {

/// @brief Return the known limit for the array length for Intel MPI on Fionn
///        for a type of the given size.
///
/// @param[in] type_size The datatype size to find the array length limit for.
///
/// @return The maximum array length of the given type.
///
constexpr size_t MPI_max_array_length(size_t type_size)
{
    // If you aren't (4096 - Chunk)/Chunk from the limit, intel mpi breaks on
    // Fionn.  Probably something to do with pages.
    return (std::numeric_limits<int>::max() - (4096U - type_size)) / type_size;
}

/// @brief Return the known limit for the array length for Intel MPI on Fionn
///        for a given type.
///
/// @tparam T The type to find the array length limit for.
///
/// @return The size in counts
///
template<typename T>
constexpr size_t MPI_max_array_length()
{
    // If you aren't (4096 - Chunk)/Chunk from the limit, intel mpi breaks on
    // Fionn.  Probably something to do with pages.
    return MPI_max_array_length(sizeof(T));
}

}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_UTILS_MPI_MPI_MAX_ARRAY_LENGTH_HH
