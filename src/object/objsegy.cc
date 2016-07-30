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
SEGY::SEGY(std::shared_ptr<ExSeisPIOL> piol_, std::string name_, const SEGYOpt & segyOpt_, std::shared_ptr<Data::Interface> data_) : Interface(piol_, name_, data_)
{
}

SEGY::SEGY(std::shared_ptr<ExSeisPIOL> piol_, std::string name_, const SEGYOpt & segyOpt, const Data::Opt & dataOpt) : Interface(piol_, name_, dataOpt)
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

void SEGY::readDOMD(const size_t offset, const size_t ns, uchar * ho) const
{
    data->read(SEGSz::getDOLoc<float>(offset, ns), SEGSz::getMDSz(), ho);
}

void SEGY::writeDOMD(const size_t offset, const size_t ns, const uchar * ho) const
{
    data->write(SEGSz::getDOLoc<float>(offset, ns), SEGSz::getMDSz(), ho);
}
}}
