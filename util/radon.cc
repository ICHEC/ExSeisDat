#define _USE_MATH_DEFINES/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \date May 2017
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include <tuple>
#define USE_MATH_DEFINES
#include <cmath>
#include "sglobal.hh"
#include "cppfileapi.hh"
#include "share/mpi.hh"
#include "file/dynsegymd.hh"

#warning include
#include <iostream>
#include <assert.h>

using namespace PIOL;

const geom_t pi = M_PI;

std::pair<size_t, size_t> decompose(ExSeisPIOL * piol, File::ReadInterface * file)
{
    return decompose(file->readNt(), piol->comm->getNumRank(), piol->comm->getRank());
}

template <class ... T>
class Uniray
{
    const size_t TupleSz = sizeof(std::tuple<T...>);
    ExSeisPIOL * piol;
    size_t sz;
    size_t offset;
    std::vector<size_t> szall;
    std::vector<std::tuple<T...>> vec;

    MPI_Win win;
    size_t rank, numRank;
    public :
    Uniray(ExSeisPIOL * piol_, size_t sz_) : piol(piol_), sz(sz_)
    {
        rank = piol->comm->getRank();
        numRank = piol->comm->getNumRank();
        auto dec = decompose(sz, numRank, rank);
        offset = dec.first;
        szall = piol->comm->gather(dec.second);
        vec.resize(dec.second);
        MPI_Win_create(vec.data(), vec.size(), TupleSz, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    }

    ~Uniray(void)
    {
        MPI_Win_free(&win);
    }

    void set(size_t i, std::tuple<T...> val)
    {
        if (i > sz)
            return;

        size_t lrank = 0;
        size_t lOffset = 0;
        for (lrank = 0; i >= lOffset + szall[lrank] && lrank < numRank; lrank++)
            lOffset += szall[lrank];

        if (lrank == rank)
            vec[i-lOffset] = val;
        else
        {
            MPI_Win_lock(MPI_LOCK_EXCLUSIVE, lrank, MPI_MODE_NOCHECK, win);
            MPI_Put(&val, TupleSz, MPIType<char>(), lrank, i-lOffset, TupleSz, MPIType<char>(), win);
            MPI_Win_unlock(lrank, win);
        }
    }

    std::tuple<T...> get(size_t i)
    {
        if (i > sz)
        {
            std::tuple<T...> empty;
            return empty;
        }
        size_t lrank = 0;
        size_t lOffset = 0;
        for (lrank = 0; i >= lOffset + szall[lrank] && lrank < numRank; lrank++)
            lOffset += szall[lrank];

        std::tuple<T...> val;
        if (lrank == rank)
            return vec[i-lOffset];
        else
        {
            MPI_Win_lock(MPI_LOCK_SHARED, lrank, MPI_MODE_NOCHECK, win);
            MPI_Get(&val, TupleSz, MPIType<char>(), lrank, i-lOffset, TupleSz, MPIType<char>(), win);
            MPI_Win_unlock(lrank, win);
            return val;
        }
    }

    std::tuple<T...> operator[](size_t i)
    {
        return get(i);
    }

    size_t size(void)
    {
        return sz;
    }
};

Uniray<size_t, llint, llint> getGathers(ExSeisPIOL * piol, File::Param * prm)
{
    size_t rank = piol->comm->getRank();
    size_t last = rank == piol->comm->getNumRank()-1;
    std::vector<std::tuple<size_t, llint, llint>> lline;
    lline.emplace_back(0U, File::getPrm<llint>(0, Meta::il, prm), File::getPrm<llint>(0, Meta::xl, prm));
    ++std::get<0>(lline.back());

    for (size_t i = 1; i < prm->size()-(last ? 0U : 1U); i++)
    {
        llint il = File::getPrm<llint>(i, Meta::il, prm);
        llint xl = File::getPrm<llint>(i, Meta::xl, prm);

        if (il != File::getPrm<llint>(i-1U, Meta::il, prm) ||
            xl != File::getPrm<llint>(i-1U, Meta::xl, prm))
            lline.emplace_back(0U, il, xl);
        ++std::get<0>(lline.back());
    }

    //If the last element is on the same gather, then the gather has already been picked up
    //by the process one rank higher.
    size_t gatherB = !last
           && File::getPrm<llint>(prm->size()-1U, Meta::il, prm) == File::getPrm<llint>(prm->size()-2U, Meta::il, prm)
           && File::getPrm<llint>(prm->size()-1U, Meta::xl, prm) == File::getPrm<llint>(prm->size()-2U, Meta::xl, prm);

    std::cout << "gatherB " << gatherB << std::endl;
    size_t lSz = lline.size() - gatherB;
    size_t offset = piol->comm->offset(lSz);

    //Nearest neighbour pass would be more appropriate
    auto left = piol->comm->gather<size_t>(gatherB ? std::get<0>(lline.back()) : 0U);
    std::get<0>(lline.front()) += (rank ? left[rank-1] : 0U);

    Uniray<size_t, llint, llint> line(piol, piol->comm->sum(lSz));
    for (size_t i = 0; i < lSz; i++)
        line.set(i + offset, lline[i]);

    std::cout << "NumGathers " << line.size() << std::endl;

    piol->comm->barrier();
    return line;
}

Uniray<size_t, llint, llint> getIlXlGathers(ExSeis piol, File::ReadInterface * radon)
{
    auto dec = decompose(piol, radon);
    dec.second += (piol.getRank() < piol.getNumRank()-1U);   //Each process should read an overlapping entry, except last

    auto rule = std::make_shared<File::Rule>(std::initializer_list<Meta>{Meta::il, Meta::xl});
    File::Param rprm(rule, dec.second);
    radon->readParam(dec.first, dec.second, &rprm);

    return getGathers(piol, &rprm);
}

std::vector<trace_t> readTrace(File::ReadInterface * vm, size_t gOffset, size_t numGather, Uniray<size_t, llint, llint> & gather)
{
    std::vector<trace_t> vtrc(numGather * vm->readNs());
    std::vector<size_t> vlist = {0U, 1U, vm->readNt() - 1U};
    File::Param vprm(vlist.size());
    vm->readParam(vlist.size(), vlist.data(), &vprm);

    size_t ilInc = File::getPrm<llint>(1U, Meta::il, &vprm) - File::getPrm<llint>(0U, Meta::il, &vprm);
    size_t ilWid = (ilInc ? File::getPrm<llint>(2U, Meta::il, &vprm) / ilInc : 0U);
    size_t xlInc = File::getPrm<llint>(2U, Meta::xl, &vprm) / (vm->readNt() / (ilWid ? ilWid : 1U));
    size_t xlWid = (xlInc ? vm->readNt() / (ilWid ? ilWid : 1U) / xlInc : 0U);

    ilInc = (ilInc ? ilInc : 1U);
    xlInc = (xlInc ? xlInc : 1U);

    std::vector<size_t> offset(numGather);
    for (size_t i = 0; i < numGather; i++)
    {
        auto val = gather[gOffset + i];
        const size_t il = std::get<1>(val);
        const size_t xl = std::get<2>(val);
        offset[i] = ((il - File::getPrm<llint>(0U, Meta::il, &vprm)) / ilInc) * xlWid + ((xl - File::getPrm<llint>(0U, Meta::xl, &vprm)) / xlInc);
    }

    vm->readTrace(offset.size(), offset.data(), vtrc.data());
    return vtrc;
}

int main(int argc, char ** argv)
{
    const size_t aGSz = 60;
    const size_t vBin = 20;

    ExSeis piol;
    size_t rank = piol.getRank();
    size_t numRank = piol.getNumRank();

    std::string rname = "radon.segy";
    std::string vname = "vm.segy";
    std::string aname = "angle.segy";

    //Assume the files are structured as 3d volumes.
    File::ReadDirect radon(piol, rname);
    File::ReadDirect vm(piol, vname);
    piol.isErr();

    //Locate gather boundaries.
    auto gather = getIlXlGathers(piol, radon);

    auto gdec = decompose(gather.size(), numRank, rank);
    size_t numGather = gdec.second;
    size_t gOffset = gdec.first;
    size_t vNs = vm->readNs();

    std::vector<trace_t> vtrc = readTrace(vm, gOffset, numGather, gather);

    size_t rNs = radon->readNs();
    size_t aNs = rNs; //TODO: Check what gareth does

    geom_t vInc = vm->readInc();
    geom_t rInc = radon->readInc();
    geom_t aInc = pi / geom_t(180);   //1 degree in radians

    size_t lOffset = 0U;
    size_t extra = piol.max(numGather) - numGather;
    File::WriteDirect angle(piol, aname);
    angle.writeNs(aNs);
    angle.writeNt(aGSz * numGather);
    angle.writeInc(aInc);

    std::cout << "Reach numGather loop" << std::endl;
    for (size_t i = 0; i < gOffset; i++)
        lOffset += std::get<0>(gather[i]);

    for (size_t i = 0; i < numGather; i++)
    {
        auto gval = gather[gOffset + i];
        const size_t rGSz = std::get<0>(gval);
        const size_t aOffset = aGSz * (i + gOffset);
        std::vector<trace_t> rtrc(rGSz * rNs);
        std::vector<trace_t> atrc(aGSz * aNs);

        radon.readTrace(lOffset, rGSz, rtrc.data());

        for (size_t j = 0; j < aGSz; j++)       //For each angle in the angle gather
            for (size_t z = 0; z < rNs; z++)    //For each sample (angle + radon)
            {
                geom_t vmModel = vtrc[i * vNs + std::min(size_t(geom_t(z * rInc) / vInc), vNs)];
                //llint k = llround(vmModel / (cos(j * aInc) * geom_t(vBin)));
                llint k = llround(vmModel / cos(j * aInc)) / vBin;
                if (k > 0 && k < rGSz)
                    atrc[j * aNs + z] = rtrc[k * rNs + z];
            }

        auto rule = std::make_shared<File::Rule>(std::initializer_list<Meta>{Meta::il, Meta::xl});
        File::Param aprm(rule, aGSz);
        for (size_t j = 0; j < aGSz; j++)
        {
            //TODO: Set the rest of the parameters
            //TODO: Check the get numbers
            File::setPrm(j, Meta::il, std::get<1>(gval), &aprm);
            File::setPrm(j, Meta::xl, std::get<2>(gval), &aprm);
        }

        angle->writeTrace(aOffset, aGSz, atrc.data(), &aprm);
        lOffset += rGSz;
    }

    for (size_t j = 0; j < extra; j++)
    {
        radon->readTrace(size_t(0), size_t(0), (trace_t *)(nullptr));
        angle->writeTrace(size_t(0), size_t(0), (trace_t *)(nullptr), (File::Param *)(nullptr));
    }

    return 0;
}

