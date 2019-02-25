////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
/// @details The \c ExSeis class
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_EXSEIS_HH
#define EXSEISDAT_PIOL_EXSEIS_HH

#include "exseisdat/piol/ExSeisPIOL.hh"

#include "exseisdat/utils/typedefs.hh"


namespace exseis {
namespace piol {

/*! This class provides access to the ExSeisPIOL class but with a simpler API
 */
class ExSeis : public ExSeisPIOL {
  public:
    /*! Constructor with optional maxLevel and which initialises MPI.
     *  @param[in] comm     The MPI communicator
     *  @param[in] max_level The maximum log level to be recorded.
     *  @return A shared pointer to a PIOL object.
     */
    static std::shared_ptr<ExSeis> make(
        exseis::utils::Verbosity max_level = exseis::utils::Verbosity::none,
        MPI_Comm comm                      = MPI_COMM_WORLD)
    {
        return std::shared_ptr<ExSeis>(new ExSeis(max_level, comm));
    }

    /*! ExSeis Deleter.
     */
    ~ExSeis();

    /*! Shortcut to get the commrank.
     *  @return The comm rank.
     */
    size_t get_rank() const;

    /*! Shortcut to get the number of ranks.
     *  @return The comm number of ranks.
     */
    size_t get_num_rank() const;

    /*! Shortcut for a communication barrier
     */
    void barrier() const;

    /*! Return the maximum value amongst the processes
     *  @param[in] n The value to take part in the reduction
     *  @return Return the maximum value amongst the processes
     */
    size_t max(size_t n) const;

    /*! @brief A function to check if an error has occured in the PIOL. If an
     *         error has occured the log is printed, the object destructor is
     *         called and the code aborts.
     *  @param[in] msg A message to be printed to the log.
     */
    void assert_ok(const std::string& msg = "") const;

  private:
    /// The constructor is private! Use the ExSeis::make(...) function.
    /// @copydetails ExSeis::make
    ExSeis(
        exseis::utils::Verbosity max_level = exseis::utils::Verbosity::none,
        MPI_Comm comm                      = MPI_COMM_WORLD);
};

}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_EXSEIS_HH
