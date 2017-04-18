/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date November 2016
 *   \brief The Set layer interface
*//*******************************************************************************************/
#ifndef APIPIOLSET_INCLUDE_GUARD
#define APIPIOLSET_INCLUDE_GUARD
#include "set/set.hh"
#include "fileops.hh"
namespace PIOL
{
extern std::shared_ptr<File::Rule> getMaxRules(void);

class Set : public InternalSet
{
    public :
    using InternalSet::sort;

    /*! Constructor
     *  \param[in] piol The PIOL object.
     *  \param[in] pattern The file-matching pattern
     *  \param[in] outfix_ The output file-name prefix
     *  \param[in] rule_ Contains a pointer to the rules to use for trace parameters.
     */
    Set(Piol piol_, std::string pattern, std::string outfix_, std::shared_ptr<File::Rule> rule_ = getMaxRules()) :
             InternalSet(piol_, pattern, outfix_, rule_)  {  }

    /*! Constructor
     *  \param[in] piol The PIOL object.
     *  \param[in] pattern The file-matching pattern
     *  \param[in] rule_ Contains a pointer to the rules to use for trace parameters.
     */
    Set(Piol piol_, std::string pattern, std::shared_ptr<File::Rule> rule_ = getMaxRules()) :
             InternalSet(piol_, pattern, "", rule_)  {  }

    /*! Constructor
     *  \param[in] piol The PIOL object.
     *  \param[in] rule_ Contains a pointer to the rules to use for trace parameters.
     */
    Set(Piol piol_, std::shared_ptr<File::Rule> rule_ = getMaxRules()) :
             InternalSet(piol_, rule_)  {  }

    /*! Sort the set by the specified sort type.
     *  \param[in] s The set handle
     *  \param[in] type The sort type
     */
    void sort(SortType type);

    /*! Get the min and the max of a set of parameters passed. This is a parallel operation. It is
     *  the collective min and max across all processes (which also must all call this file).
     *  \param[in] m1 The first parameter type
     *  \param[in] m2 The second parameter type
     *  \param[out] minmax An array of structures containing the minimum item.x,  maximum item.x, minimum item.y, maximum item.y
     *  and their respective trace numbers.
     */
    void getMinMax(Meta m1, Meta m2, CoordElem * minmax);

    /*! Preform 2 tailed taper on a set of traces
     * \param[in] nt The number of traces
     * \param[in] ns The number of samples per trace.
     * \param[in] trc The vector of samples in all traces.
     * \param[in] type The type of taper to be applied to traces.
     * \param[in] ntpstr The length of left-tail taper ramp.
     * \param[in] ntpend The length of right-tail taper ramp.
     */
    void taper(size_t nt, size_t ns, trace_t * trc, TaperType type, size_t ntpstr, size_t ntpend);

    /*! Preform 1 tailed taper on a set of traces
     * \param[in] nt The number of traces
     * \param[in] ns The number of samples per trace.
     * \param[in] trc The vector of samples in all traces.
     * \param[in] type The type of taper to be applied to traces.
     * \param[in] ntpstr The length of taper ramp.
     */
    void taper(size_t nt,size_t ns, trace_t* trc, TaperType type, size_t ntpstr);
};
}
#endif
