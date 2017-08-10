/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include "object/objsegy.hh"
#include "data/datampiio.hh"
#include "share/segy.hh"
#include "data/data.hh"
namespace PIOL { namespace Obj {
///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////    Class functions    ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Obj::Interface> makeDefaultObj(Piol piol, std::string name, FileMode mode)
{
    auto data = std::make_shared<Data::MPIIO>(piol, name, mode);
    return std::make_shared<Obj::SEGY>(piol, name, data, mode);
}

///////////////////////////////      Constructor & Destructor      ///////////////////////////////
//pragma to ignore unusued-paramter warnings here
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
SEGY::SEGY(Piol piol_, std::string name_, const SEGY::Opt & opt_, std::shared_ptr<Data::Interface> data_, FileMode mode) : Interface(piol_, name_, data_)
{
}
SEGY::SEGY(Piol piol_, std::string name_, std::shared_ptr<Data::Interface> data_, FileMode mode) : Interface(piol_, name_, data_)
{
}
#pragma GCC diagnostic pop

///////////////////////////////////       Member functions      ///////////////////////////////////
void SEGY::readHO(uchar * ho) const
{
    data->read(0LU, SEGSz::getHOSz(), ho);
}

void SEGY::writeHO(const uchar * ho) const
{
    if (ho)
        data->write(0LU, SEGSz::getHOSz(), ho);
    else
        data->write(0LU, 0U, ho);
}

void SEGY::readDO(csize_t offset, csize_t ns, csize_t sz, uchar * d) const
{
    data->read(SEGSz::getDOLoc(offset, ns), sz * SEGSz::getDOSz(ns), d);
}

void SEGY::writeDO(csize_t offset, csize_t ns, csize_t sz, const uchar * d) const
{
    data->write(SEGSz::getDOLoc(offset, ns), sz * SEGSz::getDOSz(ns), d);
}

void SEGY::readDOMD(csize_t offset, csize_t ns, csize_t sz, uchar * md) const
{
    data->read(SEGSz::getDOLoc(offset, ns), SEGSz::getMDSz(), SEGSz::getDOSz(ns), sz, md);
}

void SEGY::writeDOMD(csize_t offset, csize_t ns, csize_t sz, const uchar * md) const
{
    data->write(SEGSz::getDOLoc(offset, ns), SEGSz::getMDSz(), SEGSz::getDOSz(ns), sz, md);
}

void SEGY::readDODF(csize_t offset, csize_t ns, csize_t sz, uchar * df) const
{
    data->read(SEGSz::getDODFLoc(offset, ns), SEGSz::getDFSz(ns), SEGSz::getDOSz(ns), sz, df);
}

void SEGY::writeDODF(csize_t offset, csize_t ns, csize_t sz, const uchar * df) const
{
    data->write(SEGSz::getDODFLoc(offset, ns), SEGSz::getDFSz(ns), SEGSz::getDOSz(ns), sz, df);
}

//TODO: Add optional validation in this layer?
void SEGY::readDO(csize_t * offset, csize_t ns, csize_t sz, uchar * d) const
{
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDOLoc(offset[i], ns);
    data->read(SEGSz::getDOSz(ns), sz, dooff.data(), d);
}

void SEGY::writeDO(csize_t * offset, csize_t ns, csize_t sz, const uchar * d) const
{
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDOLoc(offset[i], ns);
    data->write(SEGSz::getDOSz(ns), sz, dooff.data(), d);
}

void SEGY::readDOMD(csize_t * offset, csize_t ns, csize_t sz, uchar * md) const
{
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDOLoc(offset[i], ns);
    data->read(SEGSz::getMDSz(), sz, dooff.data(), md);
}

void SEGY::writeDOMD(csize_t * offset, csize_t ns, csize_t sz, const uchar * md) const
{
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDOLoc(offset[i], ns);
    data->write(SEGSz::getMDSz(), sz, dooff.data(), md);
}

void SEGY::readDODF(csize_t * offset, csize_t ns, csize_t sz, uchar * df) const
{
    if (ns == 0)
        return;
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDODFLoc(offset[i], ns);
    data->read(SEGSz::getDFSz(ns), sz, dooff.data(), df);
}

void SEGY::writeDODF(csize_t * offset, csize_t ns, csize_t sz, const uchar * df) const
{
    if (ns == 0)
        return;
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDODFLoc(offset[i], ns);
    data->write(SEGSz::getDFSz(ns), sz, dooff.data(), df);
}
}}
