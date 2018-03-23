////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The \c Model3dInterface class
/// @details The \c Model3dInterface is a generic interface for reading 3d
///          volumetric seismic data.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_FILE_MODEL3DINTERFACE_HH
#define EXSEISDAT_PIOL_FILE_MODEL3DINTERFACE_HH

#include "ExSeisDat/PIOL/global.hh"
#include "ExSeisDat/PIOL/share/uniray.hh"

#include <utility>

namespace PIOL {
namespace File {

/*! @brief An intitial class for 3d volumetric seismic data
 */
class Model3dInterface {
  public:
    /// Parameters for the inline coordinate (start, count, increment)
    std::tuple<llint, llint, llint> il;

    /// Parameters for the crossline coordinate (start, count, increment)
    std::tuple<llint, llint, llint> xl;

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

}  // namespace File
}  // namespace PIOL

#endif  // EXSEISDAT_PIOL_FILE_MODEL3DINTERFACE_HH
