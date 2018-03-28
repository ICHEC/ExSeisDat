////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/ReadInterface.hh"

namespace PIOL {
namespace File {

const trace_t* TRACE_NULL = (trace_t*)1;

// TODO: Unit test
void ReadInterface::readParam(
  const size_t offset, const size_t sz, Param* prm, const size_t skip) const
{
    readTrace(offset, sz, const_cast<trace_t*>(TRACE_NULL), prm, skip);
}

void ReadInterface::readParamNonContiguous(
  const size_t sz, const size_t* offsets, Param* prm, const size_t skip) const
{
    readTraceNonContiguous(
      sz, offsets, const_cast<trace_t*>(TRACE_NULL), prm, skip);
}

const std::string& ReadInterface::readText(void) const
{
    return text;
}

size_t ReadInterface::readNs(void) const
{
    return ns;
}

geom_t ReadInterface::readInc(void) const
{
    return inc;
}

}  // namespace File
}  // namespace PIOL
