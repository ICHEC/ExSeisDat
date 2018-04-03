////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief The MPI communicator
/// @details The MPI communicator class is responsible for the concrete
///          implementation of all MPI communication specific features. That is,
///          everything within the MPI specification except those features which
///          are related to MPI-IO or overlap with MPI-IO.
////////////////////////////////////////////////////////////////////////////////
#ifndef PIOLANCCMPI_INCLUDE_GUARD
#define PIOLANCCMPI_INCLUDE_GUARD

#include "ExSeisDat/PIOL/CommunicatorInterface.hh"
#include "ExSeisDat/PIOL/anc/log.hh"

#include <mpi.h>

namespace PIOL {

/*! @brief Set whether PIOL should manage MPI initialization / finalization.
 *      By default, PIOL will manage MPI if it calls MPI_Init, and it will call
 *      MPI_Init if the PIOL::CommunicatorMPI class is initialized before
 * MPI_Init is called. If PIOL Is managing MPI, it will call MPI_Finalize on
 * program exit.
 */
void manageMPI(bool);


/*! @brief The MPI communication class. All MPI communication specific routines
 *         should be wrapped up and accessible from this class.
 */
class CommunicatorMPI : public CommunicatorInterface {
  private:
    /// The MPI communicator.
    MPI_Comm comm;

    /// For logging messages
    Log::Logger* log;

  public:
    /*! @brief The MPI-Communicator options structure.
     */
    struct Opt {
        /// This variable defines the default MPI communicator.
        MPI_Comm comm = MPI_COMM_WORLD;
    };

    /*! @brief The constructor.
     *  @param[in] log_ Pointer to log object
     *  @param[in] opt Any options for the communication layer.
     */
    CommunicatorMPI(Log::Logger* log_, const CommunicatorMPI::Opt& opt);

    /*! @brief Retrieve the MPI communicator associated with the ExSeisPIOL.
     *  @return The MPI communicator.
     */
    MPI_Comm getComm() const;

    std::vector<float> gather(const std::vector<float>& in) const;
    std::vector<double> gather(const std::vector<double>& in) const;
    std::vector<llint> gather(const std::vector<llint>& in) const;
    std::vector<size_t> gather(const std::vector<size_t>& in) const;
    size_t sum(size_t val);
    size_t max(size_t val);
    size_t min(size_t val);
    size_t offset(size_t val);
    void barrier(void) const;
};

}  // namespace PIOL

#endif
