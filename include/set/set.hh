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
#include "cppfileapi.hh"
#include <functional>
#include <deque>
namespace PIOL {
struct FileDesc;
class Set
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
    std::deque<File::Interface *> file; //!< The internal list of input files and related data.
    //std::deque<FileDesc> file; //!< The internal list of input files and related data.
    std::deque<std::deque<size_t>> traceList;

#warning Look at this
    std::deque<std::function<void(std::vector<size_t> &)>> func;

    void fillDesc(std::shared_ptr<ExSeisPIOL> piol, std::string pattern);
    public :
    Set(Piol piol_, std::string pattern, std::string outfix);
    ~Set(void);
    size_t getNt(void);
    void output(std::string oname);
};
}
#endif
