////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/piol/Rule.hh"
#include "exseisdat/piol/SEGYRuleEntry.hh"

#include <cassert>
#include <numeric>

namespace exseis {
namespace piol {

namespace {

/// A list of the default META values
const Meta default_metas[] = {
  Meta::x_src, Meta::y_src,  Meta::x_rcv, Meta::y_rcv, Meta::xCmp,
  Meta::yCmp,  Meta::Offset, Meta::il,    Meta::xl,    Meta::tn};

/// A list of the extra META values
const Meta extra_metas[] = {
  Meta::tnl,     Meta::tnr,       Meta::tne,      Meta::SrcNum,
  Meta::Tic,     Meta::VStack,    Meta::HStack,   Meta::RGElev,
  Meta::SSElev,  Meta::SDElev,    Meta::ns,       Meta::sample_interval,
  Meta::ShotNum, Meta::TraceUnit, Meta::TransUnit};


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

        if (value->type() == RuleEntry::MdType::Copy) {
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
  const std::vector<Meta>& mlist, bool full, bool defaults, bool extras)
{

    // TODO: Change this when extents are flexible
    flag.fullextent = full;
    add_index(Meta::gtn);
    add_index(Meta::ltn);

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

Rule::~Rule(void) = default;

bool Rule::add_rule(Meta m)
{
    if (rule_entry_map.find(m) != rule_entry_map.end()) {
        return false;
    }

    switch (m) {
        case Meta::gtn:
            add_index(Meta::gtn);
            return true;

        case Meta::ltn:
            add_index(Meta::ltn);
            return true;

        case Meta::WtrDepSrc:
            add_segy_float(m, Tr::WtrDepSrc, Tr::ScaleElev);
            return true;

        case Meta::WtrDepRcv:
            add_segy_float(m, Tr::WtrDepRcv, Tr::ScaleElev);
            return true;

        case Meta::x_src:
            add_segy_float(m, Tr::x_src, Tr::ScaleCoord);
            return true;

        case Meta::y_src:
            add_segy_float(m, Tr::y_src, Tr::ScaleCoord);
            return true;

        case Meta::x_rcv:
            add_segy_float(m, Tr::x_rcv, Tr::ScaleCoord);
            return true;

        case Meta::y_rcv:
            add_segy_float(m, Tr::y_rcv, Tr::ScaleCoord);
            return true;

        case Meta::xCmp:
            add_segy_float(m, Tr::xCmp, Tr::ScaleCoord);
            return true;

        case Meta::yCmp:
            add_segy_float(m, Tr::yCmp, Tr::ScaleCoord);
            return true;

        case Meta::il:
            add_long(m, Tr::il);
            return true;

        case Meta::xl:
            add_long(m, Tr::xl);
            return true;

        case Meta::Offset:
            add_long(m, Tr::CDist);
            return true;

        case Meta::tn:
            add_long(m, Tr::SeqFNum);
            return true;

        case Meta::Copy:
            add_copy();
            return true;

        case Meta::tnl:
            add_long(m, Tr::SeqNum);
            return true;

        case Meta::tnr:
            add_long(m, Tr::TORF);
            return true;

        case Meta::tne:
            add_long(m, Tr::SeqNumEns);
            return true;

        case Meta::SrcNum:
            add_long(m, Tr::ENSrcNum);
            return true;

        case Meta::Tic:
            add_short(m, Tr::TIC);
            return true;

        case Meta::VStack:
            add_short(m, Tr::VStackCnt);
            return true;

        case Meta::HStack:
            add_short(m, Tr::HStackCnt);
            return true;

        case Meta::RGElev:
            add_segy_float(m, Tr::RcvElv, Tr::ScaleElev);
            return true;

        case Meta::SSElev:
            add_segy_float(m, Tr::SurfElvSrc, Tr::ScaleElev);
            return true;

        case Meta::SDElev:
            add_segy_float(m, Tr::SrcDpthSurf, Tr::ScaleElev);
            return true;

        case Meta::ns:
            add_short(m, Tr::Ns);
            return true;

        case Meta::sample_interval:
            add_short(m, Tr::sample_interval);
            return true;

        case Meta::ShotNum:
            add_segy_float(m, Tr::ShotNum, Tr::ShotScal);
            return true;

        case Meta::TraceUnit:
            add_short(m, Tr::ValMeas);
            return true;

        case Meta::TransUnit:
            add_short(m, Tr::TransUnit);
            return true;

        // Non-standard and not handled automatically
        case Meta::dsdr:
        case Meta::Misc1:
        case Meta::Misc2:
        case Meta::Misc3:
        case Meta::Misc4:
            assert(
              false && "Non-standard Meta value, not handled automatically.");

            return false;
    }

    assert(false && "Unknown Meta value.");

    return false;
}

size_t Rule::extent(void)
{
    if (flag.fullextent) {
        return segy::segy_trace_header_size();
    }

    if (flag.badextent) {
        start = segy::segy_trace_header_size();
        end   = 0LU;

        for (const auto& r : rule_entry_map) {
            if (r.second->type() != RuleEntry::MdType::Index) {
                start = std::min(start, r.second->min());
                end   = std::max(end, r.second->max());
            }
        }

        flag.badextent = false;
    }

    return end - start;
}


size_t Rule::extent(void) const
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
            if (r.second->type() != RuleEntry::MdType::Index) {
                local_start = std::min(start, r.second->min());
                local_end   = std::max(end, r.second->max());
            }
        }
    }

    return local_end - local_start;
}

// TODO: These can be optimised to stop the double lookup if required.
void Rule::add_long(Meta m, Tr loc)
{
    auto ent = rule_entry_map.find(m);
    if (ent != rule_entry_map.end()) {
        rm_rule(m);
    }

    rule_entry_map.insert({m, std::make_unique<SEGYLongRuleEntry>(loc)});

    flag.badextent = (!flag.fullextent);
}

void Rule::add_short(Meta m, Tr loc)
{
    auto ent = rule_entry_map.find(m);
    if (ent != rule_entry_map.end()) {
        rm_rule(m);
    }

    rule_entry_map.insert({m, std::make_unique<SEGYShortRuleEntry>(loc)});

    flag.badextent = (!flag.fullextent);
}

void Rule::add_segy_float(Meta m, Tr loc, Tr scalar_location)
{
    auto ent = rule_entry_map.find(m);
    if (ent != rule_entry_map.end()) {
        rm_rule(m);
    }

    rule_entry_map.insert(
      {m, std::make_unique<SEGYFloatRuleEntry>(loc, scalar_location)});

    flag.badextent = (!flag.fullextent);
}

void Rule::add_index(Meta m)
{
    auto ent = rule_entry_map.find(m);
    if (ent != rule_entry_map.end()) {
        rm_rule(m);
    }

    rule_entry_map.insert({m, std::make_unique<SEGYIndexRuleEntry>()});
}

void Rule::add_copy(void)
{
    if (num_copy == 0) {
        rule_entry_map.insert(
          {Meta::Copy, std::make_unique<SEGYCopyRuleEntry>()});
        num_copy++;
    }
}

void Rule::rm_rule(Meta m)
{
    auto iter = rule_entry_map.find(m);
    if (iter != rule_entry_map.end()) {
        rule_entry_map.erase(m);

        flag.badextent = (!flag.fullextent);
    }
}

const RuleEntry* Rule::get_entry(Meta entry) const
{
    const auto it = rule_entry_map.find(entry);

    if (it == rule_entry_map.end()) {
        return nullptr;
    }

    return it->second.get();
}

size_t Rule::memory_usage(void) const
{
    const auto add_memory_usage = [](auto init, const auto& rule_entry_it) {
        return init + rule_entry_it.second->memory_usage();
    };

    return std::accumulate(
             rule_entry_map.begin(), rule_entry_map.end(), 0, add_memory_usage)
           + sizeof(Rule);
}

size_t Rule::memory_usage_per_header(void) const
{
    const auto get_sizeof = [](RuleEntry::MdType type) {
        switch (type) {
            case RuleEntry::MdType::Float:
                return sizeof(exseis::utils::Floating_point);

            case RuleEntry::MdType::Long:
                return sizeof(exseis::utils::Integer);

            case RuleEntry::MdType::Short:
                return sizeof(int16_t);

            case RuleEntry::MdType::Index:
                return sizeof(size_t);

            case RuleEntry::MdType::Copy:
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
                case RuleEntry::MdType::Float:
                    add_segy_float(
                      m.first, static_cast<Tr>(m.second->loc),
                      static_cast<Tr>(
                        static_cast<SEGYFloatRuleEntry*>(m.second.get())
                          ->scalar_location));
                    break;

                case RuleEntry::MdType::Long:
                    add_long(m.first, static_cast<Tr>(m.second->loc));
                    break;

                case RuleEntry::MdType::Short:
                    add_short(m.first, static_cast<Tr>(m.second->loc));
                    break;

                case RuleEntry::MdType::Index:
                    add_index(m.first);
                    break;

                case RuleEntry::MdType::Copy:
                    add_copy();
                    break;
            }
        }
    }

    return true;
}

}  // namespace piol
}  // namespace exseis
