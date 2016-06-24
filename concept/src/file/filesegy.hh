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

    std::vector<uchar> readTraceHeaders(size_t offset, size_t num);
    void writeTraceHeaders(size_t offset, std::vector<uchar> &);

    public :

    SEGY(std::shared_ptr<Comms::Interface> Comm, std::string name, Bt bType);

///////////////////////Operations on Headers/////////////////////////////////
    void writeHeader(Header header);

///////////////////////Operations on Trace Headers/////////////////////////////////
    void readCoord(size_t, MetaPair, std::vector<CoordData> &);
    void readCoord(size_t, std::vector<CoordArray> &);
    void readCoord(size_t, Coord, std::vector<CoordData> &);
    void writeCoord(size_t, Coord, std::vector<CoordData> &);
    void writeCoord(size_t, std::vector<CoordArray> &);

    void readGrid(size_t, MetaPair, std::vector<GridData> &);
    void readGrid(size_t, Grid, std::vector<GridData> &);
    void readGrid(size_t, std::vector<GridArray> &);
    void writeGrid(size_t, Grid, std::vector<GridData> &);
    void writeGrid(size_t, std::vector<GridArray> &);

    void readTraceHeader(size_t offset, std::vector<GridArray> &, std::vector<CoordArray> &);
    void writeTraceHeader(size_t offset, std::vector<GridArray> &, std::vector<CoordArray> &);

///////////////////////Operations on Traces/////////////////////////////////
    void readTraces(size_t, std::vector<real> &);
    void readTraces(std::vector<size_t> &, std::vector<real> &);
    void writeTraces(size_t, std::vector<real> &);
};
}}
#endif
