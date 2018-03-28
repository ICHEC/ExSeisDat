////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation for \c WriteDirect
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/WriteDirect.hh"

#include "ExSeisDat/PIOL/data/datampiio.hh"
#include "ExSeisDat/PIOL/file/WriteSEGY.hh"
#include "ExSeisDat/PIOL/object/objsegy.hh"

namespace PIOL {
namespace File {

WriteDirect::WriteDirect(
  std::shared_ptr<ExSeisPIOL> piol, const std::string name)
{
    const File::WriteSEGY::Opt f;
    const Obj::SEGY::Opt o;
    const Data::MPIIO::Opt d;
    auto data = std::make_shared<Data::MPIIO>(piol, name, d, FileMode::Write);
    auto obj =
      std::make_shared<Obj::SEGY>(piol, name, o, data, FileMode::Write);
    file = std::make_shared<File::WriteSEGY>(piol, name, f, obj);
}

WriteDirect::WriteDirect(std::shared_ptr<WriteInterface> file_) : file(file_) {}

WriteDirect::~WriteDirect() = default;


void WriteDirect::writeParam(
  const size_t offset, const size_t sz, const Param* prm)
{
    file->writeParam(offset, sz, prm);
}

void WriteDirect::writeTrace(
  const size_t offset, const size_t sz, trace_t* trace, const Param* prm)
{
    file->writeTrace(offset, sz, trace, prm);
}

void WriteDirect::writeTraceNonContiguous(
  const size_t sz, const size_t* offset, trace_t* trace, const Param* prm)
{
    file->writeTraceNonContiguous(sz, offset, trace, prm);
}

void WriteDirect::writeParamNonContiguous(
  const size_t sz, const size_t* offset, const Param* prm)
{
    file->writeParamNonContiguous(sz, offset, prm);
}

void WriteDirect::writeText(const std::string text_)
{
    file->writeText(text_);
}

void WriteDirect::writeNs(const size_t ns_)
{
    file->writeNs(ns_);
}

void WriteDirect::writeNt(const size_t nt_)
{
    file->writeNt(nt_);
}

void WriteDirect::writeInc(const geom_t inc_)
{
    file->writeInc(inc_);
}

}  // namespace File
}  // namespace PIOL
