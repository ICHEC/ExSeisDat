#ifndef PIOLBLOCKMPI_INCLUDE_GUARD
#define PIOLBLOCKMPI_INCLUDE_GUARD
#include <mpi.h>
#include "mpiio.hh"

namespace PIOL { namespace Block { namespace MPI {

auto file_write_at(MPI_File f, MPI_Offset o, void * d, int s, MPI_Datatype da, MPI_Status * st)
{
    return MPI_File_write_at(f, o, d, s, da, st);
}


template <typename U>
class Blck : public BlockLayer
{
#ifdef TEST_PRIVATE
    public :
#endif


    MPI_File file;
    Fp<U> ifn;
    Fp<U> ofn;

    public : //HACK

    template <typename T>
    void readData(size_t offset, T * d, size_t sz)
    {
        std::cout << "getter\n";
        MPIIO<T>(ifn, file, MPI_Offset(offset), d, MPI_Offset(sz));
    }
    template <typename T>
    void writeData(size_t offset, T * d, size_t sz)
    {
        std::cout << "setter\n";
        MPIIO<T>(ofn, file, MPI_Offset(offset), d, MPI_Offset(sz));
    }

#ifndef TEST_PRIVATE
    public :
#endif

    Blck(MPI_Comm Comm, std::string name, 
         Fp<U> Ifn = MPI_File_read_at,
         Fp<U> Ofn = file_write_at) : BlockLayer(Comm), ifn(Ifn), ofn(Ofn)
    {
        //Try write mode
        file = open(Comm, name, MPI_MODE_EXCL | MPI_MODE_UNIQUE_OPEN | MPI_MODE_CREATE | MPI_MODE_WRONLY);

        //Write mode failed, try read mode
        if (file == MPI_FILE_NULL)
        {
            file = open(this->comm, name, MPI_MODE_UNIQUE_OPEN | MPI_MODE_RDONLY);
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
    //Blck(std::string name, Fp<U> Ifn = MPI_File_read_at, Fp<U> Ofn = MPI_File_write_at)
    Blck(std::string name)
         : Blck(MPI_COMM_WORLD, name)
    {
    }
    Blck(MPI_Comm Comm, std::string name, int mode, Fp<U> Ifn = MPI_File_read_at, Fp<U> Ofn = file_write_at) : BlockLayer(Comm), ifn(Ifn), ofn(Ofn)
    {
        file = open(comm, name, mode);
        if (file != MPI_FILE_NULL)
            setView();
        else
            std::cerr << "Fatal Abort\n";
    }
    ~Blck(void)
    {
        std::cout << "Close file\n";
        MPI_File_close(&file);
    }
    size_t getFileSz()
    {
        return MPI::getFileSz(file);
    }
    void growFile(size_t sz)
    {
        std::cout << " Set file size\n";
        MPI::growFile(file, MPI_Offset(sz));
    }
    void setView(size_t offset = 0)
    {
        std::cout << " Set view\n";
        MPI::setView<char>(file, MPI_Offset(offset));
    }

    void readData(size_t o, float * f, size_t s)
    {
        return readData<float>(o, f, s);
    }
    void readData(size_t o, char * c, size_t s)
    {
        return readData<char>(o, c, s);
    }
    void writeData(size_t o, float * f, size_t s)
    {
        return writeData<float>(o, f, s);
    }
    void writeData(size_t o, char * c, size_t s)
    {
        return writeData<char>(o, c, s);
    }
};
}}}
#endif
