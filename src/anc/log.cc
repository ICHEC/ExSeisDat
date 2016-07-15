#include "log.hh"
#include <iostream>
namespace PIOL { namespace Log {
void Logger::record(size_t rank, const std::string file, const Layer layer, const Status stat, const std::string msg, const Verb verbosity)
{
    if (static_cast<size_t>(verbosity) <= static_cast<size_t>(maxLevel))
        que.push({rank, file, layer, stat, msg, verbosity});

    if (stat == Status::Error)
        error = true;
}

void Logger::procLog(void)
{
    while (!que.empty())
    {
        Item & item = que.front();

        std::cerr << item.file << " " << static_cast<size_t>(item.layer) << " " << static_cast<size_t>(item.stat) <<
                     " " << item.msg << " " << static_cast<size_t>(item.vrbsy) << std::endl;
        que.pop();
    }
}
}}
