#include "exseisdat/piol/configuration/ExSeisPIOL.hh"

using namespace exseis::utils::logging;

namespace exseis {
namespace piol {
inline namespace configuration {

ExSeisPIOL::ExSeisPIOL(
    const Verbosity max_level, const Communicator_mpi::Opt& copt)
{
    log  = std::make_shared<Log>(max_level);
    comm = std::make_shared<Communicator_mpi>(log.get(), copt);
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
        comm->~Communicator_mpi();

        std::_Exit(EXIT_FAILURE);
    }
}

}  // namespace configuration
}  // namespace piol
}  // namespace exseis
