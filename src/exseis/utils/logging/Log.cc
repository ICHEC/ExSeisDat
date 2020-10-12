////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of exseis::utils::logging::Log
////////////////////////////////////////////////////////////////////////////////

#include "exseis/utils/logging/Log.hh"

#include <iostream>

namespace exseis {
inline namespace utils {
inline namespace logging {


void Log::add_entry(Log_entry log_entry)
{
    if (log_entry.verbosity <= m_max_verbosity) {
        m_log_entries.push_back(log_entry);
    }

    if (log_entry.status == Status::Error) {
        m_error = true;
    }
}

bool Log::has_error() const
{
    return m_error;
}

void Log::process_entries()
{
    for (auto& item : m_log_entries) {
        std::cerr << item.source_position.function_name << " in "
                  << item.source_position.file_name << " at line "
                  << item.source_position.line_number << ": " << item.message
                  << " -- Status: " << status_to_string(item.status) << ", "
                  << "Verbosity: " << static_cast<size_t>(item.verbosity)
                  << std::endl;
    }

    m_log_entries.clear();
}

}  // namespace logging
}  // namespace utils
}  // namespace exseis
