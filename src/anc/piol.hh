#ifndef PIOLPIOL_INCLUDE_GUARD
#define PIOLPIOL_INCLUDE_GUARD
#include <memory>
#include "anc/comm.hh"
#include "anc/log.hh"
namespace PIOL {
struct ExSeisPIOL
{
    std::unique_ptr<Log::Logger> log;
    std::shared_ptr<Comm::Interface> comm;
    ExSeisPIOL(const Comm::Opt & comm);
    ~ExSeisPIOL(void);
    void record(const std::string file, const Log::Layer layer, const Log::Status stat, const std::string msg, const Log::Verb verbosity);
    void exit(int code);
};
}
#endif

