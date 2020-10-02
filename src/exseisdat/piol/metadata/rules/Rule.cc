////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/piol/metadata/rules/Rule.hh"
#include "exseisdat/piol/metadata/rules/Segy_rule_entry.hh"

#include <cassert>
#include <numeric>

namespace exseis {
namespace piol {
inline namespace metadata {
inline namespace rules {

namespace {

/// A list of the default META values
const Trace_metadata_key default_metas[] = {
    Trace_metadata_key::x_src,  Trace_metadata_key::y_src,
    Trace_metadata_key::x_rcv,  Trace_metadata_key::y_rcv,
    Trace_metadata_key::xCmp,   Trace_metadata_key::yCmp,
    Trace_metadata_key::Offset, Trace_metadata_key::il,
    Trace_metadata_key::xl,     Trace_metadata_key::tn};

/// A list of the extra META values
const Trace_metadata_key extra_metas[] = {
    Trace_metadata_key::tnl,      Trace_metadata_key::tnr,
    Trace_metadata_key::tne,      Trace_metadata_key::SrcNum,
    Trace_metadata_key::Tic,      Trace_metadata_key::VStack,
    Trace_metadata_key::HStack,   Trace_metadata_key::RGElev,
    Trace_metadata_key::SSElev,   Trace_metadata_key::SDElev,
    Trace_metadata_key::ns,       Trace_metadata_key::sample_interval,
    Trace_metadata_key::ShotNum,  Trace_metadata_key::TraceUnit,
    Trace_metadata_key::TransUnit};


/// @brief Implementation of the copy constructor and copy assignment for
///        exseis::piol::Rule.
///
/// @param[in] lhs The lhs of assignment, or `this` for construction.
/// @param[in] rule_entry_map The `rule_entry_map` argument of the rhs
///                           constructor.
/// @param[in] full           The `full` argument of the rhs constructor.
///
void copy_impl(Rule& lhs, const Rule::Rule_entry_map& rule_entry_map, bool full)
{
    lhs.rule_entry_map.clear();

    lhs.num_copy = 0;

    for (const auto& t : rule_entry_map) {
        const auto& key   = t.first;
        const auto& value = t.second;

        lhs.rule_entry_map.insert({key, value->clone()});

        if (value->type() == Rule_entry::MdType::Copy) {
            lhs.num_copy = 1;
        }
    }

    lhs.flag.fullextent = full;

    if (full) {
        lhs.start          = 0LU;
        lhs.end            = segy::segy_trace_header_size();
        lhs.flag.badextent = false;
    }
    else {
        lhs.flag.badextent = true;
        lhs.extent();
    }
}

}  // namespace

Rule& Rule::operator=(const Rule& rhs)
{
    copy_impl(*this, rhs.rule_entry_map, rhs.flag.fullextent);

    return *this;
}

Rule::Rule(const Rule_entry_map& rule_entry_map, bool full)
{
    copy_impl(*this, rule_entry_map, full);
}

Rule::Rule(
    const std::vector<Trace_metadata_key>& mlist,
    bool full,
    bool defaults,
    bool extras)
{

    // TODO: Change this when extents are flexible
    flag.fullextent = full;
    add_index(Trace_metadata_key::gtn);
    add_index(Trace_metadata_key::ltn);

    for (auto m : mlist) {
        add_rule(m);
    }

    if (defaults) {
        for (auto m : default_metas) {
            add_rule(m);
        }
    }

    if (extras) {
        for (auto m : extra_metas) {
            add_rule(m);
        }
    }

    if (flag.fullextent) {
        start          = 0LU;
        end            = segy::segy_trace_header_size();
        flag.badextent = false;
    }
    else {
        flag.badextent = true;
        extent();
    }
}

Rule::Rule(bool full, bool defaults, bool extras) :
    Rule({}, full, defaults, extras)
{
}

Rule::~Rule() = default;

bool Rule::add_rule(Trace_metadata_key m)
{
    if (rule_entry_map.find(m) != rule_entry_map.end()) {
        return false;
    }

    switch (m) {
        case Trace_metadata_key::gtn:
            add_index(Trace_metadata_key::gtn);
            return true;

        case Trace_metadata_key::ltn:
            add_index(Trace_metadata_key::ltn);
            return true;

        case Trace_metadata_key::WtrDepSrc:
            add_segy_float(
                m, segy::Trace_header_offsets::WtrDepSrc,
                segy::Trace_header_offsets::ScaleElev);
            return true;

        case Trace_metadata_key::WtrDepRcv:
            add_segy_float(
                m, segy::Trace_header_offsets::WtrDepRcv,
                segy::Trace_header_offsets::ScaleElev);
            return true;

        case Trace_metadata_key::x_src:
            add_segy_float(
                m, segy::Trace_header_offsets::x_src,
                segy::Trace_header_offsets::ScaleCoord);
            return true;

        case Trace_metadata_key::y_src:
            add_segy_float(
                m, segy::Trace_header_offsets::y_src,
                segy::Trace_header_offsets::ScaleCoord);
            return true;

        case Trace_metadata_key::x_rcv:
            add_segy_float(
                m, segy::Trace_header_offsets::x_rcv,
                segy::Trace_header_offsets::ScaleCoord);
            return true;

        case Trace_metadata_key::y_rcv:
            add_segy_float(
                m, segy::Trace_header_offsets::y_rcv,
                segy::Trace_header_offsets::ScaleCoord);
            return true;

        case Trace_metadata_key::xCmp:
            add_segy_float(
                m, segy::Trace_header_offsets::xCmp,
                segy::Trace_header_offsets::ScaleCoord);
            return true;

        case Trace_metadata_key::yCmp:
            add_segy_float(
                m, segy::Trace_header_offsets::yCmp,
                segy::Trace_header_offsets::ScaleCoord);
            return true;

        case Trace_metadata_key::coordinate_scalar:
            add_short(m, segy::Trace_header_offsets::ScaleCoord);
            return true;

        case Trace_metadata_key::il:
            add_long(m, segy::Trace_header_offsets::il);
            return true;

        case Trace_metadata_key::xl:
            add_long(m, segy::Trace_header_offsets::xl);
            return true;

        case Trace_metadata_key::Offset:
            add_long(m, segy::Trace_header_offsets::CDist);
            return true;

        case Trace_metadata_key::tn:
            add_long(m, segy::Trace_header_offsets::SeqFNum);
            return true;

        case Trace_metadata_key::Copy:
            add_copy();
            return true;

        case Trace_metadata_key::tnl:
            add_long(m, segy::Trace_header_offsets::SeqNum);
            return true;

        case Trace_metadata_key::tnr:
            add_long(m, segy::Trace_header_offsets::TORF);
            return true;

        case Trace_metadata_key::tne:
            add_long(m, segy::Trace_header_offsets::SeqNumEns);
            return true;

        case Trace_metadata_key::SrcNum:
            add_long(m, segy::Trace_header_offsets::ENSrcNum);
            return true;

        case Trace_metadata_key::Tic:
            add_short(m, segy::Trace_header_offsets::TIC);
            return true;

        case Trace_metadata_key::VStack:
            add_short(m, segy::Trace_header_offsets::VStackCnt);
            return true;

        case Trace_metadata_key::HStack:
            add_short(m, segy::Trace_header_offsets::HStackCnt);
            return true;

        case Trace_metadata_key::RGElev:
            add_segy_float(
                m, segy::Trace_header_offsets::RcvElv,
                segy::Trace_header_offsets::ScaleElev);
            return true;

        case Trace_metadata_key::SSElev:
            add_segy_float(
                m, segy::Trace_header_offsets::SurfElvSrc,
                segy::Trace_header_offsets::ScaleElev);
            return true;

        case Trace_metadata_key::SDElev:
            add_segy_float(
                m, segy::Trace_header_offsets::SrcDpthSurf,
                segy::Trace_header_offsets::ScaleElev);
            return true;

        case Trace_metadata_key::ns:
            add_short(m, segy::Trace_header_offsets::Ns);
            return true;

        case Trace_metadata_key::sample_interval:
            add_short(m, segy::Trace_header_offsets::sample_interval);
            return true;

        case Trace_metadata_key::ShotNum:
            add_segy_float(
                m, segy::Trace_header_offsets::ShotNum,
                segy::Trace_header_offsets::ShotScal);
            return true;

        case Trace_metadata_key::TraceUnit:
            add_short(m, segy::Trace_header_offsets::ValMeas);
            return true;

        case Trace_metadata_key::TransUnit:
            add_short(m, segy::Trace_header_offsets::TransUnit);
            return true;

        // Non-standard and not handled automatically
        case Trace_metadata_key::dsdr:
        case Trace_metadata_key::Misc1:
        case Trace_metadata_key::Misc2:
        case Trace_metadata_key::Misc3:
        case Trace_metadata_key::Misc4:
            assert(
                false
                && "Non-standard Trace_metadata_key value, not handled automatically.");

            return false;
    }

    assert(false && "Unknown Trace_metadata_key value.");

    return false;
}

size_t Rule::extent()
{
    if (flag.fullextent) {
        return segy::segy_trace_header_size();
    }

    if (flag.badextent) {
        start = segy::segy_trace_header_size();
        end   = 0LU;

        for (const auto& r : rule_entry_map) {
            if (r.second->type() != Rule_entry::MdType::Index) {
                start = std::min(start, r.second->min());
                end   = std::max(end, r.second->max());
            }
        }

        flag.badextent = false;
    }

    return end - start;
}


size_t Rule::extent() const
{
    if (flag.fullextent) {
        return segy::segy_trace_header_size();
    }

    auto local_start = start;
    auto local_end   = end;

    if (flag.badextent) {
        local_start = segy::segy_trace_header_size();
        local_end   = 0LU;

        for (const auto& r : rule_entry_map) {
            if (r.second->type() != Rule_entry::MdType::Index) {
                local_start = std::min(start, r.second->min());
                local_end   = std::max(end, r.second->max());
            }
        }
    }

    return local_end - local_start;
}

// TODO: These can be optimised to stop the double lookup if required.
void Rule::add_long(Trace_metadata_key m, segy::Trace_header_offsets loc)
{
    auto ent = rule_entry_map.find(m);
    if (ent != rule_entry_map.end()) {
        rm_rule(m);
    }

    rule_entry_map.insert({m, std::make_unique<Segy_long_rule_entry>(loc)});

    flag.badextent = (!flag.fullextent);
}

void Rule::add_short(Trace_metadata_key m, segy::Trace_header_offsets loc)
{
    auto ent = rule_entry_map.find(m);
    if (ent != rule_entry_map.end()) {
        rm_rule(m);
    }

    rule_entry_map.insert({m, std::make_unique<Segy_short_rule_entry>(loc)});

    flag.badextent = (!flag.fullextent);
}

void Rule::add_segy_float(
    Trace_metadata_key m,
    segy::Trace_header_offsets loc,
    segy::Trace_header_offsets scalar_location)
{
    auto ent = rule_entry_map.find(m);
    if (ent != rule_entry_map.end()) {
        rm_rule(m);
    }

    rule_entry_map.insert(
        {m, std::make_unique<Segy_float_rule_entry>(loc, scalar_location)});

    flag.badextent = (!flag.fullextent);
}

void Rule::add_index(Trace_metadata_key m)
{
    auto ent = rule_entry_map.find(m);
    if (ent != rule_entry_map.end()) {
        rm_rule(m);
    }

    rule_entry_map.insert({m, std::make_unique<Segy_index_rule_entry>()});
}

void Rule::add_copy()
{
    if (num_copy == 0) {
        rule_entry_map.insert({Trace_metadata_key::Copy,
                               std::make_unique<Segy_copy_rule_entry>()});
        num_copy++;
    }
}

void Rule::rm_rule(Trace_metadata_key m)
{
    auto iter = rule_entry_map.find(m);
    if (iter != rule_entry_map.end()) {
        rule_entry_map.erase(m);

        flag.badextent = (!flag.fullextent);
    }
}

const Rule_entry* Rule::get_entry(Trace_metadata_key entry) const
{
    const auto it = rule_entry_map.find(entry);

    if (it == rule_entry_map.end()) {
        return nullptr;
    }

    return it->second.get();
}

size_t Rule::memory_usage() const
{
    const auto add_memory_usage = [](auto init, const auto& rule_entry_it) {
        return init + rule_entry_it.second->memory_usage();
    };

    return std::accumulate(
               rule_entry_map.begin(), rule_entry_map.end(), 0,
               add_memory_usage)
           + sizeof(Rule);
}

size_t Rule::memory_usage_per_header() const
{
    const auto get_sizeof = [](Rule_entry::MdType type) {
        switch (type) {
            case Rule_entry::MdType::Float:
                return sizeof(exseis::utils::Floating_point);

            case Rule_entry::MdType::Long:
                return sizeof(exseis::utils::Integer);

            case Rule_entry::MdType::Short:
                return sizeof(int16_t);

            case Rule_entry::MdType::Index:
                return sizeof(size_t);

            case Rule_entry::MdType::Copy:
                return segy::segy_trace_header_size();
        }

        assert(false && "Rule::memory_usage_per_header: Unknown MdType");
    };

    const auto add_sizeof = [&](auto init, const auto& rule_entry_it) {
        return init + get_sizeof(rule_entry_it.second->type());
    };

    return std::accumulate(
        rule_entry_map.begin(), rule_entry_map.end(), 0, add_sizeof);
}

bool Rule::add_rule(const Rule& r)
{
    for (auto& m : r.rule_entry_map) {
        if (rule_entry_map.find(m.first) == rule_entry_map.end()) {
            switch (m.second->type()) {
                case Rule_entry::MdType::Float:
                    add_segy_float(
                        m.first,
                        static_cast<segy::Trace_header_offsets>(m.second->loc),
                        static_cast<segy::Trace_header_offsets>(
                            static_cast<Segy_float_rule_entry*>(m.second.get())
                                ->scalar_location));
                    break;

                case Rule_entry::MdType::Long:
                    add_long(
                        m.first,
                        static_cast<segy::Trace_header_offsets>(m.second->loc));
                    break;

                case Rule_entry::MdType::Short:
                    add_short(
                        m.first,
                        static_cast<segy::Trace_header_offsets>(m.second->loc));
                    break;

                case Rule_entry::MdType::Index:
                    add_index(m.first);
                    break;

                case Rule_entry::MdType::Copy:
                    add_copy();
                    break;
            }
        }
    }

    return true;
}

}  // namespace rules
}  // namespace metadata
}  // namespace piol
}  // namespace exseis
