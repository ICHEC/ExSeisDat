////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
/// @details The \c ReadDirect class
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_WRITEDIRECT_HH
#define EXSEISDAT_PIOL_WRITEDIRECT_HH

#include "ExSeisDat/PIOL/ExSeisPIOL.hh"
#include "ExSeisDat/PIOL/WriteInterface.hh"

#include "ExSeisDat/PIOL/DataMPIIO.hh"
#include "ExSeisDat/PIOL/Param.hh"

#include <memory>
#include <string>

namespace PIOL {

/*! This class implements the C++14 File Layer API for the PIOL. It constructs
 *  the Data, Object and File layers.
 */
class WriteDirect {
  protected:
    /// The pointer to the base class (polymorphic)
    std::shared_ptr<WriteInterface> file;

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
    template<class D, class O, class F>
    WriteDirect(
      std::shared_ptr<ExSeisPIOL> piol,
      const std::string name,
      const D& d,
      const O& o,
      const F& f)
    {
        auto data =
          std::make_shared<typename D::Type>(piol, name, d, FileMode::Write);

        auto obj = std::make_shared<typename O::Type>(
          piol, name, o, data, FileMode::Write);

        file = std::make_shared<typename F::Type>(piol, name, f, obj);

        if (!file) {
            piol->log->record(
              name, Log::Layer::API, Log::Status::Error,
              "WriteInterface creation failure in WriteDirect<F,O,D>()",
              PIOL_VERBOSITY_NONE);
        }
    }

    /*! Constructor without options.
     *  @param[in] piol This PIOL ptr is not modified but is used to instantiate
     *                  another shared_ptr.
     *  @param[in] name The name of the file associated with the instantiation.
     */
    WriteDirect(std::shared_ptr<ExSeisPIOL> piol, const std::string name);

    /*! Copy constructor for a std::shared_ptr<ReadInterface> object
     * @param[in] file The ReadInterface shared_ptr object.
     */
    WriteDirect(std::shared_ptr<WriteInterface> file);

    /*! Empty destructor
     */
    ~WriteDirect(void);

    /*! Overload of member of pointer access
     *  @return Return the base File layer class Interface.
     */
    WriteInterface& operator->() const { return *file; }

    /*! Operator to convert to an Interface object.
     *  @return Return the internal \c Interface pointer.
     */
    operator WriteInterface&() const { return *file; }

    /*! @brief Write the human readable text from the file.
     *  @param[in] text_ The new string containing the text (in ASCII format).
     */
    void writeText(const std::string text_);

    /*! @brief Write the number of samples per trace
     *  @param[in] ns_ The new number of samples per trace.
     */
    void writeNs(const size_t ns_);

    /*! @brief Write the number of traces in the file
     *  @param[in] nt_ The new number of traces.
     */
    void writeNt(const size_t nt_);

    /*! @brief Write the increment between trace samples.
     *  @param[in] inc_ The new increment between trace samples.
     */
    void writeInc(const geom_t inc_);

    /*! @brief Read the traces from offset to offset+sz.
     *  @param[in] offset The starting trace number.
     *  @param[in] sz     The number of traces to process
     *  @param[in] trace  A contiguous array of each trace
     *                    (size sz*ns*sizeof(trace_t))
     *  @param[in] prm    The parameter structure
     *  @warning This function is not thread safe.
     *
     *  @details When prm==PIOL_PARAM_NULL only the trace DF is written.
     */
    void writeTrace(
      const size_t offset,
      const size_t sz,
      trace_t* trace,
      const Param* prm = PIOL_PARAM_NULL);

    /*! @brief Write the trace parameters from offset to offset+sz to the
     *         respective trace headers.
     *  @param[in] offset The starting trace number.
     *  @param[in] sz     The number of traces to process.
     *  @param[in] prm    The parameter structure
     *
     *  @details It is assumed that this operation is not an update. Any
     *           previous contents of the trace header will be overwritten.
     */
    void writeParam(const size_t offset, const size_t sz, const Param* prm);

    /*! @brief write the traces specified by the offsets in the passed offset
     *         array.
     *  @param[in] sz     The number of traces to process
     *  @param[in] offset An array of trace numbers to write.
     *  @param[in] trace  A contiguous array of each trace
     *                    (size sz*ns*sizeof(trace_t))
     *  @param[in] prm    The parameter structure
     *
     *  @details When prm==PIOL_PARAM_NULL only the trace DF is written.  It is
     *           assumed that the parameter writing operation is not an update.
     *           Any previous contents of the trace header will be overwritten.
     */
    void writeTraceNonContiguous(
      const size_t sz,
      const size_t* offset,
      trace_t* trace,
      const Param* prm = PIOL_PARAM_NULL);

    /*! @brief write the traces specified by the offsets in the passed offset
     *         array.
     *  @param[in] sz     The number of traces to process
     *  @param[in] offset An array of trace numbers to write.
     *  @param[in] prm    The parameter structure
     *
     *  @details It is assumed that the parameter writing operation is not an
     *           update. Any previous contents of the trace header will be
     *           overwritten.
     */
    void writeParamNonContiguous(
      const size_t sz, const size_t* offset, const Param* prm);
};

}  // namespace PIOL

#endif  // EXSEISDAT_PIOL_WRITEDIRECT_HH
