////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The \c Input_file class
/// @details The \c Input_file is a generic interface for reading
///          seismic data files.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEIS_PIOL_FILE_INPUT_FILE_HH
#define EXSEIS_PIOL_FILE_INPUT_FILE_HH

#include "exseis/piol/io_driver/IO_driver.hh"
#include "exseis/piol/metadata/Trace_metadata.hh"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace exseis {
inline namespace piol {
inline namespace file {

/*! @brief The \c Input_file class is a generic interface for reading
 *         seismic data files.
 */
class Input_file {
  public:
    class Implementation;

  protected:
    /// A pointer to an underlying implementation
    std::unique_ptr<Input_file::Implementation> m_implementation = nullptr;

  public:
    /// @brief Initialize Input_file from an underlying implementation
    /// @param[in] implementation An underlying implementation
    Input_file(std::unique_ptr<Input_file::Implementation> implementation) :
        m_implementation(std::move(implementation))
    {
    }

    /// @brief Get the name of the file.
    /// @return The name of the file.
    std::string file_name() const { return m_implementation->file_name(); }

    /// @brief Read the global file headers from the file.
    void read_file_headers() { m_implementation->read_file_headers(); }

    /*! @brief Read the human readable text from the file
     *  @return A string containing the text (in ASCII format)
     */
    const std::string& read_text() const
    {
        return m_implementation->read_text();
    }

    /*! @brief Read the number of samples per trace
     *  @return The number of samples per trace
     */
    size_t read_samples_per_trace() const
    {
        return m_implementation->read_samples_per_trace();
    }

    /*! @brief Read the number of traces in the file
     *  @return The number of traces
     */
    size_t read_number_of_traces() const
    {
        return m_implementation->read_number_of_traces();
    }

    /*! @brief Read the number of increment between trace samples
     *  @return The increment between trace samples
     */
    Floating_point read_sample_interval() const
    {
        return m_implementation->read_sample_interval();
    }

    /// @brief Read the trace parameters from trace_offset to
    ///        trace_offset+number_of_traces of the respective trace headers.
    /// @param[in] trace_offset The starting trace number.
    /// @param[in] number_of_traces The number of traces to process.
    /// @param[in] trace_metadata An array of the parameter structures
    ///                (size sizeof(Trace_metadata)*number_of_traces)
    /// @param[in] skip When reading, skip the first "skip" entries of trace_metadata
    ///
    void read_metadata(
        size_t trace_offset,
        size_t number_of_traces,
        Trace_metadata& trace_metadata,
        size_t skip = 0) const
    {
        m_implementation->read_metadata(
            trace_offset, number_of_traces, trace_metadata, skip);
    }

    /// @brief Empty call for collective operations
    void read_metadata() const { m_implementation->read_metadata(); }

    /*! @brief Read the traces from trace_offset to
     *         trace_offset+number_of_traces
     *  @param[in] trace_offset The starting trace number.
     *  @param[in] number_of_traces The number of traces to process.
     *  @param[out] trace_data The array of traces to fill from the file
     */
    void read_data(
        size_t trace_offset,
        size_t number_of_traces,
        Trace_value* trace_data) const
    {
        m_implementation->read_data(trace_offset, number_of_traces, trace_data);
    }

    /// @brief Empty call for collective operations
    void read_data() const { m_implementation->read_data(); }

    /*! @brief Read the traces from trace_offset to
     *         trace_offset+number_of_traces
     *  @param[in] trace_offset The starting trace number.
     *  @param[in] number_of_traces The number of traces to process.
     *  @param[out] trace_data The array of traces to fill from the file
     *  @param[out] trace_metadata A contiguous array of the parameter structures
     *                  (size sizeof(Trace_metadata)*number_of_traces)
     *  @param[in] skip When reading, skip the first "skip" entries of trace_metadata
     */
    void read(
        size_t trace_offset,
        size_t number_of_traces,
        Trace_value* trace_data,
        Trace_metadata& trace_metadata,
        size_t skip = 0) const
    {
        m_implementation->read(
            trace_offset, number_of_traces, trace_data, trace_metadata, skip);
    }

    /// @brief Empty call for collective operations
    void read() const { m_implementation->read(); }


    /*! @brief Read the traces specified by the trace_offsets in the passed
     *         trace_offset array.
     *  @param[in] number_of_offsets The number of traces to process
     *  @param[in] trace_offsets An array of trace numbers to read.
     *  @param[out] trace_metadata A parameter structure
     *  @param[in] skip When reading, skip the first "skip" entries of trace_metadata
     */
    void read_metadata_non_contiguous(
        size_t number_of_offsets,
        const size_t* trace_offsets,
        Trace_metadata& trace_metadata,
        size_t skip = 0) const
    {
        m_implementation->read_metadata_non_contiguous(
            number_of_offsets, trace_offsets, trace_metadata, skip);
    }

    /// @brief Empty call for collective operations
    void read_metadata_non_contiguous() const
    {
        m_implementation->read_metadata_non_contiguous();
    }

    /*! @brief Read the traces specified by the trace_offsets in the passed
     *         trace_offset array. Assumes Monotonic.
     *  @param[in] number_of_offsets The number of traces to process
     *  @param[in] trace_offsets An array of trace numbers to read (monotonic list).
     *  @param[out] trace_data A contiguous array of each trace
     *                    (size
     * number_of_traces*samples_per_trace*sizeof(Trace_value))
     */
    void read_data_non_contiguous(
        size_t number_of_offsets,
        const size_t* trace_offsets,
        Trace_value* trace_data) const
    {
        m_implementation->read_data_non_contiguous(
            number_of_offsets, trace_offsets, trace_data);
    }

    /// @brief Empty call for collective operations
    void read_data_non_contiguous() const
    {
        m_implementation->read_data_non_contiguous();
    }

    /*! @brief Read the traces specified by the trace_offsets in the passed
     *         trace_offset array. Assumes Monotonic.
     *  @param[in] number_of_offsets The number of traces to process
     *  @param[in] trace_offsets An array of trace numbers to read (monotonic list).
     *  @param[out] trace_data A contiguous array of each trace
     *                    (size
     * number_of_traces*samples_per_trace*sizeof(Trace_value))
     *  @param[out] trace_metadata A parameter structure
     *  @param[in] skip When reading, skip the first "skip" entries of trace_metadata
     */
    void read_non_contiguous(
        size_t number_of_offsets,
        const size_t* trace_offsets,
        Trace_value* trace_data,
        Trace_metadata& trace_metadata,
        size_t skip = 0) const
    {
        m_implementation->read_non_contiguous(
            number_of_offsets, trace_offsets, trace_data, trace_metadata, skip);
    }

    /// @brief Empty call for collective operations
    void read_non_contiguous() const
    {
        m_implementation->read_non_contiguous();
    }

    /*! @brief Read the traces specified by the trace_offsets in the passed
     * trace_offset array. Does not assume monotonic
     *  @param[in] number_of_offsets The number of traces to process
     *  @param[in] trace_offsets An array of trace numbers to read
     *                           (non-monotonic list).
     *  @param[out] trace_data A contiguous array of each trace
     *                    (size
     * number_of_traces*samples_per_trace*sizeof(Trace_value))
     *  @param[out] trace_metadata A parameter structure
     *  @param[in] skip When reading, skip the first "skip" entries of trace_metadata
     */
    void read_non_monotonic(
        size_t number_of_offsets,
        const size_t* trace_offsets,
        Trace_value* trace_data,
        Trace_metadata& trace_metadata,
        size_t skip = 0) const
    {
        m_implementation->read_non_monotonic(
            number_of_offsets, trace_offsets, trace_data, trace_metadata, skip);
    }

    /// @brief Empty call for collective operations
    void read_non_monotonic() const { m_implementation->read_non_monotonic(); }

    /// @brief Destructively returns the underlying IO_driver(s)
    /// @returns The underlying IO_driver(s)
    std::vector<IO_driver> io_drivers() &&
    {
        std::unique_ptr<Input_file::Implementation> impl =
            std::move(m_implementation);
        return std::move(*impl).io_drivers();
    }

    /// @brief Destructively return the Implementation
    /// @returns The Implementation
    std::unique_ptr<Input_file::Implementation> implementation() &&
    {
        return std::move(m_implementation);
    }

    /// @brief Polymorphic implementation for Input_file
    class Implementation {
      public:
        /// @virtual_destructor
        virtual ~Implementation() = default;

        /// @copydoc Input_file::file_name
        virtual std::string file_name() const = 0;

        /// @copydoc Input_file::read_file_headers
        virtual void read_file_headers() = 0;

        /// @copydoc Input_file::read_text
        virtual const std::string& read_text() const = 0;

        /// @copydoc Input_file::read_samples_per_trace
        virtual size_t read_samples_per_trace() const = 0;

        /// @copydoc Input_file::read_number_of_traces
        virtual size_t read_number_of_traces() const = 0;

        /// @copydoc Input_file::read_sample_interval
        virtual Floating_point read_sample_interval() const = 0;

        /// @copydoc Input_file::read_metadata
        virtual void read_metadata(
            size_t trace_offset,
            size_t number_of_traces,
            Trace_metadata& trace_metadata,
            size_t skip = 0) const = 0;

        /// @copydoc Input_file::read_metadata() const
        virtual void read_metadata() const = 0;

        /// @copydoc Input_file::read_data
        virtual void read_data(
            size_t trace_offset,
            size_t number_of_traces,
            Trace_value* trace_data) const = 0;

        /// @copydoc Input_file::read_data() const
        virtual void read_data() const = 0;

        /// @copydoc Input_file::read
        virtual void read(
            size_t trace_offset,
            size_t number_of_traces,
            Trace_value* trace_data,
            Trace_metadata& trace_metadata,
            size_t skip = 0) const = 0;

        /// @copydoc Input_file::read() const
        virtual void read() const = 0;

        /// @copydoc Input_file::read_metadata_non_contiguous
        virtual void read_metadata_non_contiguous(
            size_t number_of_offsets,
            const size_t* trace_offsets,
            Trace_metadata& trace_metadata,
            size_t skip = 0) const = 0;

        /// @copydoc Input_file::read_metadata_non_contiguous() const
        virtual void read_metadata_non_contiguous() const = 0;

        /// @copydoc Input_file::read_data_non_contiguous
        virtual void read_data_non_contiguous(
            size_t number_of_offsets,
            const size_t* trace_offsets,
            Trace_value* trace_data) const = 0;

        /// @copydoc Input_file::read_data_non_contiguous() const
        virtual void read_data_non_contiguous() const = 0;

        /// @copydoc Input_file::read_non_contiguous
        virtual void read_non_contiguous(
            size_t number_of_offsets,
            const size_t* trace_offsets,
            Trace_value* trace_data,
            Trace_metadata& trace_metadata,
            size_t skip = 0) const = 0;

        /// @copydoc Input_file::read_non_contiguous() const
        virtual void read_non_contiguous() const = 0;

        /// @copydoc Input_file::read_non_monotonic
        virtual void read_non_monotonic(
            size_t number_of_offsets,
            const size_t* trace_offsets,
            Trace_value* trace_data,
            Trace_metadata& trace_metadata,
            size_t skip = 0) const = 0;

        /// @copydoc Input_file::read_non_monotonic() const
        virtual void read_non_monotonic() const = 0;

        /// @copydoc Input_file::io_drivers
        virtual std::vector<IO_driver> io_drivers() && = 0;
    };
};

}  // namespace file
}  // namespace piol
}  // namespace exseis

#endif  // EXSEIS_PIOL_FILE_INPUT_FILE_HH
