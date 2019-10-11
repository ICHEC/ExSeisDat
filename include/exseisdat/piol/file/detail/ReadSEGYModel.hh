////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The \c ReadSEGYModel class reads SEGY velocity models.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_FILE_DETAIL_READSEGYMODEL_HH
#define EXSEISDAT_PIOL_FILE_DETAIL_READSEGYMODEL_HH

#include "exseisdat/piol/file/Input_file_segy.hh"
#include "exseisdat/piol/file/detail/Model3dInterface.hh"
#include "exseisdat/utils/distributed_vector/Distributed_vector.hh"

namespace exseis {
namespace piol {
inline namespace file {
inline namespace detail {

/*! @brief A SEGY class for seismic velocity models
 *
 * Velocity models indicate the velocity anomolies within a subsurface, which
 * indicate areas with high velocity anomolies correspond to denser material,
 * while lower velocities correspond to materials that are less dense.
 */
class ReadSEGYModel : public Model3dInterface, public Input_file_segy {
  public:
    /*! @brief The SEG-Y Model options structure.
     */
    using Options = Input_file_segy::Options;

    /*! @brief The Constructor.
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

}  // namespace detail
}  // namespace file
}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_FILE_DETAIL_READSEGYMODEL_HH
