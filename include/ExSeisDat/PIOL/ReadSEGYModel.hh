////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The \c ReadSEGYModel class reads SEGY velocity models.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_FILE_READSEGYMODEL_HH
#define EXSEISDAT_PIOL_FILE_READSEGYMODEL_HH

#include "ExSeisDat/PIOL/Model3dInterface.hh"
#include "ExSeisDat/PIOL/ReadSEGY.hh"

namespace PIOL {

/*! A SEGY class for seismic velocity models
 *
 * Velocity models indicate the velocity anomolies within a subsurface, which
 * indicate areas with high velocity anomolies correspond to denser material,
 * while lower velocities correspond to materials that are less dense.
 */
class ReadSEGYModel : public Model3dInterface, public ReadSEGY {
  public:
    /*! @brief The SEG-Y Model options structure.
     */
    struct Opt : public ReadSEGY::Opt {
        /// The Type of the class this structure is nested in
        typedef ReadSEGYModel Type;
    };

    /*! The Constructor, uses the default options.
     * @param[in] piol_ The piol object.
     * @param[in] name_ The name of the file.
     * @param[in] obj_ A shared pointer for the object layer object.
     */
    ReadSEGYModel(
      std::shared_ptr<ExSeisPIOL> piol_,
      const std::string name_,
      std::shared_ptr<ObjectInterface> obj_);

    /*! The Constructor.
     * @param[in] piol_ The piol object.
     * @param[in] name_ The name of the file.
     * @param[in] opt   The ReadSEGYModel options.
     * @param[in] obj_ A shared pointer for the object layer object.
     */
    ReadSEGYModel(
      std::shared_ptr<ExSeisPIOL> piol_,
      const std::string name_,
      const ReadSEGYModel::Opt& opt,
      std::shared_ptr<ObjectInterface> obj_);

    std::vector<trace_t> readModel(
      const size_t offset,
      const size_t sz,
      const Uniray<size_t, llint, llint>& gather);

    std::vector<trace_t> readModel(
      const size_t sz,
      const size_t* offset,
      const Uniray<size_t, llint, llint>& gather);
};

}  // namespace PIOL

#endif  // EXSEISDAT_PIOL_FILE_READSEGYMODEL_HH
