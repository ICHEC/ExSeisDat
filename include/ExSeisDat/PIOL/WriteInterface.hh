////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The \c WriteInterface class
/// @details The \c WriteInterface is a generic interface for reading seismic
///          data files.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_WRITEINTERFACE_HH
#define EXSEISDAT_PIOL_WRITEINTERFACE_HH

#include "ExSeisDat/PIOL/ExSeisPIOL.hh"
#include "ExSeisDat/PIOL/ObjectInterface.hh"
#include "ExSeisDat/PIOL/Param.h"

#include <memory>

namespace PIOL {

/// The NULL parameter so that the correct internal read pattern is selected
extern const trace_t* TRACE_NULL;

/*! @brief The \c ReadInterface class is a generic interface for reading
 *         seismic data files.
 */
class WriteInterface {
  protected:
    /// The PIOL object.
    std::shared_ptr<ExSeisPIOL> piol;

    /// Store the file name for debugging purposes.
    std::string name;

    /// Pointer to the Object-layer object (polymorphic).
    std::shared_ptr<ObjectInterface> obj;

    /// The number of samples per trace.
    size_t ns = 0;

    /// The number of traces.
    size_t nt = 0;

    /// Human readable text extracted from the file
    std::string text = "";

    /// The increment between samples in a trace
    geom_t inc = 0;

  public:
    /*! @brief The constructor.
     *  @param[in] piol_ This PIOL ptr is not modified but is used to
     *                   instantiate another shared_ptr.
     *  @param[in] name_ The name of the file associated with the instantiation.
     *  @param[in] obj_  Pointer to the Object-layer object (polymorphic).
     */
    WriteInterface(
      std::shared_ptr<ExSeisPIOL> piol_,
      std::string name_,
      std::shared_ptr<ObjectInterface> obj_) :
        piol(piol_),
        name(name_),
        obj(obj_)
    {
    }

    /// @brief The destructor.
    virtual ~WriteInterface() = default;

    /*! @brief Write the human readable text from the file.
     *  @param[in] text_ The new string containing the text (in ASCII format).
     */
    virtual void writeText(const std::string text_) = 0;

    /*! @brief Write the number of samples per trace
     *  @param[in] ns_ The new number of samples per trace.
     */
    virtual void writeNs(const size_t ns_) = 0;

    /*! @brief Write the number of traces in the file
     *  @param[in] nt_ The new number of traces.
     */
    virtual void writeNt(const size_t nt_) = 0;

    /*! @brief Write the number of increment between trace samples.
     *  @param[in] inc_ The new increment between trace samples.
     */
    virtual void writeInc(const geom_t inc_) = 0;

    /*! @brief Write the trace parameters from offset to offset+sz to the
     *         respective trace headers.
     *  @param[in] offset The starting trace number.
     *  @param[in] sz The number of traces to process.
     *  @param[in] prm An array of the parameter structures
     *                 (size sizeof(Param)*sz)
     *  @param[in] skip When writing, skip the first "skip" entries of prm
     *
     *  @details It is assumed that this operation is not an update. Any
     *           previous contents of the trace header will be overwritten.
     */
    void writeParam(
      const size_t offset,
      const size_t sz,
      const Param* prm,
      const size_t skip = 0);

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
    void writeParamNonContiguous(
      const size_t sz,
      const size_t* offset,
      const Param* prm,
      const size_t skip = 0);

    /*! @brief Write the traces from offset to offset+sz
     *  @param[in] offset The starting trace number.
     *  @param[in] sz The number of traces to process.
     *  @param[in] trace The array of traces to write to the file
     *  @param[in] prm A contiguous array of the parameter structures
     *                 (size sizeof(Param)*sz)
     *  @param[in] skip When writing, skip the first "skip" entries of prm
     */
    virtual void writeTrace(
      const size_t offset,
      const size_t sz,
      trace_t* trace,
      const Param* prm  = PIOL_PARAM_NULL,
      const size_t skip = 0) = 0;

    /*! @brief Write the traces specified by the offsets in the passed offset
     *         array.
     *  @param[in] sz The number of traces to process
     *  @param[in] offset An array of trace numbers to write.
     *  @param[in] trace A contiguous array of each trace
     *                   (size sz*ns*sizeof(trace_t))
     *  @param[in] prm A parameter structure
     *  @param[in] skip When writing, skip the first "skip" entries of prm
     *
     *  @details When prm==PIOL_PARAM_NULL only the trace DF is written.  It is
     *           assumed that the parameter writing operation is not an update.
     *           Any previous contents of the trace header will be overwritten.
     */
    virtual void writeTraceNonContiguous(
      const size_t sz,
      const size_t* offset,
      trace_t* trace,
      const Param* prm  = PIOL_PARAM_NULL,
      const size_t skip = 0) = 0;
};

}  // namespace PIOL

#endif  // EXSEISDAT_PIOL_WRITEINTERFACE_HH
