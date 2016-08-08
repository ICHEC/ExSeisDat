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
    /*! The Datatype (or format in SEG-Y terminology)
     */
    enum class Format : int16_t
    {
        IBM  = 1,   //<! IBM format, big endian
        TC4  = 2,   //<! Two's complement, 4 byte
        TC2  = 3,   //<! Two's complement, 2 byte
        FPG  = 4,   //<! Fixed-point gain (obsolete)
        IEEE = 5,   //<! The IEEE format, big endian
        NA1  = 6,   //<! Unused
        NA2  = 7,   //<! Unused
        TC1  = 8,   //<! Two's complement, 1 byte
    } format;       //<! Type formats

    /*! State flags structure for File::SEGY
     */
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
    void procHeader(csize_t fsz, uchar * buf);

    /*! \brief This function packs the state of the class object into the header.
     *  \param[in] buf The header object buffer
     */
    void packHeader(uchar * buf) const;

    /*! \brief This function initialises the class.
     *  \param[in] segyOpt The SEGY-File options
     */
    void Init(const File::SEGYOpt & segyOpt);

    /*! \brief The constructor used for unit testing. It does not try to create a Data object
     *  \param[in] piol_ This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_ The name of the file associated with the instantiation.
     *  \param[in] segyOpt The SEGY-File options
     *  \param[in] obj_ Pointer to the associated Obj layer object.
     */
    SEGY(const Piol piol_, const std::string name_, const File::SEGYOpt & segyOpt, const std::shared_ptr<Obj::Interface> obj_);
    public :

    /*! \brief The SEGY-Object class constructor.
     *  \param[in] piol_   This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_   The name of the file associated with the instantiation.
     *  \param[in] segyOpt The SEGY-File options
     *  \param[in] objOpt  The Object options
     *  \param[in] dataOpt The Data options
     */
    SEGY(const Piol piol_, const std::string name_, const File::SEGYOpt & segyOpt,
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
    void writeNs(csize_t ns_);

    /*! \brief Write the number of traces in the file
     *  \param[in] nt_ The new number of traces.
     */
    void writeNt(csize_t nt_);

    /*! \brief Write the number of increment between trace samples.
     *  \param[in] inc_ The new increment between trace samples.
     */
    void writeInc(const geom_t inc_);

    /*! \brief Read coordinate pairs from the ith-trace to i+sz.
     *  \param[in] item The coordinate pair of interest.
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process.
     *  \param[out] buf The buffer which is sizeof(coord_t)*sz long.
     */
    void readCoordPoint(const Coord item, csize_t offset, csize_t sz, coord_t * buf) const;

    /*! \brief Read grid pairs from the ith-trace to i+sz.
     *  \param[in] item The grid pair of interest.
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process.
     *  \param[out] buf The buffer which is sizeof(grid_t)*sz long.
     */
    void readGridPoint(const Grid item, csize_t offset, csize_t sz, grid_t * buf) const;

    /*! \brief Read the trace's from offset to offset+sz.
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process
     *  \param[out] trace A contiguous array of each trace (size sz*ns*sizeof(trace_t))
     */
    void readTrace(csize_t offset, csize_t sz, trace_t * trace) const;

    /*! \brief Read the trace's from offset to offset+sz.
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process
     *  \param[out] trace A contiguous array of each trace (size sz*ns*sizeof(trace_t))
     */
    void writeTrace(csize_t offset, csize_t sz, const trace_t * trace) const;

    /*! \brief Write the trace parameters from offset to offset+sz to the respective
     *  trace headers.
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process.
     *  \param[in] prm An array of the parameter structures (size sizeof(TraceParam)*sz)
     *
     *  \details It is assumed that this operation is not an update. Any previous
     *  contents of the trace header will be overwritten.
     */
    void writeTraceParameters(csize_t offset, csize_t sz, const TraceParam * prm) const;
};
}}
#endif
