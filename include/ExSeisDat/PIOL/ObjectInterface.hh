////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief   The Object layer interface
/// @details The Object layer interface is a base class which specific Obj
///          implementations work off.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_OBJECTINTERFACE_HH
#define EXSEISDAT_PIOL_OBJECTINTERFACE_HH

#include "ExSeisDat/PIOL/DataMPIIO.hh"
#include "ExSeisDat/PIOL/ExSeisPIOL.hh"
#include "ExSeisDat/utils/typedefs.h"

namespace exseis {
namespace PIOL {

class ObjectInterface;

/*! Make the default object layer object.
 * @param[in] piol The piol shared object.
 * @param[in] name The name of the file.
 * @param[in] mode The filemode.
 * @return Return a shared_ptr to the obj layer object.
 */
std::shared_ptr<ObjectInterface> makeDefaultObj(
  std::shared_ptr<ExSeisPIOL> piol, std::string name, FileMode mode);


/*! @brief The Obj layer interface. Specific Obj implementations
 *  work off this base class.
 */
class ObjectInterface {
  protected:
    /// Pointer to the PIOL object.
    std::shared_ptr<ExSeisPIOL> piol_;

    /// Store the file name for debugging purposes.
    std::string name_;

    /// Pointer to the Data layer object (polymorphic).
    std::shared_ptr<DataInterface> data_;

  public:
    /*! @brief The constructor.
     *  @param[in] piol A PIOL object. This PIOL ptr is not modified but is used
     *                  to instantiate another shared_ptr.
     *  @param[in] name The name of the file associated with the instantiation.
     *  @param[in] data Pointer to the Data layer object (polymorphic).
     */
    ObjectInterface(
      std::shared_ptr<ExSeisPIOL> piol,
      std::string name,
      std::shared_ptr<DataInterface> data) :
        piol_(piol),
        name_(name),
        data_(data)
    {
    }

    /*! @brief A virtual destructor to allow deletion.
     */
    virtual ~ObjectInterface(void) {}

    /// @brief  The stored PIOL object
    /// @return The stored PIOL object
    virtual std::shared_ptr<ExSeisPIOL> piol() { return piol_; }

    /// @brief  The stored file name
    /// @return The stored file name
    virtual std::string name() { return name_; }

    /// @brief  The stored Data layer object.
    /// @return The stored Data layer object.
    virtual std::shared_ptr<DataInterface> data() { return data_; }

    /*! @brief Find out the file size.
     *  @return The file size in bytes.
     */
    virtual size_t getFileSz() const;

    /*! @brief Set the file size.
     *  @param[in] sz The size in bytes
     */
    virtual void setFileSz(size_t sz) const;

    /*! @brief Read the header object.
     *  @param[out] ho An array which the caller guarantees is long enough
     *                 to hold the header object.
     */
    virtual void readHO(unsigned char* ho) const = 0;

    /*! @brief Write the header object.
     *  @param[in] ho An array which the caller guarantees is long enough to
     *                hold the header object.
     */
    virtual void writeHO(const unsigned char* ho) const = 0;

    /*! @brief Read a sequence of DOMDs.
     *  @param[in] offset The starting data-object we are interested in.
     *  @param[in] ns The number of elements per data field.
     *  @param[in] sz The number of DOMDs to be read in a row.
     *  @param[out] md An array which the caller guarantees is long enough for
     *                 the DO metadata.
     */
    virtual void readDOMD(
      size_t offset, size_t ns, size_t sz, unsigned char* md) const = 0;

    /*! @brief Write the data-object metadata.
     *  @param[in] offset The starting data-object we are interested in.
     *  @param[in] ns The number of elements per data field.
     *  @param[in] sz The number of DOMDs to be written in a row.
     *  @param[in] md An array which the caller guarantees is long enough for
     *                the data-field.
     */
    virtual void writeDOMD(
      size_t offset, size_t ns, size_t sz, const unsigned char* md) const = 0;

    /*! @brief Read a sequence of data-fields.
     *  @param[in] offset The starting data-object we are interested in.
     *  @param[in] ns The number of elements per data field.
     *  @param[in] sz The number of data-fields to be read in a row.
     *  @param[out] df An array which the caller guarantees is long enough for
     *                 the data-field.
     */
    virtual void readDODF(
      size_t offset, size_t ns, size_t sz, unsigned char* df) const = 0;

    /*! @brief Write a sequence of data-fields.
     *  @param[in] offset The starting data-object we are interested in.
     *  @param[in] ns The number of elements per data field.
     *  @param[in] sz The number of data-fields to be written in a row.
     *  @param[in] df An array which the caller guarantees is long enough for
     *                the data-field.
     */
    virtual void writeDODF(
      size_t offset, size_t ns, size_t sz, const unsigned char* df) const = 0;

    /*! @brief Read a sequence of data-objects.
     *  @param[in] offset The starting data-object we are interested in.
     *  @param[in] ns The number of elements per data field.
     *  @param[in] sz The number of data-objects to be read in a row.
     *  @param[out] d An array which the caller guarantees is long enough for
     *                the data-objects.
     */
    virtual void readDO(
      size_t offset, size_t ns, size_t sz, unsigned char* d) const = 0;

    /*! @brief Write a sequence of data-objects.
     *  @param[in] offset The starting data-object we are interested in.
     *  @param[in] ns The number of elements per data field.
     *  @param[in] sz The number of data-objects to be written in a row.
     *  @param[in] d An array which the caller guarantees is long enough for
     *               the data-objects.
     */
    virtual void writeDO(
      size_t offset, size_t ns, size_t sz, const unsigned char* d) const = 0;

    /*! @brief Read a list of data-objects.
     *  @param[in] offset An array of the starting data-objects we are
     *                    interested in
     *  @param[in] ns The number of elements per data field.
     *  @param[in] sz The number of data-objects to be read
     *  @param[out] d An array which the caller guarantees is long enough for
     *                the data-objects.
     */
    virtual void readDO(
      const size_t* offset, size_t ns, size_t sz, unsigned char* d) const = 0;

    /*! @brief Write a list of data-objects.
     *  @param[in] ns The number of elements per data field.
     *  @param[in] sz The number of data-objects to be written.
     *  @param[in] offset An array of the starting data-object we are interested
     *                    in.
     *  @param[in] d An array which the caller guarantees is long enough for
     *               the data-objects.
     */
    virtual void writeDO(
      const size_t* offset,
      size_t ns,
      size_t sz,
      const unsigned char* d) const = 0;

    /*! @brief read a list of data-object metadata blocks.
     *  @param[in] offset an array of the starting data-objects we are
     *                    interested in
     *  @param[in] ns the number of elements per data field.
     *  @param[in] sz the number of DOMD objects to be read
     *  @param[out] md an array which the caller guarantees is long enough for
     *  the metadata blocks.
     */
    virtual void readDOMD(
      const size_t* offset, size_t ns, size_t sz, unsigned char* md) const = 0;

    /*! @brief Write a list of data-object metadata blocks.
     *  @param[in] offset An array of the starting data-object we are interested
     *                    in.
     *  @param[in] ns The number of elements per data field.
     *  @param[in] sz The number of DOMD objects to be written.
     *  @param[in] md An array which the caller guarantees is long enough for
     *                the metadata blocks.
     */
    virtual void writeDOMD(
      const size_t* offset,
      size_t ns,
      size_t sz,
      const unsigned char* md) const = 0;

    /*! @brief Read a list of data-fields.
     *  @param[in] offset An array of the starting data-objects we are
     *                    interested in
     *  @param[in] ns The number of elements per data field.
     *  @param[in] sz The number of data-fields to be read
     *  @param[out] df An array which the caller guarantees is long enough for
     *                 the data-fields.
     */
    virtual void readDODF(
      const size_t* offset, size_t ns, size_t sz, unsigned char* df) const = 0;

    /*! @brief Write a list of data-fields
     *  @param[in] offset An array of the starting data-object we are interested
     *                    in.
     *  @param[in] ns The number of elements per data field.
     *  @param[in] sz The number of data-fields to be written.
     *  @param[in] df An array which the caller guarantees is long enough for
     *                the data-fields.
     */
    virtual void writeDODF(
      const size_t* offset,
      size_t ns,
      size_t sz,
      const unsigned char* df) const = 0;
};

}  // namespace PIOL
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_OBJECTINTERFACE_HH
