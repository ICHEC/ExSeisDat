////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation for \c WriteInterface
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/WriteInterface.hh"

namespace exseis {
namespace PIOL {

void WriteInterface::writeParam(
  const size_t offset, const size_t sz, const Param* prm, const size_t skip)
{
    writeTrace(
      offset, sz, const_cast<exseis::utils::Trace_value*>(TRACE_NULL), prm,
      skip);
}

void WriteInterface::writeParamNonContiguous(
  const size_t sz, const size_t* offset, const Param* prm, const size_t skip)
{
    writeTraceNonContiguous(
      sz, offset, const_cast<exseis::utils::Trace_value*>(TRACE_NULL), prm,
      skip);
}

}  // namespace PIOL
}  // namespace exseis
