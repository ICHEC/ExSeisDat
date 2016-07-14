#ifndef PIOLPARALLEL_INCLUDE_GUARD
#define PIOLPARALLEL_INCLUDE_GUARD
namespace parallel
{
template <typename T> std::pair<T, T> distrib(T rank, T nprocs, T workUnits)
{
    T DivR = workUnits%nprocs;
    T DivNum = workUnits/nprocs;
    T DivStart = DivNum * rank + std::min(rank, DivR);
    DivNum = std::min(workUnits - DivStart, DivNum + (rank < DivR));

    std::pair<T, T> DivLab(DivStart, DivStart + DivNum);
    return DivLab;
}

//Divide work into 4k blocks
//TODO: Continue
/*template <typename T, const size_t bsz> std::pair<T, T> blockDistrib(T rank, T nprocs, T workUnits)
{
    T blockNum = workUnits % bsz;
    T blockRm = workUnits / bsz;
    size_t extra = w

    std::pair<T, T> DivLab = distrib(rank, nprocs, blockNum);

    return DivLab;
}*/

}
#endif

