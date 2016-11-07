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
Rule::Rule(RuleMap translate_, bool full)
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
    if (!full)
    {
        flag.badextent = true;
        extent();
    }
}

Rule::Rule(bool full, std::vector<Meta> & m)
{
    numLong = 0;
    numShort = 0;
    numFloat = 0;

    flag.fullextent = full;

    for (size_t i = 0; i < m.size(); i++)
    {
        RuleEntry * r = NULL;
        switch (m[i])
        {
            case Meta::xSrc :
                translate[Meta::xSrc] = new SEGYFloatRuleEntry(numFloat++, Tr::xSrc, Tr::ScaleCoord);
            break;
            case Meta::ySrc :
                translate[Meta::ySrc] = new SEGYFloatRuleEntry(numFloat++, Tr::ySrc, Tr::ScaleCoord);
            break;
            case Meta::xRcv :
                translate[Meta::xRcv] = new SEGYFloatRuleEntry(numFloat++, Tr::xRcv, Tr::ScaleCoord);
            break;
            case Meta::yRcv :
                translate[Meta::yRcv] = new SEGYFloatRuleEntry(numFloat++, Tr::yRcv, Tr::ScaleCoord);
            break;
            case Meta::xCmp :
                translate[Meta::xCmp] = new SEGYFloatRuleEntry(numFloat++, Tr::xCmp, Tr::ScaleCoord);
            break;
            case Meta::yCmp :
                translate[Meta::yCmp] = new SEGYFloatRuleEntry(numFloat++, Tr::yCmp, Tr::ScaleCoord);
            break;
            case Meta::il :
                translate[Meta::il] = new SEGYLongRuleEntry(numLong++, Tr::il);
            break;
            case Meta::xl :
                translate[Meta::xl] = new SEGYLongRuleEntry(numLong++, Tr::xl);
            break;
            case Meta::tn :
                translate[Meta::tn] = new SEGYLongRuleEntry(numLong++, Tr::SeqFNum);
            break;
            default : break;    //Non-default
        }
        if (r)
            translate[m[i]] = r;
    }
    if (full)
    {
        start = 0U;
        end = SEGSz::getMDSz();
        flag.badextent = false;
    }
    else
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
        start = 0U;
        end = SEGSz::getMDSz();
        flag.badextent = false;
    }
    else
    {
        flag.badextent = true;
        extent();
    }
}

Rule::~Rule(void)
{
    for (const auto t : translate)
        delete t.second;
}

size_t Rule::extent(void)
{
    if (flag.fullextent)
        return SEGSz::getMDSz();
    if (flag.badextent)
    {
        start = SEGSz::getMDSz();
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

//TODO: These can be optimised to stop the double lookup if required.
void Rule::addLong(Meta m, Tr loc)
{
    auto ent = translate.find(m);
    if (ent != translate.end())
        delete ent->second;
    translate[m] = new SEGYLongRuleEntry(numLong++, loc);
    flag.badextent = (!flag.fullextent);
}

void Rule::addShort(Meta m, Tr loc)
{
    auto ent = translate.find(m);
    if (ent != translate.end())
        delete ent->second;
    translate[m] = new SEGYShortRuleEntry(numShort++, loc);
    flag.badextent = (!flag.fullextent);
}

void Rule::addFloat(Meta m, Tr loc, Tr scalLoc)
{
    auto ent = translate.find(m);
    if (ent != translate.end())
        delete ent->second;
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
    delete translate[m];
    translate.erase(m);
    flag.badextent = (!flag.fullextent);
}

RuleEntry * Rule::getEntry(Meta entry)
{
    return translate[entry];
}

prmRet getPrm(size_t i, Meta entry, const Param * prm)
{
    Rule * r = prm->r.get();
    RuleEntry * id = r->getEntry(entry);
    switch (id->type())
    {
        case MdType::Long :
        return prmRet(prm->i[r->numLong*i + id->num]);
        break;
        case MdType::Short :
        return prmRet(prm->s[r->numShort*i + id->num]);
        break;
        case MdType::Float :
        return prmRet(prm->f[r->numFloat*i + id->num]);
        break;
        default :
            return llint(0);
        break;
    }
}

Param::Param(std::shared_ptr<Rule> r_, csize_t sz) : r(r_)
{
    f = new geom_t[sz * r->numFloat];
    i = new llint[sz * r->numLong];
    s = new short[sz * r->numShort];
    t = new size_t[sz];
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

#warning todo: Do type checks
void setPrm(size_t i, Meta entry, geom_t val, Param * prm)
{
    Rule * r = prm->r.get();
    prm->f[i * r->numFloat + r->getEntry(entry)->num] = val;
}

void setPrm(csize_t i, const Meta entry, const llint val, Param * prm)
{
    Rule * r = prm->r.get();
    prm->i[i * r->numLong + r->getEntry(entry)->num] = val;
}

void setPrm(csize_t i, const Meta entry, const short val, Param * prm)
{
    Rule * r = prm->r.get();
    prm->s[i * r->numShort + r->getEntry(entry)->num] = val;
}


void setPrm(csize_t i, const Meta entry, prmRet ret, Param * prm)
{
    switch (prm->r->translate[entry]->type())
    {
        case MdType::Long :
        setPrm(i, entry, llint(ret), prm);
        break;
        case MdType::Short :
        setPrm(i, entry, short(ret), prm);
        break;
        case MdType::Float :
        setPrm(i, entry, geom_t(ret), prm);
        break;
    }
}

void cpyPrm(csize_t j, const Param * src, size_t k, Param * dst)
{
    Rule * srule = src->r.get();
    Rule * drule = dst->r.get();
    if (srule == drule)
    {
        Rule * r = srule;
        for (size_t i = 0; i < r->numFloat; i++)
            dst->f[k * r->numFloat + i] = src->f[j * r->numFloat + i];
        for (size_t i = 0; i < r->numLong; i++)
            dst->i[k * r->numLong + i] = src->i[j * r->numLong + i];
        for (size_t i = 0; i < r->numShort; i++)
            dst->s[k * r->numShort + i] = src->s[j * r->numShort + i];
    }
    else
        //For each rule in source
        for (auto & m : srule->translate)
        {
            //Check for a rule in destination
            auto valit = drule->translate.find(m.first);
            RuleEntry * dent = valit->second;
            RuleEntry * sent = m.second;
            //if the rule is in the destination and the types match
            if (valit != drule->translate.end() && dent->type() == sent->type())
                switch (m.second->type())
                {
                    case MdType::Long :
                    dst->i[drule->numLong*k + dent->num] = src->i[srule->numLong*j + sent->num];
                    break;
                    case MdType::Short :
                    dst->s[drule->numShort*k + dent->num] = src->s[srule->numShort*j + sent->num];
                    break;
                    case MdType::Float :
                    dst->f[drule->numFloat*k + dent->num] = src->f[srule->numFloat*j + sent->num];
                    break;
                }
        }
}

void insertParam(size_t sz, const Param * prm, uchar * buf, size_t stride)
{
    Rule * r = prm->r.get();
    size_t start = r->start;
    for (size_t i = 0; i < sz; i++)
    {
        uchar * md = &buf[(r->extent() + stride)*i];
#if defined(__INTEL_COMPILER) || __GNUC__ < 6    //Compiler defects
        std::unordered_map<Tr, int16_t, EnumHash> scal;
#else
        std::unordered_map<Tr, int16_t> scal;
#endif
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

void extractParam(size_t sz, const uchar * buf, Param * prm, size_t stride)
{
    Rule * r = prm->r.get();
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
