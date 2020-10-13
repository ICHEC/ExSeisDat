////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The SEGY implementation for \c Input_file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEIS_PIOL_FILE_INPUT_FILE_SEGY_HH
#define EXSEIS_PIOL_FILE_INPUT_FILE_SEGY_HH

#include "exseis/piol/file/Input_file.hh"
#include "exseis/piol/io_driver/IO_driver.hh"
#include "exseis/piol/parser/Blob_parser.hh"

#include "exseis/piol/segy/utils.hh"

#include <cstdint>
#include <memory>
#include <utility>


namespace exseis {
inline namespace piol {
inline namespace file {

/// @brief The SEG-Y implementation of the file layer
///
class Input_file_segy : public Input_file {
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
        Options() noexcept {};
    };

    /// @brief The SEGY-Object class constructor.
    ///
    /// @param[in] io_driver The IO_driver object to perform IO with
    /// @param[in] options   The SEGY-File options
    ///
    Input_file_segy(
        IO_driver io_driver, const Input_file_segy::Options& options = {}) :
        Input_file(std::make_unique<Input_file_segy::Implementation>(
            std::move(io_driver), options))
    {
    }


    /// @brief Polymorphic implementation for Input_file_segy
    class Implementation : public Input_file::Implementation {
        /// @brief The IO_driver to write to
        IO_driver m_io_driver;

        /// The number of samples per trace.
        size_t m_samples_per_trace = 0;

        /// The number of traces.
        size_t m_number_of_traces = 0;

        /// Human readable text extracted from the file
        std::string m_text = "";

        /// The interval between samples in a trace
        Floating_point m_sample_interval = 0;

        /// Type formats
        segy::Segy_number_format m_number_format =
            segy::Segy_number_format::IEEE_fp32;

        /// The interval factor
        double m_sample_interval_factor;

        /// Blob parsers for the trace headers
        std::map<size_t, std::unique_ptr<Blob_parser>> m_trace_header_parsers;


      public:
        /// @copydoc Input_file_segy::Input_file_segy(IO_driver, const Input_file_segy::Options&);
        Implementation(
            IO_driver io_driver, const Input_file_segy::Options& options = {});

        std::string file_name() const override;

        void read_file_headers() override;

        const std::string& read_text() const override;

        size_t read_samples_per_trace() const override;

        size_t read_number_of_traces() const override;

        Floating_point read_sample_interval() const override;

        std::map<Trace_metadata_key, Trace_metadata_info>
        trace_metadata_available() const override;

        void read_metadata(
            size_t trace_offset,
            size_t number_of_traces,
            Trace_metadata& trace_metadata,
            size_t skip = 0) const override;

        void read_metadata() const override;

        void read_data(
            size_t trace_offset,
            size_t number_of_traces,
            Trace_value* trace_data) const override;

        void read_data() const override;

        void read(
            size_t trace_offset,
            size_t number_of_traces,
            Trace_value* trace_data,
            Trace_metadata& trace_metadata,
            size_t skip = 0) const override;

        void read() const override;

        void read_metadata_non_contiguous(
            size_t number_of_offsets,
            const size_t* trace_offsets,
            Trace_metadata& trace_metadata,
            size_t skip = 0) const override;

        void read_metadata_non_contiguous() const override;

        void read_data_non_contiguous(
            size_t number_of_offsets,
            const size_t* trace_offsets,
            Trace_value* trace_data) const override;

        void read_data_non_contiguous() const override;

        void read_non_contiguous(
            size_t number_of_offsets,
            const size_t* trace_offsets,
            Trace_value* trace_data,
            Trace_metadata& trace_metadata,
            size_t skip = 0) const override;

        void read_non_contiguous() const override;

        void read_non_monotonic(
            size_t number_of_offsets,
            const size_t* trace_offsets,
            Trace_value* trace_data,
            Trace_metadata& trace_metadata,
            size_t skip = 0) const override;

        void read_non_monotonic() const override;

        std::vector<IO_driver> io_drivers() && override;
    };
};

}  // namespace file
}  // namespace piol
}  // namespace exseis

#endif  // EXSEIS_PIOL_FILE_INPUT_FILE_SEGY_HH
