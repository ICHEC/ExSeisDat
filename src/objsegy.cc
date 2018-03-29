////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/ExSeisPIOL.hh"

#include "ExSeisDat/PIOL/data/data.hh"
#include "ExSeisDat/PIOL/data/datampiio.hh"
#include "ExSeisDat/PIOL/object/objsegy.hh"
#include "ExSeisDat/PIOL/share/segy.hh"

namespace PIOL {
namespace Obj {

/////////////////////////////    Class functions    ////////////////////////////
std::shared_ptr<Obj::Interface> makeDefaultObj(
  std::shared_ptr<ExSeisPIOL> piol, std::string name, Data::FileMode mode)
{
    auto data = std::make_shared<Data::MPIIO>(piol, name, mode);
    return std::make_shared<Obj::SEGY>(piol, name, data, mode);
}

//////////////////////      Constructor & Destructor      //////////////////////
SEGY::SEGY(
  std::shared_ptr<ExSeisPIOL> piol_,
  std::string name_,
  const SEGY::Opt&,
  std::shared_ptr<Data::Interface> data_,
  Data::FileMode) :
    Interface(piol_, name_, data_)
{
}

SEGY::SEGY(
  std::shared_ptr<ExSeisPIOL> piol_,
  std::string name_,
  std::shared_ptr<Data::Interface> data_,
  Data::FileMode) :
    Interface(piol_, name_, data_)
{
}

//////////////////////////       Member functions      /////////////////////////
void SEGY::readHO(uchar* ho) const
{
    data_->read(0LU, SEGSz::getHOSz(), ho);
}

void SEGY::writeHO(const uchar* ho) const
{
    if (ho)
        data_->write(0LU, SEGSz::getHOSz(), ho);
    else
        data_->write(0LU, 0U, ho);
}

void SEGY::readDO(
  const size_t offset, const size_t ns, const size_t sz, uchar* d) const
{
    data_->read(SEGSz::getDOLoc(offset, ns), sz * SEGSz::getDOSz(ns), d);
}

void SEGY::writeDO(
  const size_t offset, const size_t ns, const size_t sz, const uchar* d) const
{
    data_->write(SEGSz::getDOLoc(offset, ns), sz * SEGSz::getDOSz(ns), d);
}

void SEGY::readDOMD(
  const size_t offset, const size_t ns, const size_t sz, uchar* md) const
{
    data_->read(
      SEGSz::getDOLoc(offset, ns), SEGSz::getMDSz(), SEGSz::getDOSz(ns), sz,
      md);
}

void SEGY::writeDOMD(
  const size_t offset, const size_t ns, const size_t sz, const uchar* md) const
{
    data_->write(
      SEGSz::getDOLoc(offset, ns), SEGSz::getMDSz(), SEGSz::getDOSz(ns), sz,
      md);
}

void SEGY::readDODF(
  const size_t offset, const size_t ns, const size_t sz, uchar* df) const
{
    data_->read(
      SEGSz::getDODFLoc(offset, ns), SEGSz::getDFSz(ns), SEGSz::getDOSz(ns), sz,
      df);
}

void SEGY::writeDODF(
  const size_t offset, const size_t ns, const size_t sz, const uchar* df) const
{
    data_->write(
      SEGSz::getDODFLoc(offset, ns), SEGSz::getDFSz(ns), SEGSz::getDOSz(ns), sz,
      df);
}

// TODO: Add optional validation in this layer?
void SEGY::readDO(
  const size_t* offset, const size_t ns, const size_t sz, uchar* d) const
{
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDOLoc(offset[i], ns);
    data_->read(SEGSz::getDOSz(ns), sz, dooff.data(), d);
}

void SEGY::writeDO(
  const size_t* offset, const size_t ns, const size_t sz, const uchar* d) const
{
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDOLoc(offset[i], ns);
    data_->write(SEGSz::getDOSz(ns), sz, dooff.data(), d);
}

void SEGY::readDOMD(
  const size_t* offset, const size_t ns, const size_t sz, uchar* md) const
{
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDOLoc(offset[i], ns);
    data_->read(SEGSz::getMDSz(), sz, dooff.data(), md);
}

void SEGY::writeDOMD(
  const size_t* offset, const size_t ns, const size_t sz, const uchar* md) const
{
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDOLoc(offset[i], ns);
    data_->write(SEGSz::getMDSz(), sz, dooff.data(), md);
}

void SEGY::readDODF(
  const size_t* offset, const size_t ns, const size_t sz, uchar* df) const
{
    if (ns == 0) return;
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDODFLoc(offset[i], ns);
    data_->read(SEGSz::getDFSz(ns), sz, dooff.data(), df);
}

void SEGY::writeDODF(
  const size_t* offset, const size_t ns, const size_t sz, const uchar* df) const
{
    if (ns == 0) return;
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDODFLoc(offset[i], ns);
    data_->write(SEGSz::getDFSz(ns), sz, dooff.data(), df);
}

}  // namespace Obj
}  // namespace PIOL
