/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include "share/casts.hh"
#include "anc/piol.hh"
#include "anc/cmpi.hh"
#include <string>

namespace PIOL {
ExSeisPIOL::ExSeisPIOL(Comm::MPI::Opt & copt, const Log::Verb maxLevel)
{
    log = std::make_unique<Log::Logger>(maxLevel);
    comm = std::make_shared<Comm::MPI>(log.get(), copt);
}

ExSeisPIOL::ExSeisPIOL(bool initMPI, const Log::Verb maxLevel)
{
    Comm::MPI::Opt copt;
    copt.initMPI = initMPI;
    ExSeisPIOL(copt, maxLevel);
}

ExSeisPIOL::ExSeisPIOL(const Log::Verb maxLevel)
{
    Comm::MPI::Opt copt;
    ExSeisPIOL(copt, maxLevel);
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
        log->record("", Log::Layer::PIOL, Log::Status::Error, "Fatal Error in PIOL. " + msg + ". Dumping Log", Log::Verb::None);
        log.reset();
        comm.reset();
        std::exit(EXIT_FAILURE);
    }
}
}
