////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////

#include "anc/piol.hh"

#include <cstdlib>

namespace PIOL {

ExSeisPIOL::ExSeisPIOL(const Verbosity maxLevel, const Comm::MPI::Opt& copt)
{
    log  = std::make_unique<Log::Logger>(maxLevel);
    comm = std::make_unique<Comm::MPI>(log.get(), copt);
}

void ExSeisPIOL::isErr(const std::string& msg) const
{
    if (log->isErr()) {
        log->record(
          "", Log::Layer::PIOL, Log::Status::Error,
          "Fatal Error in PIOL. " + msg + ". Dumping Log", PIOL_VERBOSITY_NONE);
        log->~Logger();
        comm->~Interface();
        std::_Exit(EXIT_FAILURE);
    }
}

}  // namespace PIOL
