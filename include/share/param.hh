////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief The File layer interface
/// @details The File layer interface is a base class which specific File
///          implementations work off.
////////////////////////////////////////////////////////////////////////////////
#ifndef PIOLSHAREPARAM_INCLUDE_GUARD
#define PIOLSHAREPARAM_INCLUDE_GUARD

#include "api.hh"
#include "global.hh"

namespace PIOL {
namespace File {

struct Rule;

/*! Derived class for initialising the trace parameter structure
 *  and storing a structure with the necessary rules.
 */
struct Param {
    std::vector<geom_t> f;   //!< Floating point array.
    std::vector<llint> i;    //!< Integer array.
    std::vector<int16_t> s;  //!< Short array.
    std::vector<size_t> t;   //!< trace number array.
    std::vector<uchar> c;    //!< storage for a file format specific copy
    std::shared_ptr<Rule>
      r;        //!< The rules which describe the indexing of the arrays.
    size_t sz;  //!< The number of sets of trace parameters.

    /*! Allocate the basic space required to store the arrays and store the
     *  rules.
     *  @param[in] r_ The rules which describe the layout of the arrays.
     *  @param[in] sz The number of sets of trace parameters.
     */
    Param(std::shared_ptr<Rule> r_, const size_t sz);

    /*! Allocate the basic space required to store the arrays and store the
     *  rules. Default rules
     *  @param[in] sz The number of sets of trace parameters.
     */
    Param(const size_t sz = 1);

    /*! Default destructor
     */
    ~Param();

    /*! Return the number of sets of trace parameters.
     *  @return Number of sets
     */
    size_t size(void) const;

    /*! Estimate of the total memory used
     *  @return Return estimate in bytes.
     */
    size_t memUsage(void) const;

    /*! Less-than operator. An operator overload required for template
     *  subsitution
     *  @param[in] a The Param object to compare with
     *  @return Return true if the index of the current object is less than
     *          the right operand.
     */
    bool operator<(Param& a)
    {
        return f.size() < a.f.size();  //Arbitrary function
    }

    /*! Equality operator
     *  @param[in] p Param Structure to compare with.
     *  @return Return true if the structures are equivalent.
     */
    bool operator==(Param& p) const;

    /*! Not-Equal operator
     * @param[in] p Param Structure to compare with.
     * @return Return true if the structures are equivalent.
     */
    bool operator!=(Param& p) const { return !this->operator==(p); }
};

}  // namespace File

typedef std::function<bool(const File::Param*, const size_t, const size_t)>
  CompareP;  //!< A template for the Compare less-than function

}  // namespace PIOL

#endif
