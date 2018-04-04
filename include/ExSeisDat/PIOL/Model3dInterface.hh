////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The \c Model3dInterface class
/// @details The \c Model3dInterface is a generic interface for reading 3d
///          volumetric seismic data.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_FILE_MODEL3DINTERFACE_HH
#define EXSEISDAT_PIOL_FILE_MODEL3DINTERFACE_HH

#include "ExSeisDat/PIOL/Uniray.hh"
#include "ExSeisDat/PIOL/typedefs.h"

#include <utility>

namespace PIOL {

/*! @brief An intitial class for 3d volumetric seismic data
 */
class Model3dInterface {
  public:
    /// A class for storing coordinate parameters
    struct CoordinateParameters {
        /// The initial coordinate value
        llint start;

        /// The number of coordinate values
        llint count;

        /// The increment between coordinate values
        llint increment;

        /// A value constructor for the CoordinateParameters
        /// @param[in] start     The initial coordinate value
        /// @param[in] count     The number of coordinate values
        /// @param[in] increment The increment between coordinate values
        CoordinateParameters(
          llint start = 0, llint count = 0, llint increment = 0) :
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

    /*! read the 3d file based on il and xl that match those in the given
     *  \c gather array.
     *  @param[in] offset the offset into the global array
     *  @param[in] sz the number of gathers for the local process
     *  @param[in] gather a structure which contains the il and xl coordinates
     *                    of interest
     *  @return return a vector of traces containing the trace values requested
     */
    virtual std::vector<trace_t> readModel(
      const size_t offset,
      const size_t sz,
      const Uniray<size_t, llint, llint>& gather) = 0;

    /*! Read the 3d file based on il and xl that match those in the given
     *  \c gather array.
     *  @param[in] sz The number of offsets for the local process
     *  @param[in] offset the offset into the global array
     *  @param[in] gather A structure which contains the il and xl coordinates
     *                    of interest
     *  @return Return a vector of traces containing the trace values requested
     */
    virtual std::vector<trace_t> readModel(
      const size_t sz,
      const size_t* offset,
      const Uniray<size_t, llint, llint>& gather) = 0;
};

}  // namespace PIOL

#endif  // EXSEISDAT_PIOL_FILE_MODEL3DINTERFACE_HH
