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
    using InternalSet::taper;
    using InternalSet::agc;
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

    /*! Perform tailed taper on a set of traces
     * \param[in] s The set handle
     * \param[in] type The type of taper to be applied to traces.
     * \param[in] ntpstr The length of left-tail taper ramp.
     * \param[in] ntpend The length of right-tail taper ramp.
     */
    void taper(TaperType type, size_t nTailLft, size_t nTailRt = 0U);

    /*! Scale traces using automatic gain control for visualization
     * \param[in] s The set handle
     * \param[in] type They type of agc scaling function used
     * \param[in] window Length of the agc window
     * \param[in] normR Normalization value
     */
    void Set::agc(AGCType type, size_t window, trace_t normR);
};
}
#endif
