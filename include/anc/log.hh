/*******************************************************************************************//*!
 *   @file
 *   @author Cathal O Broin - cathal@ichec.ie - first commit
 *   @copyright TBD. Do not distribute
 *   @date July 2016
 *   @brief The logging layer
 *   @details The logging layer provides a standard approach to logging and error handling throughout
 *   the PIOL while respecting the boundary between layers.
 *//*******************************************************************************************/
#ifndef PIOLANCLOG_INCLUDE_GUARD
#define PIOLANCLOG_INCLUDE_GUARD

#include "anc/verbosity.h"

#include <forward_list>
#include <string>

namespace PIOL {
namespace Log {

/*! @brief The enumeration for the different layers of the PIOL.
 */
enum class Layer : size_t {
    Data,    //!< The data layer
    Object,  //!< The object layer
    File,    //!< The file layer
    Set,     //!< The set layer
    Ops,     //!< The ops layer
    Comm,    //!< The comms layer
    Log,     //!< The Log layer itself.
    MPI,     //!< Misc MPI.
    PIOL,    //!< The PIOL itself
    API      //!< Within the APIs
};

/*! @brief The enumeration for the different status messages.
 */
enum class Status : size_t {
    Note,     //!< A general note item.
    Debug,    //!< A debug item
    Warning,  //!< A PIOL warning item
    Error,    //!< A serious error which should lead to program termination
    Request   //!< A message the end-user has specifically requested.
};

/*! @brief An Item instance is a discrete object generated by a recording event and added
 *  to the logging list.
 */
struct Item {
    std::string file;  //!< The file associated with the item
    Layer layer;       //!< The layer the item was recorded in
    Status stat;       //!< The item status
    std::string
      msg;  //!< The message for the end-user which is associated with the item.
    Verbosity vrbsy;  //!< The item verbosity.
};

/*! @brief The class for managing all logging activities.
 */
class Logger {
  private:
    Verbosity maxLevel;  //!< The maximum verbosity level
    std::forward_list<Item>
      loglist_;  //!< The linked-list container which holds the log items
    bool error;  //!< The error status.

  public:
    /*! @brief The constructor with the most options
     *  @param[in] maxLevel_ The maximum verbosity level of stored logs.
     */
    Logger(const Verbosity maxLevel_) : maxLevel(maxLevel_), error(false) {}
    Logger(void) : Logger(PIOL_VERBOSITY_NONE)  //!< @overload
    {
    }

    /*! @brief The default destructor. It processes all of the logs
     */
    ~Logger(void) { procLog(); }

    /// @brief The list of current log items.
    const std::forward_list<Item>& loglist() const { return loglist_; }

    /*! @brief Records a message to the log layer.
     *  @param[in] file The associated filename. An empty string should be passed if there is no associated file.
     *  @param[in] layer The associated layer of the PIOL.
     *  @param[in] stat The associated status.
     *  @param[in] msg The message for the end-user.
     *  @param[in] verbosity The verbosity level of the message.
     */
    void record(
      const std::string file,
      const Layer layer,
      const Status stat,
      const std::string msg,
      const Verbosity verbosity);

    /*! @overload
     * @brief Records a message to the log layer if the condition holds.
     *  @param[in] file The associated filename. An empty string should be passed if there is no associated file.
     *  @param[in] layer The associated layer of the PIOL.
     *  @param[in] stat The associated status.
     *  @param[in] msg The message for the end-user.
     *  @param[in] verbosity The verbosity level of the message.
     *  @param[in] condition If the condition is true log a message
     */
    void record(
      const std::string file,
      const Layer layer,
      const Status stat,
      const std::string msg,
      const Verbosity verbosity,
      const bool condition);

    /*! @brief Process the list of log items.
     */
    void procLog(void);

    /*! @brief Find out the number of items in the log list with a given status.
     *  @param[in] stat The status to be queried
     *  @return The number of items with a status of \c stat
     */
    size_t numStat(const Status stat) const;

    /*! @brief Check if the Log object has recorded an error.
     *  @retval true An error has been encountered.
     *  @retval false An error has not been encountered.
     */
    bool isErr(void) const;
};

}  // namespace Log
}  // namespace PIOL

#endif
