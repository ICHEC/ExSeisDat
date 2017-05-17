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
using namespace PIOL;

const geom_t pi = M_PI;

std::pair<size_t, size_t> decompose(ExSeisPIOL * piol, File::ReadInterface * file)
{
    return decompose(file->readNt(), piol->comm->getNumRank(), piol->comm->getRank());
}

template <size_t i, class ... T>
void winCreate(std::vector<MPI_Win> & win, std::vector<std::tuple<T...>> & vec)
{
    typename std::tuple_element<i, std::tuple<T...>>::type type;
    if (i < sizeof...(T))
    {
        win.push_back(0);
        MPI_Win_create(vec.data(), vec.size(), sizeof(type), MPI_INFO_NULL, MPI_COMM_WORLD, &win.back());
        winCreate<i+1U, T...>(win, vec);
    }
}

template <size_t i, class ... T>
void winPut(std::tuple<T...> & val, size_t lrank, size_t offset, std::vector<MPI_Win> win)
{
    typename std::tuple_element<i, std::tuple<T...>>::type type;
    if (i < sizeof...(T))
    {
        MPI_Put(&std::get<i>(val), 1U, MPIType<type>(), lrank, offset, 1U, MPIType<type>(), win[i]);
        winPut<i+1U, T...>(val, lrank, offset, win);
    }
}

template <size_t i, class ... T>
void winGet(std::tuple<T...> & val, size_t lrank, size_t offset, std::vector<MPI_Win> win)
{
    typename std::tuple_element<i, std::tuple<T...>>::type type;
    if (i < sizeof...(T))
    {
        MPI_Get(&std::get<i>(val), 1U, MPIType<type>(), lrank, offset, 1U, MPIType<type>(), win[i]);
        winGet<i+1U, T...>(val, lrank, offset, win);
    }
}

template <class ... T>
class Uniray
{
    ExSeisPIOL * piol;
    size_t sz;
    size_t offset;
    std::vector<size_t> szall;
    std::vector<std::tuple<T...>> vec;
    std::vector<MPI_Win> win;
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
        winCreate<0U, T...>(win, vec);
    }

    ~Uniray(void)
    {
        for (auto w : win)
            MPI_Win_free(&w);
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
            for (size_t j = 0; j < sizeof...(T); j++)
                MPI_Win_lock(MPI_LOCK_SHARED, lrank, MPI_MODE_NOCHECK, win[j]);
            winPut<0, T...>(val, lrank, i-lOffset, win);
            for (size_t j = 0; j < sizeof...(T); j++)
                MPI_Win_unlock(lrank, win[j]);
        }
    }

    std::tuple<T...> get(size_t i)
    {
        if (i > sz)
        {
            std::tuple<T...> empty;
            return empty;
            //return std::make_tuple<T...>();
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
            for (size_t j = 0; j < sizeof...(T); j++)
                MPI_Win_lock(MPI_LOCK_SHARED, lrank, MPI_MODE_NOCHECK, win[j]);
            winGet<0, T...>(val, lrank, i-lOffset, win);
            for (size_t j = 0; j < sizeof...(T); j++)
                MPI_Win_unlock(lrank, win[j]);
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
    std::vector<std::tuple<size_t, llint, llint>> lline;
    for (size_t i = 1; i < prm->size()-1U; i++)
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
    size_t gatherB = File::getPrm<llint>(prm->size()-1U, Meta::il, prm) == File::getPrm<llint>(prm->size()-2U, Meta::il, prm)
                  && File::getPrm<llint>(prm->size()-1U, Meta::xl, prm) == File::getPrm<llint>(prm->size()-2U, Meta::xl, prm);

    size_t numGather = piol->comm->sum(lline.size() - gatherB);
    size_t offset = piol->comm->offset(lline.size() - gatherB);

    Uniray<size_t, llint, llint> line(piol, numGather);
    for (size_t i = 0; i < lline.size() - gatherB; i++)
            line.set(i + offset, lline[i]);

    std::cout << numGather << " il/xl gathers have been detected\n";

    return line;
}

int main(int argc, char ** argv)
{
    ExSeis piol;
    size_t rank = piol.getRank();
    size_t numRank = piol.getNumRank();

    std::string rname = "radon.segy";
    std::string vname = "vm.segy";
    std::string dname = "dname.segy";
    std::string aname = "aname.segy";

    //Assume the files are structured as 3d volumes.
    File::ReadDirect radon(piol, rname);
    File::ReadDirect vm(piol, vname);
    File::ReadDirect dip(piol, dname);
    File::WriteDirect angle(piol, aname);
    piol.isErr();

    //Locate gather boundaries.
    auto dec = decompose(piol, radon);
    dec.second += (piol.getRank() < piol.getNumRank()-1U);   //Each process should read an overlapping entry, except last

    auto rule = std::make_shared<File::Rule>(std::initializer_list<Meta>{Meta::il, Meta::xl});
    File::Param rprm(rule, dec.second);
    radon.readParam(dec.first, dec.second, &rprm);

    Uniray<size_t, llint, llint> gather = getGathers(piol, &rprm);

    auto gdec = decompose(gather.size(), numRank, rank);
    size_t numGather = gdec.second;
    size_t gOffset = gdec.first;

    size_t aNs; //Input
    size_t agSz;
    size_t vNs = vm->readNs();

    std::vector<trace_t> vtrc(numGather * vNs);
    vm.readTrace(gOffset, numGather, vtrc.data());

    size_t rNs = radon->readNs();

    geom_t vinc = vm->readInc();
    geom_t rinc = radon->readInc();
    geom_t ainc = pi / geom_t(180.0);   //1 degree

    size_t lOffset = gOffset;
    size_t extra = piol.max(numGather) - numGather;

    for (size_t i = 0; i < numGather; i++)
    {
        const size_t rgSz = std::get<0>(gather[gOffset + i]);
        std::vector<trace_t> rtrc(rgSz * rNs);
        std::vector<trace_t> atrc(agSz * aNs);

        radon.readTrace(lOffset, rgSz, rtrc.data());

        for (size_t j = 0; j < agSz; j++)       //For each angle in the angle gather
            for (size_t z = 0; z < rNs; z++)    //For each sample (angle + radon)
            {
                geom_t vmModel = vtrc[i * vNs + std::min(size_t(geom_t(z * rinc) / vinc), vNs)];
                llint k = llround(vmModel / (cos(j * ainc) * geom_t(rgSz)));
                if (k > 0 && k < rgSz)
                    atrc[j * agSz + z] = rtrc[k * rgSz + z];
            }

        auto rule = std::make_shared<File::Rule>(true, true);

        File::Param aprm(rule, agSz);
        for (size_t j = 0; j < agSz; j++)
        {
            //TODO: Set the rest of the parameters
            //TODO: Check the get numbers
            File::setPrm(j, Meta::il, std::get<1>(gather[i]), &aprm);
            File::setPrm(j, Meta::xl, std::get<2>(gather[i]), &aprm);
        }

        angle->writeTrace(lOffset, agSz, atrc.data(), &aprm);

        lOffset += rgSz;
    }

    for (size_t j = 0; j < extra; j++)
    {
        radon->readTrace(size_t(0), size_t(0), (trace_t *)(nullptr));
        angle->writeTrace(size_t(0), size_t(0), (trace_t *)(nullptr), (File::Param *)(nullptr));
    }
}

