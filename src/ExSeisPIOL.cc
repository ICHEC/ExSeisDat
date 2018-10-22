#include "exseisdat/piol/ExSeisPIOL.hh"

using namespace exseis::utils::logging;

namespace exseis {
namespace piol {

ExSeisPIOL::ExSeisPIOL(
  const Verbosity max_level, const CommunicatorMPI::Opt& copt)
{
    log  = std::make_shared<Log>(max_level);
    comm = std::make_shared<CommunicatorMPI>(log.get(), copt);
}

void ExSeisPIOL::assert_ok(const std::string& message) const
{
    if (log->has_error()) {
        std::string formatted_message = "Fatal Error in PIOL";

        if (!message.empty()) {
            formatted_message += std::string(": ") + message;
        }

        formatted_message += ". Dumping Log.";


        log->add_entry(exseis::utils::Log_entry{
          exseis::utils::Status::Error, formatted_message,
          exseis::utils::Verbosity::none,
          EXSEISDAT_SOURCE_POSITION("exseis::piol::ExSeisPIOL::assert_ok")});

        log->~Log();
        comm->~CommunicatorMPI();

        std::_Exit(EXIT_FAILURE);
    }
}

}  // namespace piol
}  // namespace exseis
