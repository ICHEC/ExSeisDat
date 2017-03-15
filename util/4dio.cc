/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \date January 2017
 *   \brief
 *   \details This file contains the I/O related portions of the 4d Binning utility.
 *//*******************************************************************************************/
#include <numeric>
#include "4dio.hh"
#include "sglobal.hh"
#include "fileops.hh"   //For sort
#include "share/misc.hh"
namespace PIOL { namespace FOURD {
//TODO: Integration candidate
//TODO: Simple IME optimisation: Contig Read all headers, sort, random write all headers to order, IME shuffle, contig read all headers again
std::unique_ptr<Coords> getCoords(Piol piol, std::string name)
{
    auto time = MPI_Wtime();
    File::ReadDirect file(piol, name);
    auto dec = decompose(file.readNt(), piol->comm->getNumRank(), piol->comm->getRank());
    size_t offset = dec.first;
    size_t lnt = dec.second;

    auto coords = std::make_unique<Coords>(lnt);
    assert(coords.get());
    auto rule = std::make_shared<File::Rule>(std::initializer_list<Meta>{Meta::gtn, Meta::xSrc});
    /*These two lines are for some basic memory limitation calculations. In future versions of the PIOL this will be
      handled internally and in a more accurate way. User Story S-01490. The for loop a few lines below reads the trace
      parameters in batches because of this memory limit.*/
    size_t memlim = 2U*1024U*1024U*1024U - 4U * coords->sz * sizeof(geom_t);
    size_t max = memlim / (rule->paramMem() + SEGSz::getMDSz());

    //Collective I/O requries an equal number of MPI-IO calls on every process in exactly the same sequence as each other.
    //If not, the code will deadlock. Communication is done to ensure we balance out the correct number of redundant calls
    size_t biggest = piol->comm->max(lnt);
    size_t extra = biggest/max - lnt/max + (biggest % max > 0) - (lnt % max > 0);

    File::Param prm(rule, lnt);
    for (size_t i = 0; i < lnt; i += max)
    {
        size_t rblock = (i + max < lnt ? max : lnt - i);

        //WARNING: Treat ReadDirect like the internal API for using a non-exposed function
        file->readParam(offset+i, rblock, &prm, i);

        for (size_t j = 0; j < rblock; j++)
            setPrm(i + j, Meta::gtn, offset + i + j, &prm);
    }

    //Any extra readParam calls the particular process needs
    for (size_t i = 0; i < extra; i++)
        file.readParam(size_t(0), size_t(0), nullptr);
    cmsg(piol.get(), "getCoords sort");

    auto trlist = File::sort(piol.get(), &prm, [] (const File::Param & e1, const File::Param & e2) -> bool
            {
                return (File::getPrm<geom_t>(0U, Meta::xSrc, &e1) < File::getPrm<geom_t>(0U, Meta::xSrc, &e2) ? true :
                        File::getPrm<geom_t>(0U, Meta::xSrc, &e1) == File::getPrm<geom_t>(0U, Meta::xSrc, &e2) &&
                        File::getPrm<size_t>(0U, Meta::gtn, &e1) < File::getPrm<size_t>(0U, Meta::gtn, &e2));
            }, false);

    cmsg(piol.get(), "getCoords post-sort I/O");

/////////////////////////////////////////////////////////////////////////////

    //This makes a rule about what data we will access. In this particular case it's xsrc, ysrc, xrcv, yrcv.
    //Unfortunately shared pointers make things ugly in C++.
    //without shared pointers it would be File::Rule rule = { Meta::xSrc, Meta::ySrc, Meta::xRcv, Meta::yRcv };
    auto crule = std::make_shared<File::Rule>(std::initializer_list<Meta>{Meta::xSrc, Meta::ySrc, Meta::xRcv, Meta::yRcv});
    max = memlim / (crule->paramMem() + SEGSz::getMDSz() + 2U*sizeof(size_t));

    {
    File::Param prm2(crule, std::min(lnt, max));
    for (size_t i = 0; i < lnt; i += max)
    {
        size_t rblock = (i + max < lnt ? max : lnt - i);

        auto sortlist = getSortIndex(rblock, trlist.data() + i);
        auto orig = sortlist;
        for (size_t j = 0; j < sortlist.size(); j++)
            sortlist[j] = trlist[i + sortlist[j]];

        file.readParam(rblock, sortlist.data(), &prm2);

        for (size_t j = 0; j < rblock; j++)
        {
            coords->xSrc[i+orig[j]] = File::getPrm<geom_t>(j, Meta::xSrc, &prm2);
            coords->ySrc[i+orig[j]] = File::getPrm<geom_t>(j, Meta::ySrc, &prm2);
            coords->xRcv[i+orig[j]] = File::getPrm<geom_t>(j, Meta::xRcv, &prm2);
            coords->yRcv[i+orig[j]] = File::getPrm<geom_t>(j, Meta::yRcv, &prm2);
            coords->tn[i+orig[j]] = trlist[i+orig[j]];
        }
    }
    }

    //Any extra readParam calls the particular process needs
    for (size_t i = 0; i < extra; i++)
        file.readParam(0U, nullptr, nullptr);

    cmsg(piol.get(), "Read sets of coordinates from file " + name + " in " + std::to_string(MPI_Wtime()- time) + " seconds");

    return std::move(coords);
}

//TODO: Have a mechanism to change from one Param representation to another?
// This is an output related function and doesn't change the core algorithm.
void outputNonMono(Piol piol, std::string dname, std::string sname, vec<size_t> & list, vec<fourd_t> & minrs)
{
    auto time = MPI_Wtime();
    //Enable as many of the parameters as possible
    auto rule = std::make_shared<File::Rule>(true, true, true);
    //Note: Set to TimeScal for OpenCPS viewing of dataset.
    //OpenCPS is restrictive on what locations can be used
    //as scalars.
    rule->addSEGYFloat(Meta::dsdr, File::Tr::SrcMeas, File::Tr::TimeScal);

    File::ReadDirect src(piol, sname);
    File::WriteDirect dst(piol, dname);

    size_t ns = src.readNs();
    size_t lnt = list.size();
    size_t offset = 0;
    size_t biggest = 0;
    size_t sz = 0;
    {
        auto nts = piol->comm->gather(vec<size_t>{lnt});
        for (size_t i = 0; i < nts.size(); i++)
        {
            if (i == piol->comm->getRank())
                offset = sz;
            sz += nts[i];
            biggest = std::max(biggest, nts[i]);
        }
    }

    size_t memused = lnt * (sizeof(size_t) + sizeof(geom_t));
    size_t memlim = 2U*1024U*1024U*1024U;
    assert(memlim > memused);
    size_t max = (memlim - memused) / (4U*SEGSz::getDOSz(ns) + 4U*rule->extent());
    size_t extra = biggest/max - lnt/max + (biggest % max > 0) - (lnt % max > 0);

    dst.writeText("ExSeisDat 4d-bin file.\n");
    dst.writeNt(sz);
    dst.writeInc(src.readInc());
    dst.writeNs(ns);

    File::Param prm(rule, std::min(lnt, max));
    vec<trace_t> trc(ns * std::min(lnt, max));

    for (size_t i = 0; i < lnt; i += max)
    {
        size_t rblock = (i + max < lnt ? max : lnt - i);
        src.readTraceNonMono(rblock, &list[i], trc.data(), &prm);
        for (size_t j = 0; j < rblock; j++)
            setPrm(j, Meta::dsdr, minrs[i+j], &prm);

        dst.writeTrace(offset+i, rblock, trc.data(), &prm);
    }

    for (size_t i = 0; i < extra; i++)
    {
        src.readTrace(0, nullptr, nullptr, nullptr);
        dst.writeTrace(size_t(0), size_t(0), nullptr, nullptr);
    }

    cmsg(piol.get(), "Output " + sname + " to " + dname + " in " + std::to_string(MPI_Wtime()- time) + " seconds");
}
}}
