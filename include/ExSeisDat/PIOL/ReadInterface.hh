////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The \c ReadInterface class
/// @details The \c ReadInterface is a generic interface for reading seismic
///          data files.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_READINTERFACE_HH
#define EXSEISDAT_PIOL_READINTERFACE_HH

#include "ExSeisDat/PIOL/ExSeisPIOL.hh"
#include "ExSeisDat/PIOL/ObjectInterface.hh"
#include "ExSeisDat/PIOL/Param.h"

#include <memory>

namespace exseis {
namespace PIOL {

/// The NULL parameter so that the correct internal read pattern is selected
extern const exseis::utils::Trace_value* TRACE_NULL;

/*! @brief The \c ReadInterface class is a generic interface for reading
 *         seismic data files.
 */
class ReadInterface {
  protected:
    /// The PIOL object.
    std::shared_ptr<ExSeisPIOL> piol;

    /// Store the file name for debugging purposes.
    std::string name = "";

    /// Pointer to the Object-layer object (polymorphic).
    std::shared_ptr<ObjectInterface> obj;

    /// The number of samples per trace.
    size_t ns = 0;

    /// The number of traces.
    size_t nt = 0;

    /// Human readable text extracted from the file
    std::string text = "";

    /// The increment between samples in a trace
    exseis::utils::Floating_point inc = 0;

  public:
    /*! @brief The constructor.
     *  @param[in] piol_ This PIOL ptr is not modified but is used to
     *                   instantiate another shared_ptr.
     *  @param[in] name_ The name of the file associated with the instantiation.
     *  @param[in] obj_  Pointer to the Object-layer object (polymorphic).
     */
    ReadInterface(
      std::shared_ptr<ExSeisPIOL> piol_,
      std::string name_,
      std::shared_ptr<ObjectInterface> obj_) :
        piol(piol_),
        name(name_),
        obj(obj_)
    {
    }

    /// @brief The destructor.
    virtual ~ReadInterface() = default;

    /*! @brief Read the human readable text from the file
     *  @return A string containing the text (in ASCII format)
     */
    virtual const std::string& readText(void) const;

    /*! @brief Read the name of the file
     *  @return A string containing the name (in ASCII format)
     */
    const std::string& readName(void) const { return name; }

    /*! @brief Read the number of samples per trace
     *  @return The number of samples per trace
     */
    virtual size_t readNs(void) const;

    /*! @brief Read the number of traces in the file
     *  @return The number of traces
     */
    virtual size_t readNt(void) const = 0;

    /*! @brief Read the number of increment between trace samples
     *  @return The increment between trace samples
     */
    virtual exseis::utils::Floating_point readInc(void) const;

    /*! @brief Read the trace parameters from offset to offset+sz of the
     *         respective trace headers.
     *  @param[in] offset The starting trace number.
     *  @param[in] sz The number of traces to process.
     *  @param[in] prm An array of the parameter structures
     *                 (size sizeof(Param)*sz)
     *  @param[in] skip When reading, skip the first "skip" entries of prm
     */
    void readParam(size_t offset, size_t sz, Param* prm, size_t skip = 0) const;

    /*! @brief Read the traces specified by the offsets in the passed offset
     *         array.
     *  @param[in] sz The number of traces to process
     *  @param[in] offset An array of trace numbers to read.
     *  @param[out] prm A parameter structure
     *  @param[in] skip When reading, skip the first "skip" entries of prm
     */
    void readParamNonContiguous(
      size_t sz, const size_t* offset, Param* prm, size_t skip = 0) const;

    /*! @brief Read the traces from offset to offset+sz
     *  @param[in] offset The starting trace number.
     *  @param[in] sz The number of traces to process.
     *  @param[out] trace The array of traces to fill from the file
     *  @param[out] prm A contiguous array of the parameter structures
     *                  (size sizeof(Param)*sz)
     *  @param[in] skip When reading, skip the first "skip" entries of prm
     */
    virtual void readTrace(
      size_t offset,
      size_t sz,
      exseis::utils::Trace_value* trace,
      Param* prm  = PIOL_PARAM_NULL,
      size_t skip = 0) const = 0;

    /*! @brief Read the traces specified by the offsets in the passed offset
     *         array. Assumes Monotonic.
     *  @param[in] sz The number of traces to process
     *  @param[in] offset An array of trace numbers to read (monotonic list).
     *  @param[out] trace A contiguous array of each trace
     *                    (size sz*ns*sizeof(exseis::utils::Trace_value))
     *  @param[out] prm A parameter structure
     *  @param[in] skip When reading, skip the first "skip" entries of prm
     *
     *  @details When prm==PIOL_PARAM_NULL only the trace DF is read.
     */
    virtual void readTraceNonContiguous(
      size_t sz,
      const size_t* offset,
      exseis::utils::Trace_value* trace,
      Param* prm  = PIOL_PARAM_NULL,
      size_t skip = 0) const = 0;

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
     *  @details When prm==PIOL_PARAM_NULL only the trace DF is read.
     */
    virtual void readTraceNonMonotonic(
      size_t sz,
      const size_t* offset,
      exseis::utils::Trace_value* trace,
      Param* prm  = PIOL_PARAM_NULL,
      size_t skip = 0) const = 0;
};

}  // namespace PIOL
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_READINTERFACE_HH
