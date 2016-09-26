#include "global.hh"
#include "file/file.hh"
namespace PIOL {
namespace File { class Direct; }

/*! This class provides access to the ExSeisPIOL class but with a simpler API
 */
class ExSeis
{
    Piol piol;                            //!< The PIOL object.

    public :
    /*! Constructor with optional maxLevel and which initialises MPI.
     * \param[in] maxLevel The maximum log level to be recorded.
     */
    ExSeis(const Log::Verb maxLevel = Log::Verb::None);

    /*! Constructor where one can also initialise MPI optionally.
     *  \param[in] initComm Initialise MPI if true
     *  \param[in] maxLevel The maximum log level to be recorded.
     */
    ExSeis(bool initComm, const Log::Verb maxLevel = Log::Verb::None);

    /*! Cast to ExSeisPIOL *
     */
    operator ExSeisPIOL * ()
    {
        return piol.get();
    }

    /*! Cast to ExSeisPIOL shared_ptr
     */
    operator Piol ()
    {
        return piol;
    }

    /*! Shortcut to get the commrank.
     *  \return The comm rank.
     */
    size_t getRank(void)
    {
        return piol->comm->getRank();
    }
    /*! Shortcut to get the number of ranks.
     *  \return The comm number of ranks.
     */
    size_t getNumRank(void)
    {
        return piol->comm->getNumRank();
    }

    /*! Shortcut for a communication barrier
     */
    void barrier(void) const
    {
        piol->comm->barrier();
    }

    /*! \brief A function to check if an error has occured in the PIOL. If an error has occured the log is printed, the object destructor is called
     *  and the code aborts.
     * \param[in] msg A message to be printed to the log.
     */
    void isErr(const std::string msg = "") const;
};

namespace File {
/*! This class implements the C++14 File Layer API for the PIOL. It constructs the Data, Object and File layers.
 */
class Direct : public Interface
{
    std::shared_ptr<Interface> file;    //!< The pointer to the base class (polymorphic)

    public :
    /*! Constructor with options.
     *  \tparam F The nested options structure of interest for the file layer.
     *  \tparam O The nested options structure of interest for the object layer.
     *  \tparam D The nested options structure of interest for the data layer.
     *  \param[in] piol_ This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_ The name of the file associated with the instantiation.
     *  \param[in] f The File options.
     *  \param[in] o The Object options.
     *  \param[in] d The Data options.
     *  \param[in] mode The mode of file access.
     */
    template <class F, class O, class D>
    Direct(const Piol piol_, const std::string name_, const F & f, const O & o, const D & d, const FileMode mode)
                                                                 : Interface(piol_, name_, nullptr)
    {
        auto data = std::make_shared<typename D::Type>(piol, name, d, mode);
        auto obj = std::make_shared<typename O::Type>(piol, name, o, data, mode);
        file = std::make_shared<typename F::Type>(piol, name, f, obj, mode);
    }

    /*! Constructor without options.
     *  \param[in] piol_ This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_ The name of the file associated with the instantiation.
     *  \param[in] mode    The mode of file access.
     */
    Direct(const Piol piol_, const std::string name_, const FileMode mode = FileMode::Read);

    /*! Empty destructor
     */
    ~Direct(void) { }

    /*! Overload of member of pointer access
     *  \return Return the base File layer class Interface.
     */
    Interface * operator->() const
    {
        return file.get();
    }
    /*! \brief Read the human readable text from the file
     *  \return A string containing the text (in ASCII format)
     */
    const std::string & readText(void) const;

    /*! \brief Read the number of samples per trace
     *  \return The number of samples per trace
     */
    size_t readNs(void) const;

    /*! \brief Read the number of traces in the file
     *  \return The number of traces
     */
    size_t readNt(void) const;

    /*! \brief Read the number of increment between trace samples
     *  \return The increment between trace samples
     */
    geom_t readInc(void) const;

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
     *  \warning This function is not thread safe.
     */
    void writeTrace(csize_t offset, csize_t sz, trace_t * trace);

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

    /*! \brief Function to read the trace parameters from offset to offset+sz of the respective
     *  trace headers.
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process.
     *  \param[in] prm An array of the parameter structures (size sizeof(TraceParam)*sz)
     */
    void readTraceParam(csize_t offset, csize_t sz, TraceParam * prm) const;
};
}}
