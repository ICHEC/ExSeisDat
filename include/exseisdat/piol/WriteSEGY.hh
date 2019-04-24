////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The SEGY implementation of the \c WriteInterface
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_WRITESEGY_HH
#define EXSEISDAT_PIOL_WRITESEGY_HH

#include "exseisdat/piol/WriteInterface.hh"
#include "exseisdat/piol/segy/utils.hh"

namespace exseis {
namespace piol {

/*! The SEG-Y implementation of the file layer
 */
class WriteSEGY : public WriteInterface {
  protected:
    /// The PIOL object.
    std::shared_ptr<ExSeisPIOL> m_piol;

    /// Store the file name for debugging purposes.
    std::string m_name;

    /// Pointer to the Object-layer object (polymorphic).
    std::shared_ptr<ObjectInterface> m_obj;

    /// The number of samples per trace.
    size_t m_ns = 0;

    /// The number of traces.
    size_t m_nt = 0;

    /// Human readable text extracted from the file
    std::string m_text = "";

    /// The increment between samples in a trace
    exseis::utils::Floating_point m_sample_interval = 0;

    /// Whether the number of samples (ns) has been set.
    /// If it hasn't been set before file writing, the output is likely garbage.
    bool m_is_ns_set = false;

    /*! State flags structure for SEGY
     */
    struct Flags {
        /// The header should be written before SEGY object is deleted
        bool should_write_file_header = true;

        /// The file should be resized before SEGY object is deleted.
        bool resize = false;

        /// The nt value is stale and should be resynced.
        bool stalent = false;
    };

    /// State flags are stored in this structure
    Flags m_state = {};

    /// The increment factor
    double m_sample_interval_factor;

    /*! Calculate the number of traces currently stored (or implied to exist).
     *  @return Return the number of traces
     */
    size_t calc_nt();

  public:
    /*! @brief The SEG-Y options structure.
     */
    struct Options {
        /// One microsecond
        static constexpr double microsecond = 1e-6;

        /// The increment factor to multiply inc by (default to SEG-Y rev 1
        /// standard definition)
        double sample_interval_factor = 1 * microsecond;

        /// A default constructor
        Options() {}
    };

    /*! @brief The SEGY-Object class constructor.
     *  @param[in] piol    This PIOL ptr is not modified but is used to
     *                     instantiate another shared_ptr.
     *  @param[in] name    The name of the file associated with the
     *                     instantiation.
     *  @param[in] options The SEGY-File options
     */
    WriteSEGY(
        std::shared_ptr<ExSeisPIOL> piol,
        std::string name,
        const WriteSEGY::Options& options = WriteSEGY::Options());

  protected:
    /*! @brief The SEGY-Object class constructor.
     *  @param[in] piol    This PIOL ptr is not modified but is used to
     *                     instantiate another shared_ptr.
     *  @param[in] name    The name of the file associated with the
     *                     instantiation.
     *  @param[in] options The SEGY-File options
     *  @param[in] obj     The object layer object to use.
     */
    WriteSEGY(
        std::shared_ptr<ExSeisPIOL> piol,
        std::string name,
        const WriteSEGY::Options& options,
        std::shared_ptr<ObjectInterface> obj);

  public:
    /*! @brief Destructor. Processes any remaining flags
     */
    ~WriteSEGY();

    /*! @brief Flush. Processes any remaining flags
     */
    void flush();

    const std::string& file_name() const override;

    void write_text(std::string text) override;

    void write_ns(size_t ns) override;

    void write_nt(size_t nt) override;

    void write_sample_interval(
        exseis::utils::Floating_point sample_interval) override;

    void write_param(
        size_t offset,
        size_t sz,
        const Trace_metadata* prm,
        size_t skip = 0) override;

    void write_param_non_contiguous(
        size_t sz,
        const size_t* offset,
        const Trace_metadata* prm,
        size_t skip = 0) override;

    void write_trace(
        size_t offset,
        size_t sz,
        exseis::utils::Trace_value* trace,
        const Trace_metadata* prm = nullptr,
        size_t skip               = 0) override;

    void write_trace_non_contiguous(
        size_t sz,
        const size_t* offset,
        exseis::utils::Trace_value* trace,
        const Trace_metadata* prm = nullptr,
        size_t skip               = 0) override;
};

}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_WRITESEGY_HH
