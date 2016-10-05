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
    data->read(SEGSz::getDOLoc<float>(offset, ns), sz * SEGSz::getDOSz(ns), d);
}

void SEGY::writeDO(csize_t offset, csize_t ns, csize_t sz, const uchar * d) const
{
    data->write(SEGSz::getDOLoc<float>(offset, ns), sz * SEGSz::getDOSz(ns), d);
}

void SEGY::readDOMD(csize_t offset, csize_t ns, csize_t sz, uchar * md) const
{
    data->read(SEGSz::getDOLoc<float>(offset, ns), SEGSz::getMDSz(), SEGSz::getDOSz(ns), sz, md);
}

void SEGY::writeDOMD(csize_t offset, csize_t ns, csize_t sz, const uchar * md) const
{
    data->write(SEGSz::getDOLoc<float>(offset, ns), SEGSz::getMDSz(), SEGSz::getDOSz(ns), sz, md);
}

void SEGY::readDODF(csize_t offset, csize_t ns, csize_t sz, uchar * df) const
{
    data->read(SEGSz::getDODFLoc<float>(offset, ns), SEGSz::getDFSz(ns), SEGSz::getDOSz(ns), sz, df);
}

void SEGY::writeDODF(csize_t offset, csize_t ns, csize_t sz, const uchar * df) const
{
    data->write(SEGSz::getDODFLoc<float>(offset, ns), SEGSz::getDFSz(ns), SEGSz::getDOSz(ns), sz, df);
}

void SEGY::readDO(csize_t ns, csize_t sz, csize_t * offset, uchar * d) const
{
    for (size_t i = 0; i < sz; i++)
        data->read(SEGSz::getDOLoc<float>(offset[i], ns), SEGSz::getDOSz(ns), d);
}

void SEGY::writeDO(csize_t ns, csize_t sz, csize_t * offset, const uchar * d) const
{
    for (size_t i = 0; i < sz; i++)
        data->write(SEGSz::getDOLoc<float>(offset[i], ns), SEGSz::getDOSz(ns), d);
}

void SEGY::readDOMD(csize_t ns, csize_t sz, csize_t * offset, uchar * md) const
{
    for (size_t i = 0; i < sz; i++)
        readDOMD(offset[i], ns, 1U, md);
}

void SEGY::writeDOMD(csize_t ns, csize_t sz, csize_t * offset, const uchar * md) const
{
    for (size_t i = 0; i < sz; i++)
        writeDOMD(offset[i], ns, 1U, md);
}

void SEGY::readDODF(csize_t ns, csize_t sz, csize_t * offset, uchar * df) const
{
    for (size_t i = 0; i < sz; i++)
        readDODF(offset[i], ns, 1U, df);
}

void SEGY::writeDODF(csize_t ns, csize_t sz, csize_t * offset, const uchar * df) const
{
    for (size_t i = 0; i < sz; i++)
        writeDODF(offset[i], ns, 1U, df);
}
}}
