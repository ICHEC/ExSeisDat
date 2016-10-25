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

#warning temp
#include <iostream>

namespace PIOL { namespace File {
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


Rule::Rule(bool full, bool defaults)
{
    numLong = 0;
    numShort = 0;
    numFloat = 0;

    flag.fullextent = full;

    if (full)
    {
        start = 0;
        end = 240;
    }

    if (defaults)
    {
        translate[Meta::xSrc] = new FloatRuleEntry(numFloat++, Tr::xSrc, Tr::ScaleCoord);
        translate[Meta::ySrc] = new FloatRuleEntry(numFloat++, Tr::ySrc, Tr::ScaleCoord);
        translate[Meta::xRcv] = new FloatRuleEntry(numFloat++, Tr::xRcv, Tr::ScaleCoord);
        translate[Meta::yRcv] = new FloatRuleEntry(numFloat++, Tr::yRcv, Tr::ScaleCoord);
        translate[Meta::xCmp] = new FloatRuleEntry(numFloat++, Tr::xCmp, Tr::ScaleCoord);
        translate[Meta::yCmp] = new FloatRuleEntry(numFloat++, Tr::yCmp, Tr::ScaleCoord);
        translate[Meta::il] = new LongRuleEntry(numLong++, Tr::il);
        translate[Meta::xl] = new LongRuleEntry(numLong++, Tr::xl);
        translate[Meta::tn] = new LongRuleEntry(numLong++, Tr::SeqFNum);
    }
}

Rule::Rule(const Rule * rule)
{
    *this = *rule;
}


size_t Rule::extent(void)
{
    if (flag.fullextent)
        return size_t(SEGSz::Size::DOMd);
    if (flag.badextent)
    {
        start = size_t(SEGSz::Size::DOMd);
        end = 0U;
        for (const auto r : translate)
        {
            start = std::min(start, r.second->min());
            end = std::max(end, r.second->max());
        }
        flag.badextent = false;
    }
    return end-start;
}

void Rule::addLong(Meta m, Tr loc)
{
    translate[m] = new LongRuleEntry(numLong++, loc);
    flag.badextent = (!flag.fullextent);
}

void Rule::addShort(Meta m, Tr loc)
{
    translate[m] = new ShortRuleEntry(numShort++, loc);
    flag.badextent = (!flag.fullextent);
}

void Rule::addFloat(Meta m, Tr loc, Tr scalLoc)
{
    translate[m] = new FloatRuleEntry(numFloat++, loc, scalLoc);
    flag.badextent = (!flag.fullextent);
}

void Rule::rmRule(Meta m)
{
    translate.erase(m);
    flag.badextent = (!flag.fullextent);
}

DynParam::DynParam(const Rule * rule, csize_t sz_, csize_t stride_) : Rule(rule), sz(sz_), stride(stride_)
{
    prm.f = NULL;
    prm.i = NULL;
    prm.s = NULL;
    prm.t = NULL;

    if (sz > 0)
    {
        prm.f = new geom_t[sz * numFloat];
        prm.i = new llint[sz * numLong];
        prm.s = new short[sz * numShort];
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
}

prmRet DynParam::getPrm(size_t i, Meta entry)
{
    prmRet ret;
    RuleEntry * id = translate[entry];
    switch (id->type())
    {
        case MdType::Long :
        ret.val.i = prm.i[i * numLong + id->num];
        break;
        case MdType::Short :
        ret.val.s = prm.s[i * numShort + id->num];
        break;
        case MdType::Float :
        ret.val.f = prm.f[i * numFloat + id->num];
        break;
    }
    return ret;
}

#warning todo: Do type checks
void DynParam::setPrm(size_t i, Meta entry, geom_t val)
{
    prm.f[i * numFloat + translate[entry]->num] = val;
}

void DynParam::setPrm(size_t i, Meta entry, llint val)
{
    prm.i[i * numLong + translate[entry]->num] = val;
}

void DynParam::setPrm(size_t i, Meta entry, short val)
{
    prm.s[i * numShort + translate[entry]->num] = val;
}

void DynParam::fill(uchar * buf)
{
    for (size_t i = 0; i < sz; i++)
    {
        uchar * md = &buf[(extent() + stride)*i];
        std::unordered_map<Tr, int16_t, EnumHash> scal;
        std::vector<const FloatRuleEntry *> rule;
        for (const auto v : translate)
        {
            const auto t = v.second;
            switch (t->type())
            {
                case MdType::Float :
                {
                    rule.push_back(dynamic_cast<FloatRuleEntry *>(t));
                    auto tr = static_cast<Tr>(rule.back()->scalLoc);
                    int16_t val1 = (scal.find(tr) != scal.end() ? scal[tr] : 1);
                    int16_t val2 = deScale(prm.f[i * numFloat + t->num]);
                    scal[tr] = scalComp(val1, val2);
                }
                break;
                case MdType::Short :
                getBigEndian(prm.s[i * numShort + t->num], &md[t->loc-start-1U]);
                break;
                case MdType::Long :
                getBigEndian(int32_t(prm.i[i * numLong + t->num]), &md[t->loc-start-1U]);
                break;
            }
        }

        //Finish off the floats
        for (const auto & s : scal)
            getBigEndian(s.second, &md[size_t(s.first)-start-1U]);

        for (size_t j = 0; j < rule.size(); j++)
        {
            geom_t gscale = scaleConv(scal[static_cast<Tr>(rule[j]->scalLoc)]);
            getBigEndian(int32_t(std::lround(prm.f[i * numFloat + rule[j]->num] / gscale)), &md[rule[j]->loc-start-1U]);
        }
    }
}

void DynParam::take(const uchar * buf)
{
    for (size_t i = 0; i < sz; i++)
    {
        const uchar * md = &buf[(extent() + stride)*i];
        //Loop through each rule and extract data
        for (const auto v : translate)
        {
            const auto t = v.second;
            switch (t->type())
            {
                case MdType::Float :
                prm.f[i * numFloat + t->num] = scaleConv(getHost<int16_t>(&md[dynamic_cast<FloatRuleEntry *>(t)->scalLoc - start-1U]))
                                                       * geom_t(getHost<int32_t>(&md[t->loc - start-1U]));
                break;
                case MdType::Short :
                prm.s[i * numShort + t->num] = getHost<int16_t>(&md[t->loc - start-1U]);
                break;
                case MdType::Long :
                prm.i[i * numLong + t->num] = getHost<int32_t>(&md[t->loc - start-1U]);
                break;
            }
        }
    }
}
}}
