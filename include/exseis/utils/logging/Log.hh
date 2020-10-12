////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief   The logging layer
/// @details The logging layer provides a standard approach to logging and error
///          handling throughout the PIOL while respecting the boundary between
///          layers.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEIS_UTILS_LOGGING_LOG_HH
#define EXSEIS_UTILS_LOGGING_LOG_HH

#include "exseis/utils/logging/Log_entry.hh"
#include "exseis/utils/logging/Verbosity.hh"

#include <string>
#include <vector>


namespace exseis {
inline namespace utils {
inline namespace logging {


/// @brief The class for managing all logging activities.
///
/// Functions in the ExSeisDat library should send messages to the user
/// through an instance of this class, rather than directly to stdout or stderr.
///
/// The logs passed here are stored until `process_logs` is explicitly called,
/// or the class is destroyed, at which point `process_logs` is called.
///
class Log {
  private:
    /// The maximum verbosity level
    Verbosity m_max_verbosity;

    /// A list of all the unprocessed log entries
    std::vector<Log_entry> m_log_entries;

    /// The error status
    bool m_error = false;

  public:
    /// @brief Construct a new Log with a given maximum verbosity.
    ///
    /// @param[in] max_verbosity The maximum verbosity level of stored logs.
    ///
    Log(Verbosity max_verbosity = Verbosity::none) :
        m_max_verbosity(max_verbosity)
    {
    }


    /// @brief All remaining logs are processed at dectruction.
    ///
    ~Log() { process_entries(); }


    /// @brief Logs a message.
    ///
    /// @param[in] log_entry An `Log_entry` to log.
    ///
    void add_entry(Log_entry log_entry);


    /// @brief Process the list of log entries.
    ///
    void process_entries();


    /// @brief Check if the Log object has recorded an error.
    ///
    /// This is set to `true` the first time a log entry is encountered with the
    /// `Error` status, and never set back to `false`.
    ///
    /// @retval true  An error has been encountered.
    /// @retval false An error has not been encountered.
    ///
    bool has_error() const;
};


}  // namespace logging
}  // namespace utils
}  // namespace exseis

#endif  // EXSEIS_UTILS_LOGGING_LOG_HH
