/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date November 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include <glob.h>
#include "global.hh"
#include <regex>
#include <numeric>
#include <map>
#include <tuple>
#include "share/misc.hh"    //For getSort..
#include "flow/set.hh"
#include "data/datampiio.hh"
#include "file/filesegy.hh"
#include "object/objsegy.hh"
#include "share/decomp.hh"
#include "ops/gather.hh"
#include "share/uniray.hh"

#warning
#include <iostream>
#include <assert.h>

namespace PIOL {

/*! For CoordElem. Update the dst element based on if the operation gives true.
 *  If the elements have the same value, set the trace number to the
 *  smallest trace number.
 *  \tparam Op The true/false operation to use for comparison
 *  \param[in] src The source one will be using for updating
 *  \param[in, out] dst The destination which will be updated.
 */
template <typename Op>
void updateElem(CoordElem * src, CoordElem * dst)
{
    Op op;
    if (src->val == dst->val)
        dst->num = std::min(dst->num, src->num);
    else if (op(src->val, dst->val))
    {
        dst->val = src->val;
        dst->num = src->num;
    }
}

//////////////////////////////////////////////CLASS MEMBERS///////////////////////////////////////////////////////////

Set::Set(Piol piol_, std::string pattern, std::string outfix_, std::shared_ptr<File::Rule> rule_) :
                                                            piol(piol_), outfix(outfix_), rule(rule_), cache(piol_)
{
    rank = piol->comm->getRank();
    numRank = piol->comm->getNumRank();
    fillDesc(piol, pattern);
}

Set::~Set(void)
{
    if (outfix != "")
        output(outfix);
}

//TODO: Make multi-file
void Set::add(std::string name)
{
    auto data = std::make_shared<Data::MPIIO>(piol, name, FileMode::Read);
    auto obj = std::make_shared<Obj::SEGY>(piol, name, data, FileMode::Read);
    add(std::move(std::make_unique<File::ReadSEGY>(piol, name, obj)));
}

void Set::add(std::unique_ptr<File::ReadInterface> in)
{
    file.emplace_back(std::make_shared<FileDesc>());
    auto & f = file.back();
    f->ifc = std::move(in);

    auto dec = decompose(piol.get(), f->ifc.get());
    f->ilst.resize(dec.second);
    f->olst.resize(dec.second);
    std::iota(f->ilst.begin(), f->ilst.end(), dec.first);

    auto key = std::make_pair<size_t, geom_t>(f->ifc->readNs(), f->ifc->readInc());
    fmap[key].emplace_back(f);

    auto & off = offmap[key];
    std::iota(f->olst.begin(), f->olst.end(), off + dec.first);
    off += f->ifc->readNt();
}

void Set::fillDesc(std::shared_ptr<ExSeisPIOL> piol, std::string pattern)
{
    outmsg = "ExSeisPIOL: Set layer output\n";
    glob_t globs;
    int err = glob(pattern.c_str(), GLOB_TILDE | GLOB_MARK, NULL, &globs);
    if (err)
        exit(-1);

    std::regex reg(".*se?gy$", std::regex_constants::icase | std::regex_constants::optimize | std::regex::extended);

    for (size_t i = 0; i < globs.gl_pathc; i++)
        if (std::regex_match(globs.gl_pathv[i], reg))   //For each input file which matches the regex
            add(globs.gl_pathv[i]);

    globfree(&globs);
    piol->isErr();
}

void Set::summary(void) const
{
    for (auto & f : file)
    {
        std::string msg =  "name: " + f->ifc->readName() + "\n" +
                          "-\tNs: " + std::to_string(f->ifc->readNs())   + "\n" +
                          "-\tNt: " + std::to_string(f->ifc->readNt())   + "\n" +
                         "-\tInc: " + std::to_string(f->ifc->readInc())  + "\n";

        piol->log->record("", Log::Layer::Set, Log::Status::Request, msg , Log::Verb::None);
    }

    if (!rank)
    {
        for (auto & m : fmap)
            piol->log->record("", Log::Layer::Set, Log::Status::Request,
                "Local File count for (" + std::to_string(m.first.first) + " nt, " + std::to_string(m.first.second)
                    + " inc) = " + std::to_string(m.second.size()), Log::Verb::None);
        piol->log->procLog();
    }
}

void Set::sort(Compare<File::Param> sortFunc)
{
    OpOpt opt = {FuncOpt::NeedMeta, FuncOpt::ModMetaVal, FuncOpt::DepMetaVal, FuncOpt::SubSetOnly};
    rule->addRule(Meta::il);
    rule->addRule(Meta::xl);
    func.push_back(std::make_shared<Op<InPlaceMod>>(opt, rule, nullptr, [this, sortFunc] (TraceBlock * in) -> std::vector<size_t>
    {
        if (piol->comm->min(in->prm->size()) < 3U) //TODO: It will eventually be necessary to support this use case.
        {
            piol->log->record("", Log::Layer::Set, Log::Status::Error,
                "Email cathal@ichec.ie if you want to sort -very- small sets of files with multiple processes.", Log::Verb::None);
            return std::vector<size_t>();
        }
        else
        {
            return File::sort(piol.get(), in->prm.get(), sortFunc);
        }
    }));
}


void RadonState::makeState(const std::vector<size_t> & offset, const Uniray<size_t, llint, llint> & gather)
{
    std::unique_ptr<File::ReadSEGYModel> vm = File::makeReadSEGYFile<File::ReadSEGYModel>(piol, "vm.segy");   //TODO:DON'T USE MAGIC NAME
    vNs = vm->readNs();
    vInc = vm->readInc();

    vtrc = vm->readModel(offset.size(), offset.data(), gather);

    il.resize(offset.size());
    xl.resize(offset.size());

    for (size_t i = 0; i < offset.size(); i++)
    {
        auto gval = gather[offset[i]];
        il[i] = std::get<1>(gval);
        xl[i] = std::get<2>(gval);
    }
}

void Set::toAngle(std::string vmName)
{
    OpOpt opt = {FuncOpt::NeedAll, FuncOpt::ModAll, FuncOpt::DepAll, FuncOpt::Gather};
    auto state = std::make_shared<RadonState>(piol);
    func.emplace_back(std::make_shared<Op<Mod>>(opt, rule, state, [state] (const TraceBlock * in, TraceBlock * out)
    {
        csize_t iGSz = in->prm->size();
        out->ns = in->ns;
        out->inc = M_PI / geom_t(180U);   //1 degree in radians
        out->trc.resize(state->oGSz * out->ns);
        out->prm.reset(new File::Param(in->prm->r, state->oGSz));
        if (!in->prm->size())
            return;

        for (size_t j = 0; j < state->oGSz; j++)       //For each angle in the angle gather
            for (size_t z = 0; z < in->ns; z++)    //For each sample (angle + radon)
            {
                //We are using coordinate level accuracy when its not performance critical.
                geom_t vmModel = state->vtrc[in->gNum * state->vNs + std::min(size_t(geom_t(z * in->inc) / state->vInc), state->vNs)];
                llint k = llround(vmModel / cos(geom_t(j * out->inc))) / state->vBin;
                if (k > 0 && k < iGSz)
                    out->trc[j * out->ns + z] = in->trc[k * in->ns + z];
            }

        for (size_t j = 0; j < state->oGSz; j++)
        {
            //TODO: Set the rest of the parameters
            //TODO: Check the get numbers
            File::setPrm(j, Meta::il, state->il[in->gNum], out->prm.get());
            File::setPrm(j, Meta::xl, state->xl[in->gNum], out->prm.get());
        }

    }));
}

std::string Set::output(FileDeque & fQue)
{
    std::string name;
    if (!fQue.size())
        return "";

    size_t ns = fQue[0]->ifc->readNs();
    geom_t inc = fQue[0]->ifc->readInc();

    if (fQue.size() == 1)
        name = outfix + ".segy";
    else
        name = outfix + std::to_string(ns) + "_" + std::to_string(inc) + ".segy";

    //TODO: Add mechanism for selection later
    std::unique_ptr<File::WriteInterface> out = File::makeWriteSEGYFile<File::WriteSEGY>(piol, name);
    out->writeNs(ns);
    out->writeInc(inc);
    out->writeText(outmsg);

    //Assume the process can hold the list
    const size_t memlim = 2U*1024U*1024U*1024U;
    size_t max = memlim / (10U*sizeof(size_t) + SEGSz::getDOSz(ns) + 2U*rule->paramMem() + 2U*rule->memUsage()+ 2U*SEGSz::getDFSz(ns));

    size_t lnt = 0;
    for (auto f : fQue)
        lnt += f->olst.size();

    size_t fmax = std::min(lnt, max);

    auto biggest = piol->comm->max(lnt);
    size_t extra = biggest/max - lnt/max + (biggest % max > 0) - (lnt % max > 0);

    bool cacheFlush = cache.checkPrm(fQue);

    if (cacheFlush) //TODO: Improve this so that it also flushes traces
    {
        std::cout << "Flushing cache.\n";
        for (size_t i = 0; i < lnt; i += max)
        {
            size_t rblock = (i + max < lnt ? max : lnt - i);
            File::Param prm(rule, fmax);
            auto outlist = cache.getOutputTrace(fQue, i, rblock,  &prm);

            cacheFlush = 1;
            out->writeParam(outlist.size(), outlist.data(), &prm);
        }
        for (size_t i = 0; i < extra; i++)
            out->writeParam(0, nullptr, nullptr);
    }

    for (auto & f : fQue)
    {
        File::ReadInterface * in = f->ifc.get();
        size_t lnt = f->ilst.size();
        size_t fmax = std::min(lnt, max);
        size_t ns = in->readNs();

        File::Param iprm(rule, (!cacheFlush ? fmax : 0));
        File::Param oprm(rule, (!cacheFlush ? fmax : 0));
        File::Param * itprm = const_cast<File::Param *>(!cacheFlush ? &iprm : File::PARAM_NULL);
        File::Param * otprm = const_cast<File::Param *>(!cacheFlush ? &oprm : File::PARAM_NULL);

        std::vector<trace_t> itrc(fmax * ns);
        std::vector<trace_t> otrc(fmax * ns);
        auto biggest = piol->comm->max(lnt);
        size_t extra = biggest/max - lnt/max + (biggest % max > 0) - (lnt % max > 0);
        for (size_t i = 0; i < lnt; i += max)
        {
            size_t rblock = (i + max < lnt ? max : lnt - i);
            in->readTrace(rblock, f->ilst.data() + i, itrc.data(), itprm);
            std::vector<size_t> sortlist = getSortIndex(rblock, f->olst.data() + i);
            for (size_t j = 0U; j < rblock; j++)
            {
                cpyPrm(sortlist[j], itprm, j, otprm);

                for (size_t k = 0U; k < ns; k++)
                    otrc[j*ns + k] = itrc[sortlist[j]*ns + k];
                sortlist[j] = f->olst[i+sortlist[j]];
            }

            out->writeTrace(rblock, sortlist.data(), otrc.data(), otprm);
        }

        for (size_t i = 0; i < extra; i++)
        {
            in->readTrace(0, nullptr, nullptr, const_cast<File::Param *>(!cacheFlush ? nullptr : File::PARAM_NULL));
            out->writeTrace(0, nullptr, nullptr, (!cacheFlush ? nullptr : File::PARAM_NULL));
        }
    }
    return name;
}

std::unique_ptr<TraceBlock> Set::calcFuncG(FuncLst::iterator fCurr, const FuncLst::iterator fEnd, std::unique_ptr<TraceBlock> bIn)
{
    if (fCurr == fEnd || !(*fCurr)->opt.check(FuncOpt::Gather))
        return std::move(bIn);

    auto bOut = std::make_unique<TraceBlock>();
    if ((*fCurr)->opt.check(FuncOpt::Gather))
        dynamic_cast<Op<Mod> *>(fCurr->get())->func(bIn.get(), bOut.get());

    bIn.reset();

    return calcFuncG(++fCurr, fEnd, std::move(bOut));
}

//calc for subsets only
FuncLst::iterator Set::calcFuncS(FuncLst::iterator fCurr, const FuncLst::iterator fEnd, FileDeque & fQue)
{
    size_t ns = fQue[0]->ifc->readNs();

    std::shared_ptr<TraceBlock> block;

    if ((*fCurr)->opt.check(FuncOpt::NeedMeta))
    {
        if (!(*fCurr)->opt.check(FuncOpt::NeedTrcVal))
            block= cache.cachePrm((*fCurr)->rule, fQue);
        else
        {
            std::cerr << "Not implemented yet\n";
#warning Both traces and gathers are problematic
        }
    }
    else if ((*fCurr)->opt.check(FuncOpt::NeedTrcVal))
        block = cache.cacheTrc(fQue);

    //The operation call
    std::vector<size_t> trlist = dynamic_cast<Op<InPlaceMod> *>(fCurr->get())->func(block.get());

    size_t j = 0;
    for (auto & f : fQue)
    {
        std::copy(&trlist[j], &trlist[j + f->olst.size()], f->olst.begin());
        j += f->olst.size();
    }

    if (++fCurr != fEnd)
        if ((*fCurr)->opt.check(FuncOpt::SubSetOnly))
            return calcFuncS(fCurr, fEnd, fQue);
    return fCurr;
}

std::string Set::calcFunc(FuncLst::iterator fCurr, const FuncLst::iterator fEnd)
{
    if (fCurr != fEnd)
    {
        if ((*fCurr)->opt.check(FuncOpt::SubSetOnly))
        {
            std::vector<FuncLst::iterator> flist;

            for (auto & o : fmap)                        //TODO: Parallelisable
                flist.push_back(calcFuncS(fCurr, fEnd, o.second));     //Iterate across the full function list

            std::equal(flist.begin() + 1U, flist.end(), flist.begin());

            fCurr = flist.front();
        }
        else if ((*fCurr)->opt.check(FuncOpt::Gather))
        {
            //Note it isn't necessary to use a temporary file here.
            //It's just a choice for simplicity
            if (file.size() > 1U)
            {
                std::vector<std::string> names;
                std::string tOutfix = outfix;
                outfix = "temp";
                for (auto & o : fmap)
                    names.push_back(output(o.second));
                outfix = tOutfix;
                drop();
                for (std::string n : names)
                    add(n);     //TODO: Open with delete on close?
            }
            std::string gname;
            for (auto & o : file)
            {
                //Locate gather boundaries.
                auto gather = File::getIlXlGathers(piol.get(), o->ifc.get());
                auto gdec = decompose(gather.size(), numRank, rank);

                size_t numGather = gdec.second;
                size_t gOffset = gdec.first;

                std::vector<size_t> gNums;
                for (auto fTemp = fCurr; fTemp != fEnd && (*fTemp)->opt.check(FuncOpt::Gather); fTemp++)
                {
                    auto * p = dynamic_cast<Op<Mod> *>(fTemp->get());
                    assert(p);
                    for (size_t i = 0; i < numGather; i++)
                        gNums.push_back(i * numRank + rank);

                    p->state->makeState(gNums, gather);
                }

                //TODO: Loop and add rules
                #warning need better rule handling, create rule of all rules in gather functions
                auto rule = std::make_shared<File::Rule>(std::initializer_list<Meta>{Meta::il, Meta::xl});

                auto fTemp = fCurr;
                while (++fTemp != fEnd && (*fTemp)->opt.check(FuncOpt::Gather));
                gname = (fTemp != fEnd ? "gtemp.segy" : outfix + ".segy");

                //Use inputs as default values. These can be changed later
                std::unique_ptr<File::WriteInterface> out = File::makeWriteSEGYFile<File::WriteSEGY>(piol, gname);

                size_t wOffset = 0U;
                size_t iOffset = 0U;
                size_t extra = piol->comm->max(numGather) - numGather;
                size_t ig = 0;
                for (size_t gNum : gNums)
                {
                    auto gval = gather[gNum];
                    const size_t iGSz = std::get<0>(gval);

                    //Initialise the blocks
                    auto bIn = std::make_unique<TraceBlock>();
                    bIn->prm.reset(new File::Param(rule, iGSz));
                    bIn->trc.resize(iGSz * o->ifc->readNs());
                    bIn->ns = o->ifc->readNs();
                    bIn->nt = o->ifc->readNt();
                    bIn->inc = o->ifc->readInc();
                    bIn->numG = numGather;
                    bIn->gNum = ig++;

                    size_t ioff = piol->comm->offset(iGSz);
                    o->ifc->readTrace(iOffset+ioff, bIn->prm->size(), bIn->trc.data(), bIn->prm.get());
                    iOffset += piol->comm->sum(iGSz);

                    auto bOut = calcFuncG(fCurr, fEnd, std::move(bIn));

                    size_t woff = piol->comm->offset(bOut->prm->size());
                    //For simplicity, the output is now
                    out->writeNs(bOut->ns);
                    out->writeInc(bOut->inc);

                    std::cout << "rank " << rank << " gNum " << gNum << " xl " << std::get<2>(gval) << " offset " << wOffset + woff << std::endl;

                    out->writeTrace(wOffset + woff, bOut->prm->size(), (bOut->prm->size() ? bOut->trc.data() : nullptr), bOut->prm.get());
                    wOffset += piol->comm->sum(bOut->prm->size());
                }
                for (size_t i = 0; i < extra; i++)
                {
                   piol->comm->offset(0U);
                   o->ifc->readTrace(size_t(0), size_t(0), nullptr, nullptr);
                   piol->comm->sum(0U);

                   auto bIn = std::make_unique<TraceBlock>();
                   bIn->prm.reset(new File::Param(rule, 0U));
                   bIn->trc.resize(0U);
                   bIn->ns = o->ifc->readNs();
                   bIn->nt = o->ifc->readNt();
                   bIn->inc = o->ifc->readInc();

                   auto bOut = calcFuncG(fCurr, fEnd, std::move(bIn));
                   out->writeNs(bOut->ns);
                   out->writeInc(bOut->inc);

                   piol->comm->offset(0U);
                   out->writeTrace(size_t(0), size_t(0), nullptr, nullptr);
                   piol->comm->sum(0U);
                }
            }

            while (++fCurr != fEnd && (*fCurr)->opt.check(FuncOpt::Gather));
            if (fCurr != fEnd)
            {
                drop();
                add(gname);
            }
            else
                return gname;
        }
        else
        {
            std::cerr << "Error, not supported yet. TODO: Implement support for a truly global operation.\n";
            ++fCurr;
        }
        calcFunc(fCurr, fEnd);
    }
    return "";
}

std::vector<std::string> Set::output(std::string oname)
{
    outfix = oname;
    std::string out = calcFunc(func.begin(), func.end());
    func.clear();
    if (out == "")
    {
        std::vector<std::string> names;
        for (auto & o : fmap)
            names.push_back(output(o.second));
        return names;
    }
    return std::vector<std::string>{out};
}

void Set::getMinMax(MinMaxFunc<File::Param> xlam, MinMaxFunc<File::Param> ylam, CoordElem * minmax)
{
    minmax[0].val = std::numeric_limits<geom_t>::max();
    minmax[1].val = std::numeric_limits<geom_t>::min();
    minmax[2].val = std::numeric_limits<geom_t>::max();
    minmax[3].val = std::numeric_limits<geom_t>::min();
    for (size_t i = 0 ; i < 4; i++)
        minmax[i].num = std::numeric_limits<size_t>::max();

    CoordElem tminmax[4U];

    for (auto & f : file)
    {
        std::vector<File::Param> vprm;
        File::Param prm(rule, f->ilst.size());

        f->ifc->readParam(f->ilst.size(), f->ilst.data(), &prm);

        for (size_t i = 0; i < f->ilst.size(); i++)
        {
            vprm.emplace_back(rule, 1U);
            cpyPrm(i, &prm, 0, &vprm.back());
        }
#warning Minmax can't assume ordered data! Fix this!
        size_t offset = piol->comm->offset(f->ilst.size());
        File::getMinMax(piol.get(), offset, f->ilst.size(), vprm.data(), xlam, ylam, tminmax);
        for (size_t i = 0U; i < 2U; i++)
        {
            updateElem<std::less<geom_t>>(&tminmax[2U*i], &minmax[2U*i]);
            updateElem<std::greater<geom_t>>(&tminmax[2U*i+1U], &minmax[2U*i+1U]);
        }
    }
}

void Set::taper(TaperFunc func, size_t nTailLft, size_t nTailRt)
{
    Mod modify_ = [func, nTailLft, nTailRt] (size_t ns, File::Param * p, trace_t * t) { File::taper(p->size(), ns, t, func, nTailLft, nTailRt); };
    mod(modify_);
}

void Set::AGC(AGCFunc func, size_t window, trace_t normR)
{
    Mod modify_ = [func, window, normR] (size_t ns, File::Param * p, trace_t *t) {File::AGC(p->size(), ns, t, func, window, normR);};
    mod(modify_);
}

/********************************************** Non-Core **************************************************************/
void Set::sort(SortType type)
{
    Set::sort(File::getComp(type));
}

void Set::getMinMax(Meta m1, Meta m2, CoordElem * minmax)
{
    Set::getMinMax([m1](const File::Param & a) -> geom_t { return File::getPrm<geom_t>(0U, m1, &a); },
                   [m2](const File::Param & a) -> geom_t { return File::getPrm<geom_t>(0U, m2, &a); }, minmax);

}

void Set::taper(TaperType type, size_t nTailLft, size_t nTailRt)
{
    Set::taper(File::getTap(type), nTailLft, nTailRt);
}

void Set::AGC(AGCType type, size_t window, trace_t normR)
{
    Set::AGC(File::getAGCFunc(type), window,  normR);
}
}
