#include "exseisdat/utils/logging/Status.hh"

namespace exseis {
namespace utils {
inline namespace logging {

std::string status_to_string(Status status)
{
    switch (status) {
        case Status::Note:
            return "Note";

        case Status::Debug:
            return "Debug";

        case Status::Warning:
            return "Warning";

        case Status::Error:
            return "Error";

        case Status::Request:
            return "Request";
    }

    return "Unknown";
}

}  // namespace logging
}  // namespace utils
}  // namespace exseis
