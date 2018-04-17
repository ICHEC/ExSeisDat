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
        unit_t incFactor;

        /*! Constructor which provides the default Rules
         */
        Opt(void);
    };

  private:
    /// Type formats
    SEGY_utils::SEGYNumberFormat number_format =
      SEGY_utils::SEGYNumberFormat::IEEE;

    /// The increment factor
    unit_t incFactor;

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
      const std::string name_,
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
      const std::string name_,
      std::shared_ptr<ObjectInterface> obj_);

    size_t readNt(void) const;

    void readTrace(
      const size_t offset,
      const size_t sz,
      trace_t* trace,
      Param* prm        = PIOL_PARAM_NULL,
      const size_t skip = 0) const;

    void readTraceNonContiguous(
      const size_t sz,
      const size_t* offset,
      trace_t* trace,
      Param* prm        = PIOL_PARAM_NULL,
      const size_t skip = 0) const;

    void readTraceNonMonotonic(
      const size_t sz,
      const size_t* offset,
      trace_t* trace,
      Param* prm        = PIOL_PARAM_NULL,
      const size_t skip = 0) const;
};

}  // namespace PIOL
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_READSEGY_HH
