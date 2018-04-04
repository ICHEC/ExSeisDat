////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief   The Param class
/// @details The Param class is used to store the trace parameter structure.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_PARAM_HH
#define EXSEISDAT_PIOL_PARAM_HH

#include "ExSeisDat/PIOL/Rule.hh"

#include <cstddef>
#include <memory>
#include <vector>

namespace PIOL {

/*! Derived class for initialising the trace parameter structure
 *  and storing a structure with the necessary rules.
 */
struct Param {
    /// Floating point array.
    std::vector<geom_t> f;

    /// Integer array.
    std::vector<llint> i;

    /// Short array.
    std::vector<int16_t> s;

    /// trace number array.
    std::vector<size_t> t;

    /// storage for a file format specific copy
    std::vector<uchar> c;

    /// The rules which describe the indexing of the arrays.
    std::shared_ptr<Rule> r;

    /// The number of sets of trace parameters.
    size_t sz;


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
        // Arbitrary function
        return f.size() < a.f.size();
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

}  // namespace PIOL

#endif  // EXSEISDAT_PIOL_PARAM_HH
