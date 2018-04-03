////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/ExSeisPIOL.hh"

#include "ExSeisDat/PIOL/DataInterface.hh"
#include "ExSeisDat/PIOL/DataMPIIO.hh"
#include "ExSeisDat/PIOL/ObjectSEGY.hh"
#include "ExSeisDat/PIOL/share/segy.hh"

namespace PIOL {

//////////////////////      Constructor & Destructor      //////////////////////
ObjectSEGY::ObjectSEGY(
  std::shared_ptr<ExSeisPIOL> piol_,
  std::string name_,
  const ObjectSEGY::Opt&,
  std::shared_ptr<DataInterface> data_,
  FileMode) :
    ObjectInterface(piol_, name_, data_)
{
}

ObjectSEGY::ObjectSEGY(
  std::shared_ptr<ExSeisPIOL> piol_,
  std::string name_,
  std::shared_ptr<DataInterface> data_,
  FileMode) :
    ObjectInterface(piol_, name_, data_)
{
}

//////////////////////////       Member functions      /////////////////////////
void ObjectSEGY::readHO(uchar* ho) const
{
    data_->read(0LU, SEGSz::getHOSz(), ho);
}

void ObjectSEGY::writeHO(const uchar* ho) const
{
    if (ho)
        data_->write(0LU, SEGSz::getHOSz(), ho);
    else
        data_->write(0LU, 0U, ho);
}

void ObjectSEGY::readDO(
  const size_t offset, const size_t ns, const size_t sz, uchar* d) const
{
    data_->read(SEGSz::getDOLoc(offset, ns), sz * SEGSz::getDOSz(ns), d);
}

void ObjectSEGY::writeDO(
  const size_t offset, const size_t ns, const size_t sz, const uchar* d) const
{
    data_->write(SEGSz::getDOLoc(offset, ns), sz * SEGSz::getDOSz(ns), d);
}

void ObjectSEGY::readDOMD(
  const size_t offset, const size_t ns, const size_t sz, uchar* md) const
{
    data_->read(
      SEGSz::getDOLoc(offset, ns), SEGSz::getMDSz(), SEGSz::getDOSz(ns), sz,
      md);
}

void ObjectSEGY::writeDOMD(
  const size_t offset, const size_t ns, const size_t sz, const uchar* md) const
{
    data_->write(
      SEGSz::getDOLoc(offset, ns), SEGSz::getMDSz(), SEGSz::getDOSz(ns), sz,
      md);
}

void ObjectSEGY::readDODF(
  const size_t offset, const size_t ns, const size_t sz, uchar* df) const
{
    data_->read(
      SEGSz::getDODFLoc(offset, ns), SEGSz::getDFSz(ns), SEGSz::getDOSz(ns), sz,
      df);
}

void ObjectSEGY::writeDODF(
  const size_t offset, const size_t ns, const size_t sz, const uchar* df) const
{
    data_->write(
      SEGSz::getDODFLoc(offset, ns), SEGSz::getDFSz(ns), SEGSz::getDOSz(ns), sz,
      df);
}

// TODO: Add optional validation in this layer?
void ObjectSEGY::readDO(
  const size_t* offset, const size_t ns, const size_t sz, uchar* d) const
{
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDOLoc(offset[i], ns);
    data_->read(SEGSz::getDOSz(ns), sz, dooff.data(), d);
}

void ObjectSEGY::writeDO(
  const size_t* offset, const size_t ns, const size_t sz, const uchar* d) const
{
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDOLoc(offset[i], ns);
    data_->write(SEGSz::getDOSz(ns), sz, dooff.data(), d);
}

void ObjectSEGY::readDOMD(
  const size_t* offset, const size_t ns, const size_t sz, uchar* md) const
{
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDOLoc(offset[i], ns);
    data_->read(SEGSz::getMDSz(), sz, dooff.data(), md);
}

void ObjectSEGY::writeDOMD(
  const size_t* offset, const size_t ns, const size_t sz, const uchar* md) const
{
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDOLoc(offset[i], ns);
    data_->write(SEGSz::getMDSz(), sz, dooff.data(), md);
}

void ObjectSEGY::readDODF(
  const size_t* offset, const size_t ns, const size_t sz, uchar* df) const
{
    if (ns == 0) return;
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDODFLoc(offset[i], ns);
    data_->read(SEGSz::getDFSz(ns), sz, dooff.data(), df);
}

void ObjectSEGY::writeDODF(
  const size_t* offset, const size_t ns, const size_t sz, const uchar* df) const
{
    if (ns == 0) return;
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDODFLoc(offset[i], ns);
    data_->write(SEGSz::getDFSz(ns), sz, dooff.data(), df);
}

}  // namespace PIOL
