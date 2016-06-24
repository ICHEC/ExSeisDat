#ifndef PIOLSET_INCLUDE_GUARD
#define PIOLSET_INCLUDE_GUARD
#include <vector>
#include <array>
#include <memory>
#include "comm/comm.hh"
#include "file/file.hh"

namespace PIOL { namespace Set {

//Manager for the file class
//Performs memory management and parallelism
typedef std::pair<size_t, size_t> Decomp;
typedef std::pair<size_t, std::vector<File::CoordData> > CoordItem;
typedef std::pair<size_t, std::vector<File::GridData> > GridItem;
typedef std::pair<size_t, std::vector<File::MetaArray<coreal, File::Coord> > > CoordStrut;
typedef std::pair<size_t, std::vector<File::MetaArray<llint, File::Grid> > > GridStrut;
typedef std::pair<size_t, std::vector<real> > TraceData;

class Manager
{
    protected :
    std::unique_ptr<File::Interface> file;
    std::shared_ptr<Comms::Interface> comm;
    public :

    Manager(std::shared_ptr<Comms::Interface> comm_, std::unique_ptr<File::Interface> file_) : file(std::move(file_)), comm(comm_)
    {
    }

    GridStrut readGrid(void);
    GridItem readGrid(File::Grid);

    CoordStrut readCoord(void);
    CoordItem readCoord(File::Coord);

    size_t readTraceHeader(std::vector<File::GridArray> &, std::vector<File::CoordArray> &);
    TraceData readTraces(void);
    File::Header readHeader(void);
    Decomp decompose(size_t);
};

}}
#endif
