#ifndef PIOLPIOL_INCLUDE_GUARD
#define PIOLPIOL_INCLUDE_GUARD
#include <memory>
#include "anc/comm.hh"
#include "anc/log.hh"
namespace PIOL {
enum class Comm : size_t
{
    MPI
};
struct Options
{
    Comm commun;
};

struct ExSeisPIOL
{
    std::unique_ptr<Log::Logger> log;
    public :
    std::shared_ptr<Comms::Interface> comm;
    ExSeisPIOL(Options opt);
    ~ExSeisPIOL(void);
    void record(const std::string file, const Log::Layer layer, const Log::Status stat, const std::string msg, const Log::Verb verbosity);
    void exit(int code);
};
}
#endif

