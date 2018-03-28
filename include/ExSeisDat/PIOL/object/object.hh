////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief The Object layer interface
/// @details The Object layer interface is a base class which specific Obj
///          implementations work off.
////////////////////////////////////////////////////////////////////////////////
#ifndef PIOLOBJ_INCLUDE_GUARD
#define PIOLOBJ_INCLUDE_GUARD

#include "ExSeisDat/PIOL/ExSeisPIOL.hh"
#include "ExSeisDat/PIOL/anc/global.hh"

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


/*! @brief The Obj layer interface. Specific Obj implementations
 *  work off this base class.
 */
class Interface {
  protected:
    /// Pointer to the PIOL object.
    std::shared_ptr<ExSeisPIOL> piol_;

    /// Store the file name for debugging purposes.
    std::string name_;

    /// Pointer to the Data layer object (polymorphic).
    std::shared_ptr<Data::Interface> data_;

  public:
    /*! @brief The constructor.
     *  @param[in] piol A PIOL object. This PIOL ptr is not modified but is used
     *                  to instantiate another shared_ptr.
     *  @param[in] name The name of the file associated with the instantiation.
     *  @param[in] data Pointer to the Data layer object (polymorphic).
     */
    Interface(
      std::shared_ptr<ExSeisPIOL> piol,
      const std::string name,
      std::shared_ptr<Data::Interface> data) :
        piol_(piol),
        name_(name),
        data_(data)
    {
    }

    /*! @brief A virtual destructor to allow deletion.
     */
    virtual ~Interface(void) {}

    /// @brief  The stored PIOL object
    /// @return The stored PIOL object
    virtual std::shared_ptr<ExSeisPIOL> piol() { return piol_; }

    /// @brief  The stored file name
    /// @return The stored file name
    virtual std::string name() { return name_; }

    /// @brief  The stored Data layer object.
    /// @return The stored Data layer object.
    virtual std::shared_ptr<Data::Interface> data() { return data_; }

    /*! @brief Find out the file size.
     *  @return The file size in bytes.
     */
    virtual size_t getFileSz(void) const;

    /*! @brief Set the file size.
     *  @param[in] sz The size in bytes
     */
    virtual void setFileSz(const size_t sz) const;

    /*! @brief Read the header object.
     *  @param[out] ho An array which the caller guarantees is long enough
     *                 to hold the header object.
     */
    virtual void readHO(uchar* ho) const = 0;

    /*! @brief Write the header object.
     *  @param[in] ho An array which the caller guarantees is long enough to
     *                hold the header object.
     */
    virtual void writeHO(const uchar* ho) const = 0;

    /*! @brief Read a sequence of DOMDs.
     *  @param[in] offset The starting data-object we are interested in.
     *  @param[in] ns The number of elements per data field.
     *  @param[in] sz The number of DOMDs to be read in a row.
     *  @param[out] md An array which the caller guarantees is long enough for
     *                 the DO metadata.
     */
    virtual void readDOMD(
      const size_t offset,
      const size_t ns,
      const size_t sz,
      uchar* md) const = 0;

    /*! @brief Write the data-object metadata.
     *  @param[in] offset The starting data-object we are interested in.
     *  @param[in] ns The number of elements per data field.
     *  @param[in] sz The number of DOMDs to be written in a row.
     *  @param[in] md An array which the caller guarantees is long enough for
     *                the data-field.
     */
    virtual void writeDOMD(
      const size_t offset,
      const size_t ns,
      const size_t sz,
      const uchar* md) const = 0;

    /*! @brief Read a sequence of data-fields.
     *  @param[in] offset The starting data-object we are interested in.
     *  @param[in] ns The number of elements per data field.
     *  @param[in] sz The number of data-fields to be read in a row.
     *  @param[out] df An array which the caller guarantees is long enough for
     *                 the data-field.
     */
    virtual void readDODF(
      const size_t offset,
      const size_t ns,
      const size_t sz,
      uchar* df) const = 0;

    /*! @brief Write a sequence of data-fields.
     *  @param[in] offset The starting data-object we are interested in.
     *  @param[in] ns The number of elements per data field.
     *  @param[in] sz The number of data-fields to be written in a row.
     *  @param[in] df An array which the caller guarantees is long enough for
     *                the data-field.
     */
    virtual void writeDODF(
      const size_t offset,
      const size_t ns,
      const size_t sz,
      const uchar* df) const = 0;

    /*! @brief Read a sequence of data-objects.
     *  @param[in] offset The starting data-object we are interested in.
     *  @param[in] ns The number of elements per data field.
     *  @param[in] sz The number of data-objects to be read in a row.
     *  @param[out] d An array which the caller guarantees is long enough for
     *                the data-objects.
     */
    virtual void readDO(
      const size_t offset,
      const size_t ns,
      const size_t sz,
      uchar* d) const = 0;

    /*! @brief Write a sequence of data-objects.
     *  @param[in] offset The starting data-object we are interested in.
     *  @param[in] ns The number of elements per data field.
     *  @param[in] sz The number of data-objects to be written in a row.
     *  @param[in] d An array which the caller guarantees is long enough for
     *               the data-objects.
     */
    virtual void writeDO(
      const size_t offset,
      const size_t ns,
      const size_t sz,
      const uchar* d) const = 0;

    /*! @brief Read a list of data-objects.
     *  @param[in] offset An array of the starting data-objects we are
     *                    interested in
     *  @param[in] ns The number of elements per data field.
     *  @param[in] sz The number of data-objects to be read
     *  @param[out] d An array which the caller guarantees is long enough for
     *                the data-objects.
     */
    virtual void readDO(
      const size_t* offset,
      const size_t ns,
      const size_t sz,
      uchar* d) const = 0;

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
      const size_t ns,
      const size_t sz,
      const uchar* d) const = 0;

    /*! @brief read a list of data-object metadata blocks.
     *  @param[in] offset an array of the starting data-objects we are
     *                    interested in
     *  @param[in] ns the number of elements per data field.
     *  @param[in] sz the number of DOMD objects to be read
     *  @param[out] md an array which the caller guarantees is long enough for
     *  the metadata blocks.
     */
    virtual void readDOMD(
      const size_t* offset,
      const size_t ns,
      const size_t sz,
      uchar* md) const = 0;

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
      const size_t ns,
      const size_t sz,
      const uchar* md) const = 0;

    /*! @brief Read a list of data-fields.
     *  @param[in] offset An array of the starting data-objects we are
     *                    interested in
     *  @param[in] ns The number of elements per data field.
     *  @param[in] sz The number of data-fields to be read
     *  @param[out] df An array which the caller guarantees is long enough for
     *                 the data-fields.
     */
    virtual void readDODF(
      const size_t* offset,
      const size_t ns,
      const size_t sz,
      uchar* df) const = 0;

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
      const size_t ns,
      const size_t sz,
      const uchar* df) const = 0;
};

}  // namespace Obj
}  // namespace PIOL

#endif
