////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The \c Model3dInterface class
/// @details The \c Model3dInterface is a generic interface for reading 3d
///          volumetric seismic data.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_FILE_DETAIL_MODEL3DINTERFACE_HH
#define EXSEISDAT_PIOL_FILE_DETAIL_MODEL3DINTERFACE_HH

#include "exseisdat/piol/operations/gather.hh"
#include "exseisdat/utils/distributed_vector/Distributed_vector.hh"
#include "exseisdat/utils/types/typedefs.hh"

#include <utility>

namespace exseis {
namespace piol {
inline namespace file {
inline namespace detail {


using namespace exseis::utils::types;

/// @brief An intitial class for 3d volumetric seismic data
///
class Model3dInterface {
  public:
    /// @brief A class for storing coordinate parameters
    struct CoordinateParameters {
        /// @brief The initial coordinate value
        exseis::utils::Integer start;

        /// @brief The number of coordinate values
        exseis::utils::Integer count;

        /// @brief The increment between coordinate values
        exseis::utils::Integer increment;

        /// @brief A value constructor for the CoordinateParameters
        ///
        /// @param[in] start     The initial coordinate value
        /// @param[in] count     The number of coordinate values
        /// @param[in] increment The increment between coordinate values
        CoordinateParameters(
            exseis::utils::Integer start     = 0,
            exseis::utils::Integer count     = 0,
            exseis::utils::Integer increment = 0) :
            start(start), count(count), increment(increment)
        {
        }
    };

    /// @brief Parameters for the inline coordinate (start, count, increment)
    CoordinateParameters il;

    /// @brief Parameters for the crossline coordinate (start, count, increment)
    CoordinateParameters xl;

    /// @brief A virtual destructor
    virtual ~Model3dInterface() = default;

    /// @brief read the 3d file based on il and xl that match those in the given
    ///        \c gather array.
    ///
    /// @param[in] offset   the offset into the global array
    /// @param[in] sz       the number of gathers for the local process
    /// @param[in] gather   a structure which contains the il and xl coordinates
    ///                     of interest
    ///
    /// @return return a vector of traces containing the trace values requested
    ///
    virtual std::vector<exseis::utils::Trace_value> read_model(
        size_t offset,
        size_t sz,
        const exseis::utils::Distributed_vector<Gather_info>& gather) = 0;

    /// @brief Read the 3d file based on il and xl that match those in the given
    ///        \c gather array.
    ///
    /// @param[in] sz       The number of offsets for the local process
    /// @param[in] offset   the offset into the global array
    /// @param[in] gather   A structure which contains the il and xl coordinates
    ///                     of interest
    ///
    /// @return Return a vector of traces containing the trace values requested
    ///
    virtual std::vector<exseis::utils::Trace_value> read_model(
        size_t sz,
        const size_t* offset,
        const exseis::utils::Distributed_vector<Gather_info>& gather) = 0;
};

}  // namespace detail
}  // namespace file
}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_FILE_DETAIL_MODEL3DINTERFACE_HH
