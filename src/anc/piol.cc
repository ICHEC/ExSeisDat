#include "anc/piol.hh"
#include "anc/cmpi.hh"
#include <iostream>
namespace PIOL {
ExSeisPIOL::ExSeisPIOL(Options opt)
{
    log = std::make_unique<Log::Logger>();
    if (opt.commun == Comm::MPI)
        comm = std::make_shared<Comms::MPI>(MPI_COMM_WORLD);
}

ExSeisPIOL::~ExSeisPIOL(void)
{
    log.reset();
    comm.reset();
}
void ExSeisPIOL::record(const std::string file, const Log::Layer layer, const Log::Status stat, const std::string msg, const Log::Verb verbosity)
{
    log->record(comm->getRank(), file, layer, stat, msg, verbosity);
}
void ExSeisPIOL::exit(int code)
{
    record("", Log::Layer::PIOL, Log::Status::Error, "Fatal Error in PIOL. Dumping Log", Log::Verb::None);
    delete this;
    exit(-1);
}
}
