////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
/// @details The \c ReadDirect class
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_READDIRECT_HH
#define EXSEISDAT_PIOL_READDIRECT_HH

#include "ExSeisDat/PIOL/ExSeisPIOL.hh"

#include "ExSeisDat/PIOL/DataMPIIO.hh"
#include "ExSeisDat/PIOL/Distributed_vector.hh"
#include "ExSeisDat/PIOL/Param.h"

#include <memory>
#include <string>

namespace PIOL {

/*! This class implements the C++14 File Layer API for the PIOL. It constructs
 *  the Data, Object and File layers.
 */
class ReadDirect {
  protected:
    /// The pointer to the base class (polymorphic)
    std::shared_ptr<ReadInterface> file;

  public:
    /*! Constructor with options.
     *  @tparam    D    The nested options structure of interest for the data
     *                  layer.
     *  @tparam    O    The nested options structure of interest for the object
     *                  layer.
     *  @tparam    F    The nested options structure of interest for the file
     *                  layer.
     *  @param[in] piol This PIOL ptr is not modified but is used to instantiate
     *                  another shared_ptr.
     *  @param[in] name The name of the file associated with the instantiation.
     *  @param[in] d    The Data options.
     *  @param[in] o    The Object options.
     *  @param[in] f    The File options.
     */
    template<class F, class O, class D>
    ReadDirect(
      std::shared_ptr<ExSeisPIOL> piol,
      const std::string name,
      const D& d,
      const O& o,
      const F& f)
    {
        auto data =
          std::make_shared<typename D::Type>(piol, name, d, FileMode::Read);

        auto obj = std::make_shared<typename O::Type>(
          piol, name, o, data, FileMode::Read);

        file = std::make_shared<typename F::Type>(piol, name, f, obj);

        if (!file) {
            piol->log->record(
              name, Logger::Layer::API, Logger::Status::Error,
              "ReadInterface creation failure in ReadDirect<F,O,D>()",
              PIOL_VERBOSITY_NONE);
        }
    }

    /*! Constructor without options.
     *  @param[in] piol This PIOL ptr is not modified but is used to instantiate
     *                  another shared_ptr.
     *  @param[in] name The name of the file associated with the instantiation.
     */
    ReadDirect(std::shared_ptr<ExSeisPIOL> piol, const std::string name);

    /*! Copy constructor for a std::shared_ptr<ReadInterface> object
     * @param[in] file The ReadInterface shared_ptr object.
     */
    ReadDirect(std::shared_ptr<ReadInterface> file);

    /*! Empty destructor
     */
    ~ReadDirect(void);

    /*! Overload of member of pointer access
     *  @return Return the base File layer class Interface.
     */
    ReadInterface* operator->() const { return file.get(); }

    /*! Operator to convert to an Interface object.
     *  @return Return the internal \c Interface pointer.
     */
    operator ReadInterface*() const { return file.get(); }

    /*! @brief Read the human readable text from the file.
     *  @return A string containing the text (in ASCII format).
     */
    const std::string& readText(void) const;

    /*! @brief Read the number of samples per trace
     *  @return The number of samples per trace
     */
    size_t readNs(void) const;

    /*! @brief Read the number of traces in the file
     *  @return The number of traces
     */
    size_t readNt(void) const;

    /*! @brief Read the number of increment between trace samples
     *  @return The increment between trace samples
     */
    geom_t readInc(void) const;

    /*! @brief Read the traces from offset to offset+sz.
     *  @param[in]  offset The starting trace number.
     *  @param[in]  sz     The number of traces to process
     *  @param[out] trace  A contiguous array of each trace
     *                     (size sz*ns*sizeof(trace_t))
     *  @param[out] prm    The parameter structure
     *
     *  @details When prm==PIOL_PARAM_NULL only the trace DF is read.
     */
    void readTrace(
      const size_t offset,
      const size_t sz,
      trace_t* trace,
      Param* prm = PIOL_PARAM_NULL) const;

    /*! @brief Function to read the trace parameters from offset to offset+sz of
     *         the respective trace headers.
     *  @param[in]  offset The starting trace number.
     *  @param[in]  sz     The number of traces to process.
     *  @param[out] prm    The parameter structure
     */
    void readParam(const size_t offset, const size_t sz, Param* prm) const;

    /*! @brief Read the traces specified by the offsets in the passed offset
     *         array. The offsets should in ascending order,
     *         i.e. offset[i] < offset[i+1].
     *  @param[in]  sz     The number of traces to process
     *  @param[in]  offset An array of trace numbers to read.
     *  @param[out] trace  A contiguous array of each trace
     *                     (size sz*ns*sizeof(trace_t))
     *  @param[out] prm    The parameter structure
     *
     *  @details When prm==PIOL_PARAM_NULL only the trace DF is read.
     */
    void readTraceNonContiguous(
      const size_t sz,
      const size_t* offset,
      trace_t* trace,
      Param* prm = PIOL_PARAM_NULL) const;

    /*! @brief Read the traces specified by the offsets in the passed offset
     *         array. The offset array need not be in any order.
     *  @param[in]  sz     The number of traces to process
     *  @param[in]  offset An array of trace numbers to read.
     *  @param[out] trace  A contiguous array of each trace
     *                     (size sz*ns*sizeof(trace_t))
     *  @param[out] prm    The parameter structure
     *
     *  @details When prm==PIOL_PARAM_NULL only the trace DF is read.
     */
    void readTraceNonMonotonic(
      const size_t sz,
      const size_t* offset,
      trace_t* trace,
      Param* prm = PIOL_PARAM_NULL) const;

    /*! @brief Read the traces specified by the offsets in the passed offset
     *         array. The offsets should be in ascending order,
     *         i.e. offset[i] < offset[i+1].
     *  @param[in] sz     The number of traces to process
     *  @param[in] offset An array of trace numbers to read.
     *  @param[out] prm   The parameter structure
     */
    void readParamNonContiguous(
      const size_t sz, const size_t* offset, Param* prm) const;
};

}  // namespace PIOL

#endif  // EXSEISDAT_PIOL_READDIRECT_HH
