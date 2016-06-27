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
    header.text = readText();
    header.ns = readNs();
    header.nt = readNt();
    header.inc = readInc();
}
std::string Interface::readText()
{
    return text;
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

void Interface::writeText(std::string Text)
{
    text = Text;
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

/*void Interface::writeFile(Header & header, std::vector<GridArray> & grid, std::vector<CoordArray> & coord, std::vector<real> & data)
{
    assert((coord.size() == data.size() / header.ns) && (header.nt*header.ns == data.size()));
    assert((grid.size() == data.size() / header.ns));
    writeHeader(header);
}

void Interface::readFile(Header & header, std::vector<GridArray> & grid, std::vector<CoordArray> & coord, std::vector<real> & data)
{
    readHeader(header);
}*/
}}
