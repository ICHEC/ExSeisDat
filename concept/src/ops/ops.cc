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
namespace PIOL { namespace Block {
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

bool compareCMPDistance(const File::TraceHeader & arg1, const File::TraceHeader & arg2)
{
    size_t coordNum = static_cast<size_t>(File::Coord::Cmp);
    File::Pair<coreal> zerop = std::make_pair(0.0, 0.0);
    return (calcDistance(zerop, arg1.coords[coordNum]) > calcDistance(zerop, arg2.coords[coordNum]));
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

bool compareTraceVals(const std::pair<size_t, coreal> & arg1, const std::pair<size_t, coreal> & arg2)
{
    return arg1.second > arg2.second;
}

void borderExchange(Comms::MPI & comm, size_t regionSz, std::vector<std::pair<size_t, double> > tSort1, std::vector<std::pair<size_t, double> > tSort2)
{
    std::vector<MPI_Request> msg;
    if (comm.getRank())
    {
        msg.push_back(MPI_REQUEST_NULL);
        MPI_Isend(tSort2.data(), regionSz * sizeof(std::pair<size_t, double>), MPI_CHAR,
                  comm.getRank()-1, 0, MPI_COMM_WORLD, &msg.back());
        msg.push_back(MPI_REQUEST_NULL);
        MPI_Irecv(tSort1.data(), regionSz * sizeof(std::pair<size_t, double>), MPI_CHAR,
                  comm.getRank()-1, 1, MPI_COMM_WORLD, &msg.back());
    }

    if (comm.getRank() != comm.getNumRank()-1)
    {
        msg.push_back(MPI_REQUEST_NULL);
        MPI_Isend(&tSort1.data()[tSort2.size()-regionSz], regionSz * sizeof(std::pair<size_t, double>),
                  MPI_CHAR, comm.getRank()+1, 1, MPI_COMM_WORLD, &msg.back());
        msg.push_back(MPI_REQUEST_NULL);
        MPI_Irecv(&tSort1.data()[tSort1.size()-regionSz], regionSz * sizeof(std::pair<size_t, double>),
                  MPI_CHAR, comm.getRank()+1, 0, MPI_COMM_WORLD, &msg.back());
    }

    MPI_Status stat;
    for (size_t i = 0; i < msg.size(); i++)
    {
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
void Sort(Comms::MPI & comm, size_t nt, std::vector<File::TraceHeader> & thead)
{
    size_t regionSz = nt / (comm.getNumRank() * 2U);

//TODO: Separate log class
    MPI_File file;
    int Err = MPI_File_open(MPI_COMM_WORLD, "testi.dat", MPI_MODE_CREATE | MPI_MODE_UNIQUE_OPEN | MPI_MODE_WRONLY | MPI_MODE_CREATE , MPI_INFO_NULL, &file);


//For checking purposes
    size_t coordNum = static_cast<size_t>(File::Coord::Cmp);
    File::Pair<coreal> zerop = std::make_pair(0.0, 0.0);

    //Naive sort

    std::vector<std::pair<size_t, double> > tSort1(thead.size());
    std::vector<std::pair<size_t, double> > tSort2(thead.size());

    for (size_t i = 0; i < tSort1.size(); i++)
    {
        tSort1[i].first = thead[i].num;
        tSort1[i].second = calcDistance(zerop, thead[i].coords[coordNum]);
    }
    std::sort(tSort1.begin(), tSort1.end(), compareTraceVals);

    for (size_t i = 0; i < comm.getRank()-1; i++)
    {
        std::copy(tSort2.begin(), tSort2.end(), tSort1.begin());

        borderExchange(comm, regionSz, tSort1, tSort2);

        std::sort(tSort1.begin(), tSort1.end(), compareTraceVals);

        borderExchange(comm, regionSz, tSort1, tSort2);

        if (tSort1 == tSort2)
            break;

        if (!comm.getRank())
        {
            std::stringstream out;
            out << "Progress " << float(i)/float(comm.getRank()-1)*100.0 << " of max\n";
            MPI_Status stat;
            int err = MPI_File_write(file, out.str().data(), out.str().size(), MPI_CHAR, &stat);
            Block::printErr(err, &stat, "Failure to write log file");
        }
    }


    for (size_t r = 0; r < comm.getNumRank(); r++)
    {
        if (r == comm.getRank())
        {
            std::stringstream out;
            for (size_t i = 0; i < thead.size(); i++)
            {
                out << i << getOrd(i) << " trace: " << tSort1[i].second << std::endl;
            }

            int err = MPI_File_seek(file, 0, MPI_SEEK_END);
//TODO: Separate this from block
            Block::printErr(err, NULL, "Failure to update write pointer for log file");

            MPI_Status stat;
            err = MPI_File_write(file, out.str().data(), out.str().size(), MPI_CHAR, &stat);
            Block::printErr(err, &stat, "Failure to write log file");
        }
        MPI_Barrier(comm.getComm());
    }
}

void Subset(Comms::MPI & comm, std::vector<File::TraceHeader> & thead)
{

}


}}

