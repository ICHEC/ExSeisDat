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
#include "object/object.hh"
#include "file/file.hh"

namespace PIOL { namespace File {
/*! \brief The SEG-Y options structure. Currently empty.
 */
struct SEGYOpt : public Opt
{
    unit_t incFactor;   //!< The increment factor to multiply inc by.
    SEGYOpt(void);
};

/*! \brief The SEG-Y File class.
 */
class SEGY : public Interface
{
    private :

    enum class Format : int16_t
    {
        IBM  = 1,   //IBM format, big endian
        TC4  = 2,   //Two's complement, 4 byte
        TC2  = 3,   //Two's complement, 2 byte
        FPG  = 4,   //Fixed-point gain (obsolete)
        IEEE = 5,   //The IEEE format, big endian
        NA1  = 6,   //Unused
        NA2  = 7,   //Unused
        TC1  = 8,   //Two's complement, 1 byte
    } format;       //Type formats

    struct Flags
    {
        uint64_t writeHO : 1;   //!< The header should be written before File::SEGY object is deleted
        uint64_t resize : 1;    //!< The file should be resized before File::SEGY object is deleted.
    } state;                    //!< State flags are stored in this structure

    unit_t incFactor;       //!< The increment factor
    /*! \brief Read the text and binary header and store the metadata variables in this File::SEGY object.
     *  \param[in] fsz The size of the file in bytes
     *  \param[in, out] The buffer to parse. The buffer is destructively modified
     */
    void procHeader(const size_t fsz, uchar * buf);

    void packHeader(uchar * buf);
    /*! \brief This function initialises the class.
     */
    void Init(const File::SEGYOpt & segyOpt);

    /*! \brief The constructor used for unit testing. It does not try to create a Data object
     *  \param[in] piol_ This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_ The name of the file associated with the instantiation.
     *  \param[in] segyOpt The SEGY-File options
     *  \param[in] obj_ Pointer to the associated Obj layer object.
     */
    SEGY(const std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const File::SEGYOpt & segyOpt, const std::shared_ptr<Obj::Interface> obj_);
    public :

    /*! \brief The SEGY-Object class constructor.
     *  \param[in] piol_   This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_   The name of the file associated with the instantiation.
     *  \param[in] segyOpt The SEGY-File options
     *  \param[in] objOpt  The Object options
     *  \param[in] dataOpt The Data options
     */
    SEGY(const std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const File::SEGYOpt & segyOpt,
                                                  const Obj::Opt & objOpt, const Data::Opt & dataOpt);

    /*! \brief Destructor. Processes any remaining flags
     */
    ~SEGY(void);

    /*! \brief Write the human readable text from the file.
     *  \param[in] text_ The new string containing the text (in ASCII format).
     */
    void writeText(const std::string text_);

    /*! \brief Write the number of samples per trace
     *  \param[in] ns_ The new number of samples per trace.
     */
    void writeNs(const size_t ns_);

    /*! \brief Write the number of traces in the file
     *  \param[in] nt_ The new number of traces.
     */
    void writeNt(const size_t nt_);

    /*! \brief Write the number of increment between trace samples.
     *  \param[in] inc_ The new increment between trace samples.
     */
    void writeInc(const geom_t inc_);
};
}}
#endif
