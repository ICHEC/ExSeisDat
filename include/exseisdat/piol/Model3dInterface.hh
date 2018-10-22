////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The \c Model3dInterface class
/// @details The \c Model3dInterface is a generic interface for reading 3d
///          volumetric seismic data.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_MODEL3DINTERFACE_HH
#define EXSEISDAT_PIOL_MODEL3DINTERFACE_HH

#include "exseisdat/piol/operations/gather.hh"
#include "exseisdat/utils/Distributed_vector.hh"
#include "exseisdat/utils/typedefs.hh"

#include <utility>

namespace exseis {
namespace piol {

using namespace exseis::utils::typedefs;

/*! @brief An intitial class for 3d volumetric seismic data
 */
class Model3dInterface {
  public:
    /// A class for storing coordinate parameters
    struct CoordinateParameters {
        /// The initial coordinate value
        exseis::utils::Integer start;

        /// The number of coordinate values
        exseis::utils::Integer count;

        /// The increment between coordinate values
        exseis::utils::Integer increment;

        /// A value constructor for the CoordinateParameters
        /// @param[in] start     The initial coordinate value
        /// @param[in] count     The number of coordinate values
        /// @param[in] increment The increment between coordinate values
        CoordinateParameters(
          exseis::utils::Integer start     = 0,
          exseis::utils::Integer count     = 0,
          exseis::utils::Integer increment = 0) :
            start(start),
            count(count),
            increment(increment)
        {
        }
    };

    /// Parameters for the inline coordinate (start, count, increment)
    CoordinateParameters il;

    /// Parameters for the crossline coordinate (start, count, increment)
    CoordinateParameters xl;

    /// @brief A virtual destructor
    virtual ~Model3dInterface() = default;

    /*! read the 3d file based on il and xl that match those in the given
     *  \c gather array.
     *  @param[in] offset the offset into the global array
     *  @param[in] sz the number of gathers for the local process
     *  @param[in] gather a structure which contains the il and xl coordinates
     *                    of interest
     *  @return return a vector of traces containing the trace values requested
     */
    virtual std::vector<exseis::utils::Trace_value> read_model(
      size_t offset,
      size_t sz,
      const exseis::utils::Distributed_vector<Gather_info>& gather) = 0;

    /*! Read the 3d file based on il and xl that match those in the given
     *  \c gather array.
     *  @param[in] sz The number of offsets for the local process
     *  @param[in] offset the offset into the global array
     *  @param[in] gather A structure which contains the il and xl coordinates
     *                    of interest
     *  @return Return a vector of traces containing the trace values requested
     */
    virtual std::vector<exseis::utils::Trace_value> read_model(
      size_t sz,
      const size_t* offset,
      const exseis::utils::Distributed_vector<Gather_info>& gather) = 0;
};

}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_MODEL3DINTERFACE_HH
