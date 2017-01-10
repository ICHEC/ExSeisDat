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
        }

    flag.fullextent = full;

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

Rule::Rule(bool full, std::vector<Meta> & m)
{
    numLong = 0;
    numShort = 0;
    numFloat = 0;
    numIndex = 0;

    flag.fullextent = full;

    for (size_t i = 0; i < m.size(); i++)
    {
        RuleEntry * r = NULL;
        switch (m[i])
        {
            case Meta::WtrDepSrc :
                r = new SEGYFloatRuleEntry(numFloat++, Tr::WtrDepSrc, Tr::ScaleElev);
            break;
            case Meta::WtrDepRcv :
                r = new SEGYFloatRuleEntry(numFloat++, Tr::WtrDepRcv, Tr::ScaleElev);
            break;
            case Meta::xSrc :
                r = new SEGYFloatRuleEntry(numFloat++, Tr::xSrc, Tr::ScaleCoord);
            break;
            case Meta::ySrc :
                r = new SEGYFloatRuleEntry(numFloat++, Tr::ySrc, Tr::ScaleCoord);
            break;
            case Meta::xRcv :
                r = new SEGYFloatRuleEntry(numFloat++, Tr::xRcv, Tr::ScaleCoord);
            break;
            case Meta::yRcv :
                r = new SEGYFloatRuleEntry(numFloat++, Tr::yRcv, Tr::ScaleCoord);
            break;
            case Meta::xCmp :
                r = new SEGYFloatRuleEntry(numFloat++, Tr::xCmp, Tr::ScaleCoord);
            break;
            case Meta::yCmp :
                r = new SEGYFloatRuleEntry(numFloat++, Tr::yCmp, Tr::ScaleCoord);
            break;
            case Meta::il :
                r = new SEGYLongRuleEntry(numLong++, Tr::il);
            break;
            case Meta::xl :
                r = new SEGYLongRuleEntry(numLong++, Tr::xl);
            break;
            case Meta::tn :
                r = new SEGYLongRuleEntry(numLong++, Tr::SeqFNum);
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

Rule::Rule(bool full, bool defaults, bool extra)
{
    numLong = 0;
    numShort = 0;
    numFloat = 0;
    numIndex = 0;

    flag.fullextent = full;

    if (defaults)
    {
        translate[Meta::xSrc] = new SEGYFloatRuleEntry(numFloat++, Tr::xSrc, Tr::ScaleCoord);
        translate[Meta::ySrc] = new SEGYFloatRuleEntry(numFloat++, Tr::ySrc, Tr::ScaleCoord);
        translate[Meta::xRcv] = new SEGYFloatRuleEntry(numFloat++, Tr::xRcv, Tr::ScaleCoord);
        translate[Meta::yRcv] = new SEGYFloatRuleEntry(numFloat++, Tr::yRcv, Tr::ScaleCoord);
        translate[Meta::xCmp] = new SEGYFloatRuleEntry(numFloat++, Tr::xCmp, Tr::ScaleCoord);
        translate[Meta::yCmp] = new SEGYFloatRuleEntry(numFloat++, Tr::yCmp, Tr::ScaleCoord);

        translate[Meta::WtrDepSrc] = new SEGYFloatRuleEntry(numFloat++, Tr::WtrDepSrc, Tr::ScaleElev);
        translate[Meta::WtrDepRcv] = new SEGYFloatRuleEntry(numFloat++, Tr::WtrDepRcv, Tr::ScaleElev);

        translate[Meta::il] = new SEGYLongRuleEntry(numLong++, Tr::il);
        translate[Meta::xl] = new SEGYLongRuleEntry(numLong++, Tr::xl);
        translate[Meta::tn] = new SEGYLongRuleEntry(numLong++, Tr::SeqFNum);

        translate[Meta::gtn] = new SEGYIndexRuleEntry(numIndex++);
        translate[Meta::ltn] = new SEGYIndexRuleEntry(numIndex++);
    }

    if (extra)
    {
        translate[Meta::tnl] = new SEGYLongRuleEntry(numLong++, Tr::SeqNum);
        translate[Meta::tnr] = new SEGYLongRuleEntry(numLong++, Tr::TORF);
        translate[Meta::tne] = new SEGYLongRuleEntry(numLong++, Tr::SeqNumEns);
        translate[Meta::SrcNum] = new SEGYLongRuleEntry(numLong++, Tr::ENSrcNum);
        translate[Meta::Tic] = new SEGYShortRuleEntry(numShort++, Tr::TIC);
        translate[Meta::VStack] = new SEGYShortRuleEntry(numShort++, Tr::VStackCnt);
        translate[Meta::HStack] = new SEGYShortRuleEntry(numShort++, Tr::HStackCnt);
        translate[Meta::CDist] = new SEGYLongRuleEntry(numLong++, Tr::CDist);
        translate[Meta::RGElev] = new SEGYFloatRuleEntry(numFloat++, Tr::RcvElv, Tr::ScaleElev);
        translate[Meta::SSElev] = new SEGYFloatRuleEntry(numFloat++, Tr::SurfElvSrc, Tr::ScaleElev);
        translate[Meta::SDElev] = new SEGYFloatRuleEntry(numFloat++, Tr::SrcDpthSurf, Tr::ScaleElev);
        translate[Meta::ns] = new SEGYShortRuleEntry(numShort++, Tr::Ns);
        translate[Meta::inc] = new SEGYShortRuleEntry(numShort++, Tr::Inc);
        translate[Meta::ShotNum] = new SEGYFloatRuleEntry(numFloat++, Tr::ShotNum, Tr::ShotScal);
        translate[Meta::TraceUnit] = new SEGYShortRuleEntry(numShort++, Tr::ValMeas);
        translate[Meta::TransUnit] = new SEGYShortRuleEntry(numShort++, Tr::TransUnit);
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

void Rule::addFloat(Meta m, Tr loc, Tr scalLoc)
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
        case MdType::Index :
        numIndex--;
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

size_t Rule::memUsage(void) const
{
    return numLong * sizeof(SEGYLongRuleEntry) + numShort * sizeof(SEGYShortRuleEntry)
         + numFloat * sizeof(SEGYFloatRuleEntry) + numIndex * sizeof(SEGYIndexRuleEntry) +  sizeof(Rule);
}

size_t Rule::paramMem(void) const
{
    return numLong * sizeof(llint) + numShort * sizeof(int16_t)
         + numFloat * sizeof(geom_t) + numIndex * sizeof(size_t);
}

Param::Param(std::shared_ptr<Rule> r_, csize_t sz) : r(r_)
{
    f.resize(sz * r->numFloat);
    i.resize(sz * r->numLong);
    s.resize(sz * r->numShort);
    t.resize(sz * r->numIndex);
}

Param::Param(csize_t sz) : r(std::make_shared<Rule>(true, true))
{
    f.resize(sz * r->numFloat);
    i.resize(sz * r->numLong);
    s.resize(sz * r->numShort);
    t.resize(sz * r->numIndex);
}

size_t Param::size(void) const
{
    return t.size() / r->numIndex;
}

bool Param::operator==(struct Param & p) const
{
    return f == p.f && i == p.i && s == p.s && t == p.t;
}

size_t Param::memUsage(void) const
{
    return f.capacity() * sizeof(geom_t)
         + i.capacity() * sizeof(llint)
         + s.capacity() * sizeof(int16_t)
         + t.capacity() * sizeof(size_t)
                        + sizeof(Param)
                        + r->memUsage();
}

void cpyPrm(csize_t j, const Param * src, csize_t k, Param * dst)
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
                    break;
                }
        }
}

void insertParam(size_t sz, const Param * prm, uchar * buf, size_t stride)
{
    auto r = prm->r;
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
                case MdType::Index : break;
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
                case MdType::Index : break;
            }
        }
    }
}
}}
