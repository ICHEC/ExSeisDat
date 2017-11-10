/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date Autumn 2016
 *   \brief
 *   \details Primary C++ API header
 *//*******************************************************************************************/
#ifndef PIOLCPPFILEAPI_INCLUDE_GUARD
#define PIOLCPPFILEAPI_INCLUDE_GUARD
#include "global.hh"
#include "file/file.hh"
#include "file/dynsegymd.hh"

namespace PIOL {
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

    // TODO Abstract the communicator to MPI::Comm
    /*! Don't initialise MPI. Use the supplied communicator
     *  \param[in] comm The MPI communicator
     *  \param[in] maxLevel The maximum log level to be recorded.
     */
    ExSeis(MPI_Comm comm, const Log::Verb maxLevel = Log::Verb::None);

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
/*! This class implements the C++14 File Layer API for the PIOL. It constructs the Data, Object and File layers.
 */
class ReadDirect
{
    protected :
    std::shared_ptr<ReadInterface> file;      //!< The pointer to the base class (polymorphic)

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
     */
    template <class F, class O, class D>
    ReadDirect(const Piol piol, const std::string name, const F & f, const O & o, const D & d)
    {
        auto data = std::make_shared<typename D::Type>(piol, name, d, FileMode::Read);
        auto obj = std::make_shared<typename O::Type>(piol, name, o, data, FileMode::Read);
        file = std::make_shared<typename F::Type>(piol, name, f, obj);
        if (!file)
            piol->log->record(name, Log::Layer::API, Log::Status::Error,
            "ReadInterface creation failure in ReadDirect<F,O,D>()", Log::Verb::None);
    }

    /*! Constructor without options.
     *  \param[in] piol This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name The name of the file associated with the instantiation.
     */
    ReadDirect(const Piol piol, const std::string name);

    ReadDirect(void) { }

    /*! Empty destructor
     */
    ~ReadDirect(void) { }

    /*! Overload of member of pointer access
     *  \return Return the base File layer class Interface.
     */
    ReadInterface * operator->() const
    {
        return file.get();
    }

    /*! Operator to convert to an Interface object.
     *  \return Return the internal \c Interface pointer.
     */
    operator ReadInterface * () const
    {
        return file.get();
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

    /*! \brief Read the traces from offset to offset+sz.
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process
     *  \param[out] trace A contiguous array of each trace (size sz*ns*sizeof(trace_t))
     *  \param[out] prm The parameter structure
     *
     *  \details When prm==PRM_NULL only the trace DF is read.
     */
    void readTrace(csize_t offset, csize_t sz, trace_t * trace, Param * prm = const_cast<Param *>(PARAM_NULL)) const;

    /*! \brief Function to read the trace parameters from offset to offset+sz of the respective
     *  trace headers.
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process.
     *  \param[out] prm The parameter structure
     */
    void readParam(csize_t offset, csize_t sz, Param * prm) const;

     /*! \brief Read the traces specified by the offsets in the passed offset array.
     *  \param[in] sz The number of traces to process
     *  \param[in] offset An array of trace numbers to read.
     *  \param[out] trace A contiguous array of each trace (size sz*ns*sizeof(trace_t))
     *  \param[out] prm The parameter structure
     *
     *  \details When prm==PRM_NULL only the trace DF is read.
     */
    void readTrace(csize_t sz, csize_t * offset, trace_t * trace, Param * prm = const_cast<Param *>(PARAM_NULL)) const;

    void readTraceNonMono(csize_t sz, csize_t * offset, trace_t * trace, Param * prm = const_cast<Param *>(PARAM_NULL)) const;

    /*! \brief Read the traces specified by the offsets in the passed offset array.
     *  \param[in] sz The number of traces to process
     *  \param[in] offset An array of trace numbers to read.
     *  \param[out] prm The parameter structure
     */
    void readParam(csize_t sz, csize_t * offset, Param * prm) const;
};

/*! This class implements the C++14 File Layer API for the PIOL. It constructs the Data, Object and File layers.
 */
class WriteDirect
{
    protected :
    std::shared_ptr<WriteInterface> file;      //!< The pointer to the base class (polymorphic)

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
     */
    template <class F, class O, class D>
    WriteDirect(const Piol piol, const std::string name, const F & f, const O & o, const D & d)
    {
        auto data = std::make_shared<typename D::Type>(piol, name, d, FileMode::Write);
        auto obj = std::make_shared<typename O::Type>(piol, name, o, data, FileMode::Write);
        file = std::make_shared<typename F::Type>(piol, name, f, obj);
        if (!file)
            piol->log->record(name, Log::Layer::API, Log::Status::Error,
                              "WriteInterface creation failure in WriteDirect<F,O,D>()", Log::Verb::None);
    }

    /*! Constructor without options.
     *  \param[in] piol This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name The name of the file associated with the instantiation.
     */
    WriteDirect(const Piol piol, const std::string name);

    WriteDirect(void) { }

    /*! Empty destructor
     */
    ~WriteDirect(void) { }

    /*! Overload of member of pointer access
     *  \return Return the base File layer class Interface.
     */
    WriteInterface * operator->() const
    {
        return file.get();
    }

    /*! Operator to convert to an Interface object.
     *  \return Return the internal \c Interface pointer.
     */
    operator WriteInterface * () const
    {
        return file.get();
    }

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
     *  \param[in] prm The parameter structure
     *
     *  \details It is assumed that this operation is not an update. Any previous
     *  contents of the trace header will be overwritten.
     */
    void writeParam(csize_t offset, csize_t sz, const Param * prm);

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

    /*! \brief write the traces specified by the offsets in the passed offset array.
     *  \param[in] sz The number of traces to process
     *  \param[in] offset An array of trace numbers to write.
     *  \param[in] prm The parameter structure
     *
     *  \details It is assumed that the parameter writing operation is not an update. Any previous
     *  contents of the trace header will be overwritten.
     */
    void writeParam(csize_t sz, csize_t * offset, const Param * prm);
};


class ReadModel : public ReadDirect
{
    public :
    ReadModel(const Piol piol_, const std::string name_);
    std::vector<trace_t> readModel(size_t gOffset, size_t numGather, Uniray<size_t, llint, llint> & gather);
};

}}


//
// C API Wrapper types
//
typedef std::shared_ptr<PIOL::ExSeis> PIOL_ExSeisPointer;

typedef std::shared_ptr<PIOL::File::Rule> PIOL_File_RulePointer;

typedef std::shared_ptr<PIOL::File::Param> PIOL_File_ParamPointer;

typedef std::shared_ptr<PIOL::File::ReadDirect> PIOL_File_ReadDirectPointer;

typedef std::shared_ptr<PIOL::File::WriteDirect> PIOL_File_WriteDirectPointer;


#endif
