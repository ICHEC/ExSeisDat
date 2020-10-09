////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The SEGY implementation of the \c Output_file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_FILE_OUTPUT_FILE_SEGY_HH
#define EXSEISDAT_PIOL_FILE_OUTPUT_FILE_SEGY_HH

#include "exseisdat/piol/configuration/ExSeisPIOL.hh"
#include "exseisdat/piol/file/Output_file.hh"
#include "exseisdat/piol/segy/utils.hh"

namespace exseis {
namespace piol {
inline namespace file {

/// @brief The SEG-Y implementation of the file layer
///
class Output_file_segy : public Output_file {
  public:
    /// @brief The SEG-Y options structure.
    ///
    struct Options {
        /// @brief One microsecond
        static constexpr double microsecond = 1e-6;

        /// @brief The increment factor to multiply inc by (default to SEG-Y
        ///        rev 1 standard definition)
        ///
        double sample_interval_factor = 1 * microsecond;

        Options() noexcept {};
    };

    /// @brief The SEGY-Object class constructor.
    ///
    /// @param[in] piol    This PIOL ptr is not modified but is used to
    ///                    instantiate another shared_ptr.
    /// @param[in] name    The name of the file associated with the
    ///                    instantiation.
    /// @param[in] options The SEGY-File options
    ///
    Output_file_segy(
        std::shared_ptr<ExSeisPIOL> piol,
        std::string name,
        const Output_file_segy::Options& options = {}) :
        Output_file(std::make_unique<Output_file_segy::Implementation>(
            std::move(piol), std::move(name), options))
    {
    }

    /// @brief The SEGY-Object class constructor.
    ///
    /// @param[in] io_driver The IO_driver object to perform IO with
    /// @param[in] piol      This PIOL ptr is not modified but is used to
    ///                      instantiate another shared_ptr.
    /// @param[in] name      The name of the file associated with the
    ///                      instantiation.
    /// @param[in] options   The SEGY-File options
    ///
    Output_file_segy(
        IO_driver io_driver,
        std::shared_ptr<ExSeisPIOL> piol,
        std::string name,
        const Output_file_segy::Options& options = {}) :
        Output_file(std::make_unique<Output_file_segy::Implementation>(
            std::move(io_driver), std::move(piol), std::move(name), options))
    {
    }


    /// @brief Polymorphic implementation for Output_file_segy
    class Implementation : public Output_file::Implementation {
      protected:
        /// @brief The PIOL object.
        std::shared_ptr<ExSeisPIOL> m_piol;

        /// @brief Store the file name for debugging purposes.
        std::string m_name;

        /// @brief The IO_driver to write to
        IO_driver m_io_driver;

        /// @brief The number of samples per trace.
        size_t m_samples_per_trace = 0;

        /// @brief The number of traces.
        size_t m_number_of_traces = 0;

        /// @brief Human readable text extracted from the file
        std::string m_text = "";

        /// @brief The increment between samples in a trace
        exseis::utils::Floating_point m_sample_interval = 0;

        /// @brief   Whether the number of samples (samples_per_trace) has been
        ///          set.
        /// @details This should only be set once!
        bool m_samples_per_trace_set = false;

        /// @brief Whether the expected file size has changed
        bool m_size_dirty = false;

        /// @brief Whether any header data has been changed
        bool m_file_header_dirty = true;

        /// @brief Whether any file data has been written
        bool m_file_dirty = false;

        /// @brief The increment factor
        double m_sample_interval_factor;

      public:
        /// @copydoc Output_file_segy::Output_file_segy(std::shared_ptr<ExSeisPIOL>, std::string, const Output_file_segy::Options&)
        Implementation(
            std::shared_ptr<ExSeisPIOL> piol,
            std::string name,
            const Output_file_segy::Options& options =
                Output_file_segy::Options());

        /// @copydoc Output_file_segy::Output_file_segy(IO_driver, std::shared_ptr<ExSeisPIOL>, std::string, const Output_file_segy::Options&)
        Implementation(
            IO_driver io_driver,
            std::shared_ptr<ExSeisPIOL> piol,
            std::string name,
            const Output_file_segy::Options& options = {});

        /// @copydoc Output_file::Implementation::~Implementation
        /// @details Destructor also flushes file header data.
        ~Implementation() override;

        const std::string& file_name() const override;

        void write_text(std::string text) override;

        void write_samples_per_trace(size_t samples_per_trace) override;

        void write_number_of_traces(size_t number_of_traces) override;

        void write_sample_interval(
            exseis::utils::Floating_point sample_interval) override;

        void write_metadata(
            size_t trace_offset,
            size_t number_of_traces,
            const Trace_metadata& trace_metadata,
            size_t skip = 0) override;

        void write_metadata() override;

        void write_data(
            size_t trace_offset,
            size_t number_of_traces,
            const exseis::utils::Trace_value* trace_data) override;

        void write_data() override;

        void write(
            size_t trace_offset,
            size_t number_of_traces,
            const exseis::utils::Trace_value* trace_data,
            const Trace_metadata& trace_metadata,
            size_t skip = 0) override;

        void write() override;

        void write_metadata_non_contiguous(
            size_t number_of_offsets,
            const size_t* trace_offsets,
            const Trace_metadata& trace_metadata,
            size_t skip = 0) override;

        void write_metadata_non_contiguous() override;

        void write_data_non_contiguous(
            size_t number_of_offsets,
            const size_t* trace_offsets,
            const exseis::utils::Trace_value* trace_data) override;

        void write_data_non_contiguous() override;

        void write_non_contiguous(
            size_t number_of_offsets,
            const size_t* trace_offsets,
            const exseis::utils::Trace_value* trace,
            const Trace_metadata& trace_metadata,
            size_t skip = 0) override;

        void write_non_contiguous() override;

        std::vector<IO_driver> io_drivers() && override;

        void sync() override;
    };
};

}  // namespace file
}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_FILE_OUTPUT_FILE_SEGY_HH
