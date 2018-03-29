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

#include "ExSeisDat/PIOL/Rule.hh"
#include "ExSeisDat/PIOL/RuleEntry.hh"

#include "ExSeisDat/PIOL/anc/global.hh"
#include "ExSeisDat/PIOL/share/api.hh"

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


// Access

/*! Get the value associated with the particular entry.
 *  @tparam    T The type of the value
 *  @param[in] i The trace number
 *  @param[in] entry The meta entry to retrieve.
 *  @param[in] prm The parameter structure
 *  @return Return the value associated with the entry
 */
template<typename T>
T getPrm(size_t i, Meta entry, const Param* prm)
{
    Rule* r       = prm->r.get();
    RuleEntry* id = r->getEntry(entry);
    switch (id->type()) {
        case RuleEntry::MdType::Float:
            return T(prm->f[r->numFloat * i + id->num]);
            break;
        case RuleEntry::MdType::Long:
            return T(prm->i[r->numLong * i + id->num]);
            break;
        case RuleEntry::MdType::Short:
            return T(prm->s[r->numShort * i + id->num]);
            break;
        case RuleEntry::MdType::Index:
            return T(prm->t[r->numIndex * i + id->num]);
            break;
        default:
            return T(0);
            break;
    }
}

/*! Set the value associated with the particular entry.
 *  @tparam T The type of the value
 *  @param[in] i The trace number
 *  @param[in] entry The meta entry to retrieve.
 *  @param[in] ret The parameter return structure which is initialised by
 *                 passing a geom_t, llint or short.
 *  @param[in] prm The parameter structure
 */
template<typename T>
void setPrm(const size_t i, const Meta entry, T ret, Param* prm)
{
    Rule* r       = prm->r.get();
    RuleEntry* id = r->getEntry(entry);
    switch (id->type()) {
        case RuleEntry::MdType::Float:
            prm->f[r->numFloat * i + id->num] = ret;
            break;
        case RuleEntry::MdType::Long:
            prm->i[r->numLong * i + id->num] = ret;
            break;
        case RuleEntry::MdType::Short:
            prm->s[r->numShort * i + id->num] = ret;
            break;
        case RuleEntry::MdType::Index:
            prm->t[r->numIndex * i + id->num] = ret;
        default:
            break;
    }
}

/*! Copy params from one parameter structure to another.
 *  @param[in] j The trace number of the source.
 *  @param[in] src The source parameter structure.
 *  @param[in] k The trace number of the destination.
 *  @param[out] dst The destination parameter structure.
 */
void cpyPrm(const size_t j, const Param* src, const size_t k, Param* dst);


/// A template for the Compare less-than function
typedef std::function<bool(const Param*, const size_t, const size_t)> CompareP;

}  // namespace PIOL

#endif
