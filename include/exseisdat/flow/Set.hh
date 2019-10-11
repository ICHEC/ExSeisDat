////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The Set layer interface
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_SET_HH
#define EXSEISDAT_FLOW_SET_HH

#include "exseisdat/flow/detail/Cache.hh"
#include "exseisdat/flow/detail/File_descriptor.hh"
#include "exseisdat/flow/detail/Operation_parent.hh"

#include "exseisdat/piol/operations/minmax.hh"
#include "exseisdat/piol/operations/sort_operations/sort.hh"
#include "exseisdat/utils/signal_processing/Gain_function.hh"
#include "exseisdat/utils/signal_processing/mute.hh"
#include "exseisdat/utils/signal_processing/taper.hh"
#include "exseisdat/utils/signal_processing/temporalfilter.hh"

#include <deque>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace exseis {
namespace flow {

using namespace exseis::utils::types;

/*! @brief The internal set class
 */
class Set {
  public:
    /// @brief Typedef for passing in a list of FileDesc objects.
    typedef std::deque<std::shared_ptr<detail::File_descriptor>> FileDeque;

    /// @brief The function list type for the set layer
    typedef std::list<std::shared_ptr<detail::Operation_parent>> FuncLst;

  protected:
    /// The PIOL object.
    std::shared_ptr<exseis::piol::ExSeisPIOL> m_piol;

    /// The output prefix
    std::string m_outfix;

    /// The output text-header message
    std::string m_outmsg;

    /// A deque of unique pointers to file descriptors
    FileDeque m_file;

    /// A map of (ns, sample_interval) key to a deque of file descriptor
    /// pointers
    std::map<std::pair<size_t, exseis::utils::Floating_point>, FileDeque>
        m_fmap;

    /// A map of (ns, sample_interval) key to the current offset
    std::map<std::pair<size_t, exseis::utils::Floating_point>, size_t> m_offmap;

    /// Contains a pointer to the Rules for parameters
    std::shared_ptr<exseis::piol::Rule> m_rule;

    /// The cache of parameters and traces
    detail::Cache m_cache;

    /// The list of functions and related data
    FuncLst m_func;

    /// The rank of the particular process
    size_t m_rank;

    /// The number of ranks
    size_t m_num_rank;

    /*! @brief Drop all file descriptors without output.
     */
    void drop()
    {
        m_file.resize(0);
        m_fmap.clear();
        m_offmap.clear();
    }

    /*! @brief Start unwinding the function list for subset-only operations
     *         based on the given iterators.
     *
     *  @param[in] f_curr  The iterator for the current function to process.
     *  @param[in] f_end   The iterator which indicates the end of the list has
     *                     been reached.
     *  @return            Return the final iterator reached by the last deque.
     */
    FuncLst::iterator start_subset(
        FuncLst::iterator f_curr, FuncLst::iterator f_end);

    /*! @brief Start unwinding the function list for gather operations on the
     *         given iterators.
     *
     *  @param[in] f_curr  The iterator for the current function to process.
     *  @param[in] f_end   The iterator which indicates the end of the list has
     *                     been reached.
     *  @return            Return a file name if f_end is reached. Otherwise
     *                     return "".
     */
    std::string start_gather(FuncLst::iterator f_curr, FuncLst::iterator f_end);

    /*! @brief Start unwinding the function list for single-trace operations on
     *         the given iterators.
     *
     *  @param[in] f_curr  The iterator for the current function to process.
     *  @param[in] f_end   The iterator which indicates the end of the list has
     *                     been reached.
     *  @return            Return a list of all file names. Files are currently
     *                     always created.
     */
    std::vector<std::string> start_single(
        FuncLst::iterator f_curr, FuncLst::iterator f_end);

    /*! @brief The entry point for unwinding the function list for all
     * use-cases.
     *  @param[in] f_curr  The iterator for the current function to process.
     *  @param[in] f_end   The iterator which indicates the end of the list has
     *                     been reached.
     *  @return            Return a list of all file names.
     */
    std::vector<std::string> calc_func(
        FuncLst::iterator f_curr, FuncLst::iterator f_end);

    /*! @brief The entry point for unwinding the function list for single-traces
     *         and gathers only.
     *
     *  @param[in] f_curr  The iterator for the current function to process.
     *  @param[in] f_end   The iterator which indicates the end of the list has
     *                     been reached.
     *  @param[in] type    The type of function currently being processed.
     *                     Either single trace or gather.
     *  @param[in] b_in    The input trace block which can contain traces and
     *                     trace parameters.
     *  @return            Return a traceblock which contains the output from
     *                     the operation.
     *
     *  Transitions from gather to single trace are allowed but not the inverse.
     */
    std::unique_ptr<detail::Trace_block> calc_func(
        FuncLst::iterator f_curr,
        FuncLst::iterator f_end,
        detail::Function_options type,
        std::unique_ptr<detail::Trace_block> b_in);

    /*! @brief The entry point for unwinding the function list for subsets.
     *
     *  @param[in] f_curr  The iterator for the current function to process.
     *  @param[in] f_end   The iterator which indicates the end of the list has
     *                     been reached.
     *  @param[in] f_que   A deque of unique pointers to file descriptors.
     *  @return            Return the final iterator reached.
     */
    FuncLst::iterator calc_func_s(
        FuncLst::iterator f_curr, FuncLst::iterator f_end, FileDeque& f_que);

  public:
    /*! @brief Constructor
     *
     *  @param[in] piol     The PIOL object.
     *  @param[in] pattern  The file-matching pattern
     *  @param[in] outfix   The output file-name prefix
     *  @param[in] rule     Contains a pointer to the rules to use for trace
     *                      parameters.
     */
    Set(std::shared_ptr<exseis::piol::ExSeisPIOL> piol,
        std::string pattern,
        std::string outfix,
        std::shared_ptr<exseis::piol::Rule> rule =
            std::make_shared<exseis::piol::Rule>(
                std::initializer_list<exseis::piol::Trace_metadata_key>{
                    piol::Trace_metadata_key::Copy}));

    /*! @brief Constructor
     *
     *  @param[in] piol     The PIOL object.
     *  @param[in] pattern  The file-matching pattern
     *  @param[in] rule     Contains a pointer to the rules to use for trace
     *                      parameters.
     */
    Set(std::shared_ptr<exseis::piol::ExSeisPIOL> piol,
        std::string pattern,
        std::shared_ptr<exseis::piol::Rule> rule =
            std::make_shared<exseis::piol::Rule>(
                std::initializer_list<exseis::piol::Trace_metadata_key>{
                    piol::Trace_metadata_key::Copy})) :
        Set(piol, pattern, "", rule)
    {
    }

    /*! @brief Constructor overload
     *
     *  @param[in] piol  The PIOL object.
     *  @param[in] rule  Contains a pointer to the rules to use for trace
     *                   parameters.
     */
    Set(std::shared_ptr<exseis::piol::ExSeisPIOL> piol,
        std::shared_ptr<exseis::piol::Rule> rule =
            std::make_shared<exseis::piol::Rule>(
                std::initializer_list<exseis::piol::Trace_metadata_key>{
                    piol::Trace_metadata_key::Copy}));

    /*! @brief Destructor
     */
    ~Set();

    /*! @brief Sort the set using the given comparison function
     *
     *  @param[in] sort_func  The comparison function
     */
    void sort(exseis::piol::CompareP sort_func);

    /*! @brief Sort the set using the given comparison function
     *
     *  @param[in] r          The rules necessary for the sort.
     *  @param[in] sort_func  The comparison function.
     */
    void sort(
        std::shared_ptr<exseis::piol::Rule> r,
        exseis::piol::CompareP sort_func);

    /*! @brief Output using the given output prefix
     *
     *  @param[in] oname  The output prefix
     *  @return           Return a vector of the actual output names.
     */
    std::vector<std::string> output(std::string oname);

    /*! @brief Output using the output prefix stored as member variable
     *
     *  @return           Return a vector of the actual output names.
     */
    void output();

    /*! @brief Find the min and max of two given parameters (e.g x and y source
     *         coordinates) and return the associated values and trace numbers
     *         int the given structure
     *
     *  @param[in] xlam     The function for returning the first parameter
     *  @param[in] ylam     The function for returning the second parameter
     *  @param[out] minmax  The array of structures to hold the ouput
     */
    void get_min_max(
        exseis::piol::MinMaxFunc<exseis::piol::Trace_metadata> xlam,
        exseis::piol::MinMaxFunc<exseis::piol::Trace_metadata> ylam,
        exseis::piol::CoordElem* minmax);

    /*! @brief Function to add to modify function that applies a 2 tailed taper
     *         to a set of traces
     *
     *  @param[in] taper_function       Weight function for the taper ramp
     *  @param[in] taper_size_at_begin  Length of taper at beginning of trace
     *  @param[in] taper_size_at_end    Length of taper at end of trace
     */
    void taper(
        exseis::utils::Taper_function taper_function,
        size_t taper_size_at_begin,
        size_t taper_size_at_end = 0);

    /*! @brief Function to modify function that applies both a mute region at
     *         the start and end of a series of traces and a 2 tailed taper
     *
     *  @param[in] taper_function       Weight function for the taper ramp
     *  @param[in] taper_size_at_begin  Length of taper at beginning of trace
     *  @param[in] taper_size_at_end    Length of taper at end of trace
     *  @param[in] mute_size_at_begin   Length of mute before taper at
     *                                  beginning of trace
     *  @param[in] mute_size_at_end     Length of the mute at end of trace
     */
    void mute(
        exseis::utils::Taper_function taper_function,
        size_t mute_size_at_begin,
        size_t taper_size_at_begin,
        size_t taper_size_at_end,
        size_t mute_size_at_end);

    /*! @brief Function to add to modify function that applies automatic gain
     *         control to a set of traces
     *
     *  @param[in] agc_func          Staistical function used to scale traces
     *  @param[in] window            Length of the agc window
     *  @param[in] target_amplitude  Value to which traces are normalized
     */
    void agc(
        exseis::utils::Gain_function agc_func,
        size_t window,
        exseis::utils::Trace_value target_amplitude);

    /*! @brief Set the text-header of the output
     *
     *  @param[in] outmsg  The output message
     */
    void text(std::string outmsg);

    /*! @brief Summarise the current status by whatever means the PIOL
     *         intrinsically supports
     */
    void summary() const;

    /*! @brief Add a file to the set based on the ReadInterface
     *
     *  @param[in] in  The file interface
     */
    void add(std::unique_ptr<exseis::piol::Input_file> in);

    /*! @brief Add a file to the set based on the pattern/name given
     *
     *  @param[in] name  The input name or pattern
     */
    void add(std::string name);

    /*! @brief Perform the radon to angle conversion. This assumes the input set
     *         is a radon transform file.
     *
     *  @param[in] vm_name                   The name of the velocity model file.
     *  @param[in] v_bin                     The velocity model bin value.
     *  @param[in] output_traces_per_gather  The number of traces in the output
     *                                       gather.
     *  @param[in] output_sample_interval    The samples per trace for the
     *                                       output in radians
     *                                       (i.e the angle increment between
     *                                       samples.)
     *                                       (default = 1 degree).
     */
    void to_angle(
        std::string vm_name,
        size_t v_bin,
        size_t output_traces_per_gather,
        exseis::utils::Floating_point output_sample_interval = std::atan(1)
                                                               * 4.0 / 180);

    /******************************** Non-Core ********************************/

    /// @name Non-Core
    /// @{

    /*! @brief Sort the set by the specified sort type.
     *
     *  @param[in] type The sort type
     */
    void sort(exseis::piol::Sort_type type);

    /*! @brief Get the min and the max of a set of parameters passed.
     *
     * @details This is a parallel operation. It is the collective min and max
     *          across all processes (which also must all call this file).
     *
     *  @param[in] m1       The first parameter type
     *  @param[in] m2       The second parameter type
     *  @param[out] minmax  An array of structures containing the minimum
     *                      item.x, maximum item.x, minimum item.y, maximum
     *                      item.y and their respective trace numbers.
     */
    void get_min_max(
        exseis::piol::Trace_metadata_key m1,
        exseis::piol::Trace_metadata_key m2,
        exseis::piol::CoordElem* minmax);


    /*! @brief Filter traces or part of traces using a IIR Butterworth filter
     *
     * @param[in] type      Type of filter (i.e. lowpass, highpass, bandpass)
     * @param[in] domain    Filtering domaini
     * @param[in] pad       Padding pattern
     * @param[in] fs        Sampling frequency
     * @param[in] corners   Passband and stopband frequency in Hz
     * @param[in] nw        Size of trace filtering window
     * @param[in] win_cntr  Center of trace filtering window
     */
    void temporal_filter(
        exseis::utils::FltrType type,
        exseis::utils::FltrDmn domain,
        exseis::utils::PadType pad,
        exseis::utils::Trace_value fs,
        std::vector<exseis::utils::Trace_value> corners,
        size_t nw       = 0U,
        size_t win_cntr = 0U);

    /*! @brief Filter traces or part of traces using a IIR Butterworth filter
     *
     * @param[in] type      Type of filter (i.e. lowpass, highpass, bandpass)
     * @param[in] domain    Filtering domain
     * @param[in] pad       Padding pattern
     * @param[in] fs        Sampling frequency
     * @param[in] corners   Passband frequency in Hz
     * @param[in] n         Filter order
     * @param[in] nw        Size of trace filtering window
     * @param[in] win_cntr  Center of trace filtering window
     */
    void temporal_filter(
        exseis::utils::FltrType type,
        exseis::utils::FltrDmn domain,
        exseis::utils::PadType pad,
        exseis::utils::Trace_value fs,
        size_t n,
        std::vector<exseis::utils::Trace_value> corners,
        size_t nw       = 0U,
        size_t win_cntr = 0U);

    /*! @brief Filter traces or part of traces using a IIR Butterworth filter
     *
     * @param[in] type      Type of filter (i.e. lowpass, highpass, bandpass)
     * @param[in] domain    Filtering domain
     * @param[in] pad       Padding pattern
     * @param[in] fs        Sampling frequency
     * @param[in] corners   Passband frequency in Hz
     * @param[in] n         Filter order
     * @param[in] nw        Size of trace filtering window
     * @param[in] win_cntr  Center of trace filtering window
     */
    void temporal_filter(
        exseis::utils::FltrType type,
        exseis::utils::FltrDmn domain,
        exseis::utils::PadType pad,
        exseis::utils::Trace_value fs,
        size_t n,
        exseis::utils::Trace_value corners,
        size_t nw       = 0U,
        size_t win_cntr = 0U)
    {
        temporal_filter(
            type, domain, pad, fs, n,
            std::vector<exseis::utils::Trace_value>{corners, 0}, nw, win_cntr);
    }

    /// @} Non-Core
};

}  // namespace flow
}  // namespace exseis

#endif  // EXSEISDAT_FLOW_SET_HH
