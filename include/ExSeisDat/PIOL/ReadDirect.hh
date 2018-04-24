////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
/// @details The \c ReadDirect class
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_READDIRECT_HH
#define EXSEISDAT_PIOL_READDIRECT_HH

#include "ExSeisDat/PIOL/ExSeisPIOL.hh"

#include "ExSeisDat/PIOL/DataMPIIO.hh"
#include "ExSeisDat/PIOL/Param.h"
#include "ExSeisDat/PIOL/ReadInterface.hh"
#include "ExSeisDat/utils/typedefs.h"

#include <memory>
#include <string>

namespace exseis {
namespace PIOL {

using namespace exseis::utils::typedefs;

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
    ReadDirect(std::shared_ptr<ExSeisPIOL> piol, std::string name);

    /*! Copy constructor for a std::shared_ptr<ReadInterface> object
     * @param[in] file The ReadInterface shared_ptr object.
     */
    ReadDirect(std::shared_ptr<ReadInterface> file);

    /*! Empty destructor
     */
    ~ReadDirect();

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
    size_t readNs() const;

    /*! @brief Read the number of traces in the file
     *  @return The number of traces
     */
    size_t readNt() const;

    /*! @brief Read the number of increment between trace samples
     *  @return The increment between trace samples
     */
    exseis::utils::Floating_point readInc() const;

    /*! @brief Read the traces from offset to offset+sz.
     *  @param[in]  offset The starting trace number.
     *  @param[in]  sz     The number of traces to process
     *  @param[out] trace  A contiguous array of each trace
     *                     (size sz*ns*sizeof(exseis::utils::Trace_value))
     *  @param[out] prm    The parameter structure
     *
     *  @details When prm==PIOL_PARAM_NULL only the trace DF is read.
     */
    void readTrace(
      size_t offset,
      size_t sz,
      exseis::utils::Trace_value* trace,
      Param* prm = PIOL_PARAM_NULL) const;

    /*! @brief Function to read the trace parameters from offset to offset+sz of
     *         the respective trace headers.
     *  @param[in]  offset The starting trace number.
     *  @param[in]  sz     The number of traces to process.
     *  @param[out] prm    The parameter structure
     */
    void readParam(size_t offset, size_t sz, Param* prm) const;

    /*! @brief Read the traces specified by the offsets in the passed offset
     *         array. The offsets should in ascending order,
     *         i.e. offset[i] < offset[i+1].
     *  @param[in]  sz     The number of traces to process
     *  @param[in]  offset An array of trace numbers to read.
     *  @param[out] trace  A contiguous array of each trace
     *                     (size sz*ns*sizeof(exseis::utils::Trace_value))
     *  @param[out] prm    The parameter structure
     *
     *  @details When prm==PIOL_PARAM_NULL only the trace DF is read.
     */
    void readTraceNonContiguous(
      size_t sz,
      const size_t* offset,
      exseis::utils::Trace_value* trace,
      Param* prm = PIOL_PARAM_NULL) const;

    /*! @brief Read the traces specified by the offsets in the passed offset
     *         array. The offset array need not be in any order.
     *  @param[in]  sz     The number of traces to process
     *  @param[in]  offset An array of trace numbers to read.
     *  @param[out] trace  A contiguous array of each trace
     *                     (size sz*ns*sizeof(exseis::utils::Trace_value))
     *  @param[out] prm    The parameter structure
     *
     *  @details When prm==PIOL_PARAM_NULL only the trace DF is read.
     */
    void readTraceNonMonotonic(
      size_t sz,
      const size_t* offset,
      exseis::utils::Trace_value* trace,
      Param* prm = PIOL_PARAM_NULL) const;

    /*! @brief Read the traces specified by the offsets in the passed offset
     *         array. The offsets should be in ascending order,
     *         i.e. offset[i] < offset[i+1].
     *  @param[in] sz     The number of traces to process
     *  @param[in] offset An array of trace numbers to read.
     *  @param[out] prm   The parameter structure
     */
    void readParamNonContiguous(
      size_t sz, const size_t* offset, Param* prm) const;
};

}  // namespace PIOL
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_READDIRECT_HH
