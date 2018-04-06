////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The Set layer interface
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_SET_HH
#define EXSEISDAT_FLOW_SET_HH

#include "ExSeisDat/Flow/Cache.hh"
#include "ExSeisDat/Flow/FileDesc.hh"
#include "ExSeisDat/Flow/OpParent.hh"

#include "ExSeisDat/PIOL/constants.hh"
#include "ExSeisDat/PIOL/operations/agc.hh"
#include "ExSeisDat/PIOL/operations/minmax.h"
#include "ExSeisDat/PIOL/operations/sort.hh"
#include "ExSeisDat/PIOL/operations/taper.hh"
#include "ExSeisDat/PIOL/operations/temporalfilter.hh"

#include <deque>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace PIOL {

/*! The internal set class
 */
class Set {
  public:
    /// Typedef for passing in a list of FileDesc objects.
    typedef std::deque<std::shared_ptr<FileDesc>> FileDeque;

    /// The function list type for the set layer
    typedef std::list<std::shared_ptr<OpParent>> FuncLst;

  protected:
    /// The PIOL object.
    std::shared_ptr<ExSeisPIOL> piol;

    /// The output prefix
    std::string outfix;

    /// The output text-header message
    std::string outmsg;

    /// A deque of unique pointers to file descriptors
    FileDeque file;

    /// A map of (ns, inc) key to a deque of file descriptor pointers
    std::map<std::pair<size_t, geom_t>, FileDeque> fmap;

    /// A map of (ns, inc) key to the current offset
    std::map<std::pair<size_t, geom_t>, size_t> offmap;

    /// Contains a pointer to the Rules for parameters
    std::shared_ptr<Rule> rule;

    /// The cache of parameters and traces
    Cache cache;

    /// The list of functions and related data
    FuncLst func;

    /// The rank of the particular process
    size_t rank;

    /// The number of ranks
    size_t numRank;

    /*! Drop all file descriptors without output.
     */
    void drop(void)
    {
        file.resize(0);
        fmap.clear();
        offmap.clear();
    }

    /*! Start unwinding the function list for subset-only operations based on
     *  the given iterators.
     *  @param[in] fCurr The iterator for the current function to process.
     *  @param[in] fEnd The iterator which indicates the end of the list has
     *             been reached.
     *  @return Return the final iterator reached by the last deque.
     */
    FuncLst::iterator startSubset(
      FuncLst::iterator fCurr, const FuncLst::iterator fEnd);

    /*! Start unwinding the function list for gather operations on the given
     *  iterators.
     *  @param[in] fCurr The iterator for the current function to process.
     *  @param[in] fEnd The iterator which indicates the end of the list has
     *             been reached.
     *  @return Return a file name if fEnd is reached. Otherwise return "".
     */
    std::string startGather(
      FuncLst::iterator fCurr, const FuncLst::iterator fEnd);

    /*! Start unwinding the function list for single-trace operations on the
     *  given iterators.
     *  @param[in] fCurr The iterator for the current function to process.
     *  @param[in] fEnd The iterator which indicates the end of the list has
     *             been reached.
     *  @return Return a list of all file names. Files are currently always
     *          created.
     */
    std::vector<std::string> startSingle(
      FuncLst::iterator fCurr, const FuncLst::iterator fEnd);

    /*! The entry point for unwinding the function list for all use-cases.
     *  @param[in] fCurr The iterator for the current function to process.
     *  @param[in] fEnd The iterator which indicates the end of the list has
     *             been reached.
     *  @return Return a list of all file names.
     */
    std::vector<std::string> calcFunc(
      FuncLst::iterator fCurr, const FuncLst::iterator fEnd);

    /*! The entry point for unwinding the function list for single-traces and
     *  gathers only.
     *  @param[in] fCurr The iterator for the current function to process.
     *  @param[in] fEnd The iterator which indicates the end of the list has
     *             been reached.
     *  @param[in] type The type of function currently being processed. Either
     *             single trace or gather.
     *  @param[in] bIn The input trace block which can contain traces and trace
     *             parameters.
     *  @return Return a traceblock which contains the output from the
     *          operation.
     *
     *  Transitions from gather to single trace are allowed but not the inverse.
     */
    std::unique_ptr<TraceBlock> calcFunc(
      FuncLst::iterator fCurr,
      const FuncLst::iterator fEnd,
      FuncOpt type,
      const std::unique_ptr<TraceBlock> bIn);

    /*! The entry point for unwinding the function list for subsets.
     *  @param[in] fCurr The iterator for the current function to process.
     *  @param[in] fEnd The iterator which indicates the end of the list has
     *             been reached.
     *  @param[in] fQue A deque of unique pointers to file descriptors.
     *  @return Return the final iterator reached.
     */
    FuncLst::iterator calcFuncS(
      FuncLst::iterator fCurr, const FuncLst::iterator fEnd, FileDeque& fQue);

  public:
    /*! Constructor
     *  @param[in] piol_ The PIOL object.
     *  @param[in] pattern The file-matching pattern
     *  @param[in] outfix_ The output file-name prefix
     *  @param[in] rule_ Contains a pointer to the rules to use for trace
     *             parameters.
     */
    Set(
      std::shared_ptr<ExSeisPIOL> piol_,
      std::string pattern,
      std::string outfix_,
      std::shared_ptr<Rule> rule_ =
        std::make_shared<Rule>(std::initializer_list<Meta>{PIOL_META_COPY}));

    /*! Constructor
     *  @param[in] piol_ The PIOL object.
     *  @param[in] pattern The file-matching pattern
     *  @param[in] rule_ Contains a pointer to the rules to use for trace
     *             parameters.
     */
    Set(
      std::shared_ptr<ExSeisPIOL> piol_,
      std::string pattern,
      std::shared_ptr<Rule> rule_ =
        std::make_shared<Rule>(std::initializer_list<Meta>{PIOL_META_COPY})) :
        Set(piol_, pattern, "", rule_)
    {
    }

    /*! Constructor overload
     *  @param[in] piol_ The PIOL object.
     *  @param[in] rule_ Contains a pointer to the rules to use for trace
     *             parameters.
     */
    Set(
      std::shared_ptr<ExSeisPIOL> piol_,
      std::shared_ptr<Rule> rule_ =
        std::make_shared<Rule>(std::initializer_list<Meta>{PIOL_META_COPY}));

    /*! Destructor
     */
    ~Set(void);

    /*! Sort the set using the given comparison function
     *  @param[in] sortFunc The comparison function
     */
    void sort(CompareP sortFunc);

    /*! Sort the set using the given comparison function
     *  @param[in] r The rules necessary for the sort.
     *  @param[in] sortFunc The comparison function.
     */
    void sort(std::shared_ptr<Rule> r, CompareP sortFunc);

    /*! Output using the given output prefix
     *  @param[in] oname The output prefix
     *  @return Return a vector of the actual output names.
     */
    std::vector<std::string> output(std::string oname);

    /*! Find the min and max of two given parameters (e.g x and y source
     *  coordinates) and return the associated values and trace numbers in the
     *  given structure
     *  @param[in] xlam The function for returning the first parameter
     *  @param[in] ylam The function for returning the second parameter
     *  @param[out] minmax The array of structures to hold the ouput
     */
    void getMinMax(
      MinMaxFunc<Param> xlam, MinMaxFunc<Param> ylam, CoordElem* minmax);

    /*! Function to add to modify function that applies a 2 tailed taper to a
     *  set of traces
     *  @param[in] tapFunc Weight function for the taper ramp
     *  @param[in] nTailLft Length of left tail of taper
     *  @param[in] nTailRt Length of right tail of taper
     */
    void taper(TaperFunc tapFunc, size_t nTailLft, size_t nTailRt = 0);

    /*! Function to add to modify function that applies automatic gain control
     *  to a set of traces
     *  @param[in] agcFunc Staistical function used to scale traces
     *  @param[in] window Length of the agc window
     *  @param[in] normR Value to which traces are normalized
     */
    void AGC(AGCFunc agcFunc, size_t window, trace_t normR);

    /*! Set the text-header of the output
     *  @param[in] outmsg_ The output message
     */
    void text(std::string outmsg_);

    /*! Summarise the current status by whatever means the PIOL intrinsically
     *  supports
     */
    void summary(void) const;

    /*! Add a file to the set based on the ReadInterface
     *  @param[in] in The file interface
     */
    void add(std::unique_ptr<ReadInterface> in);

    /*! Add a file to the set based on the pattern/name given
     *  @param[in] name The input name or pattern
     */
    void add(std::string name);

    /*! Perform the radon to angle conversion. This assumes the input set is a
     *  radon transform file.
     *  @param[in] vmName The name of the velocity model file.
     *  @param[in] vBin The velocity model bin value.
     *  @param[in] oGSz The number of traces in the output gather.
     *  @param[in] oInc The samples per trace for the output (i.e the angle
     *             increment between samples.
     */
    void toAngle(
      std::string vmName,
      const size_t vBin,
      const size_t oGSz,
      geom_t oInc = pi / geom_t(180LU));

    /******************************** Non-Core ********************************/
    /*! Sort the set by the specified sort type.
     *  @param[in] type The sort type
     */
    void sort(SortType type);

    /*! Get the min and the max of a set of parameters passed. This is a
     *  parallel operation. It is the collective min and max across all
     *  processes (which also must all call this file).
     *  @param[in] m1 The first parameter type
     *  @param[in] m2 The second parameter type
     *  @param[out] minmax An array of structures containing the minimum item.x,
     *              maximum item.x, minimum item.y, maximum item.y and their
     *              respective trace numbers.
     */
    void getMinMax(Meta m1, Meta m2, CoordElem* minmax);

    /*! Perform tailed taper on a set of traces
     * @param[in] type The type of taper to be applied to traces.
     * @param[in] nTailLft The length of left-tail taper ramp.
     * @param[in] nTailRt The length of right-tail taper ramp.
     */
    void taper(TaperType type, size_t nTailLft, size_t nTailRt = 0U);

    /*! Scale traces using automatic gain control for visualization
     * @param[in] type They type of agc scaling function used
     * @param[in] window Length of the agc window
     * @param[in] normR Normalization value
     */
    void AGC(AGCType type, size_t window, trace_t normR);


    /*! Filter traces or part of traces using a IIR Butterworth filter
     * @param[in] type Type of filter (i.e. lowpass, highpass, bandpass)
     * @param[in] domain Filtering domaini
     * @param[in] pad Padding pattern
     * @param[in] fs Sampling frequency
     * @param[in] corners Passband and stopband frequency in Hz
     * @param[in] nw Size of trace filtering window
     * @param[in] winCntr Center of trace filtering window
     */
    void temporalFilter(
      FltrType type,
      FltrDmn domain,
      PadType pad,
      trace_t fs,
      std::vector<trace_t> corners,
      size_t nw      = 0U,
      size_t winCntr = 0U);

    /*! Filter traces or part of traces using a IIR Butterworth filter
     * @param[in] type Type of filter (i.e. lowpass, highpass, bandpass)
     * @param[in] domain Filtering domain
     * @param[in] pad Padding pattern
     * @param[in] fs Sampling frequency
     * @param[in] corners Passband frequency in Hz
     * @param[in] N Filter order
     * @param[in] nw Size of trace filtering window
     * @param[in] winCntr Center of trace filtering window
     */
    void temporalFilter(
      FltrType type,
      FltrDmn domain,
      PadType pad,
      trace_t fs,
      size_t N,
      std::vector<trace_t> corners,
      size_t nw      = 0U,
      size_t winCntr = 0U);

    /*! Filter traces or part of traces using a IIR Butterworth filter
     * @param[in] type Type of filter (i.e. lowpass, highpass, bandpass)
     * @param[in] domain Filtering domain
     * @param[in] pad Padding pattern
     * @param[in] fs Sampling frequency
     * @param[in] corners Passband frequency in Hz
     * @param[in] N Filter order
     * @param[in] nw Size of trace filtering window
     * @param[in] winCntr Center of trace filtering window
     */
    void temporalFilter(
      FltrType type,
      FltrDmn domain,
      PadType pad,
      trace_t fs,
      size_t N,
      trace_t corners,
      size_t nw      = 0U,
      size_t winCntr = 0U)
    {
        temporalFilter(
          type, domain, pad, fs, N, std::vector<trace_t>{corners, 0}, nw,
          winCntr);
    }
};

}  // namespace PIOL

#endif  // EXSEISDAT_FLOW_SET_HH
