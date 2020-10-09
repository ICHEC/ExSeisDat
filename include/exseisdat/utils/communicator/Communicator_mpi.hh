////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @date July 2016
/// @brief The MPI communicator
/// @details The MPI communicator class is responsible for the concrete
///          implementation of all MPI communication specific features. That is,
///          everything within the MPI specification except those features which
///          are related to MPI-IO or overlap with MPI-IO.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_COMMUNICATOR_COMMUNICATOR_MPI_HH
#define EXSEISDAT_UTILS_COMMUNICATOR_COMMUNICATOR_MPI_HH

#include "exseisdat/utils/communicator/Communicator.hh"
#include "exseisdat/utils/logging/Log.hh"
#include "exseisdat/utils/types/typedefs.hh"

#include <mpi.h>

namespace exseis {
namespace utils {
inline namespace communicator {

using namespace exseis::utils::types;

/// @brief Set whether PIOL should manage MPI initialization / finalization.
///        By default, PIOL will manage MPI if it calls MPI_Init, and it will
///        call MPI_Init if the piol::Communicator_mpi class is initialized
///        before MPI_Init is called. If PIOL Is managing MPI, it will call
///        MPI_Finalize on program exit.
///
/// @param[in] manage Set to true if PIOL should manage MPI initialization /
///                   finalization
///
void manage_mpi(bool manage);


/// @brief The MPI communication class. All MPI communication specific routines
///        should be wrapped up and accessible from this class.
///
class Communicator_mpi : public Communicator {
  private:
    /// The MPI communicator.
    MPI_Comm m_comm;

    /// For logging messages
    exseis::utils::Log* m_log;

  public:
    /// @brief The MPI-Communicator options structure.
    ///
    struct Opt {
        /// This variable defines the default MPI communicator.
        MPI_Comm comm = MPI_COMM_WORLD;
    };

    /// @brief The constructor.
    ///
    /// @param[in] log Pointer to log object
    /// @param[in] opt Any options for the communication layer.
    ///
    Communicator_mpi(exseis::utils::Log* log, const Communicator_mpi::Opt& opt);

    /// @brief Retrieve the MPI communicator associated with the ExSeisPIOL.
    ///
    /// @return The MPI communicator.
    ///
    MPI_Comm get_comm() const;

    std::vector<float> gather(const std::vector<float>& in) const override;
    std::vector<double> gather(const std::vector<double>& in) const override;
    std::vector<exseis::utils::Integer> gather(
        const std::vector<exseis::utils::Integer>& in) const override;
    std::vector<size_t> gather(const std::vector<size_t>& in) const override;

    /// @brief TMP
    ///
    /// @tparam T TMP
    /// @param[in] val TMP
    ///
    /// @return TMP
    ///
    /// @todo DELETE ME
    template<class T>
    std::vector<T> gather(const T& val) const
    {
        return Communicator::gather<T>(val);
    }

    size_t sum(size_t val) const override;

    size_t max(size_t val) const override;
    size_t min(size_t val) const override;

    size_t offset(size_t val) const override;

    void barrier() const override;
};

}  // namespace communicator
}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_UTILS_COMMUNICATOR_COMMUNICATOR_MPI_HH
