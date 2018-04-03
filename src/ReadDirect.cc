////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation for \c ReadDirect
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/ReadDirect.hh"

#include "ExSeisDat/PIOL/DataMPIIO.hh"
#include "ExSeisDat/PIOL/ExSeisPIOL.hh"
#include "ExSeisDat/PIOL/ObjectSEGY.hh"
#include "ExSeisDat/PIOL/ReadSEGY.hh"

namespace PIOL {

ReadDirect::ReadDirect(std::shared_ptr<ExSeisPIOL> piol, const std::string name)
{
    const ReadSEGY::Opt f;
    const ObjectSEGY::Opt o;
    const DataMPIIO::Opt d;
    auto data = std::make_shared<DataMPIIO>(piol, name, d, FileMode::Read);
    auto obj =
      std::make_shared<ObjectSEGY>(piol, name, o, data, FileMode::Read);
    file = std::make_shared<ReadSEGY>(piol, name, f, obj);
}

ReadDirect::ReadDirect(std::shared_ptr<ReadInterface> file_) : file(file_) {}

ReadDirect::~ReadDirect() = default;

const std::string& ReadDirect::readText(void) const
{
    return file->readText();
}

size_t ReadDirect::readNs(void) const
{
    return file->readNs();
}

size_t ReadDirect::readNt(void) const
{
    return file->readNt();
}

geom_t ReadDirect::readInc(void) const
{
    return file->readInc();
}

void ReadDirect::readParam(
  const size_t offset, const size_t sz, Param* prm) const
{
    file->readParam(offset, sz, prm);
}

void ReadDirect::readTrace(
  const size_t offset, const size_t sz, trace_t* trace, Param* prm) const
{
    file->readTrace(offset, sz, trace, prm);
}

void ReadDirect::readTraceNonContiguous(
  const size_t sz, const size_t* offset, trace_t* trace, Param* prm) const
{
    file->readTraceNonContiguous(sz, offset, trace, prm);
}

void ReadDirect::readTraceNonMonotonic(
  const size_t sz, const size_t* offset, trace_t* trace, Param* prm) const
{
    file->readTraceNonMonotonic(sz, offset, trace, prm);
}

void ReadDirect::readParamNonContiguous(
  const size_t sz, const size_t* offset, Param* prm) const
{
    file->readParamNonContiguous(sz, offset, prm);
}

}  // namespace PIOL
