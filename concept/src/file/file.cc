#include <vector>
#include <array>
#include <cassert>
#include <utility>
#include <string>
#include <iostream>
#include <memory>
#include <mpi.h>
#include "global.hh"
#include "object/object.hh"
#include "file/file.hh"
namespace PIOL { namespace File {
void Interface::checkFileSize()
{
    if (obj->getFileSz() != obj->getSize(readNt(), readNs()))
    {
        obj->setFileSz(readNt(), readNs());
    }
}

Interface::Interface(std::shared_ptr<Comms::Interface> Comm) : comm(Comm)
{
}
Interface::~Interface(void)
{
    //TODO:
    if (defHOUpdate)
    {
    }
}
void Interface::readHeader(Header & header)
{
    header.note = readNote();
    header.ns = readNs();
    header.nt = readNt();
    header.inc = readInc();
}
std::string Interface::readNote()
{
    return note;
}
size_t Interface::readNs()
{
    return ns;
}
size_t Interface::readNt()
{
    return nt;
}
real Interface::readInc()
{
    return inc;
}

void Interface::writeNote(std::string Note)
{
    note = Note;
    defHOUpdate = true;
}
 void Interface::writeNs(size_t Ns)
{
    ns = Ns;
    defHOUpdate = true;
}
 void Interface::writeNt(size_t Nt)
{
    nt = Nt;
    defHOUpdate = true;
}
 void Interface::writeInc(real Inc)
{
    inc = Inc;
}

void Interface::writeFile(Header & header, std::vector<CoordArray> & coord, std::vector<real> & data)
{
    assert((coord.size() == data.size() / header.ns) && (header.nt*header.ns == data.size()));
    writeHeader(header);
}
void Interface::readFile(Header & header, std::vector<CoordArray> & coord, std::vector<real> & data)
{
    readHeader(header);
}

#ifndef __ICC
constexpr 
#endif
CoordPair getCoordPair(Coord pair)
{
    switch (pair)
    {
        case Coord::Src :
            return std::make_pair(BlockMd::xSrc, BlockMd::ySrc);
        case Coord::Rcv :
            return std::make_pair(BlockMd::xRcv, BlockMd::yRcv);
        case Coord::Cmp :
            return std::make_pair(BlockMd::xCDP, BlockMd::yCDP);
        case Coord::Lin :
            return std::make_pair(BlockMd::iLin, BlockMd::xLin);
        default : 
            return std::make_pair(BlockMd::ERROR, BlockMd::ERROR);
    }
}
}}
