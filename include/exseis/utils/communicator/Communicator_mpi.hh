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
#ifndef EXSEIS_UTILS_COMMUNICATOR_COMMUNICATOR_MPI_HH
#define EXSEIS_UTILS_COMMUNICATOR_COMMUNICATOR_MPI_HH

#include "exseis/utils/communicator/Communicator.hh"
#include "exseis/utils/logging/Log.hh"
#include "exseis/utils/types/typedefs.hh"

#include <mpi.h>

namespace exseis {
inline namespace utils {
inline namespace communicator {

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
  public:
    /// @brief The constructor.
    ///
    /// @param[in] communicator The MPI Communicator
    /// @param[in] verbosity The verbosity to use for the Logger
    ///
    Communicator_mpi(
        MPI_Comm communicator       = MPI_COMM_WORLD,
        exseis::Verbosity verbosity = exseis::Verbosity::none) :
        Communicator{std::make_unique<Communicator_mpi::Implementation>(
            communicator, verbosity)}
    {
    }

    /// @brief Get the underlying MPI_Comm communicator
    /// @returns The underlying MPI_Comm communicator
    MPI_Comm mpi_communicator() const
    {
        return static_cast<const Communicator_mpi::Implementation*>(
                   m_implementation.get())
            ->mpi_communicator();
    }

    /// @brief Polymorphic implementation of an MPI Communicator
    class Implementation : public Communicator::Implementation {
      private:
        /// The MPI communicator.
        MPI_Comm m_communicator;

        /// For logging messages
        std::shared_ptr<Log> m_log;

        /// The rank of the current process
        size_t m_rank;

        /// The total number of processes
        size_t m_num_rank;

      public:
        /// @copydoc Communicator_mpi::Communicator_mpi
        Implementation(
            MPI_Comm communicator       = MPI_COMM_WORLD,
            exseis::Verbosity verbosity = exseis::Verbosity::none);

        /// @copydoc Communicator_mpi::mpi_communicator
        MPI_Comm mpi_communicator() const;

        size_t get_rank() const override;

        size_t get_num_rank() const override;

        std::vector<float> gather(const std::vector<float>& in) const override;
        std::vector<double> gather(
            const std::vector<double>& in) const override;
        std::vector<Integer> gather(
            const std::vector<Integer>& in) const override;
        std::vector<size_t> gather(
            const std::vector<size_t>& in) const override;

        size_t sum(size_t val) const override;

        size_t max(size_t val) const override;
        size_t min(size_t val) const override;

        size_t offset(size_t val) const override;

        void barrier() const override;

        std::shared_ptr<Log> log() const override;
    };
};

}  // namespace communicator
}  // namespace utils
}  // namespace exseis

#endif  // EXSEIS_UTILS_COMMUNICATOR_COMMUNICATOR_MPI_HH
