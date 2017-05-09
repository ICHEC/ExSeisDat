/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief The MPI communicator
 *   \details The MPI communicator class is responsible for the concrete implementation of
 *   all MPI communication specific features. That is, everything within the MPI specification
 *   except those features which are related to MPI-IO or overlap with MPI-IO.
*//*******************************************************************************************/
#ifndef PIOLANCCMPI_INCLUDE_GUARD
#define PIOLANCCMPI_INCLUDE_GUARD
#include "global.hh"
#include "anc/comm.hh"
#include <mpi.h>

namespace PIOL { namespace Comm {
/*! \brief The MPI communication class. All MPI communication specific routines should be wrapped up and accessible from this class.
 */
class MPI : public Comm::Interface
{
    private :
    MPI_Comm comm;      //!< The MPI communicator.
    bool init;          //!< This variable records whether the class is responsible for initialisation of MPI or not.
    Log::Logger * log;  //!< For logging messages
    public :
    /*! \brief The MPI-Communicator options structure.
     */
    struct Opt
    {
        MPI_Comm comm; //!< This variable defines the default MPI communicator.
        bool initMPI;  //!< If \c initMPI is true, MPI initialisation is performed. Otherwise it is skipped.

        /* \brief Default constructor to prevent intel warnings
         */
        Opt(void)
        {
            comm = MPI_COMM_WORLD;
            initMPI = true;
        }
    };
    /*! \brief The constructor.
     *  \param[in] log_ Pointer to log object
     *  \param[in] opt Any options for the communication layer.
     */
    MPI(Log::Logger * log_, const MPI::Opt & opt);

    /*! \brief The destructor. If the object is responsible for initialisation it deinitialises MPI
     *  in this routine.
     */
    ~MPI(void);

    /*! \brief Retrieve the MPI communicator associated with the ExSeisPIOL.
     *  \return The MPI communicator.
     */
    MPI_Comm getComm() const;

    std::vector<float> gather(const std::vector<float> & in) const;
    std::vector<double> gather(const std::vector<double> & in) const;
    std::vector<llint> gather(const std::vector<llint> & in) const;
    std::vector<size_t> gather(const std::vector<size_t> & in) const;
    size_t sum(size_t val);
    size_t max(size_t val);
    size_t min(size_t val);
    size_t offset(size_t val);
    void barrier(void) const;
};
}}
#endif

