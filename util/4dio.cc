/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \date January 2017
 *   \brief
 *   \details
 *//*******************************************************************************************/

#include "4dio.hh"
#include "fileops.hh"   //For sort
namespace PIOL { namespace FOURD {
//This trick is discussed here:
//http://stackoverflow.com/questions/1577475/c-sorting-and-keeping-track-of-indexes
vec<size_t> getSortIndex(size_t sz, size_t * list)
{
    vec<size_t> index(sz);
    std::iota(index.begin(), index.end(), 0);
    std::sort(index.begin(), index.end(), [list] (size_t s1, size_t s2) { return list[s1] < list[s2]; });
    return index;
}

//TODO: Integration candidate
/*! This function extracts the relevant parameters from the file and inserts them into a vector (coords)
 *  \param[in] piol The piol handle, used for MPI collectives.
 *  \param[in] file The input file to access parameters from.
 *  \param[in] offset The offset for the local process to access from
 *  \param[in] coords The vector for storing the parameters. Number of parameters is coords.size()/4
 */
//TODO: Simple IME optimisation: Contig Read all headers, sort, random write all headers to order, IME shuffle, contig read all headers again
std::unique_ptr<Coords> getCoords(ExSeisPIOL * piol, File::Interface * file, std::pair<size_t, size_t> dec)
{
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
    size_t biggest = piol->comm->max(max);
    size_t extra = biggest/max - lnt/max + (biggest % max > 0) - (lnt % max > 0);

    File::Param prm(rule, lnt);
    for (size_t i = 0; i < lnt; i += max)
    {
        size_t rblock = (i + max < lnt ? max : lnt - i);
        file->readParam(offset+i, rblock, &prm, i);

        for (size_t j = 0; j < rblock; j++)
            setPrm(i + j, Meta::gtn, offset + i + j, &prm);
    }

    //Any extra readParam calls the particular process needs
    for (size_t i = 0; i < extra; i++)
        file->readParam(0U, size_t(0), nullptr, 0U);

    cmsg(piol, "getCoords sort");

    auto trlist = File::sort(piol, &prm, [] (const File::Param & e1, const File::Param & e2) -> bool
            {
                return (File::getPrm<geom_t>(0U, Meta::xSrc, &e1) < File::getPrm<geom_t>(0U, Meta::xSrc, &e2) ? true :
                        File::getPrm<geom_t>(0U, Meta::xSrc, &e1) == File::getPrm<geom_t>(0U, Meta::xSrc, &e2) &&
                        File::getPrm<size_t>(0U, Meta::gtn, &e1) < File::getPrm<size_t>(0U, Meta::gtn, &e2));
            }, false);

    cmsg(piol, "getCoords post-sort I/O");

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

        file->readParam(rblock, sortlist.data(), &prm2);

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
        file->readParam(0U, nullptr, nullptr);
    return std::move(coords);
}

//TODO: Have a mechanism to change from one Param representation to another?
// This is an output related function and doesn't change the core algorithm.
void outputNonMono(ExSeisPIOL * piol, std::shared_ptr<File::Rule> rule, File::Direct & dst, File::Direct & src, vec<size_t> & list, vec<fourd_t> & minrs)
{
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

        auto idx = getSortIndex(rblock, &list[i]);
        std::vector<size_t> nodups;
        for (size_t j = 0; j < rblock; j++)
            if (!j || list[idx[j-1]] != list[idx[j]])
                nodups.push_back(list[idx[j]]);

        File::Param sprm(rule, nodups.size());
        vec<trace_t> strc(ns * nodups.size());

        cmsg(piol, "readTrace " + std::to_string(i));
        src.readTrace(nodups.size(), nodups.data(), strc.data(), &sprm);

        cmsg(piol, "TEST " + std::to_string(i));
        size_t n = 0;
        for (size_t j = 0; j < rblock; j++)
        {
            if (!j || list[idx[j-1]] != list[idx[j]])
                n = j;
            cpyPrm(n, &sprm, idx[n], &prm);

            setPrm(j, Meta::dsdr, minrs[j], &prm);
            for (size_t k = 0; k < ns; k++)
                trc[j*ns + k] = strc[idx[n]*ns + k];
        }

        cmsg(piol, "writeTrace " + std::to_string(i));
        dst.writeTrace(offset+i, rblock, trc.data(), &prm);
    }

    for (size_t i = 0; i < extra; i++)
    {
        cmsg(piol, "no readTrace " + std::to_string(i));
        src.readTrace(0, nullptr, nullptr, nullptr);
        cmsg(piol, "NTEST " + std::to_string(i));
        cmsg(piol, "no writeTrace " + std::to_string(i));
        dst.writeTrace(size_t(0), size_t(0), nullptr, nullptr);
    }
}
}}
