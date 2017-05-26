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
#include <memory>
#include <deque>
#include <map>
#include "global.hh"
#include "file/file.hh"
#include "ops/sort.hh"
#include "ops/minmax.hh"
#include "ops/agc.hh"

namespace PIOL {
/*! A file-descriptor structure which describes an input file and the decomposition for the set layer
 */
struct FileDesc
{
    std::unique_ptr<File::ReadInterface> ifc;   //!< The file interface
    size_t offset;                              //!< Local offset into the file

    //TODO: Temporary approach. This approach will NEED to be optimised in certain cases since it keeps pointless traces
    std::vector<size_t> lst;                    //!< The size of this corresponds to the local decomposition
};

typedef std::function<void(size_t, File::Param *, trace_t *)> Mod;  //!< Typedef for functions that modify traces and associated parameters

/*! The internal set class
 */
class Set
{
    private :
    Piol piol;                                                          //!< The PIOL object.
    std::string outfix;                                                 //!< The output prefix
    std::string outmsg;                                                 //!< The output text-header message
    std::deque<std::unique_ptr<FileDesc>> file;                         //!< A deque of unique pointers to file descriptors
    std::map<std::pair<size_t, geom_t>, std::deque<FileDesc *>> fmap;   //!< A map of (ns, inc) key to a deque of file descriptor pointers
    std::map<std::pair<size_t, geom_t>, size_t> offmap;                 //!< A map of (ns, inc) key to the current offset
    std::shared_ptr<File::Rule> rule;                                   //!< Contains a pointer to the Rules for parameters
    Mod modify = [] (size_t, File::Param *, trace_t *) { };                     //!< Function to modify traces and parameters

    /*! Fill the file descriptors using the given pattern
     *  \param[in] piol The PIOL object.
     *  \param[in] pattern The file-matching pattern
     */
    void fillDesc(std::shared_ptr<ExSeisPIOL> piol, std::string pattern);

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
                std::shared_ptr<File::Rule> rule_ = std::make_shared<File::Rule>(std::initializer_list<Meta>{Meta::Copy}))
                : Set(piol_, pattern, "", rule_) {  }

    /*! Constructor overload
     *  \param[in] piol_ The PIOL object.
     *  \param[in] rule_ Contains a pointer to the rules to use for trace parameters.
     */
    Set(Piol piol_, std::shared_ptr<File::Rule> rule_ = std::make_shared<File::Rule>(std::initializer_list<Meta>{Meta::Copy}))
                : piol(piol_), rule(rule_) { }

    /*! Destructor
     */
    ~Set(void);

    /*! Sort the set using the given comparison function
     *  \param[in] func The comparison function
     */
    void sort(Compare<File::Param> func);

    /*! The number of traces in the input files
     *  \return The number of traces in the input files
     */
    size_t getInNt(void);

    /*! The number of traces associated with the local process
     *  \return The number of traces associated with the local process
     */
    size_t getLNt(void);

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
     * \param[in] func Weight function for the taper ramp
     * \param[in] nTailLft Length of left tail of taper
     * \param[in] nTailRt Length of right tail of taper
     */
    void taper(TaperFunc func, size_t nTailLft, size_t nTailRt = 0);

    /*! Function to add to modify function that applies automatic gain control to a set of traces
     * \param[in] func Staistical function used to scale traces
     * \param[in] window Length of the agc window
     * \param[in] normR Value to which traces are normalized
     */
    void AGC(AGCFunc func, size_t window, trace_t normR);

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
     *  \param[in] name The input name
     */
    void add(std::string name);

    /*! Modify traces and parameters. Multiple modifies can be called.
     *  \param[in] modify_ Function to modify traces and parameters
     */
    void mod(Mod modify_)
    {
        modify = [oldmod = modify, modify_] (size_t ns, File::Param * p, trace_t * t) { oldmod(ns, p, t); modify_(ns, p, t); };
    }

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
};
}
#endif
