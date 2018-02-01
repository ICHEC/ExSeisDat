////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief The SEGY specific File layer interface
/// @details SEGY implementations for File layer
////////////////////////////////////////////////////////////////////////////////
#ifndef PIOLFILESEGY_INCLUDE_GUARD
#define PIOLFILESEGY_INCLUDE_GUARD

#include "file/dynsegymd.hh"
#include "file/file.hh"
#include "file/segymd.hh"
#include "global.hh"
#include "object/object.hh"  //For the makes

#include <memory>
#include <type_traits>

namespace PIOL {
namespace File {

/// Data Format options
enum class Format : int16_t;

/*! The SEG-Y implementation of the file layer
 */
class ReadSEGY : public ReadInterface {
  public:
    /*! @brief The SEG-Y options structure.
     */
    struct Opt {
        /// The Type of the class this structure is nested in
        typedef ReadSEGY Type;
        /// The increment factor to multiply inc by (default to SEG-Y rev 1
        /// standard definition)
        unit_t incFactor;

        /*! Constructor which provides the default Rules
         */
        Opt(void);
    };

  private:
    /// Type formats
    Format format;
    /// The increment factor
    unit_t incFactor;

    /*! @brief Read the text and binary header and store the metadata variables
     *         in this SEGY object.
     *  @param[in] fsz The size of the file in bytes
     *  @param[in, out] buf The buffer to parse. The buffer is destructively
     *                      modified
     */
    void procHeader(const size_t fsz, uchar* buf);

  public:
    /*! @brief The SEGY-Object class constructor.
     *  @param[in] piol_   This PIOL ptr is not modified but is used to
     *                     instantiate another shared_ptr.
     *  @param[in] name_   The name of the file associated with the
     *                     instantiation.
     *  @param[in] segyOpt The SEGY-File options
     *  @param[in] obj_    A shared pointer to the object layer
     */
    ReadSEGY(
      std::shared_ptr<ExSeisPIOL> piol_,
      const std::string name_,
      const ReadSEGY::Opt& segyOpt,
      std::shared_ptr<Obj::Interface> obj_);

    /*! @brief The SEGY-Object class constructor.
     *  @param[in] piol_   This PIOL ptr is not modified but is used to
     *                     instantiate another shared_ptr.
     *  @param[in] name_   The name of the file associated with the
     *                     instantiation.
     *  @param[in] obj_    A shared pointer to the object layer
     */
    ReadSEGY(
      std::shared_ptr<ExSeisPIOL> piol_,
      const std::string name_,
      std::shared_ptr<Obj::Interface> obj_);

    size_t readNt(void) const;

    void readTrace(
      const size_t offset,
      const size_t sz,
      trace_t* trace,
      Param* prm        = PIOL_PARAM_NULL,
      const size_t skip = 0) const;

    void readTraceNonContiguous(
      const size_t sz,
      const size_t* offset,
      trace_t* trace,
      Param* prm        = PIOL_PARAM_NULL,
      const size_t skip = 0) const;

    void readTraceNonMonotonic(
      const size_t sz,
      const size_t* offset,
      trace_t* trace,
      Param* prm        = PIOL_PARAM_NULL,
      const size_t skip = 0) const;
};

/*! A SEGY class for velocity models
 */
class ReadSEGYModel : public Model3dInterface, public ReadSEGY {
  public:
    /*! @brief The SEG-Y Model options structure.
     */
    struct Opt : public ReadSEGY::Opt {
        /// The Type of the class this structure is nested in
        typedef ReadSEGYModel Type;
    };

    /*!
     * @param[in] piol_ The piol object.
     * @param[in] name_ The name of the file.
     * @param[in] obj_ A shared pointer for the object layer object.
     */
    ReadSEGYModel(
      std::shared_ptr<ExSeisPIOL> piol_,
      const std::string name_,
      std::shared_ptr<Obj::Interface> obj_);

    ReadSEGYModel(
      std::shared_ptr<ExSeisPIOL> piol_,
      const std::string name_,
      const ReadSEGYModel::Opt& opt,
      std::shared_ptr<Obj::Interface> obj_);

    std::vector<trace_t> readModel(
      const size_t offset,
      const size_t sz,
      const Uniray<size_t, llint, llint>& gather);

    std::vector<trace_t> readModel(
      const size_t sz,
      const size_t* offset,
      const Uniray<size_t, llint, llint>& gather);
};

/*! The SEG-Y implementation of the file layer
 */
class WriteSEGY : public WriteInterface {
  public:
    /*! @brief The SEG-Y options structure.
     */
    struct Opt {
        /// The Type of the class this structure is nested in
        typedef WriteSEGY Type;
        /// The increment factor to multiply inc by (default to SEG-Y rev 1
        /// standard definition)
        unit_t incFactor;

        /*! Constructor which provides the default Rules
         */
        Opt(void);
    };

  private:
    bool nsSet = false;

    /// Type formats
    Format format;

    /*! State flags structure for SEGY
     */
    struct Flags {
        /// The header should be written before SEGY object is deleted
        uint64_t writeHO : 1;
        /// The file should be resized before SEGY object is deleted.
        uint64_t resize : 1;
        /// The nt value is stale and should be resynced.
        uint64_t stalent : 1;
    };

    /// State flags are stored in this structure
    Flags state;

    /// The increment factor
    unit_t incFactor;

    /*! @brief This function packs the state of the class object into the
     *         header.
     *  @param[in] buf The header object buffer
     */
    void packHeader(uchar* buf) const;

    /*! @brief This function initialises the SEGY specific portions of the
     *         class.
     *  @param[in] segyOpt The SEGY-File options
     */
    void Init(const WriteSEGY::Opt& segyOpt);

    /*! Calculate the number of traces currently stored (or implied to exist).
     *  @return Return the number of traces
     */
    size_t calcNt(void);

  public:
    /*! @brief The SEGY-Object class constructor.
     *  @param[in] piol_   This PIOL ptr is not modified but is used to
     *                     instantiate another shared_ptr.
     *  @param[in] name_   The name of the file associated with the
     *                     instantiation.
     *  @param[in] segyOpt The SEGY-File options
     *  @param[in] obj_    A shared pointer to the object layer
     */
    WriteSEGY(
      std::shared_ptr<ExSeisPIOL> piol_,
      const std::string name_,
      const WriteSEGY::Opt& segyOpt,
      std::shared_ptr<Obj::Interface> obj_);

    /*! @brief The SEGY-Object class constructor.
     *  @param[in] piol_   This PIOL ptr is not modified but is used to
     *                     instantiate another shared_ptr.
     *  @param[in] name_   The name of the file associated with the
     *                     instantiation.
     *  @param[in] obj_    A shared pointer to the object layer
     */
    WriteSEGY(
      std::shared_ptr<ExSeisPIOL> piol_,
      const std::string name_,
      std::shared_ptr<Obj::Interface> obj_);

    /*! @brief Destructor. Processes any remaining flags
     */
    ~WriteSEGY(void);

    void writeText(const std::string text_);

    void writeNs(const size_t ns_);

    void writeNt(const size_t nt_);

    void writeInc(const geom_t inc_);

    void writeTrace(
      const size_t offset,
      const size_t sz,
      trace_t* trace,
      const Param* prm  = PIOL_PARAM_NULL,
      const size_t skip = 0);

    void writeTraceNonContiguous(
      const size_t sz,
      const size_t* offset,
      trace_t* trace,
      const Param* prm  = PIOL_PARAM_NULL,
      const size_t skip = 0);
};

}  // namespace File
}  // namespace PIOL

#endif
