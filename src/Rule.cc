////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/Rule.hh"
#include "ExSeisDat/PIOL/SEGYRuleEntry.hh"

namespace PIOL {
namespace File {

/// A list of the default META values
static std::vector<Meta> default_metas{
  PIOL_META_xSrc, PIOL_META_ySrc, PIOL_META_xRcv,   PIOL_META_yRcv,
  PIOL_META_xCmp, PIOL_META_yCmp, PIOL_META_Offset, PIOL_META_il,
  PIOL_META_xl,   PIOL_META_tn};

/// A list of the extra META values
static std::vector<Meta> extra_metas{
  PIOL_META_tnl,     PIOL_META_tnr,       PIOL_META_tne,      PIOL_META_SrcNum,
  PIOL_META_Tic,     PIOL_META_VStack,    PIOL_META_HStack,   PIOL_META_RGElev,
  PIOL_META_SSElev,  PIOL_META_SDElev,    PIOL_META_ns,       PIOL_META_inc,
  PIOL_META_ShotNum, PIOL_META_TraceUnit, PIOL_META_TransUnit};

Rule::Rule(RuleMap translate_, bool full) : translate(translate_)
{
    for (const auto& t : translate)
        switch (t.second->type()) {
            case RuleEntry::MdType::Long:
                numLong++;
                break;
            case RuleEntry::MdType::Short:
                numShort++;
                break;
            case RuleEntry::MdType::Float:
                numFloat++;
                break;
            case RuleEntry::MdType::Index:
                numIndex++;
                break;
            case RuleEntry::MdType::Copy:
                numCopy++;
                break;
        }

    flag.fullextent = full;

    if (full) {
        start          = 0LU;
        end            = SEGSz::getMDSz();
        flag.badextent = false;
    }
    else {
        flag.badextent = true;
        extent();
    }
}

Rule::Rule(
  const std::vector<Meta>& mlist, bool full, bool defaults, bool extras)
{

    // TODO: Change this when extents are flexible
    flag.fullextent = full;
    addIndex(PIOL_META_gtn);
    addIndex(PIOL_META_ltn);

    for (auto m : mlist)
        addRule(m);

    if (defaults) {
        for (auto m : default_metas)
            addRule(m);
    }

    if (extras) {
        for (auto m : extra_metas)
            addRule(m);
    }

    if (flag.fullextent) {
        start          = 0LU;
        end            = SEGSz::getMDSz();
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

Rule::~Rule(void)
{
    for (const auto t : translate)
        delete t.second;
}

bool Rule::addRule(Meta m)
{
    if (translate.find(m) != translate.end()) return false;

    switch (m) {
        case PIOL_META_WtrDepSrc:
            addSEGYFloat(m, PIOL_TR_WtrDepSrc, PIOL_TR_ScaleElev);
            break;
        case PIOL_META_WtrDepRcv:
            addSEGYFloat(m, PIOL_TR_WtrDepRcv, PIOL_TR_ScaleElev);
            break;
        case PIOL_META_xSrc:
            addSEGYFloat(m, PIOL_TR_xSrc, PIOL_TR_ScaleCoord);
            break;
        case PIOL_META_ySrc:
            addSEGYFloat(m, PIOL_TR_ySrc, PIOL_TR_ScaleCoord);
            break;
        case PIOL_META_xRcv:
            addSEGYFloat(m, PIOL_TR_xRcv, PIOL_TR_ScaleCoord);
            break;
        case PIOL_META_yRcv:
            addSEGYFloat(m, PIOL_TR_yRcv, PIOL_TR_ScaleCoord);
            break;
        case PIOL_META_xCmp:
            addSEGYFloat(m, PIOL_TR_xCmp, PIOL_TR_ScaleCoord);
            break;
        case PIOL_META_yCmp:
            addSEGYFloat(m, PIOL_TR_yCmp, PIOL_TR_ScaleCoord);
            break;
        case PIOL_META_il:
            addLong(m, PIOL_TR_il);
            break;
        case PIOL_META_xl:
            addLong(m, PIOL_TR_xl);
            break;
        case PIOL_META_Offset:
            addLong(m, PIOL_TR_CDist);
            break;
        case PIOL_META_tn:
            addLong(m, PIOL_TR_SeqFNum);
            break;
        case PIOL_META_COPY:
            addCopy();
            break;
        case PIOL_META_tnl:
            addLong(m, PIOL_TR_SeqNum);
            break;
        case PIOL_META_tnr:
            addLong(m, PIOL_TR_TORF);
            break;
        case PIOL_META_tne:
            addLong(m, PIOL_TR_SeqNumEns);
            break;
        case PIOL_META_SrcNum:
            addLong(m, PIOL_TR_ENSrcNum);
            break;
        case PIOL_META_Tic:
            addShort(m, PIOL_TR_TIC);
            break;
        case PIOL_META_VStack:
            addShort(m, PIOL_TR_VStackCnt);
            break;
        case PIOL_META_HStack:
            addShort(m, PIOL_TR_HStackCnt);
            break;
        case PIOL_META_RGElev:
            addSEGYFloat(m, PIOL_TR_RcvElv, PIOL_TR_ScaleElev);
            break;
        case PIOL_META_SSElev:
            addSEGYFloat(m, PIOL_TR_SurfElvSrc, PIOL_TR_ScaleElev);
            break;
        case PIOL_META_SDElev:
            addSEGYFloat(m, PIOL_TR_SrcDpthSurf, PIOL_TR_ScaleElev);
            break;
        case PIOL_META_ns:
            addShort(m, PIOL_TR_Ns);
            break;
        case PIOL_META_inc:
            addShort(m, PIOL_TR_Inc);
            break;
        case PIOL_META_ShotNum:
            addSEGYFloat(m, PIOL_TR_ShotNum, PIOL_TR_ShotScal);
            break;
        case PIOL_META_TraceUnit:
            addShort(m, PIOL_TR_ValMeas);
            break;
        case PIOL_META_TransUnit:
            addShort(m, PIOL_TR_TransUnit);
            break;
        default:
            return false;
            break;  // Non-default
    }
    return true;
}

size_t Rule::extent(void)
{
    if (flag.fullextent) return SEGSz::getMDSz();
    if (flag.badextent) {
        start = SEGSz::getMDSz();
        end   = 0LU;
        for (const auto r : translate)
            if (r.second->type() != RuleEntry::MdType::Index) {
                start = std::min(start, r.second->min());
                end   = std::max(end, r.second->max());
            }
        flag.badextent = false;
    }
    return end - start;
}

// TODO: These can be optimised to stop the double lookup if required.
void Rule::addLong(Meta m, Tr loc)
{
    auto ent = translate.find(m);
    if (ent != translate.end()) rmRule(m);
    translate[m]   = new SEGYLongRuleEntry(numLong++, loc);
    flag.badextent = (!flag.fullextent);
}

void Rule::addShort(Meta m, Tr loc)
{
    auto ent = translate.find(m);
    if (ent != translate.end()) rmRule(m);
    translate[m]   = new SEGYShortRuleEntry(numShort++, loc);
    flag.badextent = (!flag.fullextent);
}

void Rule::addSEGYFloat(Meta m, Tr loc, Tr scalLoc)
{
    auto ent = translate.find(m);
    if (ent != translate.end()) rmRule(m);
    translate[m]   = new SEGYFloatRuleEntry(numFloat++, loc, scalLoc);
    flag.badextent = (!flag.fullextent);
}

void Rule::addIndex(Meta m)
{
    auto ent = translate.find(m);
    if (ent != translate.end()) rmRule(m);
    translate[m] = new SEGYIndexRuleEntry(numIndex++);
}

void Rule::addCopy(void)
{
    if (!numCopy) {
        translate[PIOL_META_COPY] = new SEGYCopyRuleEntry();
        numCopy++;
    }
}

void Rule::rmRule(Meta m)
{
    auto iter = translate.find(m);
    if (iter != translate.end()) {
        RuleEntry* entry       = iter->second;
        RuleEntry::MdType type = entry->type();
        size_t num             = entry->num;

        switch (type) {
            case RuleEntry::MdType::Long:
                numLong--;
                break;
            case RuleEntry::MdType::Short:
                numShort--;
                break;
            case RuleEntry::MdType::Float:
                numFloat--;
                break;
            case RuleEntry::MdType::Index:
                numIndex--;
                break;
            case RuleEntry::MdType::Copy:
                numCopy--;
                break;
        }
        delete entry;
        translate.erase(m);
        for (auto t : translate)
            if (t.second->type() == type && t.second->num > num)
                t.second->num--;

        flag.badextent = (!flag.fullextent);
    }
}

RuleEntry* Rule::getEntry(Meta entry)
{
    return translate[entry];
}

size_t Rule::memUsage(void) const
{
    return numLong * sizeof(SEGYLongRuleEntry)
           + numShort * sizeof(SEGYShortRuleEntry)
           + numFloat * sizeof(SEGYFloatRuleEntry)
           + numIndex * sizeof(SEGYIndexRuleEntry)
           + numCopy * sizeof(SEGYCopyRuleEntry) + sizeof(Rule);
}

size_t Rule::paramMem(void) const
{
    return numLong * sizeof(llint) + numShort * sizeof(int16_t)
           + numFloat * sizeof(geom_t) + numIndex * sizeof(size_t)
           + (numCopy ? SEGSz::getMDSz() : 0);
}

bool Rule::addRule(const Rule& r)
{
    for (auto& m : r.translate) {
        if (translate.find(m.first) == translate.end()) {
            switch (m.second->type()) {
                case RuleEntry::MdType::Float:
                    addSEGYFloat(
                      m.first, static_cast<Tr>(m.second->loc),
                      static_cast<Tr>(
                        static_cast<SEGYFloatRuleEntry*>(m.second)->scalLoc));
                    break;
                case RuleEntry::MdType::Long:
                    addLong(m.first, static_cast<Tr>(m.second->loc));
                    break;
                case RuleEntry::MdType::Short:
                    addShort(m.first, static_cast<Tr>(m.second->loc));
                    break;
                case RuleEntry::MdType::Index:
                    addIndex(m.first);
                    break;
                case RuleEntry::MdType::Copy:
                    addCopy();
                    break;
                default:
                    break;
            }
        }
    }

    return true;
}

}  // namespace File
}  // namespace PIOL
