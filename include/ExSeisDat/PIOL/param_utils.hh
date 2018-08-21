////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief The File layer interface
/// @details The File layer interface is a base class which specific File
///          implementations work off.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_PARAM_UTILS_HH
#define EXSEISDAT_PIOL_PARAM_UTILS_HH

#include "ExSeisDat/PIOL/Param.h"

namespace exseis {
namespace PIOL {
namespace param_utils {

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
 *                 passing a exseis::utils::Floating_point,
 *                 exseis::utils::Integer or short.
 *  @param[in] prm The parameter structure
 */
template<typename T>
void setPrm(size_t i, Meta entry, T ret, Param* prm)
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
void cpyPrm(size_t j, const Param* src, size_t k, Param* dst);


}  // namespace param_utils
}  // namespace PIOL
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_PARAM_UTILS_HH
