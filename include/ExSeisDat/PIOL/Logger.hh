////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief   The logging layer
/// @details The logging layer provides a standard approach to logging and error
///          handling throughout the PIOL while respecting the boundary between
///          layers.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_LOGGER_HH
#define EXSEISDAT_PIOL_LOGGER_HH

#include "ExSeisDat/PIOL/Verbosity.h"

#include <forward_list>
#include <string>

namespace exseis {
namespace PIOL {

/*! @brief The class for managing all logging activities.
 */
class Logger {
  public:
    enum class Layer : size_t;

    enum class Status : size_t;

  private:
    /// @brief An Item instance is a discrete object generated by a recording
    ///        event and added to the logging list.
    struct Item {
        /// The file associated with the item
        std::string file;

        /// The layer the item was recorded in
        Layer layer;

        /// The item status
        Status stat;

        /// The message for the end-user which is associated with the item.
        std::string msg;

        /// The item verbosity.
        Verbosity vrbsy;
    };

    /// The maximum verbosity level
    Verbosity maxLevel;

    /// The linked-list container which holds the log items
    std::forward_list<Item> loglist_;

    /// The error status.
    bool error;

  public:
    /*! @brief The constructor with the most options
     *  @param[in] maxLevel_ The maximum verbosity level of stored logs.
     */
    Logger(Verbosity maxLevel_) : maxLevel(maxLevel_), error(false) {}

    /// @overload
    Logger() : Logger(PIOL_VERBOSITY_NONE) {}

    /*! @brief The default destructor. It processes all of the logs
     */
    ~Logger() { procLog(); }

    /*! @brief The list of current log items.
     *  @return The list of current log items.
     */
    const std::forward_list<Item>& loglist() const { return loglist_; }

    /*! @brief Records a message to the log layer.
     *  @param[in] file The associated filename. An empty string should be
     *             passed if there is no associated file.
     *  @param[in] layer The associated layer of the PIOL.
     *  @param[in] stat The associated status.
     *  @param[in] msg The message for the end-user.
     *  @param[in] verbosity The verbosity level of the message.
     */
    void record(
      std::string file,
      Layer layer,
      Status stat,
      std::string msg,
      Verbosity verbosity);

    /*! @overload
     *  @brief Records a message to the log layer if the condition holds.
     *  @param[in] file The associated filename. An empty string should be
     *                  passed if there is no associated file.
     *  @param[in] layer The associated layer of the PIOL.
     *  @param[in] stat The associated status.
     *  @param[in] msg The message for the end-user.
     *  @param[in] verbosity The verbosity level of the message.
     *  @param[in] condition If the condition is true log a message
     */
    void record(
      std::string file,
      Layer layer,
      Status stat,
      std::string msg,
      Verbosity verbosity,
      bool condition);

    /*! @brief Process the list of log items.
     */
    void procLog();

    /*! @brief Find out the number of items in the log list with a given status.
     *  @param[in] stat The status to be queried
     *  @return The number of items with a status of \c stat
     */
    size_t numStat(Status stat) const;

    /*! @brief Check if the Log object has recorded an error.
     *  @retval true An error has been encountered.
     *  @retval false An error has not been encountered.
     */
    bool isErr() const;
};


/// @brief The enumeration for the different layers of the PIOL.
enum class Logger::Layer : size_t {
    /// The data layer
    Data,

    /// The object layer
    Object,

    /// The file layer
    File,

    /// The set layer
    Set,

    /// The ops layer
    Ops,

    /// The comms layer
    Comm,

    /// The Log layer itself.
    Log,

    /// Misc MPI.
    MPI,

    /// The PIOL itself
    PIOL,

    /// Within the APIs
    API
};

/// @brief The enumeration for the different status messages.
enum class Logger::Status : size_t {
    /// A general note item.
    Note,

    /// A debug item
    Debug,

    /// A PIOL warning item
    Warning,

    /// A serious error which should lead to program termination
    Error,

    /// A message the end-user has specifically requested.
    Request
};

}  // namespace PIOL
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_LOGGER_HH
