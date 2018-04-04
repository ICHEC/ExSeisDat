////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
/// @details The \c ExSeis class
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_EXSEIS_HH
#define EXSEISDAT_PIOL_EXSEIS_HH

#include "ExSeisDat/PIOL/ExSeisPIOL.hh"

#include "ExSeisDat/PIOL/typedefs.h"


namespace PIOL {

/*! This class provides access to the ExSeisPIOL class but with a simpler API
 */
class ExSeis : public ExSeisPIOL {
  public:
    /*! Constructor with optional maxLevel and which initialises MPI.
     *  @param[in] comm     The MPI communicator
     *  @param[in] maxLevel The maximum log level to be recorded.
     *  @return A shared pointer to a PIOL object.
     */
    static std::shared_ptr<ExSeis> New(
      const Verbosity maxLevel = PIOL_VERBOSITY_NONE,
      MPI_Comm comm            = MPI_COMM_WORLD)
    {
        return std::shared_ptr<ExSeis>(new ExSeis(maxLevel, comm));
    }

    /*! ExSeis Deleter.
     */
    ~ExSeis();

    /*! Shortcut to get the commrank.
     *  @return The comm rank.
     */
    size_t getRank(void) const;

    /*! Shortcut to get the number of ranks.
     *  @return The comm number of ranks.
     */
    size_t getNumRank(void) const;

    /*! Shortcut for a communication barrier
     */
    void barrier(void) const;

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
    void isErr(const std::string& msg = "") const;

  private:
    /// The constructor is private! Use the ExSeis::New(...) function.
    /// @copydetails ExSeis::New
    ExSeis(
      const Verbosity maxLevel = PIOL_VERBOSITY_NONE,
      MPI_Comm comm            = MPI_COMM_WORLD);
};

}  // namespace PIOL

#endif  // EXSEISDAT_PIOL_EXSEIS_HH
