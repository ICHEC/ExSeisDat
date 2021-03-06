///
/// @file
/// @brief Routines for turning MPI Errors into human readable strings.
///
#ifndef EXSEISDAT_UTILS_MPI_MPI_ERROR_TO_STRING_HH
#define EXSEISDAT_UTILS_MPI_MPI_ERROR_TO_STRING_HH

#include <mpi.h>
#include <string>

namespace exseis {
namespace utils {

/// @brief Generate a human readable MPI error message from an MPI error code
///        and status object.
///
/// @param[in] mpi_error  The MPI error code
/// @param[in] mpi_status The MPI status object (optional).
///
/// @return A human readable string detailing the error code and the status.
///
std::string MPI_error_to_string(
  int mpi_error, const MPI_Status* mpi_status = MPI_STATUS_IGNORE);

}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_UTILS_MPI_MPI_ERROR_TO_STRING_HH
