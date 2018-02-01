////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date May 2017
/// @brief
////////////////////////////////////////////////////////////////////////////////
#ifndef PIOLSETSHARE_INCLUDE_GUARD
#define PIOLSETSHARE_INCLUDE_GUARD

#include "file/file.hh"
#include "global.hh"

#include <deque>
#include <memory>

namespace PIOL {

/*! A file-descriptor structure which describes an input file and the decomposition for the set layer
 */
struct FileDesc {
    std::unique_ptr<File::ReadInterface> ifc;  //!< The file interface
    std::vector<size_t>
      ilst;  //!< The size of this corresponds to the local decomposition
    std::vector<size_t>
      olst;  //!< The size of this corresponds to the local decomposition
};

/*! The structure for holding all trace data.
 */
struct TraceBlock {
    size_t nt;    //!< Number of traces
    size_t ns;    //!< Number of samples per trace
    geom_t inc;   //!< Increment between trace samples
    size_t gNum;  //!< Gather number (if applicable)
    size_t numG;  //!< Number of gathers (if applicable)
    std::unique_ptr<File::Param>
      prm;  //!< unique pointer to parameter structure (if applicable)
    std::vector<trace_t> trc;  //!< traces (if applicable)
};

typedef std::deque<std::shared_ptr<FileDesc>>
  FileDeque;  //!< A typedef to simplify the list of file descriptors

}  // namespace PIOL

#endif
