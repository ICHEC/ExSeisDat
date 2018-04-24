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

namespace exseis {
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
      std::string name_,
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
      std::string name_,
      std::shared_ptr<DataInterface> data_,
      FileMode mode = FileMode::Read);

    void readHO(unsigned char* ho) const;

    void writeHO(const unsigned char* ho) const;

    void readDOMD(size_t offset, size_t ns, size_t sz, unsigned char* md) const;

    void writeDOMD(
      size_t offset, size_t ns, size_t sz, const unsigned char* md) const;

    void readDODF(size_t offset, size_t ns, size_t sz, unsigned char* df) const;

    void writeDODF(
      size_t offset, size_t ns, size_t sz, const unsigned char* df) const;

    void readDO(size_t offset, size_t ns, size_t sz, unsigned char* d) const;

    void writeDO(
      size_t offset, size_t ns, size_t sz, const unsigned char* d) const;

    void readDO(
      const size_t* offset, size_t ns, size_t sz, unsigned char* d) const;

    void writeDO(
      const size_t* offset, size_t ns, size_t sz, const unsigned char* d) const;

    void readDOMD(
      const size_t* offset, size_t ns, size_t sz, unsigned char* md) const;

    void writeDOMD(
      const size_t* offset,
      size_t ns,
      size_t sz,
      const unsigned char* md) const;

    void readDODF(
      const size_t* offset, size_t ns, size_t sz, unsigned char* df) const;

    void writeDODF(
      const size_t* offset,
      size_t ns,
      size_t sz,
      const unsigned char* df) const;
};

}  // namespace PIOL
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_OBJECTSEGY_HH
