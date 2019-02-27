////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The \c WriteInterface class
/// @details The \c WriteInterface is a generic interface for reading seismic
///          data files.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_WRITEINTERFACE_HH
#define EXSEISDAT_PIOL_WRITEINTERFACE_HH

#include "exseisdat/piol/ExSeisPIOL.hh"
#include "exseisdat/piol/ObjectInterface.hh"
#include "exseisdat/piol/Trace_metadata.hh"

#include <memory>

namespace exseis {
namespace piol {

/*! @brief The \c WriteInterface class is a generic interface for reading
 *         seismic data files.
 */
class WriteInterface {

  public:
    /// @name @special_member_functions
    /// @{

    /// @default_constructor{default}
    WriteInterface() = default;

    /// @copy_constructor{delete}
    WriteInterface(const WriteInterface&) = delete;
    /// @copy_assignment{delete}
    WriteInterface& operator=(const WriteInterface&) = delete;

    /// @move_constructor{delete}
    WriteInterface(WriteInterface&&) = delete;
    /// @move_assignment{delete}
    WriteInterface& operator=(WriteInterface&&) = delete;

    /// @virtual_destructor
    virtual ~WriteInterface();

    /// @}

    /// @brief Get the name of the file.
    /// @return The name of the file.
    virtual const std::string& file_name() const = 0;

    /*! @brief Write the human readable text from the file.
     *  @param[in] text The new string containing the text (in ASCII format).
     */
    virtual void write_text(std::string text) = 0;

    /*! @brief Write the number of samples per trace
     *  @param[in] ns The new number of samples per trace.
     */
    virtual void write_ns(size_t ns) = 0;

    /*! @brief Write the number of traces in the file
     *  @param[in] nt The new number of traces.
     */
    virtual void write_nt(size_t nt) = 0;

    /*! @brief Write the number of increment between trace samples.
     *  @param[in] sample_interval The new increment between trace samples.
     */
    virtual void write_sample_interval(
        exseis::utils::Floating_point sample_interval) = 0;

    /*! @brief Write the trace parameters from offset to offset+sz to the
     *         respective trace headers.
     *  @param[in] offset The starting trace number.
     *  @param[in] sz The number of traces to process.
     *  @param[in] prm An array of the parameter structures
     *                 (size sizeof(Trace_metadata)*sz)
     *  @param[in] skip When writing, skip the first "skip" entries of prm
     *
     *  @details It is assumed that this operation is not an update. Any
     *           previous contents of the trace header will be overwritten.
     */
    virtual void write_param(
        size_t offset,
        size_t sz,
        const Trace_metadata* prm,
        size_t skip = 0) = 0;

    /*! @brief Write the parameters specified by the offsets in the passed
     *         offset array.
     *  @param[in] sz The number of traces to process
     *  @param[in] offset An array of trace numbers to write.
     *  @param[in] prm A parameter structure
     *  @param[in] skip When writing, skip the first "skip" entries of prm
     *
     *  @details It is assumed that the parameter writing operation is not an
     *           update. Any previous contents of the trace header will be
     *           overwritten.
     */
    virtual void write_param_non_contiguous(
        size_t sz,
        const size_t* offset,
        const Trace_metadata* prm,
        size_t skip = 0) = 0;

    /*! @brief Write the traces from offset to offset+sz
     *  @param[in] offset The starting trace number.
     *  @param[in] sz The number of traces to process.
     *  @param[in] trace The array of traces to write to the file
     *  @param[in] prm A contiguous array of the parameter structures
     *                 (size sizeof(Trace_metadata)*sz)
     *  @param[in] skip When writing, skip the first "skip" entries of prm
     */
    virtual void write_trace(
        size_t offset,
        size_t sz,
        exseis::utils::Trace_value* trace,
        const Trace_metadata* prm = nullptr,
        size_t skip               = 0) = 0;

    /*! @brief Write the traces specified by the offsets in the passed offset
     *         array.
     *  @param[in] sz The number of traces to process
     *  @param[in] offset An array of trace numbers to write.
     *  @param[in] trace A contiguous array of each trace
     *                   (size sz*ns*sizeof(exseis::utils::Trace_value))
     *  @param[in] prm A parameter structure
     *  @param[in] skip When writing, skip the first "skip" entries of prm
     *
     *  @details When prm==nullptr only the trace data is written.  It
     *           is assumed that the parameter writing operation is not an
     *           update.  Any previous contents of the trace header will be
     *           overwritten.
     */
    virtual void write_trace_non_contiguous(
        size_t sz,
        const size_t* offset,
        exseis::utils::Trace_value* trace,
        const Trace_metadata* prm = nullptr,
        size_t skip               = 0) = 0;
};

}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_WRITEINTERFACE_HH
