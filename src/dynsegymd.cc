/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date October 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include <limits>
#include <cstring>
#include <cmath>
#include <iostream>
#include "file/segymd.hh"
#include "share/datatype.hh"
#include "file/dynsegymd.hh"

namespace PIOL { namespace File {
Rule::Rule(RuleMap translate_, bool full)
{
    numLong = 0;
    numShort = 0;
    numFloat = 0;
    numIndex = 0;
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
            case MdType::Index :
            numIndex++;
            break;
            case MdType::Copy :
            numCopy++;
            break;
        }

    flag.fullextent = full;

    if (full)
    {
        start = 0LU;
        end = SEGSz::getMDSz();
        flag.badextent = false;
    }
    else
    {
        flag.badextent = true;
        extent();
    }
}

bool Rule::addRule(Meta m)
{
    if (translate.find(m) != translate.end())
        return false;

    switch (m)
    {
        case Meta::WtrDepSrc :
            addSEGYFloat(Meta::WtrDepSrc, Tr::WtrDepSrc, Tr::ScaleElev);
        break;
        case Meta::WtrDepRcv :
            addSEGYFloat(Meta::WtrDepRcv, Tr::WtrDepRcv, Tr::ScaleElev);
        break;
        case Meta::xSrc :
            addSEGYFloat(Meta::xSrc, Tr::xSrc, Tr::ScaleCoord);
        break;
        case Meta::ySrc :
            addSEGYFloat(Meta::ySrc, Tr::ySrc, Tr::ScaleCoord);
        break;
        case Meta::xRcv :
            addSEGYFloat(Meta::xRcv, Tr::xRcv, Tr::ScaleCoord);
        break;
        case Meta::yRcv :
            addSEGYFloat(Meta::yRcv, Tr::yRcv, Tr::ScaleCoord);
        break;
        case Meta::xCmp :
            addSEGYFloat(Meta::xCmp, Tr::xCmp, Tr::ScaleCoord);
        break;
        case Meta::yCmp :
            addSEGYFloat(Meta::yCmp, Tr::yCmp, Tr::ScaleCoord);
        break;
        case Meta::il :
            addLong(Meta::il, Tr::il);
        break;
        case Meta::xl :
            addLong(Meta::xl, Tr::xl);
        break;
        case Meta::Offset :
            addLong(Meta::Offset, Tr::CDist);
        break;
        case Meta::tn :
            addLong(Meta::tn, Tr::SeqFNum);
        break;
        case Meta::Copy :
            addCopy();
        break;
        case Meta::tnl :
            addLong(Meta::tnl, Tr::SeqNum);
        break;
        case Meta::tnr :
            addLong(Meta::tnr, Tr::TORF);
        break;
        case Meta::tne :
            addLong(Meta::tne, Tr::SeqNumEns);
        break;
        case Meta::SrcNum :
            addLong(Meta::SrcNum, Tr::ENSrcNum);
        break;
        case Meta::Tic :
            addShort(Meta::Tic, Tr::TIC);
        break;
        case Meta::VStack :
            addShort(Meta::VStack, Tr::VStackCnt);
        break;
        case Meta::HStack :
            addShort(Meta::HStack, Tr::HStackCnt);
        break;
        case Meta::RGElev :
            addSEGYFloat(Meta::RGElev, Tr::RcvElv, Tr::ScaleElev);
        break;
        case Meta::SSElev :
            addSEGYFloat(Meta::SSElev, Tr::SurfElvSrc, Tr::ScaleElev);
        break;
        case Meta::SDElev :
            addSEGYFloat(Meta::SDElev, Tr::SrcDpthSurf, Tr::ScaleElev);
        break;
        case Meta::ns :
            addShort(Meta::ns, Tr::Ns);
        break;
        case Meta::inc :
            addShort(Meta::inc, Tr::Inc);
        break;
        case Meta::ShotNum :
            addSEGYFloat(Meta::ShotNum, Tr::ShotNum, Tr::ShotScal);
        break;
        case Meta::TraceUnit :
            addShort(Meta::TraceUnit, Tr::ValMeas);
        break;
        case Meta::TransUnit :
            addShort(Meta::TransUnit, Tr::TransUnit);
        break;
        default :
        return false;
        break;    //Non-default
    }
    return true;
}

Rule::Rule(std::initializer_list<Meta> mlist, bool full)
{
    numLong = 0;
    numShort = 0;
    numFloat = 0;
    numIndex = 0;
    numCopy = 0;

    //TODO: Change this when extents are flexible
    flag.fullextent = full;
    addIndex(Meta::gtn);
    addIndex(Meta::ltn);

    for (auto m : mlist)
        addRule(m);

    if (flag.fullextent)
    {
        start = 0LU;
        end = SEGSz::getMDSz();
        flag.badextent = false;
    }
    else
    {
        flag.badextent = true;
        extent();
    }
}

Rule::Rule(bool full, bool defaults, bool extra)
{
    numLong = 0;
    numShort = 0;
    numFloat = 0;
    numIndex = 0;
    numCopy = 0;

    flag.fullextent = full;

    addIndex(Meta::gtn);
    addIndex(Meta::ltn);

    if (defaults)
    {
        addRule(Meta::xSrc);
        addRule(Meta::ySrc);
        addRule(Meta::xRcv);
        addRule(Meta::yRcv);
        addRule(Meta::xCmp);
        addRule(Meta::yCmp);
        addRule(Meta::Offset);
        addRule(Meta::il);
        addRule(Meta::xl);
        addRule(Meta::tn);
    }

    if (extra)
    {
        addRule(Meta::tnl);
        addRule(Meta::tnr);
        addRule(Meta::tne);
        addRule(Meta::SrcNum);
        addRule(Meta::Tic);
        addRule(Meta::VStack);
        addRule(Meta::HStack);
        addRule(Meta::RGElev);
        addRule(Meta::SSElev);
        addRule(Meta::SDElev);
        addRule(Meta::ns);
        addRule(Meta::inc);
        addRule(Meta::ShotNum);
        addRule(Meta::TraceUnit);
        addRule(Meta::TransUnit);
    }

    if (full)
    {
        start = 0LU;
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
        end = 0LU;
        for (const auto r : translate)
            if (r.second->type() != MdType::Index)
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
        rmRule(m);
    translate[m] = new SEGYLongRuleEntry(numLong++, loc);
    flag.badextent = (!flag.fullextent);
}

void Rule::addShort(Meta m, Tr loc)
{
    auto ent = translate.find(m);
    if (ent != translate.end())
        rmRule(m);
    translate[m] = new SEGYShortRuleEntry(numShort++, loc);
    flag.badextent = (!flag.fullextent);
}

void Rule::addSEGYFloat(Meta m, Tr loc, Tr scalLoc)
{
    auto ent = translate.find(m);
    if (ent != translate.end())
        rmRule(m);
    translate[m] = new SEGYFloatRuleEntry(numFloat++, loc, scalLoc);
    flag.badextent = (!flag.fullextent);
}

void Rule::addIndex(Meta m)
{
    auto ent = translate.find(m);
    if (ent != translate.end())
        rmRule(m);
    translate[m] = new SEGYIndexRuleEntry(numIndex++);
}

void Rule::addCopy(void)
{
    auto ent = translate.find(Meta::Copy);
    if (ent == translate.end())
    {
        translate[Meta::Copy] = new SEGYCopyRuleEntry();
        numCopy++;
    }
}

void Rule::rmRule(Meta m)
{
    auto iter = translate.find(m);
    if (iter != translate.end())
    {
        RuleEntry * entry = iter->second;
        MdType type = entry->type();
        size_t num = entry->num;

        switch (type)
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
            case MdType::Index :
            numIndex--;
            break;
            case MdType::Copy :
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

RuleEntry * Rule::getEntry(Meta entry)
{
    return translate[entry];
}

size_t Rule::memUsage(void) const
{
    return numLong * sizeof(SEGYLongRuleEntry) + numShort * sizeof(SEGYShortRuleEntry)
         + numFloat * sizeof(SEGYFloatRuleEntry) + numIndex * sizeof(SEGYIndexRuleEntry)
         + numCopy * sizeof(SEGYCopyRuleEntry) +  sizeof(Rule);
}

size_t Rule::paramMem(void) const
{
    return numLong * sizeof(llint) + numShort * sizeof(int16_t)
         + numFloat * sizeof(geom_t) + numIndex * sizeof(size_t) + (numCopy ? SEGSz::getMDSz() : 0);
}

Param::Param(std::shared_ptr<Rule> r_, csize_t sz_) : r(r_), sz(sz_)
{
    f.resize(sz * r->numFloat);
    i.resize(sz * r->numLong);
    s.resize(sz * r->numShort);
    t.resize(sz * r->numIndex);

    //TODO: This must be file format agnostic
    c.resize(sz * (r->numCopy ? SEGSz::getMDSz() : 0));
}

Param::Param(csize_t sz_) : r(std::make_shared<Rule>(true, true)), sz(sz_)
{
    f.resize(sz * r->numFloat);
    i.resize(sz * r->numLong);
    s.resize(sz * r->numShort);
    t.resize(sz * r->numIndex);

    //TODO: This must be file format agnostic
    c.resize(sz * (r->numCopy ? SEGSz::getMDSz() : 0));
}

size_t Param::size(void) const
{
    return sz;
}

bool Param::operator==(struct Param & p) const
{
    return f == p.f && i == p.i && s == p.s && t == p.t && c == p.c;
}

size_t Param::memUsage(void) const
{
    return f.capacity() * sizeof(geom_t)
         + i.capacity() * sizeof(llint)
         + s.capacity() * sizeof(int16_t)
         + t.capacity() * sizeof(size_t)
         + c.capacity() * sizeof(uchar)
                        + sizeof(Param)
                        + r->memUsage();
}

void cpyPrm(csize_t j, const Param * src, csize_t k, Param * dst)
{
    if (src == File::PARAM_NULL || src == nullptr ||
        dst == File::PARAM_NULL || dst == nullptr)
        return;

    Rule * srule = src->r.get();
    Rule * drule = dst->r.get();

    if (srule->numCopy)
        extractParam(1LU, &src->c[j * SEGSz::getMDSz()], dst, 0LU, k);

    if (srule == drule)
    {
        Rule * r = srule;

        for (size_t i = 0; i < r->numFloat; i++)
            dst->f[k * r->numFloat + i] = src->f[j * r->numFloat + i];
        for (size_t i = 0; i < r->numLong; i++)
            dst->i[k * r->numLong + i] = src->i[j * r->numLong + i];
        for (size_t i = 0; i < r->numShort; i++)
            dst->s[k * r->numShort + i] = src->s[j * r->numShort + i];
        for (size_t i = 0; i < r->numIndex; i++)
            dst->t[k * r->numIndex + i] = src->t[j * r->numIndex + i];
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
                    case MdType::Float :
                    dst->f[drule->numFloat*k + dent->num] = src->f[srule->numFloat*j + sent->num];
                    break;
                    case MdType::Long :
                    dst->i[drule->numLong*k + dent->num] = src->i[srule->numLong*j + sent->num];
                    break;
                    case MdType::Short :
                    dst->s[drule->numShort*k + dent->num] = src->s[srule->numShort*j + sent->num];
                    break;
                    case MdType::Index :
                    dst->t[drule->numIndex*k + dent->num] = src->t[srule->numIndex*j + sent->num];
                    default : break;
                }
        }
}

bool Rule::addRule(Rule * r)
{
    for (auto & m : r->translate)
        if (translate.find(m.first) == translate.end())
            switch (m.second->type())
            {
                case MdType::Float :
                addSEGYFloat(m.first, static_cast<Tr>(m.second->loc), static_cast<Tr>(static_cast<SEGYFloatRuleEntry *>(m.second)->scalLoc));
                break;
                case MdType::Long :
                addLong(m.first, static_cast<Tr>(m.second->loc));
                break;
                case MdType::Short :
                addShort(m.first, static_cast<Tr>(m.second->loc));
                break;
                case MdType::Index :
                addIndex(m.first);
                break;
                case MdType::Copy :
                addCopy();
                break;
                default : break;
            }
    return true;
}



void insertParam(size_t sz, const Param * prm, uchar * buf, size_t stride, size_t skip)
{
    if (prm == nullptr || !sz)
        return;
    auto r = prm->r;
    size_t start = r->start;

    if (r->numCopy)
    {
        if (!stride)
            std::copy(&prm->c[skip * SEGSz::getMDSz()], &prm->c[(skip + sz) * SEGSz::getMDSz()], buf);
        else
            for (size_t i = 0; i < sz; i++)
                std::copy(&prm->c[(i+skip) * SEGSz::getMDSz()], &prm->c[(skip+i+1LU) * SEGSz::getMDSz()],
                          &buf[i * (stride + SEGSz::getMDSz())]);
    }

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
            size_t loc = t->loc - start-1LU;
            switch (t->type())
            {
                case MdType::Float :
                {
                    rule.push_back(dynamic_cast<SEGYFloatRuleEntry *>(t));
                    auto tr = static_cast<Tr>(rule.back()->scalLoc);
                    int16_t scal1 = (scal.find(tr) != scal.end() ? scal[tr] : 1);
                    int16_t scal2 = deScale(prm->f[(i + skip) * r->numFloat + t->num]);

                    //if the scale is bigger than 1 that means we need to use the largest
                    //to ensure conservation of the most significant digit
                    //otherwise we choose the scale that preserves the most digits
                    //after the decimal place.
                    scal[tr] = ((scal1 > 1 || scal2 > 1) ? std::max(scal1, scal2) : std::min(scal1, scal2));
                }
                break;
                case MdType::Short :
                getBigEndian(prm->s[(i + skip) * r->numShort + t->num], &md[loc]);
                break;
                case MdType::Long :
                getBigEndian(int32_t(prm->i[(i + skip) * r->numLong + t->num]), &md[loc]);
                default : break;
            }
        }

        //Finish off the floats. Floats are inherently annoying in SEG-Y
        for (const auto & s : scal)
            getBigEndian(s.second, &md[size_t(s.first)-start-1LU]);

        for (size_t j = 0; j < rule.size(); j++)
        {
            geom_t gscale = scaleConv(scal[static_cast<Tr>(rule[j]->scalLoc)]);
            getBigEndian(int32_t(std::lround(prm->f[(i + skip) * r->numFloat + rule[j]->num] / gscale)), &md[rule[j]->loc-start-1LU]);
        }
    }
}

void extractParam(size_t sz, const uchar * buf, Param * prm, size_t stride, size_t skip)
{
    if (prm == nullptr || !sz)
        return;
    Rule * r = prm->r.get();

    if (r->numCopy)
    {
        if (!stride)
            std::copy(buf, &buf[sz * SEGSz::getMDSz()], &prm->c[skip * SEGSz::getMDSz()]);
        else
        {
             const size_t mdsz = SEGSz::getMDSz();
             for (size_t i = 0; i < sz; i++)
                std::copy(&buf[i * (stride + mdsz)], &buf[i * (stride + mdsz)+mdsz], &prm->c[(i + skip) * mdsz]);
        }
    }

    for (size_t i = 0; i < sz; i++)
    {
        const uchar * md = &buf[(r->extent() + stride)*i];
        //Loop through each rule and extract data
        for (const auto v : r->translate)
        {
            const auto t = v.second;
            size_t loc = t->loc - r->start - 1LU;
            switch (t->type())
            {
                case MdType::Float :
                prm->f[(i + skip) * r->numFloat + t->num] = scaleConv(getHost<int16_t>(&md[dynamic_cast<SEGYFloatRuleEntry *>(t)->scalLoc - r->start-1LU]))
                                                   * geom_t(getHost<int32_t>(&md[loc]));
                break;
                case MdType::Short :
                prm->s[(i + skip) * r->numShort + t->num] = getHost<int16_t>(&md[loc]);
                break;
                case MdType::Long :
                prm->i[(i + skip) * r->numLong + t->num] = getHost<int32_t>(&md[loc]);
                default : break;
            }
        }
    }
}
}}
