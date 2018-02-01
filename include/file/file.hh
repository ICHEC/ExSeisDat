////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date January 2017
/// @brief The File layer interface
/// @details The File layer interface is a base class which specific File
///          implementations work off
////////////////////////////////////////////////////////////////////////////////
#ifndef PIOLFILE_INCLUDE_GUARD
#define PIOLFILE_INCLUDE_GUARD

#include "global.hh"
#include "share/param.hh"
#include "share/uniray.hh"

namespace PIOL {
namespace Obj {

/*! Make the default object layer object.
 * @param[in] piol The piol shared object.
 * @param[in] name The name of the file.
 * @param[in] mode The filemode.
 * @return Return a shared_ptr to the obj layer object.
 * @todo TODO: This hack needs a further tidyup and out of file.hh.
 */
std::shared_ptr<Obj::Interface> makeDefaultObj(
  std::shared_ptr<ExSeisPIOL> piol, std::string name, FileMode mode);

}  // namespace Obj

namespace File {

/// The NULL parameter so that the correct internal read pattern is selected
extern const trace_t*
  TRACE_NULL;

/*! Class for all file interfaces
 */
class Interface {
  protected:
    std::shared_ptr<ExSeisPIOL> piol;  //!< The PIOL object.
    std::string name;  //!< Store the file name for debugging purposes.
    std::shared_ptr<Obj::Interface>
      obj;             //!< Pointer to the Object-layer object (polymorphic).
    size_t ns;         //!< The number of samples per trace.
    size_t nt;         //!< The number of traces.
    std::string text;  //!< Human readable text extracted from the file
    geom_t inc;        //!< The increment between samples in a trace

    /*! @brief The constructor.
     *  @param[in] piol_ This PIOL ptr is not modified but is used to
     *                   instantiate another shared_ptr.
     *  @param[in] name_ The name of the file associated with the instantiation.
     *  @param[in] obj_  Pointer to the Object-layer object (polymorphic).
     */
    Interface(
      std::shared_ptr<ExSeisPIOL> piol_,
      std::string name_,
      std::shared_ptr<Obj::Interface> obj_) :
        piol(piol_),
        name(name_),
        obj(obj_)
    {
    }

    /*! Empty constructor
     */
    Interface(void) {}

    /*! @brief A virtual destructor to allow deletion, unique_ptr polymorphism.
     */
    virtual ~Interface(void) {}
};

/*! @brief The File layer interface. Specific File implementations
 *  work off this base class.
 */
class ReadInterface : public Interface {
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
      std::shared_ptr<Obj::Interface> obj_) :
        Interface(piol_, name_, obj_)
    {
    }

    ReadInterface(void) : Interface() {}

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
    virtual geom_t readInc(void) const;

    /*! @brief Read the trace parameters from offset to offset+sz of the
     *         respective trace headers.
     *  @param[in] offset The starting trace number.
     *  @param[in] sz The number of traces to process.
     *  @param[in] prm An array of the parameter structures
     *                 (size sizeof(Param)*sz)
     *  @param[in] skip When reading, skip the first "skip" entries of prm
     */
    void readParam(
      const size_t offset,
      const size_t sz,
      Param* prm,
      const size_t skip = 0) const;

    /*! @brief Read the traces specified by the offsets in the passed offset
     *         array.
     *  @param[in] sz The number of traces to process
     *  @param[in] offset An array of trace numbers to read.
     *  @param[out] prm A parameter structure
     *  @param[in] skip When reading, skip the first "skip" entries of prm
     */
    void readParamNonContiguous(
      const size_t sz,
      const size_t* offset,
      Param* prm,
      const size_t skip = 0) const;

    /*! @brief Read the traces from offset to offset+sz
     *  @param[in] offset The starting trace number.
     *  @param[in] sz The number of traces to process.
     *  @param[out] trace The array of traces to fill from the file
     *  @param[out] prm A contiguous array of the parameter structures
     *                  (size sizeof(Param)*sz)
     *  @param[in] skip When reading, skip the first "skip" entries of prm
     */
    virtual void readTrace(
      const size_t offset,
      const size_t sz,
      trace_t* trace,
      Param* prm        = PIOL_PARAM_NULL,
      const size_t skip = 0) const = 0;

    /*! @brief Read the traces specified by the offsets in the passed offset
     *         array. Assumes Monotonic.
     *  @param[in] sz The number of traces to process
     *  @param[in] offset An array of trace numbers to read (monotonic list).
     *  @param[out] trace A contiguous array of each trace
     *                    (size sz*ns*sizeof(trace_t))
     *  @param[out] prm A parameter structure
     *  @param[in] skip When reading, skip the first "skip" entries of prm
     *
     *  @details When prm==PIOL_PARAM_NULL only the trace DF is read.
     */
    virtual void readTraceNonContiguous(
      const size_t sz,
      const size_t* offset,
      trace_t* trace,
      Param* prm        = PIOL_PARAM_NULL,
      const size_t skip = 0) const = 0;

    /*! @brief Read the traces specified by the offsets in the passed offset
     *         array. Does not assume monotonic
     *  @param[in] sz The number of traces to process
     *  @param[in] offset An array of trace numbers to read
     *                    (non-monotonic list).
     *  @param[out] trace A contiguous array of each trace
     *                    (size sz*ns*sizeof(trace_t))
     *  @param[out] prm A parameter structure
     *  @param[in] skip When reading, skip the first "skip" entries of prm
     *
     *  @details When prm==PIOL_PARAM_NULL only the trace DF is read.
     */
    virtual void readTraceNonMonotonic(
      const size_t sz,
      const size_t* offset,
      trace_t* trace,
      Param* prm        = PIOL_PARAM_NULL,
      const size_t skip = 0) const = 0;
};

/*! @brief The File layer interface. Specific File implementations
 *  work off this base class.
 */
class WriteInterface : public Interface {
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
      std::shared_ptr<Obj::Interface> obj_) :
        Interface(piol_, name_, obj_)
    {
    }

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
      const size_t skip = 0)
    {
        writeTrace(offset, sz, const_cast<trace_t*>(TRACE_NULL), prm, skip);
    }

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
      const size_t skip = 0)
    {
        writeTraceNonContiguous(
          sz, offset, const_cast<trace_t*>(TRACE_NULL), prm, skip);
    }

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

/*! @brief An intitial class for 3d volumes
 */
class Model3dInterface {
  public:
    /// Parameters for the inline coordinate (start, count, increment)
    std::tuple<llint, llint, llint>
      il;
    /// Parameters for the crossline coordinate (start, count, increment)
    std::tuple<llint, llint, llint>
      xl;

    /*! read the 3d file based on il and xl that match those in the given
     *  \c gather array.
     *  @param[in] offset the offset into the global array
     *  @param[in] sz the number of gathers for the local process
     *  @param[in] gather a structure which contains the il and xl coordinates
     *                    of interest
     *  @return return a vector of traces containing the trace values requested
     */
    virtual std::vector<trace_t> readModel(
      const size_t offset,
      const size_t sz,
      const Uniray<size_t, llint, llint>& gather) = 0;

    /*! Read the 3d file based on il and xl that match those in the given
     *  \c gather array.
     *  @param[in] sz The number of offsets for the local process
     *  @param[in] offset the offset into the global array
     *  @param[in] gather A structure which contains the il and xl coordinates
     *                    of interest
     *  @return Return a vector of traces containing the trace values requested
     */
    virtual std::vector<trace_t> readModel(
      const size_t sz,
      const size_t* offset,
      const Uniray<size_t, llint, llint>& gather) = 0;
};

/*! Construct ReadSEGY objects with default object and MPI-IO layers.
 * @tparam T The type of the file layer.
 * @param[in] piol The piol shared object.
 * @param[in] name The name of the file.
 * @return Return a pointer of the respective file type.
 */
template<class T>
std::unique_ptr<typename std::enable_if<
  std::is_base_of<File::ReadInterface, T>::value,
  T>::type>
makeFile(std::shared_ptr<ExSeisPIOL> piol, const std::string& name)
{
    return std::make_unique<T>(
      piol, name, Obj::makeDefaultObj(piol, name, FileMode::Read));
}

/*! Construct WriteSEGY objects with default object and MPI-IO layers
 * @tparam T The type of the file layer
 * @param[in] piol The piol shared object
 * @param[in] name The name of the file
 * @return Return a pointer of the respective file type.
 */
template<class T>
std::unique_ptr<typename std::enable_if<
  std::is_base_of<File::WriteInterface, T>::value,
  T>::type>
makeFile(std::shared_ptr<ExSeisPIOL> piol, const std::string& name)
{
    return std::make_unique<T>(
      piol, name, Obj::makeDefaultObj(piol, name, FileMode::Write));
}

}  // namespace File
}  // namespace PIOL

#endif
