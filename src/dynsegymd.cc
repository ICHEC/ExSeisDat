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
};

struct LongRuleEntry : public RuleEntry
{
    LongRuleEntry(Tr loc_) : RuleEntry(loc_) { }
};

struct ShortRuleEntry : public RuleEntry
{
    ShortRuleEntry(Tr loc_) : RuleEntry(loc_) { }
};

struct FloatRuleEntry : public RuleEntry
{
    Tr scalLoc;
    FloatRuleEntry(Tr loc_, Tr scalLoc_) : RuleEntry(loc_), scalLoc(scalLoc_) { }
};

//A mechanism to store new rules
struct Rule
{
    std::vector<LongRuleEntry> longrules;
    std::vector<ShortRuleEntry> shortrules;
    std::vector<FloatRuleEntry> floatrules;
    std::unordered_map<Meta, std::pair<MDType, size_t>, EnumHash> translate;
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

    std::pair<MDType, size_t> addLong(Tr loc)
    {
        longrules.emplace_back(loc);
        return std::make_pair(MDType::Long, longrules.size() - 1);
    }

    std::pair<MDType, size_t> addShort(Tr loc)
    {
        shortrules.emplace_back(loc);
        return std::make_pair(MDType::Short, shortrules.size() - 1);
    }

    std::pair<MDType, size_t> addFloat(Tr loc, Tr scalLoc)
    {
        floatrules.emplace_back(loc, scalLoc);
        return std::make_pair(MDType::Float, floatrules.size() - 1);
    }
};

DynParam::DynParam(Rule * rules_, csize_t sz_, csize_t stride_) : rules(rules_), sz(sz_), stride(stride_)
{
    bool full = true;
    prm.f = NULL;
    prm.i = NULL;
    prm.s = NULL;
    prm.t = NULL;

#warning use SEGSz
    if (full)
    {
        start = 0U;
        end = 240U;
    }
    else
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
    }
    if (sz > 0)
    {
        if (sz * rules->floatrules.size())
            prm.f = new geom_t[sz * rules->floatrules.size()];

        if (sz * rules->longrules.size())
            prm.i = new llint[sz * rules->longrules.size()];

        if (sz * rules->shortrules.size())
            prm.s = new short[sz * rules->shortrules.size()];

        prm.t = new size_t[sz];
    }
}

DynParam::~DynParam(void)
{
    if (prm.i != NULL)
        delete [] prm.i;
    if (prm.s != NULL)
        delete [] prm.s;
    if (prm.f != NULL)
        delete [] prm.f;
    if (prm.t != NULL)
        delete [] prm.t;
    prm.f = NULL;
    prm.i = NULL;
    prm.s = NULL;
    prm.t = NULL;
}

prmRet DynParam::getPrm(size_t i, Meta entry)
{
    prmRet ret;
    std::pair<MDType, size_t> id = rules->translate[entry];
    switch (id.first)
    {
        case MDType::Long :
        ret.val.i = prm.i[i * rules->longrules.size() + id.second];
        break;
        case MDType::Short :
        ret.val.s = prm.s[i * rules->shortrules.size() + id.second];
        break;
        case MDType::Float :
        ret.val.f = prm.f[i * rules->floatrules.size() + id.second];
        break;
    }
    return ret;
}

void DynParam::setPrm(size_t i, Meta entry, geom_t val)
{
    auto id = rules->translate[entry];
#warning todo: Do type checks
    prm.f[i * rules->floatrules.size() + id.second] = val;
}

void DynParam::setPrm(size_t i, Meta entry, llint val)
{
    auto id = rules->translate[entry];
    prm.i[i * rules->longrules.size() + id.second] = val;
}

void DynParam::setPrm(size_t i, Meta entry, short val)
{
    auto id = rules->translate[entry];
    prm.s[i * rules->shortrules.size() + id.second] = val;
}

void DynParam::fill(uchar * buf)
{
    for (size_t i = 0; i < sz; i++)
    {
        uchar * md = &buf[(end-start + stride)*i];
//Longs
        for (size_t len = rules->longrules.size(), j = 0; j < len; j++)
            getBigEndian(int32_t(prm.i[i * len + j]), &md[size_t(rules->longrules[j].loc)-start-1U]);

//Shorts
        for (size_t len = rules->shortrules.size(), j = 0; j < len; j++)
            getBigEndian(prm.s[i * len + j], &md[size_t(rules->shortrules[j].loc)-start-1U]);

//Floats
        std::unordered_map<Tr, int16_t, EnumHash> scal;
        for (size_t len = rules->floatrules.size(), j = 0; j < len; j++)
        {
            Tr sLoc = rules->floatrules[j].scalLoc;
            int16_t val1 = (scal.find(sLoc) != scal.end() ? scal[sLoc] : 1);
            int16_t val2 = deScale(prm.f[i * len + j]);
            scal[sLoc] = scalComp(val1, val2);
        }
        for (const auto & s : scal)
            getBigEndian(s.second, &md[size_t(s.first)-start-1U]);

        for (size_t len = rules->floatrules.size(), j = 0; j < len; j++)
        {
            geom_t gscale = scaleConv(scal[rules->floatrules[j].scalLoc]);
            getBigEndian(int32_t(std::lround(prm.f[i * len + j] / gscale)), &md[size_t(rules->floatrules[j].loc)-start-1U]);
        }
    }
}

void DynParam::take(const uchar * buf)
{
    for (size_t i = 0; i < sz; i++)
    {
        const uchar * md = &buf[(end-start + stride)*i];
        //Loop through each rule and extract data
//Floats
        for (size_t len = rules->floatrules.size(), j = 0; j < len; j++)
            prm.f[i * len + j] = scaleConv(getHost<int16_t>(&md[size_t(rules->floatrules[j].scalLoc)-start-1U]))
                                  * geom_t(getHost<int32_t>(&md[size_t(rules->floatrules[j].loc)-start-1U]));

//Longs
        for (size_t len = rules->longrules.size(), j = 0; j < len; j++)
            prm.i[i * len + j] = getHost<int32_t>(&md[size_t(rules->longrules[j].loc)-start-1U]);

//Shorts
        for (size_t len = rules->shortrules.size(), j = 0; j < len; j++)
            prm.s[i * len + j] = getHost<int16_t>(&md[size_t(rules->shortrules[j].loc)-start-1U]);
    }
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
    dyn.take(md);
    for (size_t i = 0; i < sz; i++)
    {
        prm[i].src.x = dyn.getPrm(i, Meta::xSrc);
        prm[i].src.y = dyn.getPrm(i, Meta::ySrc);
        prm[i].rcv.x = dyn.getPrm(i, Meta::xRcv);
        prm[i].rcv.y = dyn.getPrm(i, Meta::yRcv);
        prm[i].cmp.x = dyn.getPrm(i, Meta::xCmp);
        prm[i].cmp.y = dyn.getPrm(i, Meta::yCmp);
        prm[i].line.il = dyn.getPrm(i, Meta::il);
        prm[i].line.xl = dyn.getPrm(i, Meta::xl);

        prm[i].tn = dyn.getPrm(i, Meta::tn);
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
        dyn.setPrm(i, Meta::il, prm[i].line.il);
        dyn.setPrm(i, Meta::xl, prm[i].line.xl);
        dyn.setPrm(i, Meta::tn, llint(prm[i].tn));
    }
    dyn.fill(md);
}

void extractTraceParam(const uchar * md, TraceParam * prm)
{
    extractDynTraceParam(1, md, prm);
}
void insertTraceParam(const TraceParam * prm, uchar * md)
{
    insertDynTraceParam(1, prm, md);
}
}}
