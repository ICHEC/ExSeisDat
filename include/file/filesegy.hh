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

namespace PIOL { namespace File {
enum class Format : int16_t;
class SEGY : public Interface
{
    public :
    /*! \brief The SEG-Y options structure.
     */
    struct Opt
    {
        typedef SEGY Type;  //!< The Type of the class this structure is nested in
        unit_t incFactor;   //!< The increment factor to multiply inc by.
        /*! The constructor sets the incFactor to the SEG-Y rev 1 standard definition.
         */
        Opt(void);
    };

    private :
    Format format;  //<! Type formats

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

    /*! \brief Read the number of traces in the file
     *  \return The number of traces
     */
    size_t readNt(void);

    /*! \brief Write the human readable text from the file.
     *  \param[in] text_ The new string containing the text (in ASCII format).
     */
    void writeText(const std::string text_);

    /*! \brief Write the number of samples per trace
     *  \param[in] ns_ The new number of samples per trace.
     */
    void writeNs(csize_t ns_);

    /*! \brief Write the number of traces in the file
     *  \param[in] nt_ The new number of traces.
     */
    void writeNt(csize_t nt_);

    /*! \brief Write the number of increment between trace samples.
     *  \param[in] inc_ The new increment between trace samples.
     */
    void writeInc(const geom_t inc_);

    /*! \brief Read the trace's from offset to offset+sz.
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process
     *  \param[out] trace A contiguous array of each trace (size sz*ns*sizeof(trace_t))
     */
   void readTrace(csize_t offset, csize_t sz, trace_t * trace, TraceParam * prm) const;

    /*! \brief Read the trace's from offset to offset+sz.
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process
     *  \param[out] trace A contiguous array of each trace (size sz*ns*sizeof(trace_t))
     *  \warning This function is not thread safe.
     */
    void writeTrace(csize_t offset, csize_t sz, trace_t * trace, const TraceParam * prm);

    /*! \brief Read the trace parameters from offset to offset+sz of the respective
     *  trace headers.
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process.
     *  \param[in] prm An array of the parameter structures (size sizeof(TraceParam)*sz)
     */
    void readTraceParam(csize_t offset, csize_t sz, TraceParam * prm) const;

    /*! \brief Write the trace parameters from offset to offset+sz to the respective
     *  trace headers.
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process.
     *  \param[in] prm An array of the parameter structures (size sizeof(TraceParam)*sz)
     *
     *  \details It is assumed that this operation is not an update. Any previous
     *  contents of the trace header will be overwritten.
     */
    void writeTraceParam(csize_t offset, csize_t sz, const TraceParam * prm);

    void readTrace(csize_t sz, csize_t * offset, trace_t * trace, TraceParam * prm) const;
    void writeTrace(csize_t sz, csize_t * offset, trace_t * trace, const TraceParam * prm);
    void readTraceParam(csize_t sz, csize_t * offset, TraceParam * prm) const;
    void writeTraceParam(csize_t sz, csize_t * offset, const TraceParam * prm);
};
}}
#endif
