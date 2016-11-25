#include <assert.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <fstream>
#include "cppfileapi.hh"
#include "set.hh"
#include "sglobal.hh"
#include "fileops.hh"

using namespace PIOL;
using namespace File;

std::vector<size_t> getSortIndex(size_t sz, const size_t * list)
{
    std::vector<size_t> index(sz);
    std::iota(index.begin(), index.end(), 0);
    std::sort(index.begin(), index.end(), [list] (size_t s1, size_t s2) { return list[s1] < list[s2]; });
    return index;
}

void reorderTraces(ExSeis piol, File::Interface * dst, File::Interface * src, size_t offset, const std::vector<size_t> & list, size_t memlim = 2U*1024U*1024U*1024U)
{
    size_t lnt = list.size();
    dst->writeNt(src->readNt());
    dst->writeInc(src->readInc());
    dst->writeText(src->readText());
    size_t ns = src->readNs();
    dst->writeNs(ns);

    assert(memlim > sizeof(size_t)*lnt);
    size_t max = (memlim - sizeof(size_t)*lnt) / (4U*SEGSz::getDOSz(ns) + sizeof(size_t));
    size_t biggest = piol.max(lnt);
    size_t extra = biggest/max - lnt/max + (biggest % max > 0) - (lnt % max > 0);

    max = std::min(lnt, max);
    std::vector<trace_t> trc(max * ns);
    std::vector<trace_t> otrc(max * ns);
    Param prm(max);
    Param oprm(max);

    for (size_t i = 0; i < lnt; i += max)
    {
        size_t rblock = std::min(max, lnt - i);
        src->readTrace(offset + i, rblock, trc.data(), &prm);

        std::vector<size_t> sortlist = getSortIndex(rblock, list.data() + i);

        for (size_t j = 0U; j < rblock; j++)
        {
            cpyPrm(sortlist[j], &prm, j, &oprm);

            for (size_t k = 0; k < ns; k++)
                otrc[j*ns + k] = trc[sortlist[j]*ns + k];

            sortlist[j] = list[i+sortlist[j]];
        }
        dst->writeTrace(rblock, sortlist.data(), otrc.data(), &oprm);
    }
    for (size_t i = 0; i < extra; i++)
    {
        src->readTrace(0, size_t(0), nullptr, const_cast<Param *>(PARAM_NULL));
        dst->writeTrace(0, nullptr, nullptr, PARAM_NULL);
    }
}

int main(int argc, char ** argv)
{
    ExSeis piol;

    std::string opt = "i:o:t:d";  //TODO: uses a GNU extension
    std::string name1 = "";
    std::string name2 = "";
    auto type = SortType::SrcRcv;
    bool debug = false;
    for (int c = getopt(argc, argv, opt.c_str()); c != -1; c = getopt(argc, argv, opt.c_str()))
        switch (c)
        {
            case 'i' :
                name1 = optarg;
            break;
            case 'o' :
                name2 = optarg;
            break;
            case 'd' :
                debug = true;
            break;
            case 't' :
                type = static_cast<SortType>(std::stoul(optarg));
            break;
            default :
                fprintf(stderr, "One of the command line arguments is invalid\n");
            break;
        }

    assert(name1.size() && name2.size());

    if (1)
    {
        Set set(piol, name1, name2);
        set.sort(SortType::SrcRcv);
        return 0;
    }

    size_t rank = piol.getRank();
    size_t numRank = piol.getNumRank();

    File::Direct src(piol, name1, FileMode::Read);
    size_t nt = src.readNt();

    if (!nt)
    {
        File::Direct dst(piol, name2, FileMode::Write);
        dst.writeNt(src.readNt());
        dst.writeInc(src.readInc());
        dst.writeText(src.readText());
        dst.writeNs(src.readNs());
        return EXIT_SUCCESS;
    }

    //Perform the decomposition and read coordinates of interest.
    auto dec = decompose(nt, numRank, rank);

    //TODO: Assumes the decomposition is small enough to allow this allocation
    File::Param prm(dec.second);
    src->readParam(dec.first, dec.second, &prm);
    for (size_t i = 0; i < prm.size(); i++)
        setPrm(i, Meta::tn, dec.first + i, &prm);

    auto list = sort(piol, type, nt, dec.first, &prm);

    if (debug)
    {
        std::ofstream fout("OUT" + std::to_string(rank));
        for (size_t r = 0; r < numRank; r++)
        {
            piol.barrier();
            if (r == rank)
                for (size_t i = 0; i < list.size(); i++)
                    fout << r << " " << list[i] << std::endl;
            fout << std::flush;
            piol.barrier();
        }
    }

    File::Direct dst(piol, name2, FileMode::Write);
    reorderTraces(piol, dst, src, dec.first, list, 2U*1024U*1024U*1024U);
    return 0;
}
