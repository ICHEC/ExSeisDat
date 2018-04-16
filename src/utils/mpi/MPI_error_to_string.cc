////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of MPI_error_to_string.
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/utils/mpi/MPI_error_to_string.hh"


namespace exseis {
namespace utils {


/// @brief Helper function to get the string associated with an MPI error code.
///
/// @param[in] mpi_error The MPI error code
///
/// @return The result from MPI_Error_string for the string, if there is one.
///         Otherwise a message noting there is no string.
static std::string get_error_string(int mpi_error)
{
    // Buffer for getting the string from MPI_Error_string.
    char buffer[MPI_MAX_ERROR_STRING] = {0};
    int buffer_size                   = MPI_MAX_ERROR_STRING;

    int err = MPI_Error_string(mpi_error, buffer, &buffer_size);

    // Make sure the buffer is terminated.
    buffer[buffer_size - 1] = '\0';

    // On failure, or if the buffer is zero length, assume there is no
    // message string.
    if (err != MPI_SUCCESS || buffer_size == 0 || strlen(buffer) == 0) {
        return "***Error string missing***";
    }

    return buffer;
}

std::string MPI_error_to_string(int mpi_error, const MPI_Status* mpi_status)
{
    using namespace std::string_literals;

    // Fix argument for user passing in a nullptr instead of MPI_STATUS_IGNORE.
    if (mpi_status == nullptr) {
        mpi_status = MPI_STATUS_IGNORE;
    }

    int mpi_error_class = 0;
    int err             = MPI_Error_class(mpi_error, &mpi_error_class);

    // MPI_Error_class shouldn't fail!!
    if (err != MPI_SUCCESS) {
        return "*** MPI_error_to_string: MPI_Error_class: "s
               + get_error_string(err) + " ***"s;
    }

    // Special return case: Success
    if (mpi_error_class == MPI_SUCCESS) {
        return "MPI_SUCCESS";
    }

    // Special return case: Error, status needed, but status omitted.
    if (
      mpi_error_class == MPI_ERR_IN_STATUS && mpi_status == MPI_STATUS_IGNORE) {
        return "MPI_Error: MPI_ERR_IN_STATUS, but status was OMITTED!"s;
    }

    // Special return case: Error in status.
    if (mpi_error == MPI_ERR_IN_STATUS) {
        return MPI_error_to_string(mpi_status->MPI_ERROR);
    }

    // Build basic error message
    return "MPI Error code: "s + std::to_string(mpi_error) + ": "s
           + get_error_string(mpi_error) + "; MPI Error class: "s
           + std::to_string(mpi_error_class) + ": "s
           + get_error_string(mpi_error_class);
}

}  // namespace utils
}  // namespace exseis
