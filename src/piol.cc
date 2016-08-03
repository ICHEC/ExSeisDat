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
#include <iostream>
#include <string>
namespace PIOL {

ExSeisPIOL::ExSeisPIOL(const Log::Verb maxLevel, const Comm::Opt & comOpt)
{
    log = std::make_unique<Log::Logger>(maxLevel);
    switch (comOpt.type)
    {
        case Comm::Type::MPI :
        {
            auto mpiOpt = castOptToDeriv<Comm::MPIOpt, Comm::Opt>(this, comOpt, "", Log::Layer::PIOL);
            if (mpiOpt == nullptr)
                return;
            auto mpicomm = new Comm::MPI(*mpiOpt);
            comm = castToBase<Comm::Interface, Comm::MPI>(this, mpicomm, "", Log::Layer::PIOL);
            if (comm == nullptr)
                return;
        }
        break;
        default :
//TODO Add error
        break;
    }
}

ExSeisPIOL::ExSeisPIOL(const Comm::Opt & comm) : ExSeisPIOL(Log::Verb::None, comm)
{
}

ExSeisPIOL::~ExSeisPIOL(void)
{
    log.reset();
    comm.reset();
}

void ExSeisPIOL::record(const std::string file, const Log::Layer layer, const Log::Status stat, const std::string msg, const Log::Verb verbosity)
{
    log->record(file, layer, stat, msg, verbosity);
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