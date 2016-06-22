#include <cmath>
#include "file/ops.hh"
namespace PIOL { namespace Op {


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

real calcDistance(const File::CoordData & point1, const File::CoordData & point2)
{
    return std::hypot(point1.first - point2.first,
                      point1.second - point2.second);
}

real calcMax(Comms::MPI & comm, const File::CoordData & point1, const std::vector<File::CoordData> & coords)
{
    return reduction<real, File::CoordData>(comm, point1, coords, std::max<real>, calcDistance);
}

real calcMin(Comms::MPI & comm, const File::CoordData & point1, const std::vector<File::CoordData> & coords)
{
    return reduction<real, File::CoordData>(comm, point1, coords, std::min<real>, calcDistance);
}

size_t findMax(Comms::MPI & comm, size_t offset, const File::CoordData & point1, const std::vector<File::CoordData> & coords)
{
    return find<real, File::CoordData>(comm, offset, point1, coords, std::max<real>, calcDistance);
}

size_t findMin(Comms::MPI & comm, size_t offset, const File::CoordData & point1, const std::vector<File::CoordData> & coords)
{
    return find<real, File::CoordData>(comm, offset, point1, coords, std::min<real>, calcDistance);
}
}}

