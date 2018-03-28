////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date May 2017
/// @brief
////////////////////////////////////////////////////////////////////////////////
#ifndef PIOLSETSHARE_INCLUDE_GUARD
#define PIOLSETSHARE_INCLUDE_GUARD

#include "ExSeisDat/PIOL/file/ReadInterface.hh"
#include "ExSeisDat/PIOL/anc/global.hh"
#include "ExSeisDat/PIOL/share/param.hh"

#include <deque>
#include <memory>
#include <vector>

namespace PIOL {

/*! A file-descriptor structure which describes an input file and the
 *  decomposition for the set layer
 */
struct FileDesc {
    /// The file interface
    std::unique_ptr<File::ReadInterface> ifc;
    /// The size of this corresponds to the local decomposition
    std::vector<size_t> ilst;
    /// The size of this corresponds to the local decomposition
    std::vector<size_t> olst;
};

/*! The structure for holding all trace data.
 */
struct TraceBlock {
    /// Number of traces
    size_t nt;
    /// Number of samples per trace
    size_t ns;
    /// Increment between trace samples
    geom_t inc;
    /// Gather number (if applicable)
    size_t gNum;
    /// Number of gathers (if applicable)
    size_t numG;
    /// unique pointer to parameter structure (if applicable)
    std::unique_ptr<File::Param> prm;
    /// traces (if applicable)
    std::vector<trace_t> trc;
};

/// A typedef to simplify the list of file descriptors
typedef std::deque<std::shared_ptr<FileDesc>> FileDeque;

}  // namespace PIOL

#endif
