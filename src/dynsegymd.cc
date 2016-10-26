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

void fill(Rule * r, size_t sz, const Param * prm, uchar * buf, size_t stride)
{
    size_t start = r->start;
    for (size_t i = 0; i < sz; i++)
    {
        uchar * md = &buf[(r->extent() + stride)*i];
        std::unordered_map<Tr, int16_t, EnumHash> scal;
        std::vector<const FloatRuleEntry *> rule;
        for (const auto v : r->translate)
        {
            const auto t = v.second;
            size_t loc = t->loc - start-1U;
            switch (t->type())
            {
                case MdType::Float :
                {
                    rule.push_back(dynamic_cast<FloatRuleEntry *>(t));
                    auto tr = static_cast<Tr>(rule.back()->scalLoc);
                    int16_t val1 = (scal.find(tr) != scal.end() ? scal[tr] : 1);
                    int16_t val2 = deScale(prm->f[i * r->numFloat + t->num]);
                    scal[tr] = scalComp(val1, val2);
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

void take(Rule * r, size_t sz, const uchar * buf, Param * prm, size_t stride)
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
                prm->f[i * r->numFloat + t->num] = scaleConv(getHost<int16_t>(&md[dynamic_cast<FloatRuleEntry *>(t)->scalLoc - r->start-1U]))
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

/*! Extract the trace parameters from a character array and copy
 *  them to a TraceParam structure
 *  \param[in] md A charachter array of raw trace header contents
 *  \param[out] prm An array of TraceParam structures
 */
void extractTraceParam(Rule * r, size_t sz, const uchar * md, TraceParam * prm, size_t stride)
{
    if (!sz)
        return;

    Param p(r, sz);
    take(r, sz, md, &p, stride);

    for (size_t i = 0; i < sz; i++)
    {
        prm[i].src.x = getPrm(r, i, Meta::xSrc, &p);
        prm[i].src.y = getPrm(r, i, Meta::ySrc, &p);
        prm[i].rcv.x = getPrm(r, i, Meta::xRcv, &p);
        prm[i].rcv.y = getPrm(r, i, Meta::yRcv, &p);
        prm[i].cmp.x = getPrm(r, i, Meta::xCmp, &p);
        prm[i].cmp.y = getPrm(r, i, Meta::yCmp, &p);
        prm[i].line.il = getPrm(r, i, Meta::il, &p);
        prm[i].line.xl = getPrm(r, i, Meta::xl, &p);
        prm[i].tn = getPrm(r, i, Meta::tn, &p);
    }
}

/*! Insert the trace parameters from a TraceParam structure and
 *  copy them into a character array ready for writing to a segy file
 *  \param[in] prm An array of TraceParam structures
 *  \param[out] md A charachter array of raw trace header contents
 */
void insertTraceParam(Rule * r, size_t sz, const TraceParam * prm, uchar * md, size_t stride)
{
    if (!sz)
        return;
    Param p(r, sz);
    for (size_t i = 0; i < sz; i++)
    {
        setPrm(r, i, Meta::xSrc, prm[i].src.x, &p);
        setPrm(r, i, Meta::ySrc, prm[i].src.y, &p);
        setPrm(r, i, Meta::xRcv, prm[i].rcv.x, &p);
        setPrm(r, i, Meta::yRcv, prm[i].rcv.y, &p);
        setPrm(r, i, Meta::xCmp, prm[i].cmp.x, &p);
        setPrm(r, i, Meta::yCmp, prm[i].cmp.y, &p);
        setPrm(r, i, Meta::il, prm[i].line.il, &p);
        setPrm(r, i, Meta::xl, prm[i].line.xl, &p);
        setPrm(r, i, Meta::tn, llint(prm[i].tn), &p);
    }
    fill(r, sz, &p, md, stride);
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
}}
