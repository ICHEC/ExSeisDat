////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/mpi_utils.hh"

namespace PIOL {
namespace MPI_utils {

void printErr(
  Logger* log,
  const std::string file,
  const Logger::Layer layer,
  const int err,
  const MPI_Status* stat,
  std::string msg)
{
    if (err != MPI_SUCCESS) {
        if ((err == MPI_ERR_IN_STATUS) && (stat != NULL || stat != nullptr))
            msg += " MPI_Status: " + std::to_string(stat->MPI_ERROR);
        else
            msg += "MPI_ERR = " + std::to_string(err);
        log->record(
          file, layer, Logger::Status::Error, msg, PIOL_VERBOSITY_NONE);
    }
}

}  // namespace MPI_utils
}  // namespace PIOL
