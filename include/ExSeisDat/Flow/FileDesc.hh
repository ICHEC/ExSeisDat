////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_FILEDESC_HH
#define EXSEISDAT_FLOW_FILEDESC_HH

#include "ExSeisDat/PIOL/ReadInterface.hh"

#include <cstddef>
#include <memory>
#include <vector>

namespace exseis {
namespace Flow {

/*! A file-descriptor structure which describes an input file and the
 *  decomposition for the set layer
 */
struct FileDesc {
    /// The file interface
    std::unique_ptr<exseis::PIOL::ReadInterface> ifc;

    /// The size of this corresponds to the local decomposition
    std::vector<size_t> ilst;

    /// The size of this corresponds to the local decomposition
    std::vector<size_t> olst;
};

}  // namespace Flow
}  // namespace exseis

#endif  // EXSEISDAT_FLOW_FILEDESC_HH
