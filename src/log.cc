////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/anc/log.hh"

#include <iostream>

namespace PIOL {
namespace Log {

void Logger::record(
  const std::string file,
  const Layer layer,
  const Status stat,
  const std::string msg,
  const Verbosity verbosity,
  bool condition)
{
    if (condition) record(file, layer, stat, msg, verbosity);
}

void Logger::record(
  const std::string file,
  const Layer layer,
  const Status stat,
  const std::string msg,
  const Verbosity verbosity)
{
    if (static_cast<size_t>(verbosity) <= static_cast<size_t>(maxLevel))
        loglist_.push_front({file, layer, stat, msg, verbosity});

    if (stat == Status::Error) error = true;
}

size_t Logger::numStat(const Status stat) const
{
    size_t sz = 0;
    for (auto& item : loglist_) {
        // The spec guarantees this is one if the equality holds
        sz += (item.stat == stat);
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

}  // namespace Log
}  // namespace PIOL
