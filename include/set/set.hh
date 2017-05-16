/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date November 2016
 *   \brief The Set layer interface
*//*******************************************************************************************/
#ifndef PIOLSET_INCLUDE_GUARD
#define PIOLSET_INCLUDE_GUARD
#include "set/share.hh"
#include "set/cache.hh"
#include "ops/minmax.hh"
#include "ops/sort.hh"
#include <functional>
#include <list>
#include <map>

namespace PIOL {
typedef std::function<std::vector<size_t>(size_t, File::Param *, trace_t *)> Mod;  //!< Typedef for functions that modify traces and associated parameters
enum class FuncOpt : size_t
{
//Data type dependencies
    NeedMeta,
    NeedTrcVal,
    NeedAll,

//Modification level:
    AddTrc,
    DelTrc,
    ModTrcVal,
    ModMetaVal,
    ReorderTrc,
    ModAll,

//Modification dependencies
    DepTrcCnt,
    DepTrcOrder,
    DepTrcVal,
    DepMetaVal,
    DepAll,

//Comms level:
    SingleTrace,
    Gather,
    SubSetOnly,
    AllTraces,
    OwnIO
};

struct OpOpt
{
    std::vector<FuncOpt> optList;
    bool check(FuncOpt opt)
    {
        auto it = std::find(optList.begin(), optList.end(), opt);
        return it != optList.end();
    }
    void add(FuncOpt opt)
    {
        optList.push_back(opt);
    }
    OpOpt(std::initializer_list<FuncOpt> list) : optList(list)
    {
    }
};

typedef std::list<std::tuple<OpOpt, std::shared_ptr<File::Rule>, Mod>> FuncLst;


/*! The internal set class
 */
class InternalSet
{
    private :
    Piol piol;                                                  //!< The PIOL object.
    std::string outfix;                                         //!< The output prefix
    std::string outmsg;                                         //!< The output text-header message
    FileDeque file;                                             //!< A deque of unique pointers to file descriptors
    std::map<std::pair<size_t, geom_t>, FileDeque> fmap;        //!< A map of (ns, inc) key to a deque of file descriptor pointers
    std::map<std::pair<size_t, geom_t>, size_t> offmap;         //!< A map of (ns, inc) key to the current offset
    std::shared_ptr<File::Rule> rule;                           //!< Contains a pointer to the Rules for parameters
    Cache cache;
    FuncLst func;

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
    InternalSet(Piol piol_, std::string pattern, std::string outfix_, std::shared_ptr<File::Rule> rule_);

    /*! Constructor overload
     *  \param[in] piol_ The PIOL object.
     *  \param[in] rule_ Contains a pointer to the rules to use for trace parameters.
     */
    InternalSet(Piol piol_, std::shared_ptr<File::Rule> rule_) : piol(piol_), rule(rule_), cache(piol_) { }

    /*! Destructor
     */
    ~InternalSet(void);

    /*! Sort the set using the given comparison function
     *  \param[in] func The comparison function
     */
    void sort(File::Compare<File::Param> func);

    /*! Output using the given output prefix
     *  \param[in] oname The output prefix
     *  \return Return a vector of the actual output names.
     */
    std::vector<std::string> output(std::string oname);
    std::string output(FileDeque & fQue);

    /*! Find the min and max of two given parameters (e.g x and y source coordinates) and return
     *  the associated values and trace numbers in the given structure
     *  \param[in] xlam The function for returning the first parameter
     *  \param[in] ylam The function for returning the second parameter
     *  \param[out] minmax The array of structures to hold the ouput
     */
    void getMinMax(File::Func<File::Param> xlam, File::Func<File::Param> ylam, CoordElem * minmax);

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
    void calcFunc(FuncLst::iterator fCurr, const FuncLst::iterator fEnd);
    FuncLst::iterator calcFunc(const FuncLst::iterator fCurr, const FuncLst::iterator fEnd, FileDeque & fQue);
};
}
#endif
