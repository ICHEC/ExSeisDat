////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/SEGYRuleEntry.hh"
#include "ExSeisDat/PIOL/segy_utils.hh"
#include "ExSeisDat/utils/encoding/number_encoding.hh"

using namespace exseis::utils;

namespace exseis {
namespace PIOL {
namespace SEGY_utils {

void insertParam(
  size_t sz, const Param* prm, unsigned char* buf, size_t stride, size_t skip)
{
    if (prm == nullptr || !sz) return;

    auto r       = prm->r;
    size_t start = r->start;

    if (r->numCopy) {
        if (!stride) {
            std::copy(
              &prm->c[skip * SEGY_utils::getMDSz()],
              &prm->c[(skip + sz) * SEGY_utils::getMDSz()], buf);
        }
        else {
            for (size_t i = 0; i < sz; i++) {
                std::copy(
                  &prm->c[(i + skip) * SEGY_utils::getMDSz()],
                  &prm->c[(skip + i + 1LU) * SEGY_utils::getMDSz()],
                  &buf[i * (stride + SEGY_utils::getMDSz())]);
            }
        }
    }

    for (size_t i = 0; i < sz; i++) {
        unsigned char* md = &buf[(r->extent() + stride) * i];

// Compiler defects
#if defined(__INTEL_COMPILER) || __GNUC__ < 6
        std::unordered_map<Tr, int16_t, EnumHash> scal;
#else
        std::unordered_map<Tr, int16_t> scal;
#endif

        std::vector<const SEGYFloatRuleEntry*> rule;

        for (const auto v : r->translate) {

            const auto t = v.second;
            size_t loc   = t->loc - start - 1LU;

            switch (t->type()) {

                case RuleEntry::MdType::Float: {

                    rule.push_back(dynamic_cast<SEGYFloatRuleEntry*>(t));

                    auto tr = static_cast<Tr>(rule.back()->scalLoc);

                    int16_t scal1 =
                      (scal.find(tr) != scal.end() ? scal[tr] : 1);
                    int16_t scal2 =
                      find_scalar(prm->f[(i + skip) * r->numFloat + t->num]);

                    // if the scale is bigger than 1 that means we need to use
                    // the largest to ensure conservation of the most
                    // significant  digit otherwise we choose the scale that
                    // preserves the  most digits after the decimal place.
                    scal[tr] =
                      ((scal1 > 1 || scal2 > 1) ? std::max(scal1, scal2) :
                                                  std::min(scal1, scal2));

                } break;

                case RuleEntry::MdType::Short: {

                    const auto be_short =
                      to_big_endian(prm->s[(i + skip) * r->numShort + t->num]);

                    std::copy(
                      std::begin(be_short), std::end(be_short), &md[loc]);

                } break;

                case RuleEntry::MdType::Long: {

                    const auto be_long = to_big_endian<int32_t>(
                      int32_t(prm->i[(i + skip) * r->numLong + t->num]));

                    std::copy(std::begin(be_long), std::end(be_long), &md[loc]);

                } break;

                default:
                    break;
            }
        }

        // Finish off the floats. Floats are inherently annoying in SEG-Y
        for (const auto& s : scal) {
            const auto be = to_big_endian(s.second);

            std::copy(
              std::begin(be), std::end(be), &md[size_t(s.first) - start - 1LU]);
        }

        for (size_t j = 0; j < rule.size(); j++) {
            exseis::utils::Floating_point gscale =
              parse_scalar(scal[static_cast<Tr>(rule[j]->scalLoc)]);

            const auto be = to_big_endian(int32_t(std::lround(
              prm->f[(i + skip) * r->numFloat + rule[j]->num] / gscale)));

            std::copy(
              std::begin(be), std::end(be), &md[rule[j]->loc - start - 1LU]);
        }
    }
}

void extractParam(
  size_t sz, const unsigned char* buf, Param* prm, size_t stride, size_t skip)
{
    if (prm == nullptr || !sz) return;
    Rule* r = prm->r.get();

    if (r->numCopy) {
        if (!stride) {
            std::copy(
              buf, &buf[sz * SEGY_utils::getMDSz()],
              &prm->c[skip * SEGY_utils::getMDSz()]);
        }
        else {
            const size_t mdsz = SEGY_utils::getMDSz();
            for (size_t i = 0; i < sz; i++) {
                std::copy(
                  &buf[i * (stride + mdsz)], &buf[i * (stride + mdsz) + mdsz],
                  &prm->c[(i + skip) * mdsz]);
            }
        }
    }

    for (size_t i = 0; i < sz; i++) {
        const unsigned char* md = &buf[(r->extent() + stride) * i];
        // Loop through each rule and extract data
        for (const auto v : r->translate) {
            const auto t = v.second;
            size_t loc   = t->loc - r->start - 1LU;
            switch (t->type()) {
                case RuleEntry::MdType::Float:
                    prm->f[(i + skip) * r->numFloat + t->num] =
                      parse_scalar(getHost<int16_t>(
                        &md
                          [dynamic_cast<SEGYFloatRuleEntry*>(t)->scalLoc
                           - r->start - 1LU]))
                      * exseis::utils::Floating_point(
                          getHost<int32_t>(&md[loc]));
                    break;
                case RuleEntry::MdType::Short:
                    prm->s[(i + skip) * r->numShort + t->num] =
                      getHost<int16_t>(&md[loc]);
                    break;
                case RuleEntry::MdType::Long:
                    prm->i[(i + skip) * r->numLong + t->num] =
                      getHost<int32_t>(&md[loc]);
                default:
                    break;
            }
        }
    }
}

exseis::utils::Floating_point parse_scalar(int16_t segy_scalar)
{
    // If scale is zero, we assume unscaled, i.e. 1.
    if (segy_scalar == 0) segy_scalar = 1;

    // Positive segy_scalar represents multiplication by value
    if (segy_scalar > 0) {
        return static_cast<exseis::utils::Floating_point>(segy_scalar);
    }
    // Negative segy_scalar represents division by value
    return 1 / static_cast<exseis::utils::Floating_point>(-segy_scalar);
}

int16_t find_scalar(exseis::utils::Floating_point val)
{
    constexpr exseis::utils::Integer tenk = 10000LL;

    // First we need to determine what scale is required to store the
    // biggest decimal value of the int.
    exseis::utils::Integer integer_part = exseis::utils::Integer(val);
    int32_t int_part                    = integer_part;

    if (integer_part != int_part) {
        // Starting with the smallest scale factor, see what is the smallest
        // scale we can apply and still hold the integer portion.
        // We drop as much precision as it takes to store the most significant
        // digit.
        for (int32_t scal = 10; scal <= tenk; scal *= 10) {
            exseis::utils::Integer v = integer_part / scal;
            int32_t iv               = v;
            if (v == iv) return scal;
        }
        return 0;
    }
    else {
        // Get the first four digits
        exseis::utils::Integer digits =
          std::llround(val * exseis::utils::Floating_point(tenk))
          - integer_part * tenk;
        // if the digits are all zero we don't need any scaling
        if (digits != 0) {
            // We try the most negative scale values we can first.
            //(scale = - 10000 / i)
            for (int32_t i = 1; i < tenk; i *= 10) {
                if (digits % (i * 10)) {
                    int16_t scaleFactor = -tenk / i;
                    // Now we test that we can still store the most significant
                    // byte
                    exseis::utils::Floating_point scal =
                      parse_scalar(scaleFactor);

                    // int32_t t = exseis::utils::Integer(val / scal) - digits;
                    int32_t t = std::lround(val / scal);
                    t /= -scaleFactor;

                    if (t == integer_part) return scaleFactor;
                }
            }
        }
        return 1;
    }
}

}  // namespace SEGY_utils
}  // namespace PIOL
}  // namespace exseis
