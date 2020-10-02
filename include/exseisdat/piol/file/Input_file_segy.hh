////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The SEGY implementation for \c Input_file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_FILE_INPUT_FILE_SEGY_HH
#define EXSEISDAT_PIOL_FILE_INPUT_FILE_SEGY_HH

#include "exseisdat/piol/file/Input_file.hh"
#include "exseisdat/piol/file/detail/ObjectSEGY.hh"
#include "exseisdat/piol/segy/utils.hh"

#include <cstdint>


namespace exseis {
namespace piol {
inline namespace file {

/// @brief The SEG-Y implementation of the file layer
///
class Input_file_segy : public Input_file {
  protected:
    /// The PIOL object.
    std::shared_ptr<ExSeisPIOL> m_piol;

    /// Store the file name for debugging purposes.
    std::string m_name = "";

    /// Pointer to the Object-layer object (polymorphic).
    std::shared_ptr<ObjectInterface> m_obj;

    /// The number of samples per trace.
    size_t m_ns = 0;

    /// The number of traces.
    size_t m_nt = 0;

    /// Human readable text extracted from the file
    std::string m_text = "";

    /// The interval between samples in a trace
    exseis::utils::Floating_point m_sample_interval = 0;

    /// Type formats
    segy::Segy_number_format m_number_format =
        segy::Segy_number_format::IEEE_fp32;

    /// The interval factor
    double m_sample_interval_factor;


  public:
    /// @brief The SEG-Y options structure.
    ///
    struct Options {
        /// One microsecond
        static constexpr double microsecond = 1e-6;

        /// The increment factor to multiply inc by (default to SEG-Y rev 1
        /// standard definition)
        double sample_interval_factor = 1 * microsecond;

        /// A default constructor
        Options() {}
    };

    /// @brief The SEGY-Object class constructor.
    ///
    /// @param[in] piol    This PIOL ptr is not modified but is used to
    ///                    instantiate another shared_ptr.
    /// @param[in] name    The name of the file associated with the
    ///                    instantiation.
    /// @param[in] options The SEGY-File options
    ///
    Input_file_segy(
        std::shared_ptr<ExSeisPIOL> piol,
        std::string name,
        const Input_file_segy::Options& options = {});

    /// @brief The SEGY-Object class constructor.
    ///
    /// @param[in] io_driver The IO_driver object to perform IO with
    /// @param[in] piol      This PIOL ptr is not modified but is used to
    ///                      instantiate another shared_ptr.
    /// @param[in] name      The name of the file associated with the
    ///                      instantiation.
    /// @param[in] options   The SEGY-File options
    ///
    Input_file_segy(
        IO_driver io_driver,
        std::shared_ptr<ExSeisPIOL> piol,
        std::string name,
        const Input_file_segy::Options& options = {});

  protected:
    /// @brief The SEGY-Object class constructor.
    ///
    /// @param[in] piol   This PIOL ptr is not modified but is used to
    ///                   instantiate another shared_ptr.
    /// @param[in] name   The name of the file associated with the
    ///                   instantiation.
    /// @param[in] options The SEGY-File options
    /// @param[in] object  A shared pointer to the object layer
    ///
    Input_file_segy(
        std::shared_ptr<ExSeisPIOL> piol,
        std::string name,
        const Input_file_segy::Options& options,
        std::shared_ptr<ObjectInterface> object);

  public:
    const std::string& file_name() const override;

    void read_file_headers() override;

    const std::string& read_text() const override;

    size_t read_ns() const override;

    size_t read_nt() const override;

    exseis::utils::Floating_point read_sample_interval() const override;

    void read_param(
        size_t offset,
        size_t sz,
        Trace_metadata* prm,
        size_t skip = 0) const override;

    void read_param_non_contiguous(
        size_t sz,
        const size_t* offset,
        Trace_metadata* prm,
        size_t skip = 0) const override;

    void read_trace(
        size_t offset,
        size_t sz,
        exseis::utils::Trace_value* trace,
        Trace_metadata* prm = nullptr,
        size_t skip         = 0) const override;

    void read_trace_non_contiguous(
        size_t sz,
        const size_t* offset,
        exseis::utils::Trace_value* trace,
        Trace_metadata* prm = nullptr,
        size_t skip         = 0) const override;

    void read_trace_non_monotonic(
        size_t sz,
        const size_t* offset,
        exseis::utils::Trace_value* trace,
        Trace_metadata* prm = nullptr,
        size_t skip         = 0) const override;
};

}  // namespace file
}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_FILE_INPUT_FILE_SEGY_HH
