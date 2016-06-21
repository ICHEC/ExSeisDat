#ifndef PIOLFILESEGY_INCLUDE_GUARD
#define PIOLFILESEGY_INCLUDE_GUARD
#include <vector>
#include <array>
#include <mpi.h>
#include "global.hh"
#include "object/objsegy.hh"
#include "file/file.hh"
#include "file/iconv.hh"

namespace PIOL { namespace File {
class SEGY : public PIOL::File::Interface
{
    typedef PIOL::Block::Type Bt;
    enum Format : short
    {
        IBM = 1,    //IBM format, big endian
        TC4,    //Two's complement, 4 byte
        TC2,    //Two's complement, 2 byte
        FPG,    //Fixed-point gain (obsolete)
        IEEE,
        NA1,
        NA2,
        TC1    //Two's complement, 1 byte
    };

    Format format;
    size_t scale;
    
    void parseHO(std::vector<uchar> &);
    std::vector<uchar> makeHeader();

    public :

    SEGY(std::shared_ptr<Comms::Interface> Comm, std::string name, Bt bType);
    void writeHeader(Header header);
    void readCoord(size_t, CoordPair, std::vector<CoordData> &);
    void readCoord(size_t, std::vector<CoordArray> &);
    void readCoord(size_t, Coord, std::vector<CoordData> &);
    void writeCoord(size_t, Coord, std::vector<CoordData> &);
    void writeCoord(size_t, std::vector<CoordArray> &);
    void readTraces(size_t, std::vector<real> &);
    void readTraces(std::vector<size_t> &, std::vector<real> &);
    void writeTraces(size_t, std::vector<real> &);
};
}}
#endif
