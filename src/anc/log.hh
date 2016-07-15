#include <string>
#include <queue>

#ifndef PIOL_ANC_LOG_INCLUDE_GUARD
#define PIOL_ANC_LOG_INCLUDE_GUARD
namespace PIOL { namespace Log {

enum class Layer : size_t
{
    Data,
    Object,
    File,
    Set,
    Operations,
    Comm,
    Log,
    MPI,    //Misc MPI
    PIOL
};

enum class Status : size_t
{
    Note,
    Debug,
    Warning,
    Error
};

enum class Verb : size_t
{
    None,
    Minimal,
    Extended,
    Verbose,
    Max
};

struct Item
{
    size_t rank;
    std::string file;
    Layer layer;
    Status stat;
    std::string msg;
    Verb vrbsy;
};

class Logger
{
    private :
    Verb maxLevel = Verb::None;
    std::queue<Item> que;
    bool error = false;

    public :
    Logger(void)
    {
    }
    Logger(const Verb maxLevel_) : maxLevel(maxLevel_)
    {
    }
    ~Logger(void)
    {
        procLog();
    }
    void record(const size_t rank, const std::string file, const Layer layer, const Status stat, const std::string msg, const Verb verbosity);
    void procLog(void);
    bool isErr(void)
    {
        return error;
    }
};
}}
#endif
