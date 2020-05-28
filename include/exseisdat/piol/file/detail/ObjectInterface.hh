////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief   The Object layer interface
/// @details The Object layer interface is a base class which specific Obj
///          implementations work off.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_FILE_DETAIL_OBJECTINTERFACE_HH
#define EXSEISDAT_PIOL_FILE_DETAIL_OBJECTINTERFACE_HH

#include "exseisdat/piol/configuration/ExSeisPIOL.hh"
#include "exseisdat/piol/io_driver/IO_driver_mpi.hh"
#include "exseisdat/utils/types/typedefs.hh"

namespace exseis {
namespace piol {
inline namespace file {
inline namespace detail {

class ObjectInterface;

/*! @brief Make the default object layer object.
 *
 * @param[in] piol The piol shared object.
 * @param[in] name The name of the file.
 * @param[in] mode The filemode.
 *
 * @return Return a shared_ptr to the obj layer object.
 */
std::shared_ptr<ObjectInterface> make_default_obj(
    std::shared_ptr<ExSeisPIOL> piol, std::string name, File_mode_mpi mode);


/*! @brief The Obj layer interface. Specific Obj implementations
 *  work off this base class.
 */
class ObjectInterface {
  public:
    /*! @brief A virtual destructor to allow deletion.
     */
    virtual ~ObjectInterface() = default;

    /// @brief  The stored PIOL object
    /// @return The stored PIOL object
    virtual std::shared_ptr<ExSeisPIOL> piol() const = 0;

    /// @brief  The stored file name
    /// @return The stored file name
    virtual std::string name() const = 0;

    /// @brief  The stored Data layer object.
    /// @return The stored Data layer object.
    virtual std::shared_ptr<IO_driver> data() const = 0;

    /*! @brief Find out the file size.
     *  @return The file size in bytes.
     */
    virtual size_t get_file_size() const = 0;

    /*! @brief Set the file size.
     *  @param[in] sz The size in bytes
     */
    virtual void set_file_size(size_t sz) const = 0;

    /*! @brief Read the header object.
     *  @param[out] ho An array which the caller guarantees is long enough
     *                 to hold the header object.
     */
    virtual void read_ho(unsigned char* ho) const = 0;

    /*! @brief Write the header object.
     *  @param[in] ho An array which the caller guarantees is long enough to
     *                hold the header object.
     */
    virtual void should_write_file_header(const unsigned char* ho) const = 0;

    /*! @brief Read a sequence of Trace Metadatas.
     *  @param[in] offset   The starting Trace we are interested in.
     *  @param[in] ns       The number of elements per Trace Data.
     *  @param[in] sz       The number of Trace Metadatas to be read in a row.
     *  @param[out] md      An array which the caller guarantees is long enough
     *                      for the Trace Metadatas.
     *                      (pointer to array of size `(trace metadata size) *
     *                      sz`)
     */
    virtual void read_trace_metadata(
        size_t offset, size_t ns, size_t sz, unsigned char* md) const = 0;

    /*! @brief Write the Trace Metadata.
     *  @param[in] offset   The starting Trace we are interested in.
     *  @param[in] ns       The number of elements per Trace Data.
     *  @param[in] sz       The number of Trace Metadatas to be written in a row.
     *  @param[in] md       An array which the caller guarantees is long enough
     *                      for the Trace Data.
     *                      (pointer to array of size `(trace metadata size) *
     *                      sz`)
     */
    virtual void write_trace_metadata(
        size_t offset, size_t ns, size_t sz, const unsigned char* md) const = 0;

    /*! @brief Read a sequence of Trace Datas.
     *  @param[in] offset   The starting Trace we are interested in.
     *  @param[in] ns       The number of elements per Trace Data.
     *  @param[in] sz       The number of Trace Datas to be read in a row.
     *  @param[out] df      An array which the caller guarantees is long enough
     *                      for the Trace Data.
     *                      (pointer to array of size `(trace data size) * sz`)
     */
    virtual void read_trace_data(
        size_t offset, size_t ns, size_t sz, unsigned char* df) const = 0;

    /*! @brief Write a sequence of Trace Datas.
     *  @param[in] offset   The starting Trace we are interested in.
     *  @param[in] ns       The number of elements per Trace Data.
     *  @param[in] sz       The number of Trace Datas to be written in a row.
     *  @param[in] df       An array which the caller guarantees is long enough
     *                      for the Trace Data.
     *                      (pointer to array of size `(trace data size) * sz`)
     */
    virtual void write_trace_data(
        size_t offset, size_t ns, size_t sz, const unsigned char* df) const = 0;

    /*! @brief Read a sequence of Traces.
     *  @param[in] offset   The starting Trace we are interested in.
     *  @param[in] ns       The number of elements per Trace Data.
     *  @param[in] sz       The number of Traces to be read in a row.
     *  @param[out] d       An array which the caller guarantees is long enough
     *                      for the Traces.
     *                      (pointer to array of size `(trace size) * sz`)
     */
    virtual void read_trace(
        size_t offset, size_t ns, size_t sz, unsigned char* d) const = 0;

    /*! @brief Write a sequence of Traces.
     *  @param[in] offset   The starting Trace we are interested in.
     *  @param[in] ns       The number of elements per Trace Data.
     *  @param[in] sz       The number of Traces to be written in a row.
     *  @param[in] d        An array which the caller guarantees is long enough
     *                      for the Traces.
     *                      (pointer to array of size `(trace size) * sz`)
     */
    virtual void write_trace(
        size_t offset, size_t ns, size_t sz, const unsigned char* d) const = 0;

    /*! @brief Read a list of Traces.
     *  @param[in] offset   An array of the Traces we are interested in.
     *  @param[in] ns       The number of elements per Trace.
     *  @param[in] sz       The number of Traces to be read
     *  @param[out] d       An array which the caller guarantees is long enough
     *                      for the Traces.
     *                      (pointer to array of size `(trace size) * sz`)
     */
    virtual void read_trace(
        const size_t* offset, size_t ns, size_t sz, unsigned char* d) const = 0;

    /*! @brief Write a list of Traces.
     *  @param[in] offset   An array of the Traces we are interested in.
     *  @param[in] ns       The number of elements per Trace.
     *  @param[in] sz       The number of Traces to be written.
     *  @param[in] d        An array which the caller guarantees is long enough
     *                      for the Traces.
     *                      (pointer to array of size `(trace size) * sz`)
     */
    virtual void write_trace(
        const size_t* offset,
        size_t ns,
        size_t sz,
        const unsigned char* d) const = 0;

    /*! @brief read a list of Trace Metadata blocks.
     *  @param[in]  offset  An array of the Traces we are interested in.
     *  @param[in]  ns      the number of elements per Trace Data.
     *  @param[in]  sz      the number of Trace Metadatas to be read.
     *  @param[out] md      An array which the caller guarantees is long enough
     *                      for the metadata blocks.
     *                      (pointer to array of size `(trace metadata size) *
     *                      sz`)
     */
    virtual void read_trace_metadata(
        const size_t* offset,
        size_t ns,
        size_t sz,
        unsigned char* md) const = 0;

    /*! @brief Write a list of Trace Metadata blocks.
     *  @param[in] offset   An array of the Traces we are interested in.
     *  @param[in] ns       The number of elements per Trace Data.
     *  @param[in] sz       The number of Trace Metadata objects to be written.
     *  @param[in] md       An array which the caller guarantees is long enough
     *                      for the metadata blocks.
     *                      (pointer to array of size `(trace metadata size) *
     *                      sz`)
     */
    virtual void write_trace_metadata(
        const size_t* offset,
        size_t ns,
        size_t sz,
        const unsigned char* md) const = 0;

    /*! @brief Read a list of Trace Datas.
     *  @param[in] offset   An array of the Traces we are interested in
     *  @param[in] ns       The number of elements per Trace Data.
     *  @param[in] sz       The number of Trace Datas to be read
     *  @param[out] df      An array which the caller guarantees is long enough
     *                      for the Trace Datas.
     *                      (pointer to array of size `(trace data size) * sz`)
     */
    virtual void read_trace_data(
        const size_t* offset,
        size_t ns,
        size_t sz,
        unsigned char* df) const = 0;

    /*! @brief Write a list of Trace Datas
     *  @param[in] offset   An array of the Traces we are interested in.
     *  @param[in] ns       The number of elements per Trace Data.
     *  @param[in] sz       The number of Trace Datas to be written.
     *  @param[in] df       An array which the caller guarantees is long enough
     *                      for the Trace Datas.
     *                      (pointer to array of size `(trace data size) * sz`)
     */
    virtual void write_trace_data(
        const size_t* offset,
        size_t ns,
        size_t sz,
        const unsigned char* df) const = 0;
};

}  // namespace detail
}  // namespace file
}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_FILE_DETAIL_OBJECTINTERFACE_HH
