////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date September 2016
/// @brief The ExSeis PIOL C++ API
////////////////////////////////////////////////////////////////////////////////
#ifndef PIOLPIOL_INCLUDE_GUARD
#define PIOLPIOL_INCLUDE_GUARD

#include "ExSeisDat/PIOL/anc/comm.hh"
#include "ExSeisDat/PIOL/anc/log.hh"
#include "ExSeisDat/PIOL/anc/mpi.hh"

#include <memory>

namespace PIOL {

/*! @brief The ExSeisPIOL structure. A single instance of this structure should
 *         be created and passed to each subsequent PIOL object which is
 *         created.
 */
class ExSeisPIOL {

  protected:
    /*! Constructor which initialized the logging level and MPI.
     *  @param[in] copt An options structure for MPI
     *  @param[in] maxLevel The maximum log level to be recorded.
     */
    ExSeisPIOL(
      const Verbosity maxLevel   = PIOL_VERBOSITY_NONE,
      const Comm::MPI::Opt& copt = Comm::MPI::Opt());

  public:
    /// The ExSeisPIOL logger
    std::unique_ptr<Log::Logger> log;
    /// The ExSeisPIOL communication
    std::unique_ptr<Comm::Interface> comm;

    /*! @brief A function to check if an error has occured in the PIOL. If an
     *         error has occured the log is printed, the object destructor is
     *         called and the code aborts.
     *  @param[in] msg A message to be printed to the log.
     */
    void isErr(const std::string& msg = "") const;
};

}  // namespace PIOL

#endif  // PIOLPIOL_INCLUDE_GUARD
