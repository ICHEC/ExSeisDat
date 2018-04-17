////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation for functions in the param_utils namespace.
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/param_utils.hh"
#include "ExSeisDat/PIOL/segy_utils.hh"

namespace exseis {
namespace PIOL {
namespace param_utils {

void cpyPrm(const size_t j, const Param* src, const size_t k, Param* dst)
{
    if (
      src == PIOL_PARAM_NULL || src == nullptr || dst == PIOL_PARAM_NULL
      || dst == nullptr)
        return;

    Rule* srule = src->r.get();
    Rule* drule = dst->r.get();

    if (srule->numCopy) {
        SEGY_utils::extractParam(
          1LU, &src->c[j * SEGY_utils::getMDSz()], dst, 0LU, k);
    }

    if (srule == drule) {
        Rule* r = srule;

        for (size_t i = 0; i < r->numFloat; i++)
            dst->f[k * r->numFloat + i] = src->f[j * r->numFloat + i];
        for (size_t i = 0; i < r->numLong; i++)
            dst->i[k * r->numLong + i] = src->i[j * r->numLong + i];
        for (size_t i = 0; i < r->numShort; i++)
            dst->s[k * r->numShort + i] = src->s[j * r->numShort + i];
        for (size_t i = 0; i < r->numIndex; i++)
            dst->t[k * r->numIndex + i] = src->t[j * r->numIndex + i];
    }
    else {
        // For each rule in source
        for (auto& m : srule->translate) {
            // Check for a rule in destination
            auto valit = drule->translate.find(m.first);

            // if the rule is in the destination and the types match
            if (valit != drule->translate.end()) {
                RuleEntry* dent = valit->second;
                RuleEntry* sent = m.second;
                if (dent->type() == sent->type()) {
                    switch (m.second->type()) {
                        case RuleEntry::MdType::Float:
                            dst->f[drule->numFloat * k + dent->num] =
                              src->f[srule->numFloat * j + sent->num];
                            break;
                        case RuleEntry::MdType::Long:
                            dst->i[drule->numLong * k + dent->num] =
                              src->i[srule->numLong * j + sent->num];
                            break;
                        case RuleEntry::MdType::Short:
                            dst->s[drule->numShort * k + dent->num] =
                              src->s[srule->numShort * j + sent->num];
                            break;
                        case RuleEntry::MdType::Index:
                            dst->t[drule->numIndex * k + dent->num] =
                              src->t[srule->numIndex * j + sent->num];
                        default:
                            break;
                    }
                }
            }
        }
    }
}

}  // namespace param_utils
}  // namespace PIOL
}  // namespace exseis
