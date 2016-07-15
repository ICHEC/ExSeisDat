#include "anc/piol.hh"
#include "anc/cmpi.hh"
#include <iostream>
#include <string>
namespace PIOL {
ExSeisPIOL::ExSeisPIOL(const Comm::Opt & com)
{
    log = std::make_unique<Log::Logger>();
    switch (com.type)
    {
        case Comm::Type::MPI :
            comm = std::make_shared<Comm::MPI>(dynamic_cast<const Comm::MPIOpt &>(com));
        break;
        default :
//TODO Add error
        break;
    }
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

void ExSeisPIOL::isErr(std::string msg)
{
    if (log->isErr())
    {
        record("", Log::Layer::PIOL, Log::Status::Error, "Fatal Error in PIOL. " + msg + ". Dumping Log", Log::Verb::None);
        log.reset();
        comm.reset();
        std::exit(EXIT_FAILURE);
    }
}

/*void ExSeisPIOL::exit(int code)
{
    record("", Log::Layer::PIOL, Log::Status::Error, "Fatal Error in PIOL. (code: " + std::to_string(code) + ") Dumping Log", Log::Verb::None);
    log.reset();
    comm.reset();
    std::exit(code);
}*/
}
