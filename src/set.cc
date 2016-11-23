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
#include <iostream>
#include <map>
#include <numeric>
#include "set/set.hh"
#include "data/datampiio.hh"
#include "file/filesegy.hh"
#include "object/objsegy.hh"
namespace PIOL {
constexpr size_t NOT_IN_OUTPUT = std::numeric_limits<size_t>::max();

std::shared_ptr<File::Rule> getMaxRules(void)
{
    auto rule = std::make_shared<File::Rule>(true, true, true);
    rule->addLong(File::Meta::Misc1, File::Tr::TransConst);
    rule->addShort(File::Meta::Misc2, File::Tr::TransExp);
    //Override the default behaviour of ShotNum
    rule->addLong(File::Meta::ShotNum, File::Tr::ShotNum);
    rule->addShort(File::Meta::Misc3, File::Tr::ShotScal);
    return rule;
}

std::pair<size_t, size_t> decompose(size_t sz, size_t numRank, size_t rank)
{
    size_t q = sz/numRank;
    size_t r = sz%numRank;
    size_t start = q * rank + std::min(rank, r);
    return std::make_pair(start, std::min(sz - start, q + (rank < r)));
}

size_t readwriteAll(ExSeisPIOL * piol, size_t doff, std::shared_ptr<File::Rule> rule, File::Interface * dst, File::Interface * src)
{
    const size_t memlim = 2U*1024U*1024U*1024U;
    auto dec = decompose(src->readNt(), piol->comm->getNumRank(), piol->comm->getRank());
    size_t offset = dec.first;
    size_t lnt = dec.second;
    size_t ns = src->readNs();
    size_t max = memlim / (2U*SEGSz::getDOSz(ns) + rule->memusage());
    size_t fmax = std::min(max, lnt);

    auto biggest = piol->comm->max(lnt);
    size_t extra = biggest/max - lnt/max + (biggest % max > 0) - (lnt % max > 0);

    File::Param prm(rule, fmax);
    std::vector<trace_t> trc(ns * fmax);
    for (size_t i = 0; i < lnt; i += max)
    {
        size_t rblock = (i + max < lnt ? max : lnt - i);
        src->readTrace(offset + i, rblock, trc.data(), &prm);
        dst->writeTrace(doff + offset + i, rblock, trc.data(), &prm);
    }
    for (size_t i = 0; i < extra; i++)
    {
        src->readTrace(size_t(0), size_t(0), nullptr, (File::Param *)nullptr);
        dst->writeTrace(size_t(0), size_t(0), nullptr, (File::Param *)nullptr);
    }
    return src->readNt();
}

//////////////////////////////////////////////CLASSS MEMBERS///////////////////////////////////////////////////////////

InternalSet::InternalSet(Piol piol_, std::string pattern, std::string outfix_) : piol(piol_), outfix(outfix_)
{
    fillDesc(piol, pattern);
}

InternalSet::~InternalSet(void)
{
    if (outfix != "")
        output(outfix);
}

void InternalSet::fillDesc(std::shared_ptr<ExSeisPIOL> piol, std::string pattern)
{
    glob_t globs;
    int err = glob(pattern.c_str(), GLOB_TILDE | GLOB_MARK, NULL, &globs);
    if (err)
        exit(-1);

    std::regex reg(".*se?gy$", std::regex_constants::icase | std::regex_constants::optimize | std::regex::extended);

    size_t off = 0;
    for (size_t i = 0; i < globs.gl_pathc; i++)
        if (std::regex_match(globs.gl_pathv[i], reg))
        {
            std::string name = globs.gl_pathv[i];
            auto data = std::make_shared<Data::MPIIO>(piol, name, FileMode::Read);
            auto obj = std::make_shared<Obj::SEGY>(piol, name, data, FileMode::Read);
            //TODO: There could be a problem with excessive amounts of open files
            file.resize(file.size() + 1);
            auto & f = file.back();
            f.ifc = std::make_unique<File::SEGY>(piol, name, obj, FileMode::Read);

            auto dec = decompose(f.ifc->readNt(), piol->comm->getNumRank(), piol->comm->getRank());

            f.lst.resize(dec.second);
            std::iota(f.lst.begin(), f.lst.end(), off + dec.first);

            off += f.ifc->readNt();
        }

    globfree(&globs);
    piol->isErr();
}

//Find total size
size_t InternalSet::getInNt(void)
{
    size_t nt = 0U;
    for (auto & f : file)
        nt += f.ifc->readNt();
    return nt;
}

size_t InternalSet::getLNt(void)
{
    size_t nt = 0U;
    for (auto & f : file)
        for (auto & l : f.lst)
            nt += (l != NOT_IN_OUTPUT);
    return nt;
}

//Find the total set size
size_t InternalSet::getSetNt(void)
{
    auto lnt = getLNt();
    //TODO: Perform a reduction here to add all the totals
    return lnt;
}

void InternalSet::summary(void) const
{
    std::map<std::pair<size_t, geom_t>, size_t> fcnts;
    for (auto & f : file)
        fcnts[std::make_pair<size_t, geom_t>(f.ifc->readNs(), f.ifc->readInc())]++;

    for (auto & m : fcnts)
        piol->log->record("", Log::Layer::Set, Log::Status::Request,
            "File count for (" + std::to_string(m.first.first) + " nt, " + std::to_string(m.first.second)
                + " inc) = " + std::to_string(m.second), Log::Verb::None);
    piol->log->procLog();
}

void InternalSet::sort(File::Compare<File::Param> func)
{
    size_t snt = getLNt();
    File::Param prm(snt);
    size_t off = 0U;
    std::vector<size_t> foff;
    for (auto & f : file)
    {
        std::vector<size_t> loff;
        for (size_t i = 0; i < f.list.size(); i++)
            if (f.list[i] != NOT_IN_OUTPUT)
                loff.push_back(i);
        File::Param fprm(loff.size());
        f.ifc->readParam(loff.size(), loff.data(), &fprm);
        for (size_t i = 0; i < loff.size(); i++)
            cpyPrm(i, &fprm, off+i, &prm);
        off += loff.size();
        foff.push_back(loff.size());
    }

    auto trlist = File::sort(piol.get(), snt, off, &prm, func);
    auto offsets = gather(std::vector<size_t>(off));
    size_t j = 0;
    for (auto & f : file)
        for (auto & l : f.lst)
            if (l != NOT_IN_OUTPUT)
                l.lst = trlist[j++];
}

void InternalSet::output(std::string oname)
{
/*    if (!func.size())   //No functions to evaluate
    {
        auto rule = getMaxRules();
        std::map<std::pair<size_t, geom_t>, std::vector<File::Interface *>> map;
        for (auto & f : file)
            map[std::make_pair<size_t, geom_t>(f.ifc->readNs(), f.ifc->readInc())].emplace_back(f.ifc.get());
        for (auto & o : map)
        {
            std::string name = oname + std::to_string(o.first.first) + "_" + std::to_string(o.first.second) + ".segy";

#warning TODO: Replace with a single call
            std::unique_ptr<File::SEGY> out;
            auto data = std::make_shared<Data::MPIIO>(piol, name, FileMode::Write);
            auto obj = std::make_shared<Obj::SEGY>(piol, name, data, FileMode::Write);
            out = std::make_unique<File::SEGY>(piol, name, obj, FileMode::Write);

            std::cout << "ns " << o.first.first << std::endl;
            out->writeNs(o.first.first);
            out->writeInc(o.first.second);
            out->writeText("ExSeisPIOL: Set layer output\n");

            size_t offset = 0U;
            for (auto & f : o.second)
                offset += readwriteAll(piol.get(), offset, rule, out.get(), f);
        }
    }*/

    auto rule = getMaxRules();
    std::map<std::pair<size_t, geom_t>, std::vector<File::Interface *>> map;
    for (auto & f : file)
        map[std::make_pair<size_t, geom_t>(f.ifc->readNs(), f.ifc->readInc())].emplace_back(f.ifc.get());
    for (auto & o : map)
    {
        size_t ns = o.first.first;
        std::string name = oname + std::to_string(ns) + "_" + std::to_string(o.first.second) + ".segy";

        std::unique_ptr<File::SEGY> out;
        auto data = std::make_shared<Data::MPIIO>(piol, name, FileMode::Write);
        auto obj = std::make_shared<Obj::SEGY>(piol, name, data, FileMode::Write);
        out = std::make_unique<File::SEGY>(piol, name, obj, FileMode::Write);

        std::cout << "ns " << ns << std::endl;
        out->writeNs(o.first.first);
        out->writeInc(o.first.second);
        out->writeText("ExSeisPIOL: Set layer output\n");


        size_t total = getLNt();
        size_t fmax = std::min(max, total);
        File::Param prm(fmax);
        std::vector<trace_t> trc(fmax * o.first.first);

/*        size_t offset = 0U;
        for (auto & f : o.second)
        {
            offset += readwriteAll(piol.get(), offset, rule, out.get(), f);
        }*/
    }
}
}
