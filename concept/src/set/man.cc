#include "set/man.hh"
#include <vector>
#include "comm/comm.hh"
#include "file/file.hh"

extern void ibm2ieee( void *to, const void *from, long long);

namespace PIOL { namespace Set {
template <typename T> std::pair<T, T> distrib(T Rank, T NProcs, T WorkUnits)
{
    T DivR = WorkUnits%NProcs;
    T DivNum = WorkUnits/NProcs;
    T DivStart = DivNum * Rank + std::min(Rank, DivR);
    DivNum = std::min(WorkUnits - DivStart, DivNum + (Rank < DivR));

    std::pair<T, T> DivLab(DivStart, DivStart + DivNum);
    return DivLab;
}

std::pair<size_t, size_t> Manager::decompose(size_t num)
{
    auto decomp = distrib<size_t>(comm->getRank(), comm->getNumRank(), num);
    //TODO: test decomp.second > decomp.first

    return std::make_pair(decomp.first, decomp.second-decomp.first);
}

CoordData Manager::readCoord(File::Coord Type)
{
    auto decomp = decompose(file->readNt());
    std::vector<File::CoordData> coord(decomp.second);
    file->readCoord(decomp.first, Type, coord);
    return std::make_pair(decomp.first, coord);
}
CoordArray Manager::readCoord(void)
{
    auto decomp = decompose(file->readNt());

    std::vector<File::CoordArray> allCoords(decomp.second);
    file->readCoord(decomp.first, allCoords);
    return std::make_pair(decomp.first, allCoords);
}

//TODO: Double-check about data copy cost
TraceData Manager::readTraces(void)
{
    auto decomp = decompose(file->readNt());
    std::vector<real> data(decomp.second * file->readNs());
    file->readTraces(0, data);
//TODO: omp target
    for (size_t i = 0; i < file->readNs(); i++)
    {
        for (size_t j = 0; j < data.size() / file->readNs(); j++)
        {
            ibm2ieee(&data[j*file->readNs() + i], &data[j*file->readNs() + i], 1);
        }
    }
    return std::make_pair(decomp.first, data);
}
File::Header Manager::readHeader(void)
{
    File::Header header;
    file->readHeader(header);
    return header;
}
}}
