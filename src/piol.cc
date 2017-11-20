/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include "anc/piol.hh"
namespace PIOL {
void ExSeisPIOL::Init(Comm::MPI::Opt & copt, const Verbosity maxLevel)
{
    log = std::make_unique<Log::Logger>(maxLevel);
    comm = std::make_unique<Comm::MPI>(log.get(), copt);
}

ExSeisPIOL::ExSeisPIOL(Comm::MPI::Opt & copt, const Verbosity maxLevel)
{
    Init(copt, maxLevel);
}

ExSeisPIOL::ExSeisPIOL(bool initMPI, const Verbosity maxLevel)
{
    Comm::MPI::Opt copt;
    copt.initMPI = initMPI;
    Init(copt, maxLevel);
}

ExSeisPIOL::ExSeisPIOL(const Verbosity maxLevel)
{
    Comm::MPI::Opt copt;
    Init(copt, maxLevel);
}

ExSeisPIOL::~ExSeisPIOL(void)
{
    log.reset();
    comm.reset();
}

void ExSeisPIOL::isErr(std::string msg)
{
    if (log->isErr())
    {
        log->record("", Log::Layer::PIOL, Log::Status::Error, "Fatal Error in PIOL. " + msg + ". Dumping Log", PIOL_VERBOSITY_NONE);
        log.reset();
        comm.reset();
        std::exit(EXIT_FAILURE);
    }
}
}
