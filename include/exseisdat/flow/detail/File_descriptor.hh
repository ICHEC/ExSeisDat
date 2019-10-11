////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_DETAIL_FILE_DESCRIPTOR_HH
#define EXSEISDAT_FLOW_DETAIL_FILE_DESCRIPTOR_HH

#include "exseisdat/piol/file/Input_file.hh"

#include <cstddef>
#include <memory>
#include <vector>

namespace exseis {
namespace flow {
namespace detail {

/*! A file-descriptor structure which describes an input file and the
 *  decomposition for the set layer
 */
struct File_descriptor {
    /// The file interface
    std::unique_ptr<exseis::piol::Input_file> ifc;

    /// The size of this corresponds to the local decomposition
    std::vector<size_t> ilst;

    /// The size of this corresponds to the local decomposition
    std::vector<size_t> olst;
};

}  // namespace detail
}  // namespace flow
}  // namespace exseis

#endif  // EXSEISDAT_FLOW_DETAIL_FILE_DESCRIPTOR_HH
