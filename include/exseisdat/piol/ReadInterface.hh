////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The \c ReadInterface class
/// @details The \c ReadInterface is a generic interface for reading seismic
///          data files.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_READINTERFACE_HH
#define EXSEISDAT_PIOL_READINTERFACE_HH

#include "exseisdat/piol/ExSeisPIOL.hh"
#include "exseisdat/piol/ObjectInterface.hh"
#include "exseisdat/piol/Trace_metadata.hh"

#include <memory>

namespace exseis {
namespace piol {

/*! @brief The \c ReadInterface class is a generic interface for reading
 *         seismic data files.
 */
class ReadInterface {
  public:
    /// @name @special_member_functions
    /// @{

    /// @default_constructor{default}
    ReadInterface() = default;

    /// @virtual_destructor
    virtual ~ReadInterface();

    /// @copy_constructor{delete}
    ReadInterface(const ReadInterface&) = delete;
    /// @copy_assignment{delete}
    ReadInterface& operator=(const ReadInterface&) = delete;

    /// @move_constructor{delete}
    ReadInterface(ReadInterface&&) = delete;
    /// @move_assignment{delete}
    ReadInterface& operator=(ReadInterface&&) = delete;

    /// @}

    /// @brief Get the name of the file.
    /// @return The name of the file.
    virtual const std::string& file_name() const = 0;

    /// @brief Read the global file headers from the file.
    virtual void read_file_headers() = 0;

    /*! @brief Read the human readable text from the file
     *  @return A string containing the text (in ASCII format)
     */
    virtual const std::string& read_text(void) const = 0;

    /*! @brief Read the number of samples per trace
     *  @return The number of samples per trace
     */
    virtual size_t read_ns(void) const = 0;

    /*! @brief Read the number of traces in the file
     *  @return The number of traces
     */
    virtual size_t read_nt(void) const = 0;

    /*! @brief Read the number of increment between trace samples
     *  @return The increment between trace samples
     */
    virtual exseis::utils::Floating_point read_sample_interval(void) const = 0;

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

}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_READINTERFACE_HH
