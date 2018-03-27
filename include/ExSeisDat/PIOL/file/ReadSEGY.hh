////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The SEGY implementation for \c ReadInterface
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_FILE_READSEGY_HH
#define EXSEISDAT_PIOL_FILE_READSEGY_HH

#include "ExSeisDat/PIOL/file/ReadInterface.hh"

#include <cstdint>


namespace PIOL {
namespace File {

/// Data Format options
enum class Format : int16_t;

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
    Format format;

    /// The increment factor
    unit_t incFactor;

    /*! @brief Read the text and binary header and store the metadata variables
     *         in this SEGY object.
     *  @param[in] fsz The size of the file in bytes
     *  @param[in, out] buf The buffer to parse. The buffer is destructively
     *                      modified
     */
    void procHeader(const size_t fsz, uchar* buf);

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
      std::shared_ptr<Obj::Interface> obj_);

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
      std::shared_ptr<Obj::Interface> obj_);

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

}  // namespace File
}  // namespace PIOL

#endif  // EXSEISDAT_PIOL_FILE_READSEGY_HH
