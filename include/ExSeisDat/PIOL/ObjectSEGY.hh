////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief The SEGY implementation of the Object layer interface
/// @details The SEGY specific implementation of the Object layer interface
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_OBJECTSEGY_HH
#define EXSEISDAT_PIOL_OBJECTSEGY_HH

#include "ExSeisDat/PIOL/ObjectInterface.hh"
#include "ExSeisDat/utils/typedefs.h"

namespace PIOL {

/*! @brief The SEG-Y Obj class.
 */
class ObjectSEGY : public ObjectInterface {
  public:
    /*! @brief The SEG-Y options structure. Currently empty.
     */
    struct Opt {
        /// The Type of the class this structure is nested in
        typedef ObjectSEGY Type;

        /*! @brief Default constructor to prevent intel warnings
         */
        Opt(void) {}
    };

    /*! @brief The ObjectSEGY class constructor.
     *  @param[in] piol_ This PIOL ptr is not modified but is used to
     *                   instantiate another shared_ptr.
     *  @param[in] name_ The name of the file associated with the instantiation.
     *  @param[in] opt_  The ObjectSEGY options
     *  @param[in] data_ Pointer to the Data layer object (polymorphic).
     *  @param[in] mode  The file mode
     */
    ObjectSEGY(
      std::shared_ptr<ExSeisPIOL> piol_,
      const std::string name_,
      const Opt& opt_,
      std::shared_ptr<DataInterface> data_,
      FileMode mode = FileMode::Read);

    /*! @brief The ObjectSEGY class constructor.
     *  @param[in] piol_ This PIOL ptr is not modified but is used to
     *                   instantiate another shared_ptr.
     *  @param[in] name_ The name of the file associated with the instantiation.
     *  @param[in] data_ Pointer to the Data layer object (polymorphic).
     *  @param[in] mode  The file mode
     */
    ObjectSEGY(
      std::shared_ptr<ExSeisPIOL> piol_,
      const std::string name_,
      std::shared_ptr<DataInterface> data_,
      FileMode mode = FileMode::Read);

    void readHO(uchar* ho) const;

    void writeHO(const uchar* ho) const;

    void readDOMD(
      const size_t offset, const size_t ns, const size_t sz, uchar* md) const;

    void writeDOMD(
      const size_t offset,
      const size_t ns,
      const size_t sz,
      const uchar* md) const;

    void readDODF(
      const size_t offset, const size_t ns, const size_t sz, uchar* df) const;

    void writeDODF(
      const size_t offset,
      const size_t ns,
      const size_t sz,
      const uchar* df) const;

    void readDO(
      const size_t offset, const size_t ns, const size_t sz, uchar* d) const;

    void writeDO(
      const size_t offset,
      const size_t ns,
      const size_t sz,
      const uchar* d) const;

    void readDO(
      const size_t* offset, const size_t ns, const size_t sz, uchar* d) const;

    void writeDO(
      const size_t* offset,
      const size_t ns,
      const size_t sz,
      const uchar* d) const;

    void readDOMD(
      const size_t* offset, const size_t ns, const size_t sz, uchar* md) const;

    void writeDOMD(
      const size_t* offset,
      const size_t ns,
      const size_t sz,
      const uchar* md) const;

    void readDODF(
      const size_t* offset, const size_t ns, const size_t sz, uchar* df) const;

    void writeDODF(
      const size_t* offset,
      const size_t ns,
      const size_t sz,
      const uchar* df) const;
};

}  // namespace PIOL

#endif  // EXSEISDAT_PIOL_OBJECTSEGY_HH
