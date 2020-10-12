////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The \c Output_file class
/// @details The \c Output_file is a generic interface for writing
///          seismic data files.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEIS_PIOL_FILE_OUTPUT_FILE_HH
#define EXSEIS_PIOL_FILE_OUTPUT_FILE_HH

#include "exseis/piol/io_driver/IO_driver.hh"
#include "exseis/piol/metadata/Trace_metadata.hh"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace exseis {
inline namespace piol {
inline namespace file {

/*! @brief The \c Output_file class is a generic interface for writing
 *         seismic data files.
 */
class Output_file {
  public:
    class Implementation;

  protected:
    /// A pointer to an underlying implementation
    std::unique_ptr<Implementation> m_implementation = nullptr;

  public:
    /// @brief Initialize Output_file from an underlying implementation
    /// @param[in] implementation An underlying implementation
    Output_file(std::unique_ptr<Implementation> implementation) :
        m_implementation(std::move(implementation))
    {
    }

    /// @brief Get the name of the file.
    /// @return The name of the file.
    std::string file_name() const { return m_implementation->file_name(); }

    /*! @brief Write the human readable text from the file.
     *  @param[in] text The new string containing the text (in ASCII format).
     */
    void write_text(std::string text) { m_implementation->write_text(text); }

    /*! @brief Write the number of samples per trace
     *  @param[in] samples_per_trace The new number of samples per trace.
     */
    void write_samples_per_trace(size_t samples_per_trace)
    {
        m_implementation->write_samples_per_trace(samples_per_trace);
    }

    /*! @brief Write the number of traces in the file
     *  @param[in] number_of_traces The new number of traces.
     */
    void write_number_of_traces(size_t number_of_traces)
    {
        m_implementation->write_number_of_traces(number_of_traces);
    }

    /*! @brief Write the number of increment between trace samples.
     *  @param[in] sample_interval The new increment between trace samples.
     */
    void write_sample_interval(Floating_point sample_interval)
    {
        m_implementation->write_sample_interval(sample_interval);
    }

    /*! @brief Write the trace parameters from trace_offset to
     * trace_offset+number_of_traces to the respective trace headers.
     *  @param[in] trace_offset The starting trace number.
     *  @param[in] number_of_traces     The number of traces to process.
     *  @param[in] trace_metadata An array of the parameter structures
     *                            (size sizeof(Trace_metadata)*number_of_traces)
     *  @param[in] skip   When writing, skip the first "skip" entries of trace_metadata
     *
     *  @details It is assumed that this operation is not an update. Any
     *           previous contents of the trace header will be overwritten.
     */
    void write_metadata(
        size_t trace_offset,
        size_t number_of_traces,
        const Trace_metadata& trace_metadata,
        size_t skip = 0)
    {
        m_implementation->write_metadata(
            trace_offset, number_of_traces, trace_metadata, skip);
    }

    /// @brief Empty call for collective operations
    void write_metadata() { m_implementation->write_metadata(); }

    /*! @brief Write the traces from trace_offset to
     * trace_offset+number_of_traces
     *  @param[in] trace_offset The starting trace number.
     *  @param[in] number_of_traces     The number of traces to process.
     *  @param[in] trace_data  The array of traces to write to the file
     */
    void write_data(
        size_t trace_offset,
        size_t number_of_traces,
        const Trace_value* trace_data)
    {
        m_implementation->write_data(
            trace_offset, number_of_traces, trace_data);
    }

    /// @brief Empty call for collective operations
    void write_data() { m_implementation->write_data(); }

    /*! @brief Write the traces from trace_offset to
     * trace_offset+number_of_traces
     *  @param[in] trace_offset The starting trace number.
     *  @param[in] number_of_traces     The number of traces to process.
     *  @param[in] trace_data  The array of traces to write to the file
     *  @param[in] trace_metadata A contiguous array of the parameter structures
     *                            (size sizeof(Trace_metadata)*number_of_traces)
     *  @param[in] skip   When writing, skip the first "skip" entries of trace_metadata
     */
    void write(
        size_t trace_offset,
        size_t number_of_traces,
        const Trace_value* trace_data,
        const Trace_metadata& trace_metadata,
        size_t skip = 0)
    {
        m_implementation->write(
            trace_offset, number_of_traces, trace_data, trace_metadata, skip);
    }

    /// @brief Empty call for collective operations
    void write() { m_implementation->write(); }

    /*! @brief Write the parameters specified by the trace_offsets in the passed
     *         trace_offset array.
     *  @param[in] number_of_offsets  The number of traces to process
     *  @param[in] trace_offsets   An array of trace numbers to write.
     *  @param[in] trace_metadata  A parameter structure
     *  @param[in] skip            When writing, skip the first "skip" entries
     *                             of trace_metadata
     */
    void write_metadata_non_contiguous(
        size_t number_of_offsets,
        const size_t* trace_offsets,
        const Trace_metadata& trace_metadata,
        size_t skip = 0)
    {
        m_implementation->write_metadata_non_contiguous(
            number_of_offsets, trace_offsets, trace_metadata, skip);
    }

    /// @brief Empty call for collective operations
    void write_metadata_non_contiguous()
    {
        m_implementation->write_metadata_non_contiguous();
    }

    /*! @brief Write the traces specified by the trace_offsets in the passed
     * trace_offset array.
     *  @param[in] number_of_offsets  The number of traces to process
     *  @param[in] trace_offsets      An array of trace numbers to write.
     *  @param[in] trace_data  A contiguous array of each trace
     *                    (size number_of_offsets*samples_per_trace
     *                           *sizeof(Trace_value))
     */
    void write_data_non_contiguous(
        size_t number_of_offsets,
        const size_t* trace_offsets,
        const Trace_value* trace_data)
    {
        m_implementation->write_data_non_contiguous(
            number_of_offsets, trace_offsets, trace_data);
    }

    /// @brief Empty call for collective operations
    void write_data_non_contiguous()
    {
        m_implementation->write_data_non_contiguous();
    }

    /*! @brief Write the traces specified by the trace_offsets in the passed
     * trace_offset array.
     *  @param[in] number_of_offsets  The number of traces to process
     *  @param[in] trace_offsets      An array of trace numbers to write.
     *  @param[in] trace_data  A contiguous array of each trace
     *                    (size number_of_offsets*samples_per_trace
     *                           *sizeof(Trace_value))
     *  @param[in] trace_metadata  A parameter structure
     *  @param[in] skip            When writing, skip the first "skip" entries
     *                             of trace_metadata
     */
    void write_non_contiguous(
        size_t number_of_offsets,
        const size_t* trace_offsets,
        const Trace_value* trace_data,
        const Trace_metadata& trace_metadata,
        size_t skip = 0)
    {
        m_implementation->write_non_contiguous(
            number_of_offsets, trace_offsets, trace_data, trace_metadata, skip);
    }

    /// @brief Empty call for collective operations
    void write_non_contiguous() { m_implementation->write_non_contiguous(); }

    /// @brief Destructively access the underlying IO_driver object(s)
    /// @return The underlying IO_driver object(s)
    std::vector<IO_driver> io_drivers() &&
    {
        auto impl = std::move(m_implementation);
        return std::move(*impl).io_drivers();
    }

    /// @brief Ensure written data is synced with the IO_driver
    void sync() { m_implementation->sync(); }


    /// @brief Polymorphic implementation for Output_file
    class Implementation {
      public:
        /// @virtual_destructor
        virtual ~Implementation() = default;

        /// @copydoc Output_file::file_name
        virtual std::string file_name() const = 0;

        /// @copydoc Output_file::write_text
        virtual void write_text(std::string text) = 0;

        /// @copydoc Output_file::write_samples_per_trace
        virtual void write_samples_per_trace(size_t samples_per_trace) = 0;

        /// @copydoc Output_file::write_number_of_traces
        virtual void write_number_of_traces(size_t number_of_traces) = 0;

        /// @copydoc Output_file::write_sample_interval
        virtual void write_sample_interval(Floating_point sample_interval) = 0;

        /// @copydoc Output_file::write_metadata
        virtual void write_metadata(
            size_t trace_offset,
            size_t number_of_traces,
            const Trace_metadata& trace_metadata,
            size_t skip = 0) = 0;

        /// @copydoc Output_file::write_metadata()
        virtual void write_metadata() = 0;

        /// @copydoc Output_file::write_data
        virtual void write_data(
            size_t trace_offset,
            size_t number_of_traces,
            const Trace_value* trace_data) = 0;

        /// @copydoc Output_file::write_data()
        virtual void write_data() = 0;

        /// @copydoc Output_file::write
        virtual void write(
            size_t trace_offset,
            size_t number_of_traces,
            const Trace_value* trace_data,
            const Trace_metadata& trace_metadata,
            size_t skip = 0) = 0;

        /// @copydoc Output_file::write()
        virtual void write() = 0;

        /// @copydoc Output_file::write_metadata_non_contiguous
        virtual void write_metadata_non_contiguous(
            size_t number_of_offsets,
            const size_t* trace_offsets,
            const Trace_metadata& trace_metadata,
            size_t skip = 0) = 0;

        /// @copydoc Output_file::write_metadata_non_contiguous()
        virtual void write_metadata_non_contiguous() = 0;

        /// @copydoc Output_file::write_data_non_contiguous
        virtual void write_data_non_contiguous(
            size_t number_of_offsets,
            const size_t* trace_offsets,
            const Trace_value* trace_data) = 0;

        /// @copydoc Output_file::write_data_non_contiguous()
        virtual void write_data_non_contiguous() = 0;

        /// @copydoc Output_file::write_non_contiguous
        virtual void write_non_contiguous(
            size_t number_of_offsets,
            const size_t* trace_offsets,
            const Trace_value* trace_data,
            const Trace_metadata& trace_metadata,
            size_t skip = 0) = 0;

        /// @copydoc Output_file::write_non_contiguous()
        virtual void write_non_contiguous() = 0;

        /// @copydoc Output_file::io_drivers
        virtual std::vector<IO_driver> io_drivers() && = 0;

        /// @copydoc Output_file::sync
        virtual void sync() = 0;
    };
};

}  // namespace file
}  // namespace piol
}  // namespace exseis

#endif  // EXSEIS_PIOL_FILE_OUTPUT_FILE_HH
