////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation for \c WriteInterface
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/file/WriteInterface.hh"

namespace PIOL {
namespace File {


void WriteInterface::writeParam(
  const size_t offset, const size_t sz, const Param* prm, const size_t skip)
{
    writeTrace(offset, sz, const_cast<trace_t*>(TRACE_NULL), prm, skip);
}

void WriteInterface::writeParamNonContiguous(
  const size_t sz, const size_t* offset, const Param* prm, const size_t skip)
{
    writeTraceNonContiguous(
      sz, offset, const_cast<trace_t*>(TRACE_NULL), prm, skip);
}

}  // namespace File
}  // namespace PIOL
