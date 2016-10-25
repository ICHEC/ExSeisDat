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

enum class MdType : size_t
{
    Long,
    Short,
    Float
};

struct RuleEntry
{
    size_t num;
    size_t loc;
    RuleEntry(size_t num_, Tr loc_) : num(num_), loc(size_t(loc_)) { }
    virtual size_t min(void) = 0;
    virtual size_t max(void) = 0;
    virtual MdType type(void) = 0;
};

struct LongRuleEntry : public RuleEntry
{
    LongRuleEntry(size_t num_, Tr loc_) : RuleEntry(num_, loc_) { }
    size_t min(void)
    {
        return loc;
    }
    size_t max(void)
    {
        return loc;
    }
    MdType type(void)
    {
        return MdType::Long;
    }
};

struct ShortRuleEntry : public RuleEntry
{
    ShortRuleEntry(size_t num_, Tr loc_) : RuleEntry(num_, loc_) { }
    size_t min(void)
    {
        return loc;
    }
    size_t max(void)
    {
        return loc;
    }
    MdType type(void)
    {
        return MdType::Short;
    }
};

struct FloatRuleEntry : public RuleEntry
{
    size_t scalLoc;
    FloatRuleEntry(size_t num_, Tr loc_, Tr scalLoc_) : RuleEntry(num_, loc_), scalLoc(size_t(scalLoc_)) { }
    size_t min(void)
    {
        return std::min(scalLoc, loc);
    }
    size_t max(void)
    {
        return std::max(scalLoc, loc);
    }
    MdType type(void)
    {
        return MdType::Float;
    }
};


//A mechanism to store new rules
struct Rule
{
    std::unordered_map<Meta, RuleEntry *, EnumHash> translate;
    size_t numLong;
    size_t numFloat;
    size_t numShort;
    void addLong(Meta m, Tr loc)
    {
        translate[m] = new LongRuleEntry(numLong++, loc);
    }

    void addShort(Meta m, Tr loc)
    {
        translate[m] = new ShortRuleEntry(numShort++, loc);
    }

    void addFloat(Meta m, Tr loc, Tr scalLoc)
    {
        translate[m] = new FloatRuleEntry(numFloat++, loc, scalLoc);
    }

    Rule(void)
    {
        numLong = 0;
        numShort = 0;
        numFloat = 0;
        addFloat(Meta::xSrc, Tr::xSrc, Tr::ScaleCoord);
        addFloat(Meta::ySrc, Tr::ySrc, Tr::ScaleCoord);
        addFloat(Meta::xRcv, Tr::xRcv, Tr::ScaleCoord);
        addFloat(Meta::yRcv, Tr::yRcv, Tr::ScaleCoord);
        addFloat(Meta::xCmp, Tr::xCmp, Tr::ScaleCoord);
        addFloat(Meta::yCmp, Tr::yCmp, Tr::ScaleCoord);
        addLong(Meta::il, Tr::il);
        addLong(Meta::xl, Tr::xl);
        addLong(Meta::tn, Tr::SeqFNum);
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
        for (const auto r : rules->translate)
        {
            start = std::min(start, r.second->min());
            end = std::max(end, r.second->max());
        }
    }
    if (sz > 0)
    {
        if (sz * rules->numFloat)
            prm.f = new geom_t[sz * rules->numFloat];

        if (sz * rules->numLong)
            prm.i = new llint[sz * rules->numLong];

        if (sz * rules->numShort)
            prm.s = new short[sz * rules->numShort];

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
    RuleEntry * id = rules->translate[entry];
    switch (id->type())
    {
        case MdType::Long :
        ret.val.i = prm.i[i * rules->numLong + id->num];
        break;
        case MdType::Short :
        ret.val.s = prm.s[i * rules->numShort + id->num];
        break;
        case MdType::Float :
        ret.val.f = prm.f[i * rules->numFloat + id->num];
        break;
    }
    return ret;
}

#warning todo: Do type checks
void DynParam::setPrm(size_t i, Meta entry, geom_t val)
{
    prm.f[i * rules->numFloat + rules->translate[entry]->num] = val;
}

void DynParam::setPrm(size_t i, Meta entry, llint val)
{
    prm.i[i * rules->numLong + rules->translate[entry]->num] = val;
}

void DynParam::setPrm(size_t i, Meta entry, short val)
{
    prm.s[i * rules->numShort + rules->translate[entry]->num] = val;
}

void DynParam::fill(uchar * buf)
{
    for (size_t i = 0; i < sz; i++)
    {
        uchar * md = &buf[(end-start + stride)*i];
        std::unordered_map<Tr, int16_t, EnumHash> scal;
        std::vector<const FloatRuleEntry *> rule;
        for (const auto v : rules->translate)
        {
            const auto t = v.second;
            switch (t->type())
            {
                case MdType::Float :
                {
                    rule.push_back(dynamic_cast<FloatRuleEntry *>(t));
                    auto tr = static_cast<Tr>(rule.back()->scalLoc);
                    int16_t val1 = (scal.find(tr) != scal.end() ? scal[tr] : 1);
                    int16_t val2 = deScale(prm.f[i * rules->numFloat + t->num]);
                    scal[tr] = scalComp(val1, val2);
                }
                break;
                case MdType::Short :
                getBigEndian(prm.s[i * rules->numShort + t->num], &md[t->loc-start-1U]);
                break;
                case MdType::Long :
                getBigEndian(int32_t(prm.i[i * rules->numLong + t->num]), &md[t->loc-start-1U]);
                break;
            }
        }

        //Finish off the floats
        for (const auto & s : scal)
            getBigEndian(s.second, &md[size_t(s.first)-start-1U]);

        for (size_t j = 0; j < rule.size(); j++)
        {
            geom_t gscale = scaleConv(scal[static_cast<Tr>(rule[j]->scalLoc)]);
            getBigEndian(int32_t(std::lround(prm.f[i * rules->numFloat + rule[j]->num] / gscale)), &md[rule[j]->loc-start-1U]);
        }
    }
}

void DynParam::take(const uchar * buf)
{
    for (size_t i = 0; i < sz; i++)
    {
        const uchar * md = &buf[(end-start + stride)*i];
        //Loop through each rule and extract data
        for (const auto v : rules->translate)
        {
            const auto t = v.second;
            switch (t->type())
            {
                case MdType::Float :
                prm.f[i * rules->numFloat + t->num] = scaleConv(getHost<int16_t>(&md[dynamic_cast<FloatRuleEntry *>(t)->scalLoc - start-1U]))
                                                       * geom_t(getHost<int32_t>(&md[t->loc - start-1U]));
                break;
                case MdType::Short :
                prm.s[i * rules->numShort + t->num] = getHost<int16_t>(&md[t->loc - start-1U]);
                break;
                case MdType::Long :
                prm.i[i * rules->numLong + t->num] = getHost<int32_t>(&md[t->loc - start-1U]);
                break;
            }
        }
    }
}

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
