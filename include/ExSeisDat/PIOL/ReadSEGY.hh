////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The SEGY implementation for \c ReadInterface
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_READSEGY_HH
#define EXSEISDAT_PIOL_READSEGY_HH

#include "ExSeisDat/PIOL/ReadInterface.hh"
#include "ExSeisDat/PIOL/segy_utils.hh"

#include <cstdint>


namespace exseis {
namespace PIOL {

/*! The SEG-Y implementation of the file layer
 */
class ReadSEGY : public ReadInterface {
  public:
    /*! @brief The SEG-Y options structure.
     */
    struct Opt {
        /// The Type of the class this structure is nested in
        typedef ReadSEGY Type;

        /// The increment factor to multiply inc by (default to SEG-Y rev 1
        /// standard definition)
        double incFactor;

        /*! Constructor which provides the default Rules
         */
        Opt(void);
    };

  private:
    /// Type formats
    SEGY_utils::SEGYNumberFormat number_format =
      SEGY_utils::SEGYNumberFormat::IEEE;

    /// The increment factor
    double incFactor;

  public:
    /*! @brief The SEGY-Object class constructor.
     *  @param[in] piol_   This PIOL ptr is not modified but is used to
     *                     instantiate another shared_ptr.
     *  @param[in] name_   The name of the file associated with the
     *                     instantiation.
     *  @param[in] segyOpt The SEGY-File options
     *  @param[in] obj_    A shared pointer to the object layer
     */
    ReadSEGY(
      std::shared_ptr<ExSeisPIOL> piol_,
      std::string name_,
      const ReadSEGY::Opt& segyOpt,
      std::shared_ptr<ObjectInterface> obj_);

    /*! @brief The SEGY-Object class constructor.
     *  @param[in] piol_   This PIOL ptr is not modified but is used to
     *                     instantiate another shared_ptr.
     *  @param[in] name_   The name of the file associated with the
     *                     instantiation.
     *  @param[in] obj_    A shared pointer to the object layer
     */
    ReadSEGY(
      std::shared_ptr<ExSeisPIOL> piol_,
      std::string name_,
      std::shared_ptr<ObjectInterface> obj_);

    size_t readNt() const;

    void readTrace(
      size_t offset,
      size_t sz,
      exseis::utils::Trace_value* trace,
      Param* prm  = PIOL_PARAM_NULL,
      size_t skip = 0) const;

    void readTraceNonContiguous(
      size_t sz,
      const size_t* offset,
      exseis::utils::Trace_value* trace,
      Param* prm  = PIOL_PARAM_NULL,
      size_t skip = 0) const;

    void readTraceNonMonotonic(
      size_t sz,
      const size_t* offset,
      exseis::utils::Trace_value* trace,
      Param* prm  = PIOL_PARAM_NULL,
      size_t skip = 0) const;
};

}  // namespace PIOL
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_READSEGY_HH
