////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/DataInterface.hh"
#include "ExSeisDat/PIOL/DataMPIIO.hh"
#include "ExSeisDat/PIOL/ExSeisPIOL.hh"
#include "ExSeisDat/PIOL/ObjectSEGY.hh"
#include "ExSeisDat/PIOL/segy_utils.hh"

namespace exseis {
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
void ObjectSEGY::readHO(unsigned char* ho) const
{
    data_->read(0LU, SEGY_utils::getHOSz(), ho);
}

void ObjectSEGY::writeHO(const unsigned char* ho) const
{
    if (ho) {
        data_->write(0LU, SEGY_utils::getHOSz(), ho);
    }
    else {
        data_->write(0LU, 0U, ho);
    }
}

void ObjectSEGY::readDO(
  const size_t offset, const size_t ns, const size_t sz, unsigned char* d) const
{
    data_->read(
      SEGY_utils::getDOLoc(offset, ns), sz * SEGY_utils::getDOSz(ns), d);
}

void ObjectSEGY::writeDO(
  const size_t offset,
  const size_t ns,
  const size_t sz,
  const unsigned char* d) const
{
    data_->write(
      SEGY_utils::getDOLoc(offset, ns), sz * SEGY_utils::getDOSz(ns), d);
}

void ObjectSEGY::readDOMD(
  const size_t offset,
  const size_t ns,
  const size_t sz,
  unsigned char* md) const
{
    data_->read(
      SEGY_utils::getDOLoc(offset, ns), SEGY_utils::getMDSz(),
      SEGY_utils::getDOSz(ns), sz, md);
}

void ObjectSEGY::writeDOMD(
  const size_t offset,
  const size_t ns,
  const size_t sz,
  const unsigned char* md) const
{
    data_->write(
      SEGY_utils::getDOLoc(offset, ns), SEGY_utils::getMDSz(),
      SEGY_utils::getDOSz(ns), sz, md);
}

void ObjectSEGY::readDODF(
  const size_t offset,
  const size_t ns,
  const size_t sz,
  unsigned char* df) const
{
    data_->read(
      SEGY_utils::getDODFLoc(offset, ns), SEGY_utils::getDFSz(ns),
      SEGY_utils::getDOSz(ns), sz, df);
}

void ObjectSEGY::writeDODF(
  const size_t offset,
  const size_t ns,
  const size_t sz,
  const unsigned char* df) const
{
    data_->write(
      SEGY_utils::getDODFLoc(offset, ns), SEGY_utils::getDFSz(ns),
      SEGY_utils::getDOSz(ns), sz, df);
}

// TODO: Add optional validation in this layer?
void ObjectSEGY::readDO(
  const size_t* offset,
  const size_t ns,
  const size_t sz,
  unsigned char* d) const
{
    std::vector<size_t> dooff(sz);

    for (size_t i = 0; i < sz; i++) {
        dooff[i] = SEGY_utils::getDOLoc(offset[i], ns);
    }

    data_->read(SEGY_utils::getDOSz(ns), sz, dooff.data(), d);
}

void ObjectSEGY::writeDO(
  const size_t* offset,
  const size_t ns,
  const size_t sz,
  const unsigned char* d) const
{
    std::vector<size_t> dooff(sz);

    for (size_t i = 0; i < sz; i++) {
        dooff[i] = SEGY_utils::getDOLoc(offset[i], ns);
    }

    data_->write(SEGY_utils::getDOSz(ns), sz, dooff.data(), d);
}

void ObjectSEGY::readDOMD(
  const size_t* offset,
  const size_t ns,
  const size_t sz,
  unsigned char* md) const
{
    std::vector<size_t> dooff(sz);

    for (size_t i = 0; i < sz; i++) {
        dooff[i] = SEGY_utils::getDOLoc(offset[i], ns);
    }

    data_->read(SEGY_utils::getMDSz(), sz, dooff.data(), md);
}

void ObjectSEGY::writeDOMD(
  const size_t* offset,
  const size_t ns,
  const size_t sz,
  const unsigned char* md) const
{
    std::vector<size_t> dooff(sz);

    for (size_t i = 0; i < sz; i++) {
        dooff[i] = SEGY_utils::getDOLoc(offset[i], ns);
    }

    data_->write(SEGY_utils::getMDSz(), sz, dooff.data(), md);
}

void ObjectSEGY::readDODF(
  const size_t* offset,
  const size_t ns,
  const size_t sz,
  unsigned char* df) const
{
    if (ns == 0) {
        return;
    }

    std::vector<size_t> dooff(sz);

    for (size_t i = 0; i < sz; i++) {
        dooff[i] = SEGY_utils::getDODFLoc(offset[i], ns);
    }

    data_->read(SEGY_utils::getDFSz(ns), sz, dooff.data(), df);
}

void ObjectSEGY::writeDODF(
  const size_t* offset,
  const size_t ns,
  const size_t sz,
  const unsigned char* df) const
{
    if (ns == 0) {
        return;
    }

    std::vector<size_t> dooff(sz);

    for (size_t i = 0; i < sz; i++) {
        dooff[i] = SEGY_utils::getDODFLoc(offset[i], ns);
    }

    data_->write(SEGY_utils::getDFSz(ns), sz, dooff.data(), df);
}

}  // namespace PIOL
}  // namespace exseis
