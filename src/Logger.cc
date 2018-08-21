////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/Logger.hh"

#include <iostream>

namespace exseis {
namespace PIOL {

void Logger::record(
  const std::string file,
  const Logger::Layer layer,
  const Logger::Status stat,
  const std::string msg,
  const Verbosity verbosity,
  bool condition)
{
    if (condition) {
        record(file, layer, stat, msg, verbosity);
    }
}

void Logger::record(
  const std::string file,
  const Logger::Layer layer,
  const Logger::Status stat,
  const std::string msg,
  const Verbosity verbosity)
{
    if (verbosity <= maxLevel) {
        loglist_.push_front({file, layer, stat, msg, verbosity});
    }

    if (stat == Logger::Status::Error) {
        error = true;
    }
}

size_t Logger::numStat(const Logger::Status stat) const
{
    size_t sz = 0;
    for (auto& item : loglist_) {
        // The spec guarantees this is one if the equality holds
        if (item.stat == stat) {
            sz++;
        }
    }

    return sz;
}

bool Logger::isErr(void) const
{
    return error;
}

void Logger::procLog(void)
{
    for (auto& item : loglist_) {
        std::cerr << item.file << " " << static_cast<size_t>(item.layer) << " "
                  << static_cast<size_t>(item.stat) << " " << item.msg << " "
                  << static_cast<size_t>(item.vrbsy) << std::endl;
    }

    loglist_.clear();
}

}  // namespace PIOL
}  // namespace exseis
