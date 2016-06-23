#include <cmath>
#include "comm/mpi.hh"
#include "ops/ops.hh"
namespace PIOL {
template <typename T, typename U>
T reduction(Comms::MPI & comm, const U & obj1, const std::vector<U> & vec, std::function<const T & (const T &, const T &)> reducOp, std::function<T(const U &, const U &)> eval)
{
    if (!vec.size())
        return T(0);

    T val = eval(obj1, vec[0]);
    for (size_t i = 1; i < vec.size(); i++)
        val = reducOp(val, eval(obj1, vec[i]));

    return comm.reduce(val, reducOp);
}

template <typename T, typename U>
T find(Comms::MPI & comm, size_t offset, const U & obj1, const std::vector<U> & vec, std::function<const T & (const T &, const T &)> reducOp, std::function<T(const U &, const U &)> eval)
{
    if (!vec.size())
        return T(0);

    size_t num = 0;
    T val = eval(obj1, vec[0]);
    for (size_t i = 1; i < vec.size(); i++)
    {
        T newVal = reducOp(val, eval(obj1, vec[i]));
        if (newVal != val)
        {
            val = newVal;
            num = i;
        }
    }

    return comm.reduce(val, offset, num, reducOp);
}
template <typename T>
T calcDistance(const File::Pair<T> & pair1, const File::Pair<T> & pair2)
{
    return std::hypot(pair1.first - pair2.first,
                      pair1.second - pair2.second);
}

template <typename T> inline
T calcMax(Comms::MPI & comm, const File::Pair<T> & pair1, const std::vector<File::Pair<T> > & pairs)
{
    return reduction<T, File::Pair<T> >(comm, pair1, pairs, std::max<T>, calcDistance<T>);
}

template <typename T> inline
T calcMin(Comms::MPI & comm, const File::Pair<T> & pair1, const std::vector<File::Pair<T> > & pairs)
{
    return reduction<T, File::Pair<T> >(comm, pair1, pairs, std::min<T>, calcDistance<T>);
}

template <typename T> inline
size_t findMax(Comms::MPI & comm, size_t offset, const File::Pair<T> & pair1, const std::vector<File::Pair<T> > & pairs)
{
    return find<T,File::Pair<T> >(comm, offset, pair1, pairs, std::max<T>, calcDistance<T>);
}

template <typename T> inline
size_t findMin(Comms::MPI & comm, size_t offset, const File::Pair<T> & pair1, const std::vector<File::Pair<T> > & pairs)
{
    return find<T,File::Pair<T> >(comm, offset, pair1, pairs, std::max<T>, calcDistance<T>);
}
/////////////////////////////////
namespace Ops {
coreal Ops::calcMin(Comms::MPI & comm, const File::CoordData & point1, const std::vector<File::CoordData> & coords)
{
    return PIOL::calcMin<coreal>(comm, point1, coords);
}
llint Ops::calcMin(Comms::MPI & comm, const File::GridData & point1, const std::vector<File::GridData> & grids)
{
    return PIOL::calcMin<llint>(comm, point1, grids);
}

coreal Ops::calcMax(Comms::MPI & comm, const File::CoordData & point1, const std::vector<File::CoordData> & coords)
{
    return PIOL::calcMax<coreal>(comm, point1, coords);
}
llint Ops::calcMax(Comms::MPI & comm, const File::GridData & point1, const std::vector<File::GridData> & grids)
{
    return PIOL::calcMax<llint>(comm, point1, grids);
}
size_t Ops::findMin(Comms::MPI & comm, size_t offset, const File::CoordData & point1, const std::vector<File::CoordData> & coords)
{
    return PIOL::findMin<coreal>(comm, offset, point1, coords);
}
size_t Ops::findMin(Comms::MPI & comm, size_t offset, const File::GridData & point1, const std::vector<File::GridData> & grids)
{
    return PIOL::findMin<llint>(comm, offset, point1, grids);
}
size_t Ops::findMax(Comms::MPI & comm, size_t offset, const File::CoordData & point1, const std::vector<File::CoordData> & coords)
{
    return PIOL::findMax<coreal>(comm, offset, point1, coords);
}
size_t Ops::findMax(Comms::MPI & comm, size_t offset, const File::GridData & point1, const std::vector<File::GridData> & grids)
{
    return PIOL::findMax<llint>(comm, offset, point1, grids);
}
}}

