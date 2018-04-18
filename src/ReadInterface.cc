////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/ReadInterface.hh"

namespace exseis {
namespace PIOL {

const exseis::utils::Trace_value* TRACE_NULL = (exseis::utils::Trace_value*)1;

// TODO: Unit test
void ReadInterface::readParam(
  const size_t offset, const size_t sz, Param* prm, const size_t skip) const
{
    readTrace(
      offset, sz, const_cast<exseis::utils::Trace_value*>(TRACE_NULL), prm,
      skip);
}

void ReadInterface::readParamNonContiguous(
  const size_t sz, const size_t* offsets, Param* prm, const size_t skip) const
{
    readTraceNonContiguous(
      sz, offsets, const_cast<exseis::utils::Trace_value*>(TRACE_NULL), prm,
      skip);
}

const std::string& ReadInterface::readText(void) const
{
    return text;
}

size_t ReadInterface::readNs(void) const
{
    return ns;
}

exseis::utils::Floating_point ReadInterface::readInc(void) const
{
    return inc;
}

}  // namespace PIOL
}  // namespace exseis
