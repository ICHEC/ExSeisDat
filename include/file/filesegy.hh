/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief The SEGY specific File layer interface
 *   \details SEGY implementations for File layer
*//*******************************************************************************************/
#ifndef PIOLFILESEGY_INCLUDE_GUARD
#define PIOLFILESEGY_INCLUDE_GUARD
#include <memory>
#include "global.hh"
#include "file/file.hh"
#include "file/segymd.hh"
#include "file/dynsegymd.hh"
#include "object/object.hh" //For the makes
namespace PIOL { namespace File {
enum class Format : int16_t;    //!< Data Format options

/*! A template function to create a read-only file object with default object & data layer settings
 * \tparam T The type of the file layer
 * \param[in] piol The piol shared object
 * \param[in] name The name of the file
 * \return Return a pointer of the respective file type.
 */
template <class T>
std::unique_ptr<T> makeReadSEGYFile(Piol piol, const std::string name)
{
    auto obj = Obj::makeDefaultObj(piol, name, FileMode::Read);
    auto file = std::make_unique<T>(piol, name, obj);
    return std::move(file);
}

/*! A template function to create a write-only file object with default object & data layer settings
 * \tparam T The type of the file layer
 * \param[in] piol The piol shared object
 * \param[in] name The name of the file
 * \return Return a pointer of the respective file type.
 */
template <class T>
std::unique_ptr<T> makeWriteSEGYFile(Piol piol, const std::string name)
{
    auto obj = Obj::makeDefaultObj(piol, name, FileMode::Write);
    auto file = std::make_unique<T>(piol, name, obj);
    return std::move(file);
}

/*! The SEG-Y implementation of the file layer
 */
class ReadSEGY : public ReadInterface
{
    public :
    /*! \brief The SEG-Y options structure.
     */
    struct Opt
    {
        typedef ReadSEGY Type;  //!< The Type of the class this structure is nested in
        unit_t incFactor;       //!< The increment factor to multiply inc by (default to SEG-Y rev 1 standard definition)

        /*! Constructor which provides the default Rules
         */
        Opt(void);
    };

    private :
    Format format;              //<! Type formats
    unit_t incFactor;           //!< The increment factor

    /*! \brief Read the text and binary header and store the metadata variables in this SEGY object.
     *  \param[in] fsz The size of the file in bytes
     *  \param[in, out] buf The buffer to parse. The buffer is destructively modified
     */
    void procHeader(csize_t fsz, uchar * buf);

    /*! \brief This function initialises the SEGY specific portions of the class.
     *  \param[in] segyOpt The SEGY-File options
     */
    void Init(const ReadSEGY::Opt & opt);

    public :
    /*! \brief The SEGY-Object class constructor.
     *  \param[in] piol_   This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_   The name of the file associated with the instantiation.
     *  \param[in] segyOpt The SEGY-File options
     *  \param[in] obj_    A shared pointer to the object layer
     */
    ReadSEGY(const Piol piol_, const std::string name_, const ReadSEGY::Opt & segyOpt, std::shared_ptr<Obj::Interface> obj_);

    /*! \brief The SEGY-Object class constructor.
     *  \param[in] piol_   This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_   The name of the file associated with the instantiation.
     *  \param[in] obj_    A shared pointer to the object layer
     */
    ReadSEGY(const Piol piol_, const std::string name_, std::shared_ptr<Obj::Interface> obj_);

    size_t readNt(void);

    void readTrace(csize_t offset, csize_t sz, trace_t * trace, Param * prm = const_cast<Param *>(PARAM_NULL), csize_t skip = 0) const;

    void readTrace(csize_t sz, csize_t * offset, trace_t * trace, Param * prm = const_cast<Param *>(PARAM_NULL), csize_t skip = 0) const;

    void readTraceNonMono(csize_t sz, csize_t * offset, trace_t * trace, Param * prm = const_cast<Param *>(PARAM_NULL), csize_t skip = 0) const;
};

/*! A SEGY class for velocity models
 */
class ReadSEGYModel : public Model3dInterface, public ReadSEGY
{
    public :
    /*!
     \param[in] piol_ The piol object.
     \param[in] name The name of the file.
     \param[in] obj_ A shared pointer for the object layer object.
     */
    ReadSEGYModel(const Piol piol_, const std::string name_, std::shared_ptr<Obj::Interface> obj_);
    std::vector<trace_t> readModel(size_t gOffset, size_t numGather, const Uniray<size_t, llint, llint> & gather);
    std::vector<trace_t> readModel(csize_t sz, csize_t * offset, const Uniray<size_t, llint, llint> & gather);
};

/*! The SEG-Y implementation of the file layer
 */
class WriteSEGY : public WriteInterface
{
    public :
    /*! \brief The SEG-Y options structure.
     */
    struct Opt
    {
        typedef WriteSEGY Type; //!< The Type of the class this structure is nested in
        unit_t incFactor;       //!< The increment factor to multiply inc by (default to SEG-Y rev 1 standard definition)

        /*! Constructor which provides the default Rules
         */
        Opt(void);
    };

    private :
    bool nsSet = false;

    Format format;              //<! Type formats

    /*! State flags structure for SEGY
     */
    struct Flags
    {
        uint64_t writeHO : 1;   //!< The header should be written before SEGY object is deleted
        uint64_t resize : 1;    //!< The file should be resized before SEGY object is deleted.
        uint64_t stalent : 1;   //!< The nt value is stale and should be resynced.
    } state;                    //!< State flags are stored in this structure

    unit_t incFactor;           //!< The increment factor

    /*! \brief This function packs the state of the class object into the header.
     *  \param[in] buf The header object buffer
     */
    void packHeader(uchar * buf) const;

    /*! \brief This function initialises the SEGY specific portions of the class.
     *  \param[in] segyOpt The SEGY-File options
     */
    void Init(const WriteSEGY::Opt & segyOpt);

    /*! Calculate the number of traces currently stored (or implied to exist).
     *  \return Return the number of traces
     */
    size_t calcNt(void);

    public :
    /*! \brief The SEGY-Object class constructor.
     *  \param[in] piol_   This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_   The name of the file associated with the instantiation.
     *  \param[in] segyOpt The SEGY-File options
     *  \param[in] obj_    A shared pointer to the object layer
     */
    WriteSEGY(const Piol piol_, const std::string name_, const WriteSEGY::Opt & segyOpt, std::shared_ptr<Obj::Interface> obj_);

    /*! \brief The SEGY-Object class constructor.
     *  \param[in] piol_   This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_   The name of the file associated with the instantiation.
     *  \param[in] obj_    A shared pointer to the object layer
     */
    WriteSEGY(const Piol piol_, const std::string name_, std::shared_ptr<Obj::Interface> obj_);

    /*! \brief Destructor. Processes any remaining flags
     */
    ~WriteSEGY(void);

    void writeText(const std::string text_);

    void writeNs(csize_t ns_);

    void writeNt(csize_t nt_);

    void writeInc(const geom_t inc_);

    void writeTrace(csize_t offset, csize_t sz, trace_t * trace, const Param * prm = PARAM_NULL, csize_t skip = 0);

    void writeTrace(csize_t sz, csize_t * offset, trace_t * trace, const Param * prm = PARAM_NULL, csize_t skip = 0);
};
}}
#endif
