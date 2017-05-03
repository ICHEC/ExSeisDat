/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date November 2016
 *   \brief The Set layer interface
*//*******************************************************************************************/
#ifndef PIOLSET_INCLUDE_GUARD
#define PIOLSET_INCLUDE_GUARD
#include "global.hh"
#include "file/file.hh"
#include "ops/minmax.hh"
#include "ops/sort.hh"
#include <functional>
#include <memory>
#include <deque>
#include <list>
#include <map>

namespace PIOL {
/*! A file-descriptor structure which describes an input file and the decomposition for the set layer
 */
struct FileDesc
{
    std::shared_ptr<File::ReadInterface> ifc;   //!< The file interface
    std::vector<size_t> ilst;                    //!< The size of this corresponds to the local decomposition
    std::vector<size_t> olst;                    //!< The size of this corresponds to the local decomposition
};

typedef std::function<std::vector<size_t>(size_t, File::Param *, trace_t *)> Mod;  //!< Typedef for functions that modify traces and associated parameters

enum class FuncOpt : size_t
{
    NeedMeta,
    NeedTrcVal,
    ModTrcVal,
    DelTrc,
    AddTrc,
    DepTrcCnt,
    ReorderTrc,
    DepOrderTrc,
    DepMeta,
    RotCoord,
    DepCoordRot,
    AllTraces,
    OwnIO,
    SubSetOnly
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

struct CacheElem
{
    std::deque<std::shared_ptr<FileDesc>> desc;
    std::unique_ptr<File::Param> prm;
    std::vector<trace_t> trc;
    CacheElem(std::deque<std::shared_ptr<FileDesc>> & desc_, std::unique_ptr<File::Param> prm_)
    {
        desc = desc_;
        prm = std::move(prm_);
    }
};

class Cache
{
    std::vector<CacheElem> cache;
    public :
    File::Param * cachePrm(ExSeisPIOL * piol, std::shared_ptr<File::Rule> rule, std::deque<std::shared_ptr<FileDesc>> & desc);
};

/*! The internal set class
 */
class InternalSet
{
    private :
    Piol piol;                                                          //!< The PIOL object.
    std::string outfix;                                                 //!< The output prefix
    std::string outmsg;                                                 //!< The output text-header message
    std::deque<std::shared_ptr<FileDesc>> file;                         //!< A deque of unique pointers to file descriptors
    std::map<std::pair<size_t, geom_t>, std::deque<std::shared_ptr<FileDesc>>> fmap;   //!< A map of (ns, inc) key to a deque of file descriptor pointers
    std::map<std::pair<size_t, geom_t>, size_t> offmap;                 //!< A map of (ns, inc) key to the current offset
    std::shared_ptr<File::Rule> rule;                                   //!< Contains a pointer to the Rules for parameters

//    Mod modify = [] (size_t, File::Param *, trace_t *) -> std::vector<size_t> { return std::vector<size_t>(); };                     //!< Function to modify traces and parameters

    std::list<std::tuple<OpOpt, std::shared_ptr<File::Rule>, Mod>> func;

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
    InternalSet(Piol piol_, std::shared_ptr<File::Rule> rule_) : piol(piol_), rule(rule_) { }

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
    void calcFunc(std::list<std::tuple<OpOpt, std::shared_ptr<File::Rule>, Mod>> func, Cache & cache);
/*    void mod(Mod modify_)
    {
        modify = [oldmod = modify, modify_] (size_t, File::Param * p, trace_t * t) { oldmod(p, t); modify_(p, t); };
    }*/
};
}
#endif
