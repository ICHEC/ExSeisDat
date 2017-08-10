/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date November 2016
 *   \brief The Set layer interface
*//*******************************************************************************************/
#ifndef PIOLSET_INCLUDE_GUARD
#define PIOLSET_INCLUDE_GUARD
#include <functional>
#include <list>
#include <map>
#include "global.hh"
#include "share/units.hh"
#include "flow/share.hh"
#include "flow/cache.hh"
#include "ops/minmax.hh"
#include "ops/sort.hh"
#include "ops/agc.hh"

//#warning temp
//#include "file/filesegy.hh"
namespace PIOL {
typedef std::function<void(const TraceBlock * in, TraceBlock * out)> Mod;  //!< Typedef for functions that have separate input and output of traces/parameters
typedef std::function<std::vector<size_t>(TraceBlock * data)> InPlaceMod;  //!< Typedef for functions that have the same input and output of traces/parameters

/*! A parent class to allow gather operations to maintain a state.
 */
struct gState
{
    /*! A virtual function which can be overridden to create the gather-operation state.
     *  \param[in] offset A list of gather-numbers to be processed by the local process.
     *  \param[in] gather The global array of gathers.
     */
    virtual void makeState(const std::vector<size_t> & offset, const Uniray<size_t, llint, llint> & gather) = 0;
};

/*! The radon state structure.
 */
struct RadonState : public gState
{
    Piol piol;                  //!< The piol object.
    std::string vmname;         //!< The name of the Velocity Model (VM) file.
    std::vector<trace_t> vtrc;  //!< Trace data read from the VM file.
    std::vector<llint> il;      //!< A list of inlines corresponding to the VM data read.
    std::vector<llint> xl;      //!< A list of crosslines corresponding to the VM data read.

    size_t vNs;                 //!< The number of samples per trace for the VM.
    size_t vBin;                //!< The binning factor to be used.
    size_t oGSz;                //!< The number of traces per gather in the angle output.
    geom_t vInc;                //!< The increment between samples in the VM file.
    geom_t oInc;                //!< The increment between samples in the output file (radians).
    /*! Constructor for the radon state.
     * \param[in] piol_ The piol object.
     * \param[in] vmname_ The VM file.
     * \param[in] vBin_ The velocity model bin parameter
     * \param[in] oGSz_  The number of traces in the angle output.
     * \param[in] oInc_ The number of increments.
     */
    RadonState(Piol piol_, std::string vmname_, csize_t vBin_, csize_t oGSz_, const geom_t oInc_)
                          : piol(piol_), vmname(vmname_), vNs(0U), vBin(vBin_), oGSz(oGSz_), vInc(geom_t(0)), oInc(oInc_) {}

    void makeState(const std::vector<size_t> & offset, const Uniray<size_t, llint, llint> & gather);
};

/*! Enum class for the various function options.
 */
enum class FuncOpt : size_t
{
//Data type dependencies
    NeedMeta,       //!< Metadata required to be read.
    NeedTrcVal,     //!< Trace values required to be read.

//Modification level:
    AddTrc,         //!< Traces are added by the operation.
    DelTrc,         //!< Traces are deleted by the operation.
    ModTrcVal,      //!< Trace values are modified by the operation.
    ModTrcLen,      //!< Trace lengths are modified by the operation.
    ModMetaVal,     //!< Metadata values are modified by the operation.
    ReorderTrc,     //!< Traces are reordered by the operation.

//Modification dependencies
    DepTrcCnt,      //!< There is a dependency on the number of traces.
    DepTrcOrder,    //!< There is a dependency on the order of traces.
    DepTrcVal,      //!< There is a dependency on the value of traces.
    DepMetaVal,     //!< There is a dependency on the metadata values.

//Comms level:
    SingleTrace,    //!< Each output trace requires info from one input trace.
    Gather,         //!< Each output trace requires info from traces in the same gather.
    SubSetOnly,     //!< Each output trace requires info from all traces in a subset of files.
    AllTraces,      //!< Each output trace requires info from all traces in the set.
    OwnIO           //!< Management of traces is complicated and custom.
};

/*! A structure to hold operation options.
 */
class OpOpt
{
    std::vector<FuncOpt> optList;       //!< A list of the function options.
    public :

    /*! Empty constructor.
     */
    OpOpt(void) { }

    /*! Initialise the options list with an initialiser list.
     *  \param[in] list An initialiser list of options.
     */
    OpOpt(std::initializer_list<FuncOpt> list) : optList(list)
    {
    }

    /*! Check if an option is present in the list.
     *  \param[in] opt The function option.
     *  \return Return true if the option is present in the list.
     */
    bool check(FuncOpt opt)
    {
        auto it = std::find(optList.begin(), optList.end(), opt);
        return it != optList.end();
    }

    /*! Add an option to the list.
     *  \param[in] opt The function option.
     */
    void add(FuncOpt opt)
    {
        optList.push_back(opt);
    }

};

/*! Operations parents. Specific classes of operations inherit from this parent
 */
struct OpParent
{
    OpOpt opt;                          //!< Operation options.
    std::shared_ptr<File::Rule> rule;   //!< Relevant parameter rules for the operation.
    std::shared_ptr<gState> state;      //!< Gather state if applicable.

    /*! Construct.
     *  \param[in] opt_ Operation options.
     *  \param[in] rule_ Rules parameter rules for the operation
     *  \param[in] state_ Gather state object if applicable.
     */
    OpParent(OpOpt & opt_, std::shared_ptr<File::Rule> rule_, std::shared_ptr<gState> state_) : opt(opt_), rule(rule_), state(state_) { }

    /*! Virtual destructor for unique_ptr polymorphism.
     */
    virtual ~OpParent(void) {}
};

/*! Template for creating a structure for a particular operation type.
 */
template <typename T>
struct Op : public OpParent
{
    T func;     //!< The particular std::function object for the operaton

    /*! Construct.
     *  \param[in] opt_ Operation options.
     *  \param[in] rule_ Rules parameter rules for the operation
     *  \param[in] state_ Gather state object if applicable.
     *  \param[in] func_ The particular std::function implementation.
     */
    Op(OpOpt & opt_, std::shared_ptr<File::Rule> rule_, std::shared_ptr<gState> state_, T func_) : OpParent(opt_, rule_, state_), func(func_)
    { }
};

//If this was C++17 then a std::variant could be used
typedef std::list<std::shared_ptr<OpParent>> FuncLst;           //!< The function list type for the set layer

/*! The internal set class
 */
class Set
{
    private :
    Piol piol;                                                  //!< The PIOL object.
    std::string outfix;                                         //!< The output prefix
    std::string outmsg;                                         //!< The output text-header message
    FileDeque file;                                             //!< A deque of unique pointers to file descriptors
    std::map<std::pair<size_t, geom_t>, FileDeque> fmap;        //!< A map of (ns, inc) key to a deque of file descriptor pointers
    std::map<std::pair<size_t, geom_t>, size_t> offmap;         //!< A map of (ns, inc) key to the current offset
    std::shared_ptr<File::Rule> rule;                           //!< Contains a pointer to the Rules for parameters
    Cache cache;                                                //!< The cache of parameters and traces
    FuncLst func;                                               //!< The list of functions and related data
    size_t rank;                                                //!< The rank of the particular process
    size_t numRank;                                             //!< The number of ranks

    /*! Drop all file descriptors without output.
     */
    void drop(void)
    {
        file.resize(0);
        fmap.clear();
        offmap.clear();
    }

    /*! Start unwinding the function list for subset-only operations based on the given iterators.
     *  \param[in] fCurr The iterator for the current function to process.
     *  \param[in] fEnd The iterator which indicates the end of the list has been reached.
     *  \return Return the final iterator reached by the last deque.
     */
    FuncLst::iterator startSubset(FuncLst::iterator fCurr, const FuncLst::iterator fEnd);

    /*! Start unwinding the function list for gather operations on the given iterators.
     *  \param[in] fCurr The iterator for the current function to process.
     *  \param[in] fEnd The iterator which indicates the end of the list has been reached.
     *  \return Return a file name if fEnd is reached. Otherwise return "".
     */
    std::string startGather(FuncLst::iterator fCurr, const FuncLst::iterator fEnd);

    /*! Start unwinding the function list for single-trace operations on the given iterators.
     *  \param[in] fCurr The iterator for the current function to process.
     *  \param[in] fEnd The iterator which indicates the end of the list has been reached.
     *  \return Return a list of all file names. Files are currently always created.
     */
    std::vector<std::string> startSingle(FuncLst::iterator fCurr, const FuncLst::iterator fEnd);

    /*! The entry point for unwinding the function list for all use-cases.
     *  \param[in] fCurr The iterator for the current function to process.
     *  \param[in] fEnd The iterator which indicates the end of the list has been reached.
     *  \return Return a list of all file names.
     */
    std::vector<std::string> calcFunc(FuncLst::iterator fCurr, const FuncLst::iterator fEnd);

    /*! The entry point for unwinding the function list for single-traces and gathers only.
     *  \param[in] fCurr The iterator for the current function to process.
     *  \param[in] fEnd The iterator which indicates the end of the list has been reached.
     *  \param[in] type The type of function currently being processed. Either single trace or gather.
     *  \param[in] bIn The input trace block which can contain traces and trace parameters.
     *  \return Return a traceblock which contains the output from the operation.
     *
     *  Transitions from gather to single trace are allowed but not the inverse.
     */
    std::unique_ptr<TraceBlock> calcFunc(FuncLst::iterator fCurr, const FuncLst::iterator fEnd, FuncOpt type, const std::unique_ptr<TraceBlock> bIn);

    /*! The entry point for unwinding the function list for subsets.
     *  \param[in] fCurr The iterator for the current function to process.
     *  \param[in] fEnd The iterator which indicates the end of the list has been reached.
     *  \param[in] fQue A deque of unique pointers to file descriptors.
     *  \return Return the final iterator reached.
     */
    FuncLst::iterator calcFuncS(FuncLst::iterator fCurr, const FuncLst::iterator fEnd, FileDeque & fQue);

    public :

    /*! Constructor
     *  \param[in] piol_ The PIOL object.
     *  \param[in] pattern The file-matching pattern
     *  \param[in] outfix_ The output file-name prefix
     *  \param[in] rule_ Contains a pointer to the rules to use for trace parameters.
     */
    Set(Piol piol_, std::string pattern, std::string outfix_,
        std::shared_ptr<File::Rule> rule_ = std::make_shared<File::Rule>(std::initializer_list<Meta>{Meta::Copy}));

    /*! Constructor
     *  \param[in] piol_ The PIOL object.
     *  \param[in] pattern The file-matching pattern
     *  \param[in] rule_ Contains a pointer to the rules to use for trace parameters.
     */
    Set(Piol piol_, std::string pattern,
        std::shared_ptr<File::Rule> rule_ = std::make_shared<File::Rule>(std::initializer_list<Meta>{Meta::Copy})) :
        Set(piol_, pattern, "", rule_)
    {}

    /*! Constructor overload
     *  \param[in] piol_ The PIOL object.
     *  \param[in] rule_ Contains a pointer to the rules to use for trace parameters.
     */
    Set(Piol piol_, std::shared_ptr<File::Rule> rule_ = std::make_shared<File::Rule>(std::initializer_list<Meta>{Meta::Copy}))
        : piol(piol_), rule(rule_), cache(piol_)
    {
        rank = piol->comm->getRank();
        numRank = piol->comm->getNumRank();
    }

    /*! Destructor
     */
    ~Set(void);

    /*! Sort the set using the given comparison function
     *  \param[in] sortFunc The comparison function
     */
    void sort(CompareP sortFunc);

    /*! Sort the set using the given comparison function
     *  \param[in] r The rules necessary for the sort.
     *  \param[in] sortFunc The comparison function.
     */
    void sort(std::shared_ptr<File::Rule> r, CompareP sortFunc);

    /*! Output using the given output prefix
     *  \param[in] oname The output prefix
     *  \return Return a vector of the actual output names.
     */
    std::vector<std::string> output(std::string oname);

    /*! Find the min and max of two given parameters (e.g x and y source coordinates) and return
     *  the associated values and trace numbers in the given structure
     *  \param[in] xlam The function for returning the first parameter
     *  \param[in] ylam The function for returning the second parameter
     *  \param[out] minmax The array of structures to hold the ouput
     */
    void getMinMax(MinMaxFunc<File::Param> xlam, MinMaxFunc<File::Param> ylam, CoordElem * minmax);

    /*! Function to add to modify function that applies a 2 tailed taper to a set of traces
     * \param[in] tapFunc Weight function for the taper ramp
     * \param[in] nTailLft Length of left tail of taper
     * \param[in] nTailRt Length of right tail of taper
     */
    void taper(TaperFunc tapFunc, size_t nTailLft, size_t nTailRt = 0);

    /*! Function to add to modify function that applies automatic gain control to a set of traces
     * \param[in] agcFunc Staistical function used to scale traces
     * \param[in] window Length of the agc window
     * \param[in] normR Value to which traces are normalized
     */
    void AGC(AGCFunc agcFunc, size_t window, trace_t normR);

    /*! Set the text-header of the output
     *  \param[in] outmsg_ The output message
     */
    void text(std::string outmsg_)
    {
        outmsg = outmsg_;
    }

    /*! Summarise the current status by whatever means the PIOL intrinsically supports
     */
    void summary(void) const;

    /*! Add a file to the set based on the File::ReadInterface
     *  \param[in] in The file interface
     */
    void add(std::unique_ptr<File::ReadInterface> in);

    /*! Add a file to the set based on the pattern/name given
     *  \param[in] name The input name or pattern
     */
    void add(std::string name);

    /*! Perform the radon to angle conversion. This assumes the input set is a radon transform file.
     *  \param[in] vmName The name of the velocity model file.
     *  \param[in] vBin The velocity model bin value.
     *  \param[in] oGSz The number of traces in the output gather.
     *  \param[in] oInc The samples per trace for the output (i.e the angle increment between samples.
     */
    void toAngle(std::string vmName, csize_t vBin, csize_t oGSz, geom_t oInc = Math::pi / geom_t(180LU));

    /************************************* Non-Core *****************************************************/
    /*! Sort the set by the specified sort type.
     *  \param[in] type The sort type
     */
    void sort(SortType type);

    /*! Get the min and the max of a set of parameters passed. This is a parallel operation. It is
     *  the collective min and max across all processes (which also must all call this file).
     *  \param[in] m1 The first parameter type
     *  \param[in] m2 The second parameter type
     *  \param[out] minmax An array of structures containing the minimum item.x,  maximum item.x, minimum item.y, maximum item.y
     *  and their respective trace numbers.
     */
    void getMinMax(Meta m1, Meta m2, CoordElem * minmax);

    /*! Perform tailed taper on a set of traces
     * \param[in] type The type of taper to be applied to traces.
     * \param[in] nTailLft The length of left-tail taper ramp.
     * \param[in] nTailRt The length of right-tail taper ramp.
     */
    void taper(TaperType type, size_t nTailLft, size_t nTailRt = 0U);

    /*! Scale traces using automatic gain control for visualization
     * \param[in] type They type of agc scaling function used
     * \param[in] window Length of the agc window
     * \param[in] normR Normalization value
     */
    void AGC(AGCType type, size_t window, trace_t normR);


    /*! Filter traces or part of traces using a IIR Butterworth filter
     * \param[in] type Type of filter (i.e. lowpass, highpass, bandpass)
     * \param[in] domain Filtering domaini
     * \param[in] pad Padding pattern
     * \param[in] fs Sampling frequency
     * \param[in] corners Passband and stopband frequency in Hz
     * \param[in] nw Size of trace filtering window
     * \param[in] winCntr Center of trace filtering window
     */
    void temporalFilter(FltrType type, FltrDmn domain, PadType pad, trace_t fs, std::vector<trace_t> corners, size_t nw = 0U, size_t winCntr = 0U);

    /*! Filter traces or part of traces using a IIR Butterworth filter
     * \param[in] type Type of filter (i.e. lowpass, highpass, bandpass)
     * \param[in] domain Filtering domain
     * \param[in] pad Padding pattern
     * \param[in] fs Sampling frequency
     * \param[in] corners Passband frequency in Hz
     * \param[in] N Filter order
     * \param[in] nw Size of trace filtering window
     * \param[in] winCntr Center of trace filtering window
     */
    void temporalFilter(FltrType type, FltrDmn domain, PadType pad, trace_t fs, size_t N, std::vector<trace_t> corners, size_t nw = 0U, size_t winCntr = 0U);

    /*! Filter traces or part of traces using a IIR Butterworth filter
     * \param[in] type Type of filter (i.e. lowpass, highpass, bandpass)
     * \param[in] domain Filtering domain
     * \param[in] pad Padding pattern
     * \param[in] fs Sampling frequency
     * \param[in] corners Passband frequency in Hz
     * \param[in] N Filter order
     * \param[in] nw Size of trace filtering window
     * \param[in] winCntr Center of trace filtering window
    */
    void temporalFilter(FltrType type, FltrDmn domain, PadType pad, trace_t fs, size_t N, trace_t corners, size_t nw = 0U, size_t winCntr = 0U)
    {
        temporalFilter(type, domain, pad, fs, N, std::vector<trace_t>{corners, 0_t}, nw, winCntr);
    }
};
}
#endif
