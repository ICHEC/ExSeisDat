/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date October 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include <algorithm>
#include "file/dynsegymd.hh"

#warning temp
#include <iostream>

namespace PIOL { namespace File {
struct SEGYLongRuleEntry : public RuleEntry
{
    SEGYLongRuleEntry(size_t num_, Tr loc_) : RuleEntry(num_, size_t(loc_)) { }
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

struct SEGYShortRuleEntry : public RuleEntry
{
    SEGYShortRuleEntry(size_t num_, Tr loc_) : RuleEntry(num_, size_t(loc_)) { }
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

struct SEGYFloatRuleEntry : public RuleEntry
{
    size_t scalLoc;
    SEGYFloatRuleEntry(size_t num_, Tr loc_, Tr scalLoc_) : RuleEntry(num_, size_t(loc_)), scalLoc(size_t(scalLoc_)) { }
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

Rule::Rule(std::unordered_map<Meta, RuleEntry *, EnumHash> translate_, bool full)
{
    numLong = 0;
    numShort = 0;
    numFloat = 0;
    translate = translate_;
    for (const auto & t : translate)
        switch (t.second->type())
        {
            case MdType::Long :
            numLong++;
            break;
            case MdType::Short :
            numShort++;
            break;
            case MdType::Float :
            numFloat++;
            break;
        }

    flag.fullextent = full;
    if (full)
    {
        flag.badextent = true;
        extent();
    }
}

Rule::Rule(bool full, bool defaults)
{
    numLong = 0;
    numShort = 0;
    numFloat = 0;

    flag.fullextent = full;

    if (defaults)
    {
        translate[Meta::xSrc] = new SEGYFloatRuleEntry(numFloat++, Tr::xSrc, Tr::ScaleCoord);
        translate[Meta::ySrc] = new SEGYFloatRuleEntry(numFloat++, Tr::ySrc, Tr::ScaleCoord);
        translate[Meta::xRcv] = new SEGYFloatRuleEntry(numFloat++, Tr::xRcv, Tr::ScaleCoord);
        translate[Meta::yRcv] = new SEGYFloatRuleEntry(numFloat++, Tr::yRcv, Tr::ScaleCoord);
        translate[Meta::xCmp] = new SEGYFloatRuleEntry(numFloat++, Tr::xCmp, Tr::ScaleCoord);
        translate[Meta::yCmp] = new SEGYFloatRuleEntry(numFloat++, Tr::yCmp, Tr::ScaleCoord);
        translate[Meta::il] = new SEGYLongRuleEntry(numLong++, Tr::il);
        translate[Meta::xl] = new SEGYLongRuleEntry(numLong++, Tr::xl);
        translate[Meta::tn] = new SEGYLongRuleEntry(numLong++, Tr::SeqFNum);
    }
    if (full)
    {
        start = 0;
        end = 240;
        flag.badextent = false;
    }
    else
    {
        flag.badextent = true;
        extent();
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
    translate[m] = new SEGYLongRuleEntry(numLong++, loc);
    flag.badextent = (!flag.fullextent);
}

void Rule::addShort(Meta m, Tr loc)
{
    translate[m] = new SEGYShortRuleEntry(numShort++, loc);
    flag.badextent = (!flag.fullextent);
}

void Rule::addFloat(Meta m, Tr loc, Tr scalLoc)
{
    translate[m] = new SEGYFloatRuleEntry(numFloat++, loc, scalLoc);
    flag.badextent = (!flag.fullextent);
}

void Rule::rmRule(Meta m)
{
    switch (translate[m]->type())
    {
        case MdType::Long :
        numLong--;
        break;
        case MdType::Short :
        numShort--;
        break;
        case MdType::Float :
        numFloat--;
        break;
    }
    translate.erase(m);
    flag.badextent = (!flag.fullextent);
}

RuleEntry * Rule::getEntry(Meta entry)
{
    return translate[entry];
}

prmRet getPrm(Rule * r, size_t i, Meta entry, const Param * prm)
{
    prmRet ret;
    RuleEntry * id = r->getEntry(entry);
    switch (id->type())
    {
        case MdType::Long :
        ret.val.i = prm->i[i * r->numLong + id->num];
        break;
        case MdType::Short :
        ret.val.s = prm->s[i * r->numShort + id->num];
        break;
        case MdType::Float :
        ret.val.f = prm->f[i * r->numFloat + id->num];
        break;
    }
    return ret;
}

#warning todo: Do type checks
void setPrm(Rule * r, size_t i, Meta entry, geom_t val, Param * prm)
{
    prm->f[i * r->numFloat + r->getEntry(entry)->num] = val;
}

void setPrm(Rule * r, size_t i, Meta entry, llint val, Param * prm)
{
    prm->i[i * r->numLong + r->getEntry(entry)->num] = val;
}

void setPrm(Rule * r, size_t i, Meta entry, short val, Param * prm)
{
    prm->s[i * r->numShort + r->getEntry(entry)->num] = val;
}

Param::Param(const Rule * rule, csize_t sz)
{
    f = NULL;
    i = NULL;
    s = NULL;
    t = NULL;

    if (sz > 0)
    {
        f = new geom_t[sz * rule->numFloat];
        i = new llint[sz * rule->numLong];
        s = new short[sz * rule->numShort];
        t = new size_t[sz];
    }
}

Param::~Param(void)
{
    if (f != NULL)
        delete [] f;
    if (i != NULL)
        delete [] i;
    if (s != NULL)
        delete [] s;
    if (t != NULL)
        delete [] t;
}

void insertParam(Rule * r, size_t sz, const Param * prm, uchar * buf, size_t stride)
{
    size_t start = r->start;
    for (size_t i = 0; i < sz; i++)
    {
        uchar * md = &buf[(r->extent() + stride)*i];
        std::unordered_map<Tr, int16_t, EnumHash> scal;
        std::vector<const SEGYFloatRuleEntry *> rule;
        for (const auto v : r->translate)
        {
            const auto t = v.second;
            size_t loc = t->loc - start-1U;
            switch (t->type())
            {
                case MdType::Float :
                {
                    rule.push_back(dynamic_cast<SEGYFloatRuleEntry *>(t));
                    auto tr = static_cast<Tr>(rule.back()->scalLoc);
                    int16_t scal1 = (scal.find(tr) != scal.end() ? scal[tr] : 1);
                    int16_t scal2 = deScale(prm->f[i * r->numFloat + t->num]);

                    //if the scale is bigger than 1 that means we need to use the largest
                    //to ensure conservation of the most significant digit
                    //otherwise we choose the scale that preserves the most digits
                    //after the decimal place.
                    scal[tr] = ((scal1 > 1 || scal2 > 1) ? std::max(scal1, scal2) : std::min(scal1, scal2));
                }
                break;
                case MdType::Short :
                getBigEndian(prm->s[i * r->numShort + t->num], &md[loc]);
                break;
                case MdType::Long :
                getBigEndian(int32_t(prm->i[i * r->numLong + t->num]), &md[loc]);
                break;
            }
        }

        //Finish off the floats. Floats are inherently annoying in SEG-Y
        for (const auto & s : scal)
            getBigEndian(s.second, &md[size_t(s.first)-start-1U]);

        for (size_t j = 0; j < rule.size(); j++)
        {
            geom_t gscale = scaleConv(scal[static_cast<Tr>(rule[j]->scalLoc)]);
            getBigEndian(int32_t(std::lround(prm->f[i * r->numFloat + rule[j]->num] / gscale)), &md[rule[j]->loc-start-1U]);
        }
    }
}

void extractParam(Rule * r, size_t sz, const uchar * buf, Param * prm, size_t stride)
{
    for (size_t i = 0; i < sz; i++)
    {
        const uchar * md = &buf[(r->extent() + stride)*i];
        //Loop through each rule and extract data
        for (const auto v : r->translate)
        {
            const auto t = v.second;
            size_t loc = t->loc - r->start - 1U;
            switch (t->type())
            {
                case MdType::Float :
                prm->f[i * r->numFloat + t->num] = scaleConv(getHost<int16_t>(&md[dynamic_cast<SEGYFloatRuleEntry *>(t)->scalLoc - r->start-1U]))
                                                    * geom_t(getHost<int32_t>(&md[loc]));
                break;
                case MdType::Short :
                prm->s[i * r->numShort + t->num] = getHost<int16_t>(&md[loc]);
                break;
                case MdType::Long :
                prm->i[i * r->numLong + t->num] = getHost<int32_t>(&md[loc]);
                break;
            }
        }
    }
}
}}
