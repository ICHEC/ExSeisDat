#ifndef PIOLPARALLEL_INCLUDE_GUARD 
#define PIOLPARALLEL_INCLUDE_GUARD
namespace parallel
{
template <typename T> std::pair<T, T> distrib(T Rank, T NProcs, T WorkUnits)
{
    T DivR = WorkUnits%NProcs;
    T DivNum = WorkUnits/NProcs;
    T DivStart = DivNum * Rank + std::min(Rank, DivR);
    DivNum = std::min(WorkUnits - DivStart, DivNum + (Rank < DivR));

    std::pair<T, T> DivLab(DivStart, DivStart + DivNum);
    return DivLab;
}

}
#endif

