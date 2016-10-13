/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief The base communicator layer
 *   \details The base class exists so that the ExSeisPIOL is not completely tied to MPI.
 *   This is useful for two reasons. Firstly, it means that MPI specific work is localised in a
 *   specific portion of the code rather than being distributed throughout and common functionality
 *   can be grouped up, but also MPI could be switched for another inter-process communication
 *   technology if one is of particular interest.
 *//*******************************************************************************************/
#ifndef PIOLANCCOMM_INCLUDE_GUARD
#define PIOLANCCOMM_INCLUDE_GUARD
#include "global.hh"
#include <vector>
namespace PIOL { namespace Comm {
/*! \brief The Communication layer interface. Specific communication implementations
 *  work off this base class.
 */
class Interface
{
    protected :
    size_t rank;            //!< A number in the sequence from 0 to some maximum (numRank-1) which indicates the process number.
    size_t numRank;         //!< The total number of processes which are executing together.

    public :
    /*! \brief A virtual destructor to allow deletion.
     */
    virtual ~Interface(void) { }

    /*! \brief Returns the rank of the process executing the function/
     *  \return The rank.
     */
    virtual size_t getRank() const
    {
        return rank;
    }
    /*! \brief Returns the number of processes which are executing together.
     *  \return The number of processes (i.e number of ranks).
     */
    virtual size_t getNumRank() const
    {
        return numRank;
    }

    /*! \brief Pass a geom_t value and return the corresponding value on each process
     *  \param[in] val The local value to use in the gather
     *  \return Return a vector where the nth element is the value from the nth rank.
     */
    virtual std::vector<geom_t> gather(const std::vector<geom_t> & val) const = 0;

    /*! \brief Pass a llint value and return the corresponding value on each process
     *  \param[in] val The local value to use in the gather
     *  \return Return a vector where the nth element is the value from the nth rank.
     */
    virtual std::vector<llint> gather(const std::vector<llint> & val) const = 0;

    /*! \brief Pass a size_t value and return the corresponding value on each process
     *  \param[in] val The local value to use in the gather
     *  \return Return a vector where the nth element is the value from the nth rank.
     */
    virtual std::vector<size_t> gather(const std::vector<size_t> & val) const = 0;

    virtual size_t sum(size_t val) = 0;
    virtual size_t max(size_t val) = 0;

    /*! \brief A barrier between all processes which are members of the communication collective.
     */
    virtual void barrier(void) const = 0;    //!< Implementations of this pure virtual function will perform a collective wait.
};
}}
#endif
