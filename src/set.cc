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
#include "share/misc.hh"    //For getSort..
#include "set/set.hh"
#include "data/datampiio.hh"
#include "file/filesegy.hh"
#include "object/objsegy.hh"
#warning
#include <iostream>

namespace PIOL {
std::unique_ptr<File::WriteInterface> makeSEGYFile(Piol piol, std::string name, csize_t ns, const geom_t inc, const std::string text)
{
    auto data = std::make_shared<Data::MPIIO>(piol, name, FileMode::Write);
    auto obj = std::make_shared<Obj::SEGY>(piol, name, data, FileMode::Write);
    auto out = std::make_unique<File::WriteSEGY>(piol, name, obj);

    out->writeNs(ns);
    out->writeInc(inc);
    out->writeText(text);

    return std::move(out);
}

/*! Perform a 1d decomposition so that the load is optimally balanced.
 *  \param[in] sz The sz of the 1d domain
 *  \param[in] numRank The number of ranks to perform the decomposition over
 *  \param[in] rank The rank of the local process
 *  \return Return a pair, the first element is the offset for the local process,
 *          the second is the size for the local process.
 */
std::pair<size_t, size_t> decompose(size_t sz, size_t numRank, size_t rank)
{
    size_t q = sz/numRank;
    size_t r = sz%numRank;
    size_t start = q * rank + std::min(rank, r);
    return std::make_pair(start, std::min(sz - start, q + (rank < r)));
}

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

InternalSet::InternalSet(Piol piol_, std::string pattern, std::string outfix_, std::shared_ptr<File::Rule> rule_) :
                                                            piol(piol_), outfix(outfix_), rule(rule_), cache(piol_)
{
    fillDesc(piol, pattern);
}

InternalSet::~InternalSet(void)
{
    if (outfix != "")
        output(outfix);
}

//TODO: Make multi-file
void InternalSet::add(std::string name)
{
    auto data = std::make_shared<Data::MPIIO>(piol, name, FileMode::Read);
    auto obj = std::make_shared<Obj::SEGY>(piol, name, data, FileMode::Read);
    add(std::move(std::make_unique<File::ReadSEGY>(piol, name, obj)));
}

void InternalSet::add(std::unique_ptr<File::ReadInterface> in)
{
    file.emplace_back(std::make_shared<FileDesc>());
    auto & f = file.back();
    f->ifc = std::move(in);

    auto dec = decompose(f->ifc->readNt(), piol->comm->getNumRank(), piol->comm->getRank());
    f->ilst.resize(dec.second);
    f->olst.resize(dec.second);
    std::iota(f->ilst.begin(), f->ilst.end(), dec.first);

    auto key = std::make_pair<size_t, geom_t>(f->ifc->readNs(), f->ifc->readInc());
    fmap[key].emplace_back(f);

    auto & off = offmap[key];
    std::iota(f->olst.begin(), f->olst.end(), off + dec.first);
    off += f->ifc->readNt();
}

void InternalSet::fillDesc(std::shared_ptr<ExSeisPIOL> piol, std::string pattern)
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

void InternalSet::summary(void) const
{
    for (auto & f : file)
    {
        std::string msg =  "name: " + f->ifc->readName() + "\n" +
                          "-\tNs: " + std::to_string(f->ifc->readNs())   + "\n" +
                          "-\tNt: " + std::to_string(f->ifc->readNt())   + "\n" +
                         "-\tInc: " + std::to_string(f->ifc->readInc())  + "\n";

        piol->log->record("", Log::Layer::Set, Log::Status::Request, msg , Log::Verb::None);
    }

    if (!piol->comm->getRank())
    {
        for (auto & m : fmap)
            piol->log->record("", Log::Layer::Set, Log::Status::Request,
                "Local File count for (" + std::to_string(m.first.first) + " nt, " + std::to_string(m.first.second)
                    + " inc) = " + std::to_string(m.second.size()), Log::Verb::None);
        piol->log->procLog();
    }
}

void InternalSet::sort(File::Compare<File::Param> sortFunc)
{
    OpOpt opt = {FuncOpt::SubSetOnly, FuncOpt::DepCoordRot, FuncOpt::DepMeta};

    func.emplace_back(opt, rule, [this, sortFunc] (size_t ns, File::Param * prm, trace_t * trc) -> std::vector<size_t>
        {
            if (piol->comm->min(prm->size()) < 3U) //TODO: It will eventually be necessary to support this use case.
            {
                piol->log->record("", Log::Layer::Set, Log::Status::Error,
                    "Email cathal@ichec.ie if you want to sort -very- small sets of files with multiple processes.", Log::Verb::None);
                return std::vector<size_t>();
            }
            else
            {
                return File::sort(piol.get(), prm, sortFunc);
            }
        });
}


std::string InternalSet::output(FileDeque & fQue)
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
    std::unique_ptr<File::WriteInterface> out = makeSEGYFile(piol, name, ns, inc, outmsg);

    //Assume the process can hold the list
    const size_t memlim = 2U*1024U*1024U*1024U;
    size_t max = memlim / (10U*sizeof(size_t) + SEGSz::getDOSz(ns) + 2U*rule->paramMem() + 2U*rule->memUsage()+ 2U*SEGSz::getDFSz(ns));

//TODO: Needs Copy
//    if (cache.checkPrm(fQue))
//        prm = cache.cachePrm(std::get<1>(*fCurr), o.second);

//    if (cache.checkTrc(fQue))
#warning this will need to be cleverer to avoid memory issues
//        trc = cache.cacheTrc(fQue);
//    File::Param * prm = nullptr;
//    trace_t * trc = nullptr;
    for (auto & f : fQue)
    {
        File::ReadInterface * in = f->ifc.get();
        size_t lnt = f->ilst.size();
        size_t fmax = std::min(lnt, max);
        size_t ns = in->readNs();

        File::Param iprm(rule, fmax);
        File::Param oprm(rule, fmax);
        std::vector<trace_t> itrc(fmax * ns);
        std::vector<trace_t> otrc(fmax * ns);
        auto biggest = piol->comm->max(lnt);
        size_t extra = biggest/max - lnt/max + (biggest % max > 0) - (lnt % max > 0);
        for (size_t i = 0; i < lnt; i += max)
        {
            size_t rblock = (i + max < lnt ? max : lnt - i);
            in->readTrace(rblock, f->ilst.data() + i, itrc.data(), &iprm);
            std::vector<size_t> sortlist = getSortIndex(rblock, f->olst.data() + i);
            for (size_t j = 0U; j < rblock; j++)
            {
                cpyPrm(sortlist[j], &iprm, j, &oprm);

                for (size_t k = 0U; k < ns; k++)
                    otrc[j*ns + k] = itrc[sortlist[j]*ns + k];
                sortlist[j] = f->olst[i+sortlist[j]];
            }

            out->writeTrace(rblock, sortlist.data(), otrc.data(), &oprm);
        }
        for (size_t i = 0; i < extra; i++)
        {
            in->readTrace(0, nullptr, nullptr, const_cast<File::Param *>(File::PARAM_NULL));
            out->writeTrace(0, nullptr, nullptr, File::PARAM_NULL);
        }
    }

    return name;
}

//calc for subsets only
FuncLst::iterator InternalSet::calcFunc(FuncLst::iterator fCurr, const FuncLst::iterator fEnd, FileDeque & fQue)
{
    OpOpt & opt = std::get<0>(*fCurr);
    if (!opt.check(FuncOpt::NeedTrcVal))
    {
        size_t ns = fQue[0]->ifc->readNs();
        File::Param * prm = cache.cachePrm(std::get<1>(*fCurr), fQue);

        auto lfunc = std::get<2>(*fCurr);
        std::vector<size_t> trlist = lfunc(ns, prm, (trace_t *)nullptr);    //The actual function call

        size_t j = 0;
        for (auto & f : fQue)
        {
            std::copy(&trlist[j], &trlist[j + f->olst.size()], f->olst.begin());
            j += f->olst.size();
        }
    }
    else
    {
#warning  TODO: Code currently does not process traces
    }

    if (++fCurr != fEnd)
    {
        opt = std::get<0>(*fCurr);
        if (opt.check(FuncOpt::SubSetOnly))
            return calcFunc(fCurr, fEnd, fQue);
    }
    return fCurr;
}

void InternalSet::calcFunc(FuncLst::iterator fCurr, const FuncLst::iterator fEnd)
{
    if (fCurr != fEnd)
    {
        OpOpt & opt = std::get<0>(*fCurr);
        if (opt.check(FuncOpt::SubSetOnly))
        {
            std::vector<FuncLst::iterator> flist;

            for (auto & o : fmap)                        //TODO: Parallelisable
                flist.push_back(calcFunc(fCurr, fEnd, o.second));     //Iterate across the full function list

            std::equal(flist.begin() + 1U, flist.end(), flist.begin());

            fCurr = flist.front();
        }
        else
        {
    #warning TODO: Implement support for a truly global operation
            std::cerr << "Error, not supported yet\n";
            ++fCurr;
        }
        calcFunc(fCurr, fEnd);
    }
}

#warning use oname
std::vector<std::string> InternalSet::output(std::string oname)
{
    calcFunc(func.begin(), func.end());
    func.clear();

    std::vector<std::string> names;
    for (auto & o : fmap)
        names.push_back(output(o.second));
    return names;
}

void InternalSet::getMinMax(File::Func<File::Param> xlam, File::Func<File::Param> ylam, CoordElem * minmax)
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
}
