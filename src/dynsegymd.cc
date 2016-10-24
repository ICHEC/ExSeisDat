/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date October 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/

#include "file/dynsegymd.hh"
#include "file/segymd.hh"
#include <algorithm>
#include <unordered_map>

#warning temp
#include <iostream>

namespace PIOL { namespace File {
//New SEG-Y functions
void setScale(size_t loc, const int16_t scale, uchar * md, size_t start)
{
    getBigEndian(scale, &md[loc-start-1U]);
}

//////
struct EnumHash
{
    template <typename T>
    size_t operator()(T t) const
    {
        return static_cast<size_t>(t);
    }
};

enum MDType
{
    Long,
    Short,
    Float
};

struct RuleEntry
{
    Tr loc;
    RuleEntry(Tr loc_) : loc(loc_) { }
    virtual MDType type(void) const = 0;
};

struct LongRuleEntry : public RuleEntry
{
    LongRuleEntry(Tr loc_) : RuleEntry(loc_) { }
    MDType type(void) const
    {
        return MDType::Long;
    }
};

struct ShortRuleEntry : public RuleEntry
{
    ShortRuleEntry(Tr loc_) : RuleEntry(loc_) { }
    MDType type(void) const
    {
        return MDType::Short;
    }
};

struct FloatRuleEntry : public RuleEntry
{
    Tr scalLoc;
    FloatRuleEntry(Tr scalLoc_, Tr loc_) : RuleEntry(loc_), scalLoc(scalLoc_) { }
    MDType type(void) const
    {
        return MDType::Float;
    }
};

//A mechanism to store new rules
struct Rule
{
    std::vector<LongRuleEntry> longrules;
    std::vector<ShortRuleEntry> shortrules;
    std::vector<FloatRuleEntry> floatrules;
    std::unordered_map<Meta, RuleEntry *, EnumHash> translate;
    Rule(void)
    {
        translate[Meta::xSrc] = addFloat(Tr::xSrc, Tr::ScaleCoord);
        translate[Meta::ySrc] = addFloat(Tr::ySrc, Tr::ScaleCoord);
        translate[Meta::xRcv] = addFloat(Tr::xRcv, Tr::ScaleCoord);
        translate[Meta::yRcv] = addFloat(Tr::yRcv, Tr::ScaleCoord);
        translate[Meta::xCmp] = addFloat(Tr::xCmp, Tr::ScaleCoord);
        translate[Meta::yCmp] = addFloat(Tr::yCmp, Tr::ScaleCoord);
        translate[Meta::il] = addLong(Tr::il);
        translate[Meta::xl] = addLong(Tr::xl);
        translate[Meta::tn] = addLong(Tr::SeqFNum);
    }

//TODO: Don't allow duplicates?

    RuleEntry * addLong(Tr loc)
    {
#if __cplusplus > 201402L
        return longrules.emplace_back(loc);
#else
        longrules.emplace_back(loc);
        return &longrules.back();
#endif
    }

    RuleEntry * addShort(Tr loc)
    {
#if __cplusplus > 201402L
        return shortrules.emplace_back(loc);
#else
        shortrules.emplace_back(loc);
        return &shortrules.back();
#endif
    }

    RuleEntry * addFloat(Tr scalLoc, Tr loc)
    {
#if __cplusplus > 201402L
        return floatrules.emplace_back(loc);
#else
        floatrules.emplace_back(scalLoc, loc);
        return &floatrules.back();
#endif
    }
};

DynParam::DynParam(Rule * rules_, csize_t sz_, csize_t stride_) : rules(rules_), sz(sz_), stride(stride_)
{
    start = 240U;
    end = 0U;
    for (size_t i = 0; i < rules->longrules.size(); i++)
    {
        start = std::min(start, size_t(rules->longrules[i].loc));
        end = std::max(end, size_t(rules->longrules[i].loc));
    }

    for (size_t i = 0; i < rules->shortrules.size(); i++)
    {
        start = std::min(start, size_t(rules->shortrules[i].loc));
        end = std::max(start, size_t(rules->shortrules[i].loc));
    }

    for (size_t i = 0; i < rules->floatrules.size(); i++)
    {
        start = std::min(start, std::min(size_t(rules->floatrules[i].loc), size_t(rules->floatrules[i].scalLoc)));
        end = std::max(start, std::max(size_t(rules->floatrules[i].loc), size_t(rules->floatrules[i].scalLoc)));
    }

    if (sz > 0)
    {
        buf.resize(sz * (end-start));
        prm.i = new llint[sz];
        prm.s = new short[sz];
        prm.f = new geom_t[sz];
        prm.t = new size_t[sz];
    }
}

DynParam::~DynParam(void)
{
    delete prm.i;
    delete prm.s;
    delete prm.f;
    delete prm.t;
}

prmRet DynParam::getPrm(size_t i, Meta entry)
{
    prmRet ret;
    RuleEntry * id = rules->translate[entry];
    switch (id->type())
    {
        case MDType::Long :
        ret.val.i = prm.i[i * rules->longrules.size() + std::distance(&rules->longrules[0], static_cast<LongRuleEntry*>(id))];
        break;
        case MDType::Short :
        ret.val.s = prm.s[i * rules->shortrules.size() + std::distance(&rules->shortrules[0], static_cast<ShortRuleEntry*>(id))];
        break;
        case MDType::Float :
        ret.val.f = prm.f[i * rules->floatrules.size() + std::distance(&rules->floatrules[0], static_cast<FloatRuleEntry*>(id))];
        break;
    }
    return ret;
}

void DynParam::setPrm(size_t i, Meta entry, geom_t val)
{
    auto id = static_cast<FloatRuleEntry *>(rules->translate[entry]);
    if (!id)
        std::cerr << "Non-existent rule / bad cast.\n";
    prm.f[i * rules->floatrules.size() + std::distance(&rules->floatrules[0], id)] = val;
}

void DynParam::setPrm(size_t i, Meta entry, llint val)
{
    auto id = static_cast<LongRuleEntry *>(rules->translate[entry]);
    if (!id)
        std::cerr << "Non-existent rule / bad cast.\n";
    prm.f[i * rules->longrules.size() + std::distance(&rules->longrules[0], id)] = val;
}

void DynParam::setPrm(size_t i, Meta entry, short val)
{
    auto id = static_cast<ShortRuleEntry *>(rules->translate[entry]);
    if (!id)
        std::cerr << "Non-existent rule / bad cast.\n";
    prm.s[i * rules->shortrules.size() + std::distance(&rules->shortrules[0], id)] = val;
}

void DynParam::setData(uchar * buf)
{
    for (size_t i = 0; i < sz; i++)
    {
        uchar * md = &buf[(end-start + stride)*i];
        for (size_t len = rules->longrules.size(), j = 0; j < len; j++)
            getBigEndian(int32_t(prm.s[i * len + j]), &md[size_t(rules->longrules[j].loc) - start]);

        for (size_t len = rules->shortrules.size(), j = 0; j < len; j++)
            getBigEndian(prm.s[i * len + j], &md[size_t(rules->shortrules[j].loc) - start-1U]);

        std::unordered_map<Tr, int16_t, EnumHash> scal;
        for (size_t len = rules->floatrules.size(), j = 0; j < len; j++)
        {
            Tr sLoc = rules->floatrules[j].scalLoc;
            scal[sLoc] = scalComp((scal.find(sLoc) == scal.end() ? 1 : scal[sLoc]), deScale(prm.f[i * len + j]));
        }
        for (const auto & s : scal)
            setScale(size_t(s.first)-1, s.second - start, md, start);

        for (size_t len = rules->floatrules.size(), j = 0; j < len; j++)
        {
            geom_t gscale = scaleConv(scal[rules->floatrules[j].scalLoc]);
            getBigEndian(int32_t(std::lround(prm.f[i * len + j] / gscale)), &buf[size_t(rules->floatrules[j].loc) - start - 1U]);
        }
    }
}

void DynParam::getData(const uchar * buf)
{
}

/*struct RuleStore
{
    Rule r;
};*/

/*! Extract the trace parameters from a character array and copy
 *  them to a TraceParam structure
 *  \param[in] md A charachter array of raw trace header contents
 *  \param[out] prm An array of TraceParam structures
 */
void extractDynTraceParam(size_t sz, const uchar * md, TraceParam * prm)
{
    Rule r;
    DynParam dyn(&r, sz, 0);
    dyn.getData(md);
    for (size_t i = 0; i < sz; i++)
    {
        prm[i].src.x = dyn.getPrm(i, Meta::xSrc);
        prm[i].src.y = dyn.getPrm(i, Meta::ySrc);
        prm[i].rcv.x = dyn.getPrm(i, Meta::xRcv);
        prm[i].rcv.y = dyn.getPrm(i, Meta::yRcv);
        prm[i].cmp.x = dyn.getPrm(i, Meta::xCmp);
        prm[i].cmp.y = dyn.getPrm(i, Meta::yCmp);
        prm[i].tn = dyn.getPrm(i, Meta::yCmp);
    }
}

/*! Insert the trace parameters from a TraceParam structure and
 *  copy them into a character array ready for writing to a segy file
 *  \param[in] prm An array of TraceParam structures
 *  \param[out] md A charachter array of raw trace header contents
 */
void insertDynTraceParam(size_t sz, const TraceParam * prm, uchar * md)
{
    Rule r;
    DynParam dyn(&r, sz, 0);
    for (size_t i = 0; i < sz; i++)
    {
        dyn.setPrm(i, Meta::xSrc, prm[i].src.x);
        dyn.setPrm(i, Meta::ySrc, prm[i].src.y);
        dyn.setPrm(i, Meta::xRcv, prm[i].rcv.x);
        dyn.setPrm(i, Meta::yRcv, prm[i].rcv.y);
        dyn.setPrm(i, Meta::xCmp, prm[i].cmp.x);
        dyn.setPrm(i, Meta::yCmp, prm[i].cmp.y);
        dyn.setPrm(i, Meta::yCmp, llint(prm[i].tn));
    }
    dyn.setData(md);
}

}}
