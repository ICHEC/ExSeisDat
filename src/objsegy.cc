/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include "object/objsegy.hh"
#include "share/segy.hh"
#include "data/data.hh"
namespace PIOL { namespace Obj {
///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////    Class functions    ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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
    data->read(0U, SEGSz::getHOSz(), ho);
}

void SEGY::writeHO(const uchar * ho) const
{
    data->write(0U, SEGSz::getHOSz(), ho);
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

void SEGY::readDO(csize_t ns, csize_t sz, csize_t * offset, uchar * d) const
{
    size_t dosz = SEGSz::getDOSz(ns);
    for (size_t i = 0; i < sz; i++)
        data->read(SEGSz::getDOLoc(offset[i], ns), dosz, &d[i*dosz]);
}

void SEGY::writeDO(csize_t ns, csize_t sz, csize_t * offset, const uchar * d) const
{
    size_t dosz = SEGSz::getDOSz(ns);
    for (size_t i = 0; i < sz; i++)
        data->write(SEGSz::getDOLoc(offset[i], ns), dosz, &d[i*dosz]);
}

void SEGY::readDOMD(csize_t ns, csize_t sz, csize_t * offset, uchar * md) const
{
    size_t mdsz = SEGSz::getMDSz();
    for (size_t i = 0; i < sz; i++)
        data->read(SEGSz::getDOLoc(offset[i], ns), mdsz, &md[i * mdsz]);
}

void SEGY::writeDOMD(csize_t ns, csize_t sz, csize_t * offset, const uchar * md) const
{
    size_t mdsz = SEGSz::getMDSz();
    for (size_t i = 0; i < sz; i++)
        data->write(SEGSz::getDOLoc(offset[i], ns), mdsz, &md[i*mdsz]);
}

void SEGY::readDODF(csize_t ns, csize_t sz, csize_t * offset, uchar * df) const
{
    size_t dfsz = SEGSz::getDFSz(ns);
    for (size_t i = 0; i < sz; i++)
        data->read(SEGSz::getDODFLoc(offset[i], ns), dfsz, &df[i*dfsz]);
}

void SEGY::writeDODF(csize_t ns, csize_t sz, csize_t * offset, const uchar * df) const
{
    size_t dfsz = SEGSz::getDFSz(ns);
    for (size_t i = 0; i < sz; i++)
        data->write(SEGSz::getDODFLoc(offset[i], ns), dfsz, &df[i*dfsz]);
}
}}
