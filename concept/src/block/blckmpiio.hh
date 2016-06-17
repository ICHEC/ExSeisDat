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
class Interface : public PIOL::Block::Interface
{
#ifdef TEST_PRIVATE
    public :
#endif

    MPI_File file;
    MPI_Comm comm;
    Fp<U> ifn;
    Fp<U> ofn;
    MPI_Comm mcomm;
    public : //HACK

    template <typename T>
    void readData(size_t offset, T * d, size_t sz)
    {
        MPIIO<T>(ifn, file, MPI_Offset(offset), d, MPI_Offset(sz));
    }
    template <typename T>
    void writeData(size_t offset, T * d, size_t sz)
    {
        MPIIO<T>(ofn, file, MPI_Offset(offset), d, MPI_Offset(sz));
    }

#ifndef TEST_PRIVATE
    public :
#endif

    Interface(Comms::Interface & Comm, std::string name, 
         Fp<U> Ifn = MPI_File_read_at,
         Fp<U> Ofn = file_write_at) : Block::Interface(Comm), ifn(Ifn), ofn(Ofn)
    {
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
/*    Interface(std::string name)
         : Interface(MPI_COMM_WORLD, name)
    {
    }*/
    Interface(Comms::Interface & Comm, std::string name, int mode, Fp<U> Ifn = MPI_File_read_at, Fp<U> Ofn = file_write_at) : PIOL::Block::Interface(Comm), ifn(Ifn), ofn(Ofn)
    {
        file = open(mcomm, name, mode);
        if (file != MPI_FILE_NULL)
            setView();
        else
            std::cerr << "Fatal Abort\n";
    }
    ~Interface(void)
    {
        std::cout << "Close file\n";
        MPI_File_close(&file);
    }
    size_t getFileSz()
    {
        return MPI::getFileSz(file);
    }
    void setFileSz(size_t sz)
    {
        std::cout << " Set file size\n";
        MPI::growFile(file, MPI_Offset(sz));
    }
    void setView(size_t offset = 0)
    {
        std::cout << " Set view\n";
        MPI::setView<unsigned char>(file, MPI_Offset(offset));
    }

    void readData(size_t o, float * f, size_t s)
    {
        return readData<float>(o, f, s);
    }
    void readData(size_t o, unsigned char * c, size_t s)
    {
        return readData<unsigned char>(o, c, s);
    }
    void writeData(size_t o, float * f, size_t s)
    {
        return writeData<float>(o, f, s);
    }
    void writeData(size_t o, unsigned char * c, size_t s)
    {
        return writeData<unsigned char>(o, c, s);
    }
};
}}}
#endif
