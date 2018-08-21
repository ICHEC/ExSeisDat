#include "ExSeisDat/PIOL/ExSeisPIOL.hh"

namespace exseis {
namespace PIOL {

ExSeisPIOL::ExSeisPIOL(
  const Verbosity maxLevel, const CommunicatorMPI::Opt& copt)
{
    log  = std::make_unique<Logger>(maxLevel);
    comm = std::make_unique<CommunicatorMPI>(log.get(), copt);
}

void ExSeisPIOL::isErr(const std::string& msg) const
{
    if (log->isErr()) {
        log->record(
          "", Logger::Layer::PIOL, Logger::Status::Error,
          "Fatal Error in PIOL. " + msg + ". Dumping Log", PIOL_VERBOSITY_NONE);
        log->~Logger();
        comm->~CommunicatorMPI();
        std::_Exit(EXIT_FAILURE);
    }
}

}  // namespace PIOL
}  // namespace exseis
