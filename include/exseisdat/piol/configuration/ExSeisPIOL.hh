////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The \c ExSeisPIOL class. This holds data shared by most classes
///        in the library.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_CONFIGURATION_EXSEISPIOL_HH
#define EXSEISDAT_PIOL_CONFIGURATION_EXSEISPIOL_HH

#include "exseisdat/utils/communicator/Communicator_mpi.hh"
#include "exseisdat/utils/logging/Log.hh"
#include "exseisdat/utils/logging/Verbosity.hh"

#include <memory>
#include <string>

namespace exseis {
namespace piol {
inline namespace configuration {

using exseis::utils::communicator::Communicator_mpi;

/// @brief The ExSeisPIOL structure. A single instance of this structure should
///        be created and passed to each subsequent PIOL object which is
///        created.
///
class ExSeisPIOL {

  protected:
    /// @brief Constructor which initialized the logging level and MPI.
    ///
    /// @param[in] copt         An options structure for MPI
    /// @param[in] max_level    The maximum log level to be recorded.
    ///
    ExSeisPIOL(
        exseis::utils::Verbosity max_level = exseis::utils::Verbosity::none,
        const Communicator_mpi::Opt& copt  = Communicator_mpi::Opt());

  public:
    /// @brief The ExSeisPIOL logger
    std::shared_ptr<exseis::utils::Log> log;

    /// @brief The ExSeisPIOL communication
    std::shared_ptr<Communicator_mpi> comm;

    /// @brief A function to check if an error has occured in the PIOL. If an
    ///        error has occured the log is printed, the object destructor is
    ///        called and the code aborts.
    ///
    /// @param[in] msg A message to be printed to the log.
    ///
    void assert_ok(const std::string& msg = "") const;
};

}  // namespace configuration
}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_CONFIGURATION_EXSEISPIOL_HH
