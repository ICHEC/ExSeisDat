////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The Set layer interface
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_SET_HH
#define EXSEISDAT_FLOW_SET_HH

#include "ExSeisDat/Flow/Cache.hh"
#include "ExSeisDat/Flow/FileDesc.hh"
#include "ExSeisDat/Flow/OpParent.hh"

#include "ExSeisDat/PIOL/operations/minmax.h"
#include "ExSeisDat/PIOL/operations/sort.hh"
#include "ExSeisDat/PIOL/operations/taper.hh"
#include "ExSeisDat/PIOL/operations/temporalfilter.hh"
#include "ExSeisDat/utils/gain_control/Gain_function.h"

#include <deque>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace exseis {
namespace Flow {

using namespace exseis::utils::typedefs;

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
    std::shared_ptr<exseis::PIOL::ExSeisPIOL> piol;

    /// The output prefix
    std::string outfix;

    /// The output text-header message
    std::string outmsg;

    /// A deque of unique pointers to file descriptors
    FileDeque file;

    /// A map of (ns, inc) key to a deque of file descriptor pointers
    std::map<std::pair<size_t, exseis::utils::Floating_point>, FileDeque> fmap;

    /// A map of (ns, inc) key to the current offset
    std::map<std::pair<size_t, exseis::utils::Floating_point>, size_t> offmap;

    /// Contains a pointer to the Rules for parameters
    std::shared_ptr<exseis::PIOL::Rule> rule;

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
      std::shared_ptr<exseis::PIOL::ExSeisPIOL> piol_,
      std::string pattern,
      std::string outfix_,
      std::shared_ptr<exseis::PIOL::Rule> rule_ =
        std::make_shared<exseis::PIOL::Rule>(
          std::initializer_list<exseis::PIOL::Meta>{PIOL_META_COPY}));

    /*! Constructor
     *  @param[in] piol_ The PIOL object.
     *  @param[in] pattern The file-matching pattern
     *  @param[in] rule_ Contains a pointer to the rules to use for trace
     *             parameters.
     */
    Set(
      std::shared_ptr<exseis::PIOL::ExSeisPIOL> piol_,
      std::string pattern,
      std::shared_ptr<exseis::PIOL::Rule> rule_ =
        std::make_shared<exseis::PIOL::Rule>(
          std::initializer_list<exseis::PIOL::Meta>{PIOL_META_COPY})) :
        Set(piol_, pattern, "", rule_)
    {
    }

    /*! Constructor overload
     *  @param[in] piol_ The PIOL object.
     *  @param[in] rule_ Contains a pointer to the rules to use for trace
     *             parameters.
     */
    Set(
      std::shared_ptr<exseis::PIOL::ExSeisPIOL> piol_,
      std::shared_ptr<exseis::PIOL::Rule> rule_ =
        std::make_shared<exseis::PIOL::Rule>(
          std::initializer_list<exseis::PIOL::Meta>{PIOL_META_COPY}));

    /*! Destructor
     */
    ~Set(void);

    /*! Sort the set using the given comparison function
     *  @param[in] sortFunc The comparison function
     */
    void sort(exseis::PIOL::CompareP sortFunc);

    /*! Sort the set using the given comparison function
     *  @param[in] r The rules necessary for the sort.
     *  @param[in] sortFunc The comparison function.
     */
    void sort(
      std::shared_ptr<exseis::PIOL::Rule> r, exseis::PIOL::CompareP sortFunc);

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
      exseis::PIOL::MinMaxFunc<exseis::PIOL::Param> xlam,
      exseis::PIOL::MinMaxFunc<exseis::PIOL::Param> ylam,
      exseis::PIOL::CoordElem* minmax);

    /*! Function to add to modify function that applies a 2 tailed taper to a
     *  set of traces
     *  @param[in] tapFunc Weight function for the taper ramp
     *  @param[in] nTailLft Length of left tail of taper
     *  @param[in] nTailRt Length of right tail of taper
     */
    void taper(
      exseis::PIOL::TaperFunc tapFunc, size_t nTailLft, size_t nTailRt = 0);

    /*! Function to add to modify function that applies automatic gain control
     *  to a set of traces
     *  @param[in] agcFunc Staistical function used to scale traces
     *  @param[in] window Length of the agc window
     *  @param[in] target_amplitude Value to which traces are normalized
     */
    void AGC(
      exseis::utils::Gain_function agcFunc,
      size_t window,
      exseis::utils::Trace_value target_amplitude);

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
    void add(std::unique_ptr<exseis::PIOL::ReadInterface> in);

    /*! Add a file to the set based on the pattern/name given
     *  @param[in] name The input name or pattern
     */
    void add(std::string name);

    /*! Perform the radon to angle conversion. This assumes the input set is a
     *  radon transform file.
     *  @param[in] vmName The name of the velocity model file.
     *  @param[in] vBin The velocity model bin value.
     *  @param[in] oGSz The number of traces in the output gather.
     *  @param[in] oInc The samples per trace for the output in radians
     *                  (i.e the angle increment between samples.)
     *                  (default = 1 degree).
     */
    void toAngle(
      std::string vmName,
      const size_t vBin,
      const size_t oGSz,
      exseis::utils::Floating_point oInc = std::atan(1) * 4.0 / 180);

    /******************************** Non-Core ********************************/
    /*! Sort the set by the specified sort type.
     *  @param[in] type The sort type
     */
    void sort(exseis::PIOL::SortType type);

    /*! Get the min and the max of a set of parameters passed. This is a
     *  parallel operation. It is the collective min and max across all
     *  processes (which also must all call this file).
     *  @param[in] m1 The first parameter type
     *  @param[in] m2 The second parameter type
     *  @param[out] minmax An array of structures containing the minimum item.x,
     *              maximum item.x, minimum item.y, maximum item.y and their
     *              respective trace numbers.
     */
    void getMinMax(
      exseis::PIOL::Meta m1,
      exseis::PIOL::Meta m2,
      exseis::PIOL::CoordElem* minmax);

    /*! Perform tailed taper on a set of traces
     * @param[in] type The type of taper to be applied to traces.
     * @param[in] nTailLft The length of left-tail taper ramp.
     * @param[in] nTailRt The length of right-tail taper ramp.
     */
    void taper(
      exseis::PIOL::TaperType type, size_t nTailLft, size_t nTailRt = 0U);


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
      exseis::PIOL::FltrType type,
      exseis::PIOL::FltrDmn domain,
      exseis::PIOL::PadType pad,
      exseis::utils::Trace_value fs,
      std::vector<exseis::utils::Trace_value> corners,
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
      exseis::PIOL::FltrType type,
      exseis::PIOL::FltrDmn domain,
      exseis::PIOL::PadType pad,
      exseis::utils::Trace_value fs,
      size_t N,
      std::vector<exseis::utils::Trace_value> corners,
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
      exseis::PIOL::FltrType type,
      exseis::PIOL::FltrDmn domain,
      exseis::PIOL::PadType pad,
      exseis::utils::Trace_value fs,
      size_t N,
      exseis::utils::Trace_value corners,
      size_t nw      = 0U,
      size_t winCntr = 0U)
    {
        temporalFilter(
          type, domain, pad, fs, N,
          std::vector<exseis::utils::Trace_value>{corners, 0}, nw, winCntr);
    }
};

}  // namespace Flow
}  // namespace exseis

#endif  // EXSEISDAT_FLOW_SET_HH
