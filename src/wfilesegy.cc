/*******************************************************************************************//*!
 *   @file
 *   @author Cathal O Broin - cathal@ichec.ie - first commit
 *   @copyright TBD. Do not distribute
 *   @date July 2016
 *   @brief
 *   @details WriteSEGY functions
 *//*******************************************************************************************/

#include "file/filesegy.hh"
#include "global.hh"
#include "object/object.hh"
#include "share/misc.hh"

#include <cmath>
#include <cstring>
#include <limits>

namespace PIOL {
namespace File {

///////////////////////////////      Constructor & Destructor      ///////////////////////////////
WriteSEGY::Opt::Opt(void)
{
    incFactor = SI::Micro;
}

WriteSEGY::WriteSEGY(
  std::shared_ptr<ExSeisPIOL> piol_,
  const std::string name_,
  const WriteSEGY::Opt& opt,
  std::shared_ptr<Obj::Interface> obj_) :
    WriteInterface(piol_, name_, obj_)
{
    Init(opt);
}

WriteSEGY::WriteSEGY(
  std::shared_ptr<ExSeisPIOL> piol_,
  const std::string name_,
  std::shared_ptr<Obj::Interface> obj_) :
    WriteInterface(piol_, name_, obj_)
{
    WriteSEGY::Opt opt;
    Init(opt);
}

WriteSEGY::~WriteSEGY(void)
{
    if (!piol->log
           ->isErr())  //TODO: On error this can be a source of a deadlock
    {
        calcNt();
        if (state.resize) obj->setFileSz(SEGSz::getFileSz(nt, ns));
        if (state.writeHO) {
            if (!piol->comm->getRank()) {
                std::vector<uchar> buf(SEGSz::getHOSz());
                packHeader(buf.data());
                obj->writeHO(buf.data());
            }
            else
                obj->writeHO(NULL);
        }
    }
}

///////////////////////////////////       Member functions      ///////////////////////////////////
void WriteSEGY::packHeader(uchar* buf) const
{
    for (size_t i = 0; i < text.size(); i++)
        buf[i] = text[i];

    setMd(Hdr::NumSample, int16_t(ns), buf);
    setMd(Hdr::Type, int16_t(Format::IEEE), buf);
    setMd(Hdr::Increment, int16_t(std::lround(inc / incFactor)), buf);

    //Currently these are hard-coded entries:
    setMd(Hdr::Units, 0x0001, buf);       //The unit system.
    setMd(Hdr::SEGYFormat, 0x0100, buf);  //The version of the SEGY format.
    setMd(
      Hdr::FixedTrace, 0x0001,
      buf);  //We always deal with fixed traces at present.
    setMd(
      Hdr::Extensions, 0x0000,
      buf);  //We do not support text extensions at present.
}

void WriteSEGY::Init(const WriteSEGY::Opt& opt)
{
    incFactor = opt.incFactor;
    memset(&state, 0, sizeof(Flags));
    format        = Format::IEEE;
    ns            = 0LU;
    nt            = 0LU;
    inc           = geom_t(0);
    text          = "";
    state.writeHO = true;
}

size_t WriteSEGY::calcNt(void)
{
    if (state.stalent) {
        nt            = piol->comm->max(nt);
        state.stalent = false;
        state.resize  = true;
    }
    return nt;
}

void WriteSEGY::writeText(const std::string text_)
{
    if (text != text_) {
        text = text_;
        text.resize(SEGSz::getTextSz());
        state.writeHO = true;
    }
}

void WriteSEGY::writeNs(const size_t ns_)
{
    if (ns_ > size_t(std::numeric_limits<int16_t>::max())) {
        piol->log->record(
          name, Log::Layer::File, Log::Status::Error,
          "Ns value is too large for SEG-Y", PIOL_VERBOSITY_NONE);
        return;
    }

    if (ns != ns_) {
        ns            = ns_;
        state.resize  = true;
        state.writeHO = true;
    }
    nsSet = true;
}

void WriteSEGY::writeNt(const size_t nt_)
{
    if (nt != nt_) {
        nt           = nt_;
        state.resize = true;
    }
    state.stalent = false;
}

void WriteSEGY::writeInc(const geom_t inc_)
{
    if (std::isnormal(inc_) == false) {
        piol->log->record(
          name, Log::Layer::File, Log::Status::Error,
          "The SEG-Y Increment " + std::to_string(inc_) + " is not normal.",
          PIOL_VERBOSITY_NONE);
        return;
    }

    if (inc != inc_) {
        inc           = inc_;
        state.writeHO = true;
    }
}

/*! Template function for writing SEG-Y traces and parameters, random and contiguous.
 *  @tparam T The type of offset (pointer or size_t)
 *  @param[in] obj The object-layer object.
 *  @param[in] ns The number of samples per trace.
 *  @param[in] offset The offset(s). If T == size_t * this is an array, otherwise its a single offset.
 *  @param[in] sz The number of traces to write
 *  @param[in] trc Pointer to trace array.
 *  @param[in] prm Pointer to parameter structure.
 *  @param[in] skip Skip \c skip entries in the parameter structure
 */
template<typename T>
void writeTraceT(
  Obj::Interface* obj,
  const size_t ns,
  T offset,
  const size_t sz,
  trace_t* trc,
  const Param* prm,
  const size_t skip)
{
    uchar* tbuf = reinterpret_cast<uchar*>(trc);
    if (trc != TRACE_NULL && trc != nullptr)
        for (size_t i = 0; i < ns * sz; i++)
            reverse4Bytes(&tbuf[i * sizeof(float)]);

    if (prm == PIOL_PARAM_NULL)
        obj->writeDODF(offset, ns, sz, tbuf);
    else {
        const size_t blockSz =
          (trc == TRACE_NULL ? SEGSz::getMDSz() : SEGSz::getDOSz(ns));
        std::vector<uchar> alloc(blockSz * sz);
        uchar* buf = (sz ? alloc.data() : nullptr);
        insertParam(sz, prm, buf, blockSz - SEGSz::getMDSz(), skip);

        if (trc == TRACE_NULL)
            obj->writeDOMD(offset, ns, sz, buf);
        else {
            for (size_t i = 0; i < sz; i++)
                std::copy(
                  &tbuf[i * SEGSz::getDFSz(ns)],
                  &tbuf[(i + 1) * SEGSz::getDFSz(ns)],
                  &buf[i * SEGSz::getDOSz(ns) + SEGSz::getMDSz()]);
            obj->writeDO(offset, ns, sz, buf);
        }
    }

    if (trc != TRACE_NULL && trc != nullptr)
        for (size_t i = 0; i < ns * sz; i++)
            reverse4Bytes(&tbuf[i * sizeof(float)]);
}

void WriteSEGY::writeTrace(
  const size_t offset,
  const size_t sz,
  trace_t* trc,
  const Param* prm,
  const size_t skip)
{
    if (!nsSet)
        piol->log->record(
          name, Log::Layer::File, Log::Status::Error,
          "The number of samples per trace (ns) has not been set. The output is probably erroneous.",
          PIOL_VERBOSITY_NONE);

    writeTraceT(obj.get(), ns, offset, sz, trc, prm, skip);
    state.stalent = true;
    nt            = std::max(offset + sz, nt);
}

void WriteSEGY::writeTraceNonContiguous(
  const size_t sz,
  const size_t* offset,
  trace_t* trc,
  const Param* prm,
  const size_t skip)
{
    if (!nsSet)
        piol->log->record(
          name, Log::Layer::File, Log::Status::Error,
          "The number of samples per trace (ns) has not been set. The output is probably erroneous.",
          PIOL_VERBOSITY_NONE);

    writeTraceT(obj.get(), ns, offset, sz, trc, prm, skip);
    state.stalent = true;
    if (sz) nt = std::max(offset[sz - 1LU] + 1LU, nt);
}

}  // namespace File
}  // namespace PIOL
