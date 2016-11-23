#ifndef PIOLCPPFILEAPI_INCLUDE_GUARD
#define PIOLCPPFILEAPI_INCLUDE_GUARD
#include "global.hh"
#include "file/file.hh"
#include "file/dynsegymd.hh"
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

    /*! Return the maximum value amongst the processes
     *  \param[in] n The value to take part in the reduction
     *  \return Return the maximum value amongst the processes
     */
    size_t max(size_t n) const
    {
        return piol->comm->max(n);
    }

    /*! \brief A function to check if an error has occured in the PIOL. If an error has occured the log is printed, the object destructor is called
     *  and the code aborts.
     * \param[in] msg A message to be printed to the log.
     */
    void isErr(const std::string msg = "") const;
};

namespace File {
/*! \brief This structure holds all relevant trace parameters for describing a single trace
 *  excluding what is contained in the header.
 */
struct TraceParam
{
    coord_t src;    //!< The Source coordinate
    coord_t rcv;    //!< The Receiver coordinate
    coord_t cmp;    //!< The common midpoint
    grid_t line;    //!< The line coordinates (il, xl)
    size_t tn;      //!< TODO: Add unit test
};

extern const TraceParam * PRM_NULL; //!< Null parameter

/*! This class implements the C++14 File Layer API for the PIOL. It constructs the Data, Object and File layers.
 */
class Direct
{
    private :
    std::shared_ptr<Interface> file;      //!< The pointer to the base class (polymorphic)
    std::shared_ptr<Rule> rule;           //!< Rule to use internally for TraceParam calls

    public :
    /*! Constructor with options.
     *  \tparam F The nested options structure of interest for the file layer.
     *  \tparam O The nested options structure of interest for the object layer.
     *  \tparam D The nested options structure of interest for the data layer.
     *  \param[in] piol This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name The name of the file associated with the instantiation.
     *  \param[in] f The File options.
     *  \param[in] o The Object options.
     *  \param[in] d The Data options.
     *  \param[in] mode The mode of file access.
     *  \param[in] rule_ The rule set to use for TraceParam calls.
     */
    template <class F, class O, class D>
    Direct(const Piol piol, const std::string name, const F & f, const O & o, const D & d, const FileMode mode,
           std::shared_ptr<Rule> rule_ = std::make_shared<Rule>(true, true))
    {
        auto data = std::make_shared<typename D::Type>(piol, name, d, mode);
        auto obj = std::make_shared<typename O::Type>(piol, name, o, data, mode);
        file = std::make_shared<typename F::Type>(piol, name, f, obj, mode);
        rule = rule_;
    }

    /*! Constructor without options.
     *  \param[in] piol This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name The name of the file associated with the instantiation.
     *  \param[in] mode  The mode of file access.
     *  \param[in] rule_ The rule set to use for TraceParam calls.
     */
    Direct(const Piol piol, const std::string name, const FileMode mode, std::shared_ptr<Rule> rule_ = std::make_shared<Rule>(true, true));

    Direct(void) { }

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

    /*! Operator to convert to an Interface object.
     *  \return Return the internal \c Interface pointer.
     */
    operator Interface * () const
    {
        return file.get();
    }

    /*! Get the rules used by the object.
     *  \return Return a \c shared_ptr to the internal rules used.

     */
    std::shared_ptr<Rule> getRule(void) const
    {
        return rule;
    }

    /*! \brief Read the human readable text from the file.
     *  \return A string containing the text (in ASCII format).
     */
    const std::string & readText(void) const;

    /*! \brief Read the number of samples per trace
     *  \return The number of samples per trace
     */
    size_t readNs(void) const;

    /*! \brief Read the number of traces in the file
     *  \return The number of traces
     */
    size_t readNt(void);

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

    /*! \brief Write the increment between trace samples.
     *  \param[in] inc_ The new increment between trace samples.
     */
    void writeInc(const geom_t inc_);

    /*! \brief Read the traces from offset to offset+sz.
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process
     *  \param[out] trace A contiguous array of each trace (size sz*ns*sizeof(trace_t))
     *  \param[out] prm A contiguous array of the parameter structures (size sizeof(TraceParam)*sz)
     *
     *  \details When prm==PRM_NULL only the trace DF is read.
     */
    void readTrace(csize_t offset, csize_t sz, trace_t * trace, TraceParam * prm) const;

    /*! \brief Read the traces from offset to offset+sz.
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process
     *  \param[out] trace A contiguous array of each trace (size sz*ns*sizeof(trace_t))
     *  \param[out] prm The parameter structure
     *
     *  \details When prm==PRM_NULL only the trace DF is read.
     */
    void readTrace(csize_t offset, csize_t sz, trace_t * trace, Param * prm = const_cast<Param *>(PARAM_NULL)) const;

    /*! \brief Read the traces from offset to offset+sz.
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process
     *  \param[in] trace A contiguous array of each trace (size sz*ns*sizeof(trace_t))
     *  \param[in] prm A contiguous array of the parameter structures (size sizeof(TraceParam)*sz)
     *  \warning This function is not thread safe.
     *
     *  \details When prm==PRM_NULL only the trace DF is written.
     */
    void writeTrace(csize_t offset, csize_t sz, trace_t * trace, const TraceParam * prm);

    /*! \brief Read the traces from offset to offset+sz.
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process
     *  \param[in] trace A contiguous array of each trace (size sz*ns*sizeof(trace_t))
     *  \param[in] prm The parameter structure
     *  \warning This function is not thread safe.
     *
     *  \details When prm==PRM_NULL only the trace DF is written.
     */
    void writeTrace(csize_t offset, csize_t sz, trace_t * trace, const Param * prm = PARAM_NULL);

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

    /*! \brief Write the trace parameters from offset to offset+sz to the respective
     *  trace headers.
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process.
     *  \param[in] prm The parameter structure
     *
     *  \details It is assumed that this operation is not an update. Any previous
     *  contents of the trace header will be overwritten.
     */
    void writeTraceParam(csize_t offset, csize_t sz, const Param * prm);

    /*! \brief Function to read the trace parameters from offset to offset+sz of the respective
     *  trace headers.
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process.
     *  \param[in] prm An array of the parameter structures (size sizeof(TraceParam)*sz)
     */
    void readTraceParam(csize_t offset, csize_t sz, TraceParam * prm) const;

    /*! \brief Function to read the trace parameters from offset to offset+sz of the respective
     *  trace headers.
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process.
     *  \param[out] prm The parameter structure
     */
    void readTraceParam(csize_t offset, csize_t sz, Param * prm) const;

    /*! \brief Read the traces specified by the offsets in the passed offset array.
     *  \param[in] sz The number of traces to process
     *  \param[in] offset An array of trace numbers to read.
     *  \param[out] trace A contiguous array of each trace (size sz*ns*sizeof(trace_t))
     *  \param[out] prm A contiguous array of the parameter structures (size sizeof(TraceParam)*sz)
     *
     *  \details When prm==PRM_NULL only the trace DF is read.
     */
    void readTrace(csize_t sz, csize_t * offset, trace_t * trace, TraceParam * prm) const;

     /*! \brief Read the traces specified by the offsets in the passed offset array.
     *  \param[in] sz The number of traces to process
     *  \param[in] offset An array of trace numbers to read.
     *  \param[out] trace A contiguous array of each trace (size sz*ns*sizeof(trace_t))
     *  \param[out] prm The parameter structure
     *
     *  \details When prm==PRM_NULL only the trace DF is read.
     */
    void readTrace(csize_t sz, csize_t * offset, trace_t * trace, Param * prm = const_cast<Param *>(PARAM_NULL)) const;

    /*! \brief write the traces specified by the offsets in the passed offset array.
     *  \param[in] sz The number of traces to process
     *  \param[in] offset An array of trace numbers to write.
     *  \param[in] trace A contiguous array of each trace (size sz*ns*sizeof(trace_t))
     *  \param[in] prm A contiguous array of the parameter structures (size sizeof(TraceParam)*sz)
     *
     *  \details When prm==PRM_NULL only the trace DF is written.
     *  It is assumed that the parameter writing operation is not an update. Any previous
     *  contents of the trace header will be overwritten.
     */
    void writeTrace(csize_t sz, csize_t * offset, trace_t * trace, const TraceParam * prm);

    /*! \brief write the traces specified by the offsets in the passed offset array.
     *  \param[in] sz The number of traces to process
     *  \param[in] offset An array of trace numbers to write.
     *  \param[in] trace A contiguous array of each trace (size sz*ns*sizeof(trace_t))
     *  \param[in] prm The parameter structure
     *
     *  \details When prm==PRM_NULL only the trace DF is written.
     *  It is assumed that the parameter writing operation is not an update. Any previous
     *  contents of the trace header will be overwritten.
     */
    void writeTrace(csize_t sz, csize_t * offset, trace_t * trace, const Param * prm = PARAM_NULL);

    /*! \brief Read the traces specified by the offsets in the passed offset array.
     *  \param[in] sz The number of traces to process
     *  \param[in] offset An array of trace numbers to read.
     *  \param[out] prm A contiguous array of the parameter structures (size sizeof(TraceParam)*sz)
     */
    void readTraceParam(csize_t sz, csize_t * offset, TraceParam * prm) const;

    /*! \brief Read the traces specified by the offsets in the passed offset array.
     *  \param[in] sz The number of traces to process
     *  \param[in] offset An array of trace numbers to read.
     *  \param[out] prm The parameter structure
     */
    void readTraceParam(csize_t sz, csize_t * offset, Param * prm) const;

    /*! \brief write the traces specified by the offsets in the passed offset array.
     *  \param[in] sz The number of traces to process
     *  \param[in] offset An array of trace numbers to write.
     *  \param[in] prm A contiguous array of the parameter structures (size sizeof(TraceParam)*sz)
     *
     *  \details It is assumed that the parameter writing operation is not an update. Any previous
     *  contents of the trace header will be overwritten.
     */
    void writeTraceParam(csize_t sz, csize_t * offset, const TraceParam * prm);

    /*! \brief write the traces specified by the offsets in the passed offset array.
     *  \param[in] sz The number of traces to process
     *  \param[in] offset An array of trace numbers to write.
     *  \param[in] prm The parameter structure
     *
     *  \details It is assumed that the parameter writing operation is not an update. Any previous
     *  contents of the trace header will be overwritten.
     */
    void writeTraceParam(csize_t sz, csize_t * offset, const Param * prm);
};
}}
#endif
