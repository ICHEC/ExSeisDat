////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/piol/metadata/rules/Segy_rule_entry.hh"
#include "exseisdat/piol/segy/utils.hh"
#include "exseisdat/utils/encoding/number_encoding.hh"

#include <cmath>

using namespace exseis::utils;

namespace exseis {
namespace piol {
namespace segy {

void insert_trace_metadata(
    size_t number_of_traces,
    const Trace_metadata& prm,
    unsigned char* buf,
    size_t stride,
    size_t skip)
{

    const auto& r = prm.rules;
    size_t start  = r.start;

    // If a copy of the original metadata is held, write this before the
    // specifically parsed/modified metadata.
    if (r.num_copy != 0) {
        if (stride == 0) {
            std::copy(
                prm.raw_metadata.begin()
                    + skip * segy::segy_trace_header_size(),
                prm.raw_metadata.begin()
                    + (skip + number_of_traces)
                          * segy::segy_trace_header_size(),
                buf);
        }
        else {
            for (size_t i = 0; i < number_of_traces; i++) {
                std::copy(
                    &prm.raw_metadata
                         [(i + skip) * segy::segy_trace_header_size()],
                    &prm.raw_metadata
                         [(skip + i + 1LU) * segy::segy_trace_header_size()],
                    &buf[i * (stride + segy::segy_trace_header_size())]);
            }
        }
    }

    // Loop through the traces, writing the stored metadata.
    for (size_t i = 0; i < number_of_traces; i++) {

        // Get the position in the metadata buffer where the current trace
        // metadata starts.
        unsigned char* md = &buf[(r.extent() + stride) * i];

        // Keep a list of entries that need to be scaled for the SEGY format.
        std::unordered_map<
            Trace_header_offsets, int16_t,
            exseis::utils::Enum_hash<Trace_header_offsets>>
            scal;

        std::vector<std::pair<Trace_metadata_key, const Segy_float_rule_entry*>>
            floating_point_rules;

        for (const auto& v : r.rule_entry_map) {

            const auto& entry = v.first;
            const auto& t     = v.second;
            const size_t loc  = t->loc - start - 1LU;

            switch (t->type()) {

                case Rule_entry::MdType::Float: {

                    floating_point_rules.push_back(
                        {entry, dynamic_cast<Segy_float_rule_entry*>(t.get())});

                    auto tr = static_cast<Trace_header_offsets>(
                        floating_point_rules.back().second->scalar_location);

                    const int16_t scal1 =
                        (scal.find(tr) != scal.end() ? scal[tr] : 1);
                    const int16_t scal2 =
                        find_scalar(prm.get_floating_point(i + skip, entry));

                    // if the scale is bigger than 1 that means we need to use
                    // the largest to ensure conservation of the most
                    // significant  digit otherwise we choose the scale that
                    // preserves the  most digits after the decimal place.
                    scal[tr] =
                        ((scal1 > 1 || scal2 > 1) ? std::max(scal1, scal2) :
                                                    std::min(scal1, scal2));

                } break;

                case Rule_entry::MdType::Short: {

                    const std::array<unsigned char, 2> be_short =
                        to_big_endian<int16_t>(
                            int16_t(prm.get_integer(i + skip, entry)));

                    std::copy(
                        std::begin(be_short), std::end(be_short), &md[loc]);

                } break;

                case Rule_entry::MdType::Long: {

                    const std::array<unsigned char, 4> be_long =
                        to_big_endian<int32_t>(
                            int32_t(prm.get_integer(i + skip, entry)));

                    std::copy(std::begin(be_long), std::end(be_long), &md[loc]);

                } break;

                case Rule_entry::MdType::Index:
                    // Index rules aren't stored on disk.
                    break;

                case Rule_entry::MdType::Copy:
                    // The Copy rule has been handled above.
                    break;
            }
        }

        // Finish off the floats. Floats are inherently annoying in SEG-Y
        for (const auto& s : scal) {
            const auto be = to_big_endian(s.second);

            std::copy(
                std::begin(be), std::end(be),
                &md[size_t(s.first) - start - 1LU]);
        }

        for (size_t j = 0; j < floating_point_rules.size(); j++) {
            exseis::utils::Floating_point gscale =
                parse_scalar(scal[static_cast<Trace_header_offsets>(
                    floating_point_rules[j].second->scalar_location)]);

            const auto be = to_big_endian(int32_t(std::lround(
                prm.get_floating_point(i + skip, floating_point_rules[j].first)
                / gscale)));

            std::copy(
                std::begin(be), std::end(be),
                &md[floating_point_rules[j].second->loc - start - 1LU]);
        }
    }
}

void extract_trace_metadata(
    size_t number_of_traces,
    const unsigned char* buf,
    Trace_metadata& prm,
    size_t stride,
    size_t skip)
{

    const Rule& r = prm.rules;

    // If a copy of the metadata is required, read this first.
    if (r.num_copy != 0) {
        if (stride == 0) {
            std::copy(
                buf, &buf[number_of_traces * segy::segy_trace_header_size()],
                &prm.raw_metadata[skip * segy::segy_trace_header_size()]);
        }
        else {
            // The size of the trace metadata in bytes
            const size_t metadata_size = segy::segy_trace_header_size();
            for (size_t i = 0; i < number_of_traces; i++) {
                std::copy(
                    &buf[i * (stride + metadata_size)],
                    &buf[i * (stride + metadata_size) + metadata_size],
                    &prm.raw_metadata[(i + skip) * metadata_size]);
            }
        }
    }

    // For each trace, read the specified metadata
    for (size_t i = 0; i < number_of_traces; i++) {

        // Get the starting point in the metadata buffer for the current trace
        const unsigned char* md = &buf[(r.extent() + stride) * i];

        // Loop through each rule and extract data
        for (const auto& v : r.rule_entry_map) {

            const auto& entry = v.first;
            const auto& t     = v.second;
            size_t loc        = t->loc - r.start - 1LU;

            switch (t->type()) {
                case Rule_entry::MdType::Float: {

                    const size_t scalar_offset =
                        dynamic_cast<Segy_float_rule_entry*>(t.get())
                            ->scalar_location
                        - r.start - 1LU;

                    const auto parsed_scalar =
                        parse_scalar(from_big_endian<int16_t>(
                            md[scalar_offset + 0lu], md[scalar_offset + 1lu]));

                    const auto unscaled_value = from_big_endian<int32_t>(
                        md[loc + 0lu], md[loc + 1lu], md[loc + 2lu],
                        md[loc + 3lu]);


                    using F_type = exseis::utils::Floating_point;

                    const auto value =
                        parsed_scalar * static_cast<F_type>(unscaled_value);

                    prm.set_floating_point(i + skip, entry, value);

                } break;

                case Rule_entry::MdType::Short: {

                    const auto value =
                        from_big_endian<int16_t>(md[loc + 0lu], md[loc + 1lu]);

                    prm.set_integer(i + skip, entry, value);

                } break;

                case Rule_entry::MdType::Long: {

                    const auto value = from_big_endian<int32_t>(
                        md[loc + 0lu], md[loc + 1lu], md[loc + 2lu],
                        md[loc + 3lu]);

                    prm.set_integer(i + skip, entry, value);

                } break;

                case Rule_entry::MdType::Index:
                    // Index types aren't stored in the file.
                    break;

                case Rule_entry::MdType::Copy:
                    // The Copy rule has already been handled above.
                    break;
            }
        }
    }
}

exseis::utils::Floating_point parse_scalar(int16_t segy_scalar)
{
    // If scale is zero, we assume unscaled, i.e. 1.
    if (segy_scalar == 0) {
        segy_scalar = 1;
    }

    // Positive segy_scalar represents multiplication by value
    if (segy_scalar > 0) {
        return static_cast<exseis::utils::Floating_point>(segy_scalar);
    }

    // Negative segy_scalar represents division by value
    return 1 / static_cast<exseis::utils::Floating_point>(-segy_scalar);
}

int16_t find_scalar(exseis::utils::Floating_point val)
{
    static_assert(
        std::numeric_limits<int16_t>::max() > 10000,
        "int16_t isn't big enough to hold the value 10,000.");

    constexpr int16_t tenk = 10000;

    // First we need to determine what scale is required to store the
    // biggest decimal value of the int.
    exseis::utils::Integer integer_part = exseis::utils::Integer(val);
    int32_t int_part                    = static_cast<int32_t>(integer_part);

    if (integer_part != int_part) {
        // Starting with the smallest scale factor, see what is the smallest
        // scale we can apply and still hold the integer portion.
        // We drop as much precision as it takes to store the most significant
        // digit.
        for (int16_t scal = 10; scal <= tenk; scal *= 10) {
            exseis::utils::Integer v = integer_part / scal;
            int32_t iv               = static_cast<int32_t>(v);

            if (v == iv) {
                return scal;
            }
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
            for (int16_t i = 1; i < tenk; i *= 10) {
                if ((digits % (i * 10)) != 0) {
                    int16_t scale_factor = -tenk / i;
                    // Now we test that we can still store the most significant
                    // byte
                    exseis::utils::Floating_point scal =
                        parse_scalar(scale_factor);

                    // int32_t t = exseis::utils::Integer(val / scal) - digits;
                    int32_t t = static_cast<int32_t>(std::lround(val / scal));
                    t /= -scale_factor;

                    if (t == integer_part) {
                        return scale_factor;
                    }
                }
            }
        }
        return 1;
    }
}

}  // namespace segy
}  // namespace piol
}  // namespace exseis
