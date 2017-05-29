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
#include "flow/share.hh"
#include "flow/cache.hh"
#include "ops/minmax.hh"
#include "ops/sort.hh"

#warning temp
#include "file/filesegy.hh"
namespace PIOL {
typedef std::function<void(const TraceBlock * in, TraceBlock * out)> Mod;  //!< Typedef for functions that modify traces and associated parameters
typedef std::function<std::vector<size_t>(TraceBlock * data)> InPlaceMod;  //!< Typedef for functions that modify traces and associated parameters
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
    OpOpt(void) { }
    OpOpt(std::initializer_list<FuncOpt> list) : optList(list)
    {
    }
};

struct gState
{
    virtual void makeState(csize_t gOffset, csize_t numGather, Uniray<size_t, llint, llint> & gather) { }
};

extern std::unique_ptr<File::ReadSEGYModel> makeModelFile(Piol piol, std::string name);

struct RadonState : public gState
{
    Piol piol;
    std::vector<trace_t> vtrc;
    std::vector<llint> il;
    std::vector<llint> xl;

    size_t vNs;
    size_t vBin;
    size_t oGSz;
    geom_t vInc;
    trace_t oInc;   //1 degree in radians

    RadonState(Piol piol_) : piol(piol_), vNs(0U), vBin(20U), oGSz(60U),
                             vInc(geom_t(0)), oInc(M_PI / geom_t(180U))
    {}

    void makeState(csize_t gOffset, csize_t numGather, Uniray<size_t, llint, llint> & gather)
    {
        auto vm = makeModelFile(piol, "vm.segy");   //TODO:DON'T USE MAGIC NAME
        vtrc = vm->readModel(gOffset, numGather, gather);
        il.resize(numGather);
        xl.resize(numGather);
        for (size_t i = 0; i < numGather; i++)
        {
            auto gval = gather[gOffset + i];
            il[i] = std::get<1>(gval);
            xl[i] = std::get<2>(gval);
        }

        vNs = vm->readNs();
        vInc = vm->readInc();
    }
};

struct OpParent
{
    OpOpt opt;
    std::shared_ptr<File::Rule> rule;
    std::shared_ptr<gState> state;
    OpParent(OpOpt & opt_, std::shared_ptr<File::Rule> rule_, std::shared_ptr<gState> state_) : opt(opt_), rule(rule_), state(state_) { }
    OpParent(void) { }
    virtual ~OpParent(void) {}
};

template <typename T>
struct Op : public OpParent
{
    T func;
    Op(OpOpt & opt_, std::shared_ptr<File::Rule> rule_, std::shared_ptr<gState> state_, T func_) : OpParent(opt_, rule_, state_), func(func_)
    {

    }
};

//If this was C++17 then a std::variant could be used
typedef std::list<std::shared_ptr<OpParent>> FuncLst;

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

    void drop(void)
    {
        file.resize(0);
        fmap.clear();
        offmap.clear();
    }

    /*! Add a file to the set based on the pattern/name given
     *  \param[in] name The input name
     */
    void add(std::string name);

    /*! Modify traces and parameters. Multiple modifies can be called.
     *  \param[in] modify_ Function to modify traces and parameters
     */
    void calcFunc(FuncLst::iterator fCurr, const FuncLst::iterator fEnd);
    FuncLst::iterator calcFuncS(const FuncLst::iterator fCurr, const FuncLst::iterator fEnd, FileDeque & fQue);
    std::unique_ptr<TraceBlock> calcFuncG(FuncLst::iterator fCurr, const FuncLst::iterator fEnd, const std::unique_ptr<TraceBlock> bIn);

    void toAngle(std::string vmName);
};
}
#endif
