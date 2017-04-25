/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include "file/file.hh"
namespace PIOL { namespace File {
const Param * PARAM_NULL = (Param *)1;
const trace_t * TRACE_NULL = (trace_t *)1;

//TODO: Unit test
void ReadInterface::readParam(csize_t offset, csize_t sz, Param * prm, csize_t skip) const
{
    if (offset >= nt && sz)   //Nothing to be read.
    {
        piol->log->record(name, Log::Layer::File, Log::Status::Warning,
            "readParam() was called for a zero byte read", Log::Verb::None);
        return;
    }

    readTrace(offset, sz, const_cast<trace_t *>(TRACE_NULL), prm, skip);
}

void ReadInterface::readParam(csize_t sz, csize_t * offset, Param * prm, csize_t skip) const
{
    readTrace(sz, offset, const_cast<trace_t *>(TRACE_NULL), prm, skip);
}

const std::string & ReadInterface::readText(void) const
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
}}
