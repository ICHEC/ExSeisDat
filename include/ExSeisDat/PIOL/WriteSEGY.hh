////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The SEGY implementation of the \c WriteInterface
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_FILE_WRITESEGY_HH
#define EXSEISDAT_PIOL_FILE_WRITESEGY_HH

#include "ExSeisDat/PIOL/WriteInterface.hh"
#include "ExSeisDat/PIOL/segy_utils.hh"

namespace PIOL {

/*! The SEG-Y implementation of the file layer
 */
class WriteSEGY : public WriteInterface {
  public:
    /*! @brief The SEG-Y options structure.
     */
    struct Opt {
        /// The Type of the class this structure is nested in
        typedef WriteSEGY Type;
        /// The increment factor to multiply inc by (default to SEG-Y rev 1
        /// standard definition)
        unit_t incFactor;

        /*! Constructor which provides the default Rules
         */
        Opt(void);
    };

  private:
    /// Whether the number of samples (ns) has been set.
    /// If it hasn't been set before file writing, the output is likely garbage.
    bool nsSet = false;

    /*! State flags structure for SEGY
     */
    struct Flags {
        /// The header should be written before SEGY object is deleted
        bool writeHO = false;

        /// The file should be resized before SEGY object is deleted.
        bool resize = false;

        /// The nt value is stale and should be resynced.
        bool stalent = false;
    };

    /// State flags are stored in this structure
    Flags state;

    /// The increment factor
    unit_t incFactor;

    /*! Calculate the number of traces currently stored (or implied to exist).
     *  @return Return the number of traces
     */
    size_t calcNt(void);

  public:
    /*! @brief The SEGY-Object class constructor.
     *  @param[in] piol_   This PIOL ptr is not modified but is used to
     *                     instantiate another shared_ptr.
     *  @param[in] name_   The name of the file associated with the
     *                     instantiation.
     *  @param[in] segyOpt The SEGY-File options
     *  @param[in] obj_    A shared pointer to the object layer
     */
    WriteSEGY(
      std::shared_ptr<ExSeisPIOL> piol_,
      const std::string name_,
      const WriteSEGY::Opt& segyOpt,
      std::shared_ptr<ObjectInterface> obj_);

    /*! @brief The SEGY-Object class constructor.
     *  @param[in] piol_   This PIOL ptr is not modified but is used to
     *                     instantiate another shared_ptr.
     *  @param[in] name_   The name of the file associated with the
     *                     instantiation.
     *  @param[in] obj_    A shared pointer to the object layer
     */
    WriteSEGY(
      std::shared_ptr<ExSeisPIOL> piol_,
      const std::string name_,
      std::shared_ptr<ObjectInterface> obj_);

    /*! @brief Destructor. Processes any remaining flags
     */
    ~WriteSEGY(void);

    void writeText(const std::string text_);

    void writeNs(const size_t ns_);

    void writeNt(const size_t nt_);

    void writeInc(const geom_t inc_);

    void writeTrace(
      const size_t offset,
      const size_t sz,
      trace_t* trace,
      const Param* prm  = PIOL_PARAM_NULL,
      const size_t skip = 0);

    void writeTraceNonContiguous(
      const size_t sz,
      const size_t* offset,
      trace_t* trace,
      const Param* prm  = PIOL_PARAM_NULL,
      const size_t skip = 0);
};

}  // namespace PIOL

#endif  // EXSEISDAT_PIOL_FILE_WRITESEGY_HH
