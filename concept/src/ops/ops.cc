#include <algorithm>
#include <functional>
#include <cmath>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "ops/ops.hh"
#include "comm/mpi.hh"
#include "file/file.hh"

//Temporary
#include "set/man.hh"
//TODO: Make this unnecessary
namespace PIOL { namespace Data {
void printErr(int err, MPI_Status * stat, std::string msg);
}}

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
template <typename T>
T calcXSize(const File::Pair<T> & pair1, const File::Pair<T> & pair2)
{
    return pair2.first - pair1.first;
}
template <typename T>
const T & min(const T & a, const T & b)
{
    return std::min<T>(a, b);
}
template <typename T>
const T & max(const T & a, const T & b)
{
    return std::max<T>(a, b);
}

template <typename T> inline
T calcMin(Comms::MPI & comm, const File::Pair<T> & pair1, const std::vector<File::Pair<T> > & pairs)
{
    return reduction<T, File::Pair<T> >(comm, pair1, pairs, min<T>, calcDistance<T>);
}

template <typename T> inline
T calcMax(Comms::MPI & comm, const File::Pair<T> & pair1, const std::vector<File::Pair<T> > & pairs)
{
    return reduction<T, File::Pair<T> >(comm, pair1, pairs, max<T>, calcDistance<T>);
}

template <typename T> inline
size_t findMax(Comms::MPI & comm, size_t offset, const File::Pair<T> & pair1, const std::vector<File::Pair<T> > & pairs)
{
    return find<T, File::Pair<T> >(comm, offset, pair1, pairs, max<T>, calcDistance<T>);
}

template <typename T> inline
size_t findMin(Comms::MPI & comm, size_t offset, const File::Pair<T> & pair1, const std::vector<File::Pair<T> > & pairs)
{
    return find<T, File::Pair<T> >(comm, offset, pair1, pairs, min<T>, calcDistance<T>);
}
/////////////////////////////////
namespace Ops {
coreal calcMin(Comms::MPI & comm, const File::CoordData & point1, const std::vector<File::CoordData> & coords)
{
    return PIOL::calcMin<coreal>(comm, point1, coords);
}
llint calcMin(Comms::MPI & comm, const File::GridData & point1, const std::vector<File::GridData> & grids)
{
    return PIOL::calcMin<llint>(comm, point1, grids);
}
coreal calcMax(Comms::MPI & comm, const File::CoordData & point1, const std::vector<File::CoordData> & coords)
{
    return PIOL::calcMax<coreal>(comm, point1, coords);
}
llint calcMax(Comms::MPI & comm, const File::GridData & point1, const std::vector<File::GridData> & grids)
{
    return PIOL::calcMax<llint>(comm, point1, grids);
}
size_t findMin(Comms::MPI & comm, size_t offset, const File::CoordData & point1, const std::vector<File::CoordData> & coords)
{
    return PIOL::findMin<coreal>(comm, offset, point1, coords);
}
size_t findMin(Comms::MPI & comm, size_t offset, const File::GridData & point1, const std::vector<File::GridData> & grids)
{
    return PIOL::findMin<llint>(comm, offset, point1, grids);
}
size_t findMax(Comms::MPI & comm, size_t offset, const File::CoordData & point1, const std::vector<File::CoordData> & coords)
{
    return PIOL::findMax<coreal>(comm, offset, point1, coords);
}
size_t findMax(Comms::MPI & comm, size_t offset, const File::GridData & point1, const std::vector<File::GridData> & grids)
{
    return PIOL::findMax<llint>(comm, offset, point1, grids);
}

std::string getOrd(size_t i)
{
    i = i % 10;
    switch (i)
    {
        case 1 :
            return "st";
        case 2 :
            return "nd";
        case 3 :
            return "rd";
        default :
            return "th";
    }
}

void Print(Comms::MPI & comm, MPI_File file, std::string msg)
{
    if (!comm.getRank())
    {
        int err = MPI_File_seek(file, 0, MPI_SEEK_END);
        std::stringstream out;
        out << msg;
        MPI_Status stat;
        err = MPI_File_write(file, out.str().data(), out.str().size(), MPI_CHAR, &stat);
        Data::printErr(err, &stat, "Failure to write log file");
    }
    MPI_File_sync(file);
}
void Print(Comms::MPI & comm, std::string msg)
{
    MPI_Barrier(MPI_COMM_WORLD);
    if (!comm.getRank())
    {
        std::cout << msg << std::endl;
    }
}
bool compareTraceVals(const std::pair<size_t, coreal> & arg1, const std::pair<size_t, coreal> & arg2)
{
    return arg1.second > arg2.second;
}
template <typename T>
void borderExchange(Comms::MPI & comm, size_t regionSz, std::vector<std::pair<size_t, T> > & tSort1, std::vector<std::pair<size_t, T> > & tSort2)
{
    //This can be optimised later
    std::copy(tSort2.begin(), tSort2.end(), tSort1.begin());

    std::vector<MPI_Request> msg;
    if (comm.getRank())
    {
        msg.push_back(MPI_REQUEST_NULL);
        MPI_Isend(tSort2.data(), regionSz * sizeof(std::pair<size_t, T>), MPI_CHAR,
                  comm.getRank()-1, 0, MPI_COMM_WORLD, &msg.back());
        msg.push_back(MPI_REQUEST_NULL);
        MPI_Irecv(tSort1.data(), regionSz * sizeof(std::pair<size_t, T>), MPI_CHAR,
                  comm.getRank()-1, 1, MPI_COMM_WORLD, &msg.back());
    }
    if (comm.getRank() != comm.getNumRank()-1)
    {
        msg.push_back(MPI_REQUEST_NULL);
        MPI_Isend(&tSort2.data()[tSort2.size()-regionSz], regionSz * sizeof(std::pair<size_t, T>),
                  MPI_CHAR, comm.getRank()+1, 1, MPI_COMM_WORLD, &msg.back());
        msg.push_back(MPI_REQUEST_NULL);
        MPI_Irecv(&tSort1.data()[tSort2.size()-regionSz], regionSz * sizeof(std::pair<size_t, T>),
                  MPI_CHAR, comm.getRank()+1, 0, MPI_COMM_WORLD, &msg.back());
    }

    MPI_Status stat;
    for (size_t i = 0; i < msg.size(); i++)
    {
        assert(msg[i] != MPI_REQUEST_NULL);
        int err = MPI_Wait(&msg[i], &stat);

        if (err != MPI_SUCCESS)
        {
            std::cerr << "Wait " << i << std::endl;
            std::cerr << " MPI Error " << stat.MPI_ERROR << std::endl;
            std::exit(-1);
        }
    }
}
//bool compareCMPDistance(const File::TraceHeader & arg1, const File::TraceHeader & arg2)
std::vector<size_t> Sort(Comms::MPI & comm, size_t offset, size_t nt, std::vector<File::TraceHeader> & thead)
{
    typedef llint T;
    size_t regionSz = std::min(nt / (comm.getNumRank() * 2U), std::numeric_limits<int>::max() / sizeof(std::pair<size_t, T>));

//TODO: Separate log class
    MPI_File file;
    int Err = MPI_File_open(MPI_COMM_WORLD, "test.dat", MPI_MODE_CREATE | MPI_MODE_UNIQUE_OPEN | MPI_MODE_WRONLY | MPI_MODE_CREATE , MPI_INFO_NULL, &file);


//For checking purposes
    size_t gridNum = static_cast<size_t>(File::Grid::Lin);
    File::Pair<llint> zerop = std::make_pair(0.0, 0.0);
    //size_t coordNum = static_cast<size_t>(File::Coord::Cmp);
    //File::Pair<coreal> zerop = std::make_pair(0.0, 0.0);

    //Naive sort

    std::vector<std::pair<size_t, T> > tSort1(thead.size());
    std::vector<std::pair<size_t, T> > tSort2(thead.size());
    std::vector<std::pair<size_t, T> > tSort3(thead.size());

    for (size_t i = 0; i < tSort1.size(); i++)
    {
        tSort1[i].first = offset + i;
        tSort1[i].second = calcXSize(zerop, thead[i].grids[gridNum]);
    }

    if (!comm.getRank())
    {
        for (size_t i = 0; i < 10; i++)
            std::cout << " Test " << tSort1[i].first << " " << tSort1[i].second << " " << thead[i].grids[gridNum].first << std::endl;
    }

    std::sort(tSort1.begin(), tSort1.end(), compareTraceVals);

    double time = 0.0;
    //This is the maximum amount of sorting we need to do for information to
    //travel from one end to another
    for (size_t i = 0; i < comm.getNumRank(); i++)
    {
        double startTime = MPI_Wtime();
        std::copy(tSort1.begin(), tSort1.end(), tSort2.begin());

        borderExchange(comm, regionSz, tSort1, tSort2);

        std::sort(tSort1.begin(), tSort1.end(), compareTraceVals);

        borderExchange(comm, regionSz, tSort3, tSort1);

        int reduced = (tSort2 != tSort3);
        int greduced;
        int err = MPI_Allreduce(&reduced, &greduced, 1, MPI_INT, MPI_SUM, comm.getComm());
        if (!greduced)
            break;

        {
            std::stringstream out;
            out << "\n\nProgress " << int(double(i)/double(comm.getNumRank()-1)*100.0) << "%\n";

            time += MPI_Wtime()-startTime;
            out << "Time so far: " << time << std::endl;

            Print(comm, file, out.str());
        }
    }

    {
        std::stringstream out;
        out << "\n\nProgress 100%\n";
        Print(comm, file, out.str());
    }

    for (size_t i = 0; i < comm.getNumRank(); i++)
    {
        T prev = T(0);
        if (i == comm.getRank())
        {
            for (size_t j = 0; j < tSort1.size(); j++)
            {
                if (tSort2[j].second != prev)
                {
                    std::cout << "rank "<< i << " offset " << offset << " inline " << tSort2[j].second << std::endl;
                    prev = tSort2[j].second;
                }
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    std::vector<size_t> traceNum(thead.size());
    for (size_t i = 0; i < tSort2.size(); i++)
        traceNum[i] = tSort2[i].first;

    return traceNum;
}

void Subset(Comms::MPI & comm, std::vector<File::TraceHeader> & thead)
{

}


}}

