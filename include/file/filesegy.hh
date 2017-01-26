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

namespace PIOL { namespace File {
enum class Format : int16_t;    //!< Data Format options

/*! The SEG-Y implementation of the file layer
 */
class SEGY : public Interface
{
    public :
    /*! \brief The SEG-Y options structure.
     */
    struct Opt
    {
        typedef SEGY Type;          //!< The Type of the class this structure is nested in
        unit_t incFactor;           //!< The increment factor to multiply inc by (default to SEG-Y rev 1 standard definition)

        /*! Constructor which provides the default Rules
         */
        Opt(void);
    };

    private :
    Format format;          //<! Type formats

    /*! State flags structure for File::SEGY
     */
    struct Flags
    {
        uint64_t writeHO : 1;   //!< The header should be written before File::SEGY object is deleted
        uint64_t resize : 1;    //!< The file should be resized before File::SEGY object is deleted.
        uint64_t stalent : 1;   //!< The nt value is stale and should be resynced.
    } state;                    //!< State flags are stored in this structure

    unit_t incFactor;           //!< The increment factor
    FileMode mode;              //!< The file mode (Read, Write etc)
    /*! \brief Read the text and binary header and store the metadata variables in this File::SEGY object.
     *  \param[in] fsz The size of the file in bytes
     *  \param[in, out] buf The buffer to parse. The buffer is destructively modified
     */
    void procHeader(csize_t fsz, uchar * buf);

    /*! \brief This function packs the state of the class object into the header.
     *  \param[in] buf The header object buffer
     */
    void packHeader(uchar * buf) const;

    /*! \brief This function initialises the SEGY specific portions of the class.
     *  \param[in] segyOpt The SEGY-File options
     *  \param[in] mode_    The mode of file access.
     */
    void Init(const File::SEGY::Opt & segyOpt, const FileMode mode_);
    public :
    /*! \brief The SEGY-Object class constructor.
     *  \param[in] piol_   This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_   The name of the file associated with the instantiation.
     *  \param[in] segyOpt The SEGY-File options
     *  \param[in] obj_    A shared pointer to the object layer
     *  \param[in] mode    The mode of file access.
     */
    SEGY(const Piol piol_, const std::string name_, const File::SEGY::Opt & segyOpt, std::shared_ptr<Obj::Interface> obj_, const FileMode mode = FileMode::Read);

    /*! \brief The SEGY-Object class constructor.
     *  \param[in] piol_   This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_   The name of the file associated with the instantiation.
     *  \param[in] obj_    A shared pointer to the object layer
     *  \param[in] mode    The mode of file access.
     */
    SEGY(const Piol piol_, const std::string name_, std::shared_ptr<Obj::Interface> obj_, const FileMode mode = FileMode::Read);

    /*! \brief Destructor. Processes any remaining flags
     */
    ~SEGY(void);

    size_t readNt(void);

    void writeText(const std::string text_);

    void writeNs(csize_t ns_);

    void writeNt(csize_t nt_);

    void writeInc(const geom_t inc_);

    void readTrace(csize_t offset, csize_t sz, trace_t * trace, Param * prm, csize_t skip) const;

    void writeTrace(csize_t offset, csize_t sz, trace_t * trace, const Param * prm, csize_t skip);

    void readParam(csize_t offset, csize_t sz, Param * prm, csize_t skip) const;

    void writeParam(csize_t offset, csize_t sz, const Param * prm, csize_t skip);

    void readTrace(csize_t sz, csize_t * offset, trace_t * trace, Param * prm, csize_t skip) const;

    void writeTrace(csize_t sz, csize_t * offset, trace_t * trace, const Param * prm, csize_t skip);

    void readParam(csize_t sz, csize_t * offset, Param * prm, csize_t skip) const;

    void writeParam(csize_t sz, csize_t * offset, const Param * prm, csize_t skip);
};
}}
#endif
