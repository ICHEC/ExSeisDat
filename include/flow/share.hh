/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date May 2017
 *   \brief
*//*******************************************************************************************/
#ifndef PIOLSETSHARE_INCLUDE_GUARD
#define PIOLSETSHARE_INCLUDE_GUARD
#include "global.hh"
#include "file/file.hh"
#include <deque>
#include <memory>

#warning io
#include <iostream>
namespace PIOL {
/*! A file-descriptor structure which describes an input file and the decomposition for the set layer
 */
struct FileDesc
{
    std::unique_ptr<File::ReadInterface> ifc;   //!< The file interface
    std::vector<size_t> ilst;                   //!< The size of this corresponds to the local decomposition
    std::vector<size_t> olst;                   //!< The size of this corresponds to the local decomposition

    ~FileDesc(void)
    {
        ifc.reset();
    }
};

struct TraceBlock
{
    size_t nt;
    size_t ns;
    geom_t inc;
    size_t gNum;
    size_t numG;
    std::unique_ptr<File::Param> prm;
    std::vector<trace_t> trc;
};

typedef std::deque<std::shared_ptr<FileDesc>> FileDeque;
}
#endif
