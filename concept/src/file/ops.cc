#include <cmath>
#include "file/ops.hh"
namespace PIOL { namespace Op {

template <typename T, typename U>
T reduction(Comms::MPI & comm, const U & obj1, const std::vector<U> & vec, std::function<const T & (const T &, const T &)> reducOp, std::function<T(const U &, const U &)> eval)
{
    real val = 0.0;
    for (const U obj2 : vec)
        val = reducOp(val, eval(obj1, obj2));
    
    return comm.reduce(val, reducOp);
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
}}
