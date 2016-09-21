/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief The ExSeisPIOL class
 *   \details This layer allows ancillary classes associated with all ExSeisPIOL files to be placed
 *   in a single object
 *//*******************************************************************************************/
#ifndef PIOLPIOL_INCLUDE_GUARD
#define PIOLPIOL_INCLUDE_GUARD
#include <memory>
#include "anc/comm.hh"
#include "anc/log.hh"
namespace PIOL {
/*! \brief The ExSeisPIOL structure. A single instance of this structure should be created and
 *         passed to each subsequent PIOL object which is created.
 */
class ExSeisPIOL
{
    public :
    std::unique_ptr<Log::Logger> log;       //!< The log stores all of the records for debugging and error checking.
    std::shared_ptr<Comm::Interface> comm;  //!< The communicator used for communication by the PIOL.

    /*! \brief The destructor outputs all of the log messages and deletes the communicator.
     *  This will free the communicator if the class contains the last shared pointer.
     */
    ~ExSeisPIOL(void);

    /*! \brief Records a message to the log layer.
     *  \param[in] file The associated filename. An empty string should be passed if there is no associated file.
     *  \param[in] layer The associated layer of the PIOL.
     *  \param[in] stat The associated status.
     *  \param[in] msg The message for the end-user.
     *  \param[in] verbosity The verbosity level of the message.
     */
    void record(const std::string file, const Log::Layer layer, const Log::Status stat, const std::string msg, const Log::Verb verbosity);

    /*! \brief A function to check if an error has occured in the PIOL. If an error has occured the log is printed, the object destructor is called
     *  and the code aborts.
     */
    void isErr(const std::string msg);

    /*! \overload
     */
    void isErr(void)
    {
        isErr("");
    }
};
typedef std::shared_ptr<ExSeisPIOL> Piol; //!< Typedef of the PIOL shared ptr.
}
#endif

