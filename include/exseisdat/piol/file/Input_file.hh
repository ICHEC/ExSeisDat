////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The \c Input_file class
/// @details The \c Input_file is a generic interface for reading
///          seismic data files.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_FILE_INPUT_FILE_HH
#define EXSEISDAT_PIOL_FILE_INPUT_FILE_HH

#include "exseisdat/piol/configuration/ExSeisPIOL.hh"
#include "exseisdat/piol/file/detail/ObjectInterface.hh"
#include "exseisdat/piol/metadata/Trace_metadata.hh"

#include <memory>

namespace exseis {
namespace piol {
inline namespace file {

/*! @brief The \c Input_file class is a generic interface for reading
 *         seismic data files.
 */
class Input_file {
  public:
    /// @name @special_member_functions
    /// @{

    /// @default_constructor{default}
    Input_file() = default;

    /// @virtual_destructor
    virtual ~Input_file();

    /// @copy_constructor{delete}
    Input_file(const Input_file&) = delete;
    /// @copy_assignment{delete}
    Input_file& operator=(const Input_file&) = delete;

    /// @move_constructor{delete}
    Input_file(Input_file&&) = delete;
    /// @move_assignment{delete}
    Input_file& operator=(Input_file&&) = delete;

    /// @}

    /// @brief Get the name of the file.
    /// @return The name of the file.
    virtual const std::string& file_name() const = 0;

    /// @brief Read the global file headers from the file.
    virtual void read_file_headers() = 0;

    /*! @brief Read the human readable text from the file
     *  @return A string containing the text (in ASCII format)
     */
    virtual const std::string& read_text() const = 0;

    /*! @brief Read the number of samples per trace
     *  @return The number of samples per trace
     */
    virtual size_t read_ns() const = 0;

    /*! @brief Read the number of traces in the file
     *  @return The number of traces
     */
    virtual size_t read_nt() const = 0;

    /*! @brief Read the number of increment between trace samples
     *  @return The increment between trace samples
     */
    virtual exseis::utils::Floating_point read_sample_interval() const = 0;

    /*! @brief Read the trace parameters from offset to offset+sz of the
     *         respective trace headers.
     *  @param[in] offset The starting trace number.
     *  @param[in] sz The number of traces to process.
     *  @param[in] prm An array of the parameter structures
     *                 (size sizeof(Trace_metadata)*sz)
     *  @param[in] skip When reading, skip the first "skip" entries of prm
     */
    virtual void read_param(
        size_t offset,
        size_t sz,
        Trace_metadata* prm,
        size_t skip = 0) const = 0;

    /*! @brief Read the traces specified by the offsets in the passed offset
     *         array.
     *  @param[in] sz The number of traces to process
     *  @param[in] offset An array of trace numbers to read.
     *  @param[out] prm A parameter structure
     *  @param[in] skip When reading, skip the first "skip" entries of prm
     */
    virtual void read_param_non_contiguous(
        size_t sz,
        const size_t* offset,
        Trace_metadata* prm,
        size_t skip = 0) const = 0;

    /*! @brief Read the traces from offset to offset+sz
     *  @param[in] offset The starting trace number.
     *  @param[in] sz The number of traces to process.
     *  @param[out] trace The array of traces to fill from the file
     *  @param[out] prm A contiguous array of the parameter structures
     *                  (size sizeof(Trace_metadata)*sz)
     *  @param[in] skip When reading, skip the first "skip" entries of prm
     */
    virtual void read_trace(
        size_t offset,
        size_t sz,
        exseis::utils::Trace_value* trace,
        Trace_metadata* prm = nullptr,
        size_t skip         = 0) const = 0;

    /*! @brief Read the traces specified by the offsets in the passed offset
     *         array. Assumes Monotonic.
     *  @param[in] sz The number of traces to process
     *  @param[in] offset An array of trace numbers to read (monotonic list).
     *  @param[out] trace A contiguous array of each trace
     *                    (size sz*ns*sizeof(exseis::utils::Trace_value))
     *  @param[out] prm A parameter structure
     *  @param[in] skip When reading, skip the first "skip" entries of prm
     *
     *  @details When prm==nullptr only the trace data is read.
     */
    virtual void read_trace_non_contiguous(
        size_t sz,
        const size_t* offset,
        exseis::utils::Trace_value* trace,
        Trace_metadata* prm = nullptr,
        size_t skip         = 0) const = 0;

    /*! @brief Read the traces specified by the offsets in the passed offset
     *         array. Does not assume monotonic
     *  @param[in] sz The number of traces to process
     *  @param[in] offset An array of trace numbers to read
     *                    (non-monotonic list).
     *  @param[out] trace A contiguous array of each trace
     *                    (size sz*ns*sizeof(exseis::utils::Trace_value))
     *  @param[out] prm A parameter structure
     *  @param[in] skip When reading, skip the first "skip" entries of prm
     *
     *  @details When prm==nullptr only the trace data is read.
     */
    virtual void read_trace_non_monotonic(
        size_t sz,
        const size_t* offset,
        exseis::utils::Trace_value* trace,
        Trace_metadata* prm = nullptr,
        size_t skip         = 0) const = 0;
};

}  // namespace file
}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_FILE_INPUT_FILE_HH
