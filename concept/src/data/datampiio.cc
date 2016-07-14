#include <mpi.h>
#include <limits>
#include <iostream>
#include <vector>
#include <typeinfo>
#include "datampiio.hh"
#include "comm/mpi.hh"

namespace PIOL { namespace Data {
//extern MPI_File open(MPI_Comm, std::string, int);
//extern size_t getFileSz(MPI_File);
//extern void setFileSz(MPI_File, size_t);
//extern void printErr(int, MPI_Status *, std::string);

//typedef int (* MPI_Fp)(MPI_File, MPI_Offset, void *, int, MPI_Datatype, MPI_Status *);
//typedef int (* MPI_IFp)(MPI_File, MPI_Offset, void *, int, MPI_Datatype, MPI_Request *);
void printErr(int err, MPI_Status * stat, std::string msg)
{
    if (err != MPI_SUCCESS)
    {
        std::cout << "MPI Err " << err << " " << msg;
        if (err == MPI_ERR_IN_STATUS)
            std::cout << " MPI_Status: " << stat->MPI_ERROR;
        std::cout << std::endl;
        exit(EXIT_FAILURE);
    }
}

template <typename T>
#ifndef __ICC
constexpr
#endif
MPI_Datatype Type()
{
    return (typeid(T) == typeid(double)             ? MPI_DOUBLE
         : (typeid(T) == typeid(long double)        ? MPI_LONG_DOUBLE
         : (typeid(T) == typeid(char)               ? MPI_CHAR
         : (typeid(T) == typeid(uchar)      ? MPI_UNSIGNED_CHAR
         : (typeid(T) == typeid(int)                ? MPI_INT
         : (typeid(T) == typeid(long int)           ? MPI_LONG
         : (typeid(T) == typeid(unsigned long int)  ? MPI_UNSIGNED_LONG
         : (typeid(T) == typeid(unsigned int)       ? MPI_UNSIGNED
         : (typeid(T) == typeid(long long int)      ? MPI_LONG_LONG_INT
         : (typeid(T) == typeid(float)              ? MPI_FLOAT
         : (typeid(T) == typeid(signed short)       ? MPI_SHORT
         : (typeid(T) == typeid(unsigned short)     ? MPI_UNSIGNED_SHORT
         : MPI_BYTE))))))))))));
}

template <typename T>
constexpr MPI_Offset getLim()
{
    //If you aren't (4096 - Chunk)/Chunk from the limit, intel mpi breaks.
    //Probably something to do with pages.
    //return MPI_Offset((std::numeric_limits<int>::max() - (4096U - sizeof(T))) / sizeof(T));
    return MPI_Offset((std::numeric_limits<int>::max() - (4096U - sizeof(T))) / sizeof(T));
}

template <typename T, typename U = MPI_Status>
U doMPIIO(Fp<U> fn, MPI_File & file, MPI_Offset offset, T * d, size_t sz)
{
    U arg;
    int err = MPI_SUCCESS;
    auto max = getLim<T>();
    MPI_Offset q = sz / max;
    MPI_Offset r = sz % max;

#ifdef DEBUG
    std::cout << "off = " << offset << " q = " << q << " r = " << r << "r+q*max = " << r+q*max << std::endl 
             << "max = " << max << " sz " << sz << "tsize " << sizeof(T) << std::endl;
#endif

    for (MPI_Offset i = 0; i < q; i++)
    {
#ifdef DEBUG
        std::cout << " fn(file, " << offset + i*max << ", &d[i*max], " << max << ", Type<T>(), &arg);\n";
#endif
        err = fn(file, offset + i*max, &d[i*max], max, Type<T>(), &arg);
        if (err != MPI_SUCCESS)
            break;
    }

    if (err == MPI_SUCCESS)
    {
#ifdef DEBUG
        std::cout << " fn(file, " << offset + q*max << ", &d[q*max], " << r << ", Type<T>(), &arg);\n";
#endif
        err = fn(file, offset + q*max, &d[q*max], r, Type<T>(), &arg);
    }
    else
        std::cerr << "Error with MPIIO\n";

    printErr(err, (typeid(T) == typeid(MPI_Status) ? &arg : NULL), " MPIIO Failure\n");

    return arg;
}

template <typename T, typename U = MPI_Status>
U doMPIIO(Fp<U> fn, MPI_File & file, MPI_Offset offset, std::vector<T> & d)
{
    return doMPIIO<T, U>(fn, file, offset, &d.front(), d.size());
}

template <typename T, typename U=T>
void gSetView(MPI_File file, MPI_Offset offset = 0)
{
    MPI_Info info = MPI_INFO_NULL;
    int err = MPI_File_set_view(file, offset, Type<T>(), Type<U>(), "native", info);
    printErr(err, NULL, "MPI_File_set_view failure\n"); 
}

#ifdef DEBUG
void debugErr(int err, MPI_Status * stat, std::string msg)
{
    if (err != MPI_SUCCESS)
    {
        std::cout << "MPI Err " << err << " " << msg;
        if (err == MPI_ERR_IN_STATUS)
            std::cout << " MPI_Status: " << stat->MPI_ERROR;
        std::cout << std::endl;
        exit(EXIT_FAILURE);
    }
}
#else
#define debugErr(x, y, z)
#endif 

//template void MPIIO<real>(MPI_Fp, MPI_File & File, MPI_Offset Offset, std::vector<real> & D);

MPI_File open(MPI_Comm comm, std::string name, int mode)
{
    MPI_Info info = MPI_INFO_NULL;
    MPI_File file = MPI_FILE_NULL;
    int err = MPI_File_open(comm, name.c_str(), mode, info, &file);

    debugErr(err, NULL, "MPI_File failure " + name + " \n");
    if (err != MPI_SUCCESS)
        return MPI_FILE_NULL;
    else
        return file;
}

////////MPI-IO Class Interface//////////////////

MPIIO::MPIIO(std::shared_ptr<Comms::MPI> Comm, std::string name,
     Fp<MPI_Status> Ifn,
     Fp<MPI_Status> Ofn) : Data::Interface(Comm), ifn(Ifn), ofn(Ofn)
{
    mcomm = Comm->getComm();
    //Try write mode
    file = open(mcomm, name, MPI_MODE_EXCL | MPI_MODE_UNIQUE_OPEN | MPI_MODE_CREATE | MPI_MODE_WRONLY);

    //Write mode failed, try read mode
    if (file == MPI_FILE_NULL)
    {
        file = open(mcomm, name, MPI_MODE_UNIQUE_OPEN | MPI_MODE_RDONLY);
    }
    if (file != MPI_FILE_NULL)
    {
        setView();
    }
    else
    {
        std::cerr << "failed to initialise MPI Block instance\n";
        exit(-1);
    }
}

int mpiio_write_at(MPI_File f, MPI_Offset o, void * d, int s, MPI_Datatype da, MPI_Status * st)
{
    return MPI_File_write_at(f, o, d, s, da, st);
}

MPIIO::MPIIO(std::shared_ptr<Comms::MPI> Comm, std::string name, int mode, 
             Fp<MPI_Status> Ifn, Fp<MPI_Status> Ofn) 
           : PIOL::Data::Interface(Comm), ifn(Ifn), ofn(Ofn)
{
    mcomm = Comm->getComm();
    file = open(mcomm, name, mode);
    if (file != MPI_FILE_NULL)
        setView();
    else
        std::cerr << "Fatal Abort\n";
}
MPIIO::~MPIIO(void)
{
    MPI_File_close(&file);
}
size_t MPIIO::getFileSz()
{
    MPI_Offset fsz;
    int err = MPI_File_get_size(file, &fsz);
    printErr(err, NULL, "error getting the file size");
    return size_t(fsz);
}
void MPIIO::setFileSz(size_t sz)
{
    int err = MPI_File_set_size(file, MPI_Offset(sz));
    printErr(err, NULL, "Error resizing file\n");
}
void MPIIO::setView(size_t offset)
{
    gSetView<uchar>(file, MPI_Offset(offset));
}

void MPIIO::readData(size_t o, float * f, size_t s)
{
    doMPIIO<float>(ifn, file, MPI_Offset(o), f, MPI_Offset(s));
}
void MPIIO::readData(size_t o, uchar * c, size_t s)
{
    doMPIIO<uchar>(ifn, file, MPI_Offset(o), c, MPI_Offset(s));
}
void MPIIO::writeData(size_t o, float * f, size_t s)
{
    doMPIIO<float>(ofn, file, MPI_Offset(o), f, MPI_Offset(s));
}
void MPIIO::writeData(size_t o, uchar * c, size_t s)
{
    doMPIIO<uchar>(ofn, file, MPI_Offset(o), c, MPI_Offset(s));
}
}}
