/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date September 2016
 *   \brief The ExSeis PIOL C++ API
 *//*******************************************************************************************/
#ifndef PIOLPIOL_INCLUDE_GUARD
#define PIOLPIOL_INCLUDE_GUARD
#include <memory>
#include "anc/log.hh"
#include "anc/comm.hh"
#include "anc/cmpi.hh"
namespace PIOL {
/*! \brief The ExSeisPIOL structure. A single instance of this structure should be created and
 *         passed to each subsequent PIOL object which is created.
 */
class ExSeisPIOL
{
    public :
    std::unique_ptr<Log::Logger> log;
    std::shared_ptr<Comm::Interface> comm;

    ExSeisPIOL(const Log::Verb maxLevel = Log::Verb::None);
    ExSeisPIOL(bool initMPI, const Log::Verb maxLevel = Log::Verb::None);
    ExSeisPIOL(Comm::MPI::Opt & copt, const Log::Verb maxLevel = Log::Verb::None);
    ~ExSeisPIOL(void);

    /*! \brief A function to check if an error has occured in the PIOL. If an error has occured the log is printed, the object destructor is called
     *  and the code aborts.
     * \param[in] msg A message to be printed to the log.
     */
    void isErr(const std::string msg = "");
};
typedef std::shared_ptr<ExSeisPIOL> Piol;
}
#endif

