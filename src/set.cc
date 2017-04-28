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

/*! Read a list of traces from the input and write it to the output.
 *  \param[in] piol A pointer to the PIOL object.
 *  \param[in] rule The rule to use for the trace parameters.
 *  \param[in] max The maximum number of traces to read/write at a time.
 *  \param[in] f The input file descriptor.
 *  \param[in] modify
 *  \param[out] out The output file interface.
 */
void readWriteTraces(ExSeisPIOL * piol, std::shared_ptr<File::Rule> rule, size_t max, FileDesc * f, Mod modify,
                                        File::WriteInterface * out)
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
        modify(&iprm, itrc.data());
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

InternalSet::InternalSet(Piol piol_, std::string pattern, std::string outfix_, std::shared_ptr<File::Rule> rule_) : piol(piol_), outfix(outfix_), rule(rule_)
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
    auto in = std::make_unique<File::ReadSEGY>(piol, name, obj);
    add(std::move(in));
}

//TODO: Combine this with fillDesc?
void InternalSet::add(std::unique_ptr<File::ReadInterface> in)
{
    file.emplace_back(std::make_unique<FileDesc>());
    auto & f = file.back();
    f->ifc = std::move(in);

    auto dec = decompose(f->ifc->readNt(), piol->comm->getNumRank(), piol->comm->getRank());
    f->ilst.resize(dec.second);
    f->olst.resize(dec.second);
    std::iota(f->ilst.begin(), f->ilst.end(), dec.first);

    auto key = std::make_pair<size_t, geom_t>(f->ifc->readNs(), f->ifc->readInc());
    fmap[key].emplace_back(f.get());

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

size_t InternalSet::getInNt(void)
{
    size_t nt = 0U;
    for (auto & f : file)
        nt += f->ifc->readNt();
    return nt;
}

size_t InternalSet::getLNt(void)
{
    size_t nt = 0U;
    for (auto & f : file)
        nt += f->olst.size();
    return nt;
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

void InternalSet::sort(File::Compare<File::Param> func)
{
    for (auto & o : fmap)   //Per target output file
    {
        size_t lsnt = 0U;
        for (auto & f : o.second)
            lsnt += f->olst.size();
        size_t off = piol->comm->offset(lsnt);

        size_t nt = 0U;
        for (auto & f : o.second)
            nt += f->ifc->readNt();

        if (nt < 3U * piol->comm->getNumRank()) //TODO: It will eventually be necessary to support this use case.
        {
            piol->log->record("", Log::Layer::Set, Log::Status::Error,
                "Email cathal@ichec.ie if you want to sort -very- small sets of files with multiple processes.", Log::Verb::None);
            return;
        }
        else
        {
            //TODO: Do not make assumptions about Parameter sizes fitting in memory.
            File::Param prm(lsnt);
            size_t loff = 0;
            size_t c = 0;
            for (auto & f : o.second)
            {
                f->ifc->readParam(f->ilst.size(), f->ilst.data(), &prm, loff);
                for (size_t i = 0; i < f->ilst.size(); i++)
                {
                    setPrm(loff+i, Meta::gtn, off + loff + i, &prm);
                    setPrm(loff+i, Meta::ltn, f->ilst[i] * o.second.size() + c, &prm);
                }
                c++;
                loff += f->ilst.size();
            }

            auto trlist = File::sort(piol.get(), &prm, func);
            size_t j = 0;
            for (auto & f : o.second)
#warning do a copy instead
                for (auto & l : f->olst)
                    l = trlist[j++];
        }
    }
}

std::vector<std::string> InternalSet::output(std::string oname)
{
    std::vector<std::string> names;
    for (auto & o : fmap)
    {
        size_t ns = o.first.first;
        std::string name;
        if (fmap.size() == 1)
            name = oname + ".segy";
        else
            name = oname + std::to_string(ns) + "_" + std::to_string(o.first.second) + ".segy";
        names.push_back(name);

        auto data = std::make_shared<Data::MPIIO>(piol, name, FileMode::Write);
        auto obj = std::make_shared<Obj::SEGY>(piol, name, data, FileMode::Write);
        auto out = std::make_unique<File::WriteSEGY>(piol, name, obj);

        out->writeNs(o.first.first);
        out->writeInc(o.first.second);
        out->writeText(outmsg);

        //Assume the process can hold the list
        const size_t memlim = 2U*1024U*1024U*1024U;
        size_t max = memlim / (10U*sizeof(size_t) + SEGSz::getDOSz(ns) + 2U*rule->paramMem() + 2U*rule->memUsage()+ 2U*SEGSz::getDFSz(ns));

//TODO: This is not the ideal for small files. per input file read/write
// The ideal is to have a buffer for each which is emptied when full or EOF. This is a little tricky
// because the write buffer would interleave with the read a bit.

        for (auto & f : o.second)
            readWriteTraces(piol.get(), rule, max, f, modify, out.get());
    }
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
