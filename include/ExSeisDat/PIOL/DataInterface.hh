////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief   The Data layer interface
/// @details The Data layer interface is a base class which specific data I/O
///          implementations work off.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_DATAINTERFACE_HH
#define EXSEISDAT_PIOL_DATAINTERFACE_HH

#include "ExSeisDat/PIOL/ExSeisPIOL.hh"
#include "ExSeisDat/utils/typedefs.h"

namespace exseis {
namespace PIOL {

/*! @brief The Data layer interface. Specific data I/O implementations
 *  work off this base class.
 */
class DataInterface {
  protected:
    /// Pointer to the PIOL object.
    std::shared_ptr<ExSeisPIOL> piol_;

    /// For convienience
    Logger* log_;

    /// Store the file name for debugging purposes.
    const std::string name_;

  public:
    /*! @brief The constructor.
     *  @param[in] piol This PIOL ptr is not modified but is used to
     *                   instantiate another shared_ptr.
     *  @param[in] name The name of the file associated with the instantiation.
     */
    DataInterface(std::shared_ptr<ExSeisPIOL> piol, const std::string name) :
        piol_(piol),
        log_(piol->log.get()),
        name_(name)
    {
    }

    /*! @brief A virtual destructor to allow deletion.
     */
    virtual ~DataInterface(void) = default;

    /*! @brief The stored ExSeisPIOL object.
     *  @return A shared_ptr to the ExSeisPIOL object.
     */
    virtual std::shared_ptr<ExSeisPIOL> piol() { return piol_; }

    /*! @brief The stored log object.
     *  @return A pointer to the log object.
     */
    virtual const Logger* log() { return log_; }

    /*! @brief The stored file name.
     *  @return The stored file name.
     */
    virtual std::string name() { return name_; }

    /*! @brief find out the file size.
     *  @return The file size in bytes.
     */
    virtual size_t getFileSz() const = 0;

    /*! @brief set the file size.
     *  @param[in] sz The size in bytes
     */
    virtual void setFileSz(const size_t sz) const = 0;

    /*! @brief Read from storage.
     *  @param[in] offset The offset in bytes from the current internal shared
     *                    pointer
     *  @param[in] sz     The amount of data to read from disk
     *  @param[out] d     The array to store the output in
     */
    virtual void read(
      const size_t offset, const size_t sz, unsigned char* d) const = 0;

    /*! @brief Read data from storage in blocks.
     *  @param[in] offset The offset in bytes from the current internal shared
     *                    pointer
     *  @param[in] bsz    The size of a block in bytes
     *  @param[in] osz    The number of bytes between the \c start of blocks
     *  @param[in] nb     The number of blocks
     *  @param[out] d     The array to store the output in
     */
    virtual void read(
      const size_t offset,
      const size_t bsz,
      const size_t osz,
      const size_t nb,
      unsigned char* d) const = 0;

    /*! read a file where each block is determined from the list of offset
     *  @param[in] bsz    The size of a block in bytes
     *  @param[in] sz     The number of blocks to read and so the size of the
     *                    offset array
     *  @param[in] offset The list of offsets (in bytes from the current
     *                    internal shared pointer)
     *  @param[out] d     The array to store the output in
     */
    virtual void read(
      const size_t bsz,
      const size_t sz,
      const size_t* offset,
      unsigned char* d) const = 0;

    /*! write a file where each block is determined from the list of offset
     *  @param[in] bsz    The size of a block in bytes
     *  @param[in] sz     The number of blocks to write and so the size of the
     *                    offset array
     *  @param[in] offset The list of offsets (in bytes from the current
     *                    internal shared pointer)
     *  @param[in] d      The array to get the input from
     */
    virtual void write(
      const size_t bsz,
      const size_t sz,
      const size_t* offset,
      const unsigned char* d) const = 0;

    /*! @brief Write to storage.
     *  @param[in] offset The offset in bytes from the current internal shared
     *                    pointer
     *  @param[in] sz     The amount of data to write to disk
     *  @param[in] d      The array to read data output from
     */
    virtual void write(
      const size_t offset, const size_t sz, const unsigned char* d) const = 0;

    /*! @brief Write data to storage in blocks.
     *  @param[in] offset The offset in bytes from the current internal shared
     *                    pointer
     *  @param[in] bsz    The size of a block in bytes
     *  @param[in] osz    The number of bytes between the \c start of blocks
     *  @param[in] nb     The number of blocks
     *  @param[in] d      The array to read data output from
     */
    virtual void write(
      const size_t offset,
      const size_t bsz,
      const size_t osz,
      const size_t nb,
      const unsigned char* d) const = 0;
};

}  // namespace PIOL
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_DATAINTERFACE_HH
