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
#include <map>
namespace PIOL {

struct FileDesc
{
    std::unique_ptr<File::Interface> ifc;

    size_t offset;  //Local offset into the file

    std::vector<size_t> lst;    //The size of this corresponds to the local decomposition
    //TODO: Temporary approach. This approach will NEED to be optimised since it keeps pointless traces
};

class InternalSet
{
    /*! State flags structure for Set
     */
#warning None of these flags are supported yet
    struct Flags
    {
        uint64_t fidelity : 1;  //!< When copying data perform it at the object layer to ensure byte-for-byte copying
        uint64_t varNs : 1;     //!< The number of samples per trace is variable.
        uint64_t varInc : 1;    //!< The increment between traces is variable.
    } state;                    //!< State flags are stored in this structure
    Piol piol;
    std::string outfix;
    std::string outmsg;
    std::deque<FileDesc> file;
    std::map<std::pair<size_t, geom_t>, std::deque<FileDesc *>> fmap;

    std::deque<std::function<void(std::vector<size_t> &)>> func;
    std::shared_ptr<File::Rule> rule;

    void fillDesc(std::shared_ptr<ExSeisPIOL> piol, std::string pattern);
    public :
    InternalSet(Piol piol_, std::string pattern, std::string outfix_, std::shared_ptr<File::Rule> rule_);
    ~InternalSet(void);

    void sort(File::Compare<File::Param> func);
    size_t getInNt(void);
    size_t getSetNt(void);
    size_t getLNt(void);

    std::vector<std::string> output(std::string oname);

    void getMinMax(File::Func<File::Param> xlam, File::Func<File::Param> ylam, File::CoordElem * minmax);

    void text(std::string outmsg_)
    {
        outmsg = outmsg_;
    }
    void summary(void) const;
};
}
#endif
