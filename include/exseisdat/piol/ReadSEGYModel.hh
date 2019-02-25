////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The \c ReadSEGYModel class reads SEGY velocity models.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_READSEGYMODEL_HH
#define EXSEISDAT_PIOL_READSEGYMODEL_HH

#include "exseisdat/piol/Model3dInterface.hh"
#include "exseisdat/piol/ReadSEGY.hh"
#include "exseisdat/utils/Distributed_vector.hh"

namespace exseis {
namespace piol {

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
    using Options = ReadSEGY::Options;

    /*! The Constructor.
     * @param[in] piol   The piol object.
     * @param[in] name   The name of the file.
     * @param[in] options The ReadSEGYModel options.
     */
    ReadSEGYModel(
        std::shared_ptr<ExSeisPIOL> piol,
        std::string name,
        const ReadSEGYModel::Options& options = ReadSEGYModel::Options());

    std::vector<exseis::utils::Trace_value> read_model(
        size_t offset,
        size_t sz,
        const exseis::utils::Distributed_vector<Gather_info>& gather) override;

    std::vector<exseis::utils::Trace_value> read_model(
        size_t sz,
        const size_t* offset,
        const exseis::utils::Distributed_vector<Gather_info>& gather) override;
};

}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_READSEGYMODEL_HH
