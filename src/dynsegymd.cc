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

static std::vector<Meta> default_metas {
    PIOL_META_xSrc, PIOL_META_ySrc,
    PIOL_META_xRcv, PIOL_META_yRcv,
    PIOL_META_xCmp, PIOL_META_yCmp,
    PIOL_META_Offset,
    PIOL_META_il, PIOL_META_xl,
    PIOL_META_tn
};

static std::vector<Meta> extra_metas {
    PIOL_META_tnl, PIOL_META_tnr, PIOL_META_tne,
    PIOL_META_SrcNum,
    PIOL_META_Tic,
    PIOL_META_VStack, PIOL_META_HStack,
    PIOL_META_RGElev, PIOL_META_SSElev, PIOL_META_SDElev,
    PIOL_META_ns,
    PIOL_META_inc,
    PIOL_META_ShotNum,
    PIOL_META_TraceUnit, PIOL_META_TransUnit
};

Rule::Rule(RuleMap translate_, bool full) : translate(translate_)
{
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

Rule::Rule(const std::vector<Meta>& mlist,
           bool full, bool defaults, bool extras)
{

    //TODO: Change this when extents are flexible
    flag.fullextent = full;
    addIndex(PIOL_META_gtn);
    addIndex(PIOL_META_ltn);

    for (auto m : mlist)
        addRule(m);

    if(defaults)
    {
        for(auto m : default_metas)
            addRule(m);
    }

    if(extras)
    {
        for(auto m : extra_metas)
            addRule(m);
    }

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

Rule::Rule(bool full, bool defaults, bool extras): Rule({}, full, defaults, extras) {}

Rule::~Rule(void)
{
    for (const auto t : translate)
        delete t.second;
}

bool Rule::addRule(Meta m)
{
    if (translate.find(m) != translate.end())
        return false;

    switch (m)
    {
        case PIOL_META_WtrDepSrc :
            addSEGYFloat(m, PIOL_TR_WtrDepSrc, PIOL_TR_ScaleElev);
        break;
        case PIOL_META_WtrDepRcv :
            addSEGYFloat(m, PIOL_TR_WtrDepRcv, PIOL_TR_ScaleElev);
        break;
        case PIOL_META_xSrc :
            addSEGYFloat(m, PIOL_TR_xSrc, PIOL_TR_ScaleCoord);
        break;
        case PIOL_META_ySrc :
            addSEGYFloat(m, PIOL_TR_ySrc, PIOL_TR_ScaleCoord);
        break;
        case PIOL_META_xRcv :
            addSEGYFloat(m, PIOL_TR_xRcv, PIOL_TR_ScaleCoord);
        break;
        case PIOL_META_yRcv :
            addSEGYFloat(m, PIOL_TR_yRcv, PIOL_TR_ScaleCoord);
        break;
        case PIOL_META_xCmp :
            addSEGYFloat(m, PIOL_TR_xCmp, PIOL_TR_ScaleCoord);
        break;
        case PIOL_META_yCmp :
            addSEGYFloat(m, PIOL_TR_yCmp, PIOL_TR_ScaleCoord);
        break;
        case PIOL_META_il :
            addLong(m, PIOL_TR_il);
        break;
        case PIOL_META_xl :
            addLong(m, PIOL_TR_xl);
        break;
        case PIOL_META_Offset :
            addLong(m, PIOL_TR_CDist);
        break;
        case PIOL_META_tn :
            addLong(m, PIOL_TR_SeqFNum);
        break;
        case PIOL_META_COPY :
            addCopy();
        break;
        case PIOL_META_tnl :
            addLong(m, PIOL_TR_SeqNum);
        break;
        case PIOL_META_tnr :
            addLong(m, PIOL_TR_TORF);
        break;
        case PIOL_META_tne :
            addLong(m, PIOL_TR_SeqNumEns);
        break;
        case PIOL_META_SrcNum :
            addLong(m, PIOL_TR_ENSrcNum);
        break;
        case PIOL_META_Tic :
            addShort(m, PIOL_TR_TIC);
        break;
        case PIOL_META_VStack :
            addShort(m, PIOL_TR_VStackCnt);
        break;
        case PIOL_META_HStack :
            addShort(m, PIOL_TR_HStackCnt);
        break;
        case PIOL_META_RGElev :
            addSEGYFloat(m, PIOL_TR_RcvElv, PIOL_TR_ScaleElev);
        break;
        case PIOL_META_SSElev :
            addSEGYFloat(m, PIOL_TR_SurfElvSrc, PIOL_TR_ScaleElev);
        break;
        case PIOL_META_SDElev :
            addSEGYFloat(m, PIOL_TR_SrcDpthSurf, PIOL_TR_ScaleElev);
        break;
        case PIOL_META_ns :
            addShort(m, PIOL_TR_Ns);
        break;
        case PIOL_META_inc :
            addShort(m, PIOL_TR_Inc);
        break;
        case PIOL_META_ShotNum :
            addSEGYFloat(m, PIOL_TR_ShotNum, PIOL_TR_ShotScal);
        break;
        case PIOL_META_TraceUnit :
            addShort(m, PIOL_TR_ValMeas);
        break;
        case PIOL_META_TransUnit :
            addShort(m, PIOL_TR_TransUnit);
        break;
        default :
        return false;
        break;    //Non-default
    }
    return true;
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
    if (!numCopy)
    {
        translate[PIOL_META_COPY] = new SEGYCopyRuleEntry();
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
    if (r->numFloat)
        f.resize(sz * r->numFloat);

    if (r->numLong)
        i.resize(sz * r->numLong);

    if (r->numShort)
        s.resize(sz * r->numShort);

    if (r->numIndex)
        t.resize(sz * r->numIndex);

    if (r->numCopy) //TODO: This must be file format agnostic
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
    if (src == PIOL_PARAM_NULL || src == nullptr ||
        dst == PIOL_PARAM_NULL || dst == nullptr)
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

            //if the rule is in the destination and the types match
            if (valit != drule->translate.end())
            {
                RuleEntry * dent = valit->second;
                RuleEntry * sent = m.second;
                if (dent->type() == sent->type())
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
}

bool Rule::addRule(const Rule& r)
{
    for (auto & m : r.translate)
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
