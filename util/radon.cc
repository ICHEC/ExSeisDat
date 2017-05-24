/*******************************************************************************************//*!
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

#include "share/decomp.hh"
#include "ops/gather.hh"

#warning include
#include <iostream>
#include <assert.h>
#include "file/filesegy.hh"

using namespace PIOL;

const geom_t pi = M_PI;

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
    File::ReadModel vm(piol, vname);
    piol.isErr();

    //Locate gather boundaries.
    auto gather = File::getIlXlGathers(piol, radon);

    auto gdec = decompose(gather.size(), numRank, rank);
    size_t numGather = gdec.second;
    size_t gOffset = gdec.first;
    size_t vNs = vm->readNs();

    std::vector<trace_t> vtrc = vm.readModel(gOffset, numGather, gather);

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
                //We are using coordinate level accuracy when its not performance critical.
                geom_t vmModel = vtrc[i * vNs + std::min(size_t(geom_t(z * rInc) / vInc), vNs)];
                llint k = llround(vmModel / cos(geom_t(j * aInc))) / vBin;
                if (k > 0 && k < rGSz)
                    atrc[j * aNs + z] += rtrc[k * rNs + z];
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

