////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @date July 2016
/// @brief The SEGY implementation of the Object layer interface
/// @details The SEGY specific implementation of the Object layer interface
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_FILE_DETAIL_OBJECTSEGY_HH
#define EXSEISDAT_PIOL_FILE_DETAIL_OBJECTSEGY_HH

#include "exseisdat/piol/file/detail/ObjectInterface.hh"
#include "exseisdat/utils/types/typedefs.hh"

#include <memory>

namespace exseis {
namespace piol {
inline namespace file {
inline namespace detail {

/*! @brief The SEG-Y Obj class.
 */
class ObjectSEGY : public ObjectInterface {
  private:
    /// Pointer to the PIOL object.
    std::shared_ptr<ExSeisPIOL> m_piol;

    /// Store the file name for debugging purposes.
    std::string m_name;

    /// Pointer to the Data layer object (polymorphic).
    std::unique_ptr<IO_driver> m_data;

  public:
    /*! @brief The ObjectSEGY class constructor.
     *  @param[in] piol This PIOL ptr is not modified but is used to
     *                  instantiate another shared_ptr.
     *  @param[in] name The name of the file associated with the instantiation.
     *  @param[in] data Pointer to the Data layer object (polymorphic).
     */
    ObjectSEGY(
        std::shared_ptr<ExSeisPIOL> piol,
        std::string name,
        std::unique_ptr<IO_driver> data);

    std::shared_ptr<ExSeisPIOL> piol() const override;

    std::string name() const override;

    const IO_driver& data() const override;
    IO_driver& data() override;

    size_t get_file_size() const override;

    void set_file_size(size_t sz) const override;

    void read_ho(unsigned char* ho) const override;

    void should_write_file_header(const unsigned char* ho) const override;

    void read_trace_metadata(
        size_t offset, size_t ns, size_t sz, unsigned char* md) const override;

    void write_trace_metadata(
        size_t offset,
        size_t ns,
        size_t sz,
        const unsigned char* md) const override;

    void read_trace_data(
        size_t offset, size_t ns, size_t sz, unsigned char* df) const override;

    void write_trace_data(
        size_t offset,
        size_t ns,
        size_t sz,
        const unsigned char* df) const override;

    void read_trace(
        size_t offset, size_t ns, size_t sz, unsigned char* d) const override;

    void write_trace(
        size_t offset,
        size_t ns,
        size_t sz,
        const unsigned char* d) const override;

    void read_trace(
        const size_t* offset,
        size_t ns,
        size_t sz,
        unsigned char* d) const override;

    void write_trace(
        const size_t* offset,
        size_t ns,
        size_t sz,
        const unsigned char* d) const override;

    void read_trace_metadata(
        const size_t* offset,
        size_t ns,
        size_t sz,
        unsigned char* md) const override;

    void write_trace_metadata(
        const size_t* offset,
        size_t ns,
        size_t sz,
        const unsigned char* md) const override;

    void read_trace_data(
        const size_t* offset,
        size_t ns,
        size_t sz,
        unsigned char* df) const override;

    void write_trace_data(
        const size_t* offset,
        size_t ns,
        size_t sz,
        const unsigned char* df) const override;
};

}  // namespace detail
}  // namespace file
}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_FILE_DETAIL_OBJECTSEGY_HH
