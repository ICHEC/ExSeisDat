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
    /*! Initialise ExSeisPIOL
     *  \param[in] copt An options structure for MPI
     *  \param[in] maxLevel The maximum log level to be recorded.
     */
    void Init(Comm::MPI::Opt & copt, const Log::Verb maxLevel = Log::Verb::None);

    public :

    /*! Constructor where one can also initialise MPI optionally.
     *  \param[in] copt An options structure for MPI
     *  \param[in] maxLevel The maximum log level to be recorded.
     */
    ExSeisPIOL(Comm::MPI::Opt & copt, const Log::Verb maxLevel = Log::Verb::None);

    /*! Constructor where one can also initialise MPI optionally.
     *  \param[in] initMPI Initialise MPI if true
     *  \param[in] maxLevel The maximum log level to be recorded.
     */
    ExSeisPIOL(bool initMPI, const Log::Verb maxLevel = Log::Verb::None);

    /*! Constructor with optional maxLevel and which initialises MPI.
     * \param[in] maxLevel The maximum log level to be recorded.
     */
    ExSeisPIOL(const Log::Verb maxLevel = Log::Verb::None);

    ~ExSeisPIOL(void);

    std::unique_ptr<Log::Logger> log;       //!< The ExSeisPIOL logger
    std::unique_ptr<Comm::Interface> comm;  //!< The ExSeisPIOL communication

    /*! \brief A function to check if an error has occured in the PIOL. If an error has occured the log is printed, the object destructor is called
     *  and the code aborts.
     * \param[in] msg A message to be printed to the log.
     */
    void isErr(const std::string msg = "");
};
typedef std::shared_ptr<ExSeisPIOL> Piol;   //!< A typedef for the ExSeisPIOL shared_ptr
}
#endif

