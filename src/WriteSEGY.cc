////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief
/// @details WriteSEGY functions
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/WriteSEGY.hh"

#include "ExSeisDat/PIOL/ObjectInterface.hh"
#include "ExSeisDat/PIOL/segy_utils.hh"
#include "ExSeisDat/PIOL/share/units.hh"

#include <cstring>
#include <limits>

namespace PIOL {

//////////////////////      Constructor & Destructor      //////////////////////

WriteSEGY::Opt::Opt(void)
{
    incFactor = SI::Micro;
}

WriteSEGY::WriteSEGY(
  std::shared_ptr<ExSeisPIOL> piol_,
  const std::string name_,
  const WriteSEGY::Opt& opt,
  std::shared_ptr<ObjectInterface> obj_) :
    WriteInterface(piol_, name_, obj_),
    incFactor(opt.incFactor)
{
    using namespace SEGY_utils;

    memset(&state, 0, sizeof(Flags));
    state.writeHO = true;
}

WriteSEGY::WriteSEGY(
  std::shared_ptr<ExSeisPIOL> piol_,
  const std::string name_,
  std::shared_ptr<ObjectInterface> obj_) :
    WriteSEGY(piol_, name_, WriteSEGY::Opt(), obj_)
{
}

WriteSEGY::~WriteSEGY(void)
{
    using namespace SEGY_utils;

    // TODO: On error this can be a source of a deadlock
    if (!piol->log->isErr()) {
        calcNt();

        if (state.resize) {
            obj->setFileSz(SEGSz::getFileSz(nt, ns));
        }

        if (state.writeHO) {
            // Write file header on rank 0.
            if (piol->comm->getRank() == 0) {
                // The buffer to build the header in
                std::vector<uchar> header_buffer(SEGSz::getHOSz());

                // Write the text header into the start of the buffer.
                std::copy(
                  std::begin(text), std::end(text), std::begin(header_buffer));

                // Write ns, the number format, and the interval
                getBigEndian<int16_t>(
                  ns, &header_buffer[SEGYFileHeaderByte::NumSample]);
                getBigEndian<int16_t>(
                  static_cast<int16_t>(SEGYNumberFormat::IEEE),
                  &header_buffer[SEGYFileHeaderByte::Type]);
                getBigEndian<int16_t>(
                  std::lround(inc / incFactor),
                  &header_buffer[SEGYFileHeaderByte::Interval]);

                // Currently these are hard-coded entries:
                // The unit system.
                getBigEndian<int16_t>(
                  0x0001, &header_buffer[SEGYFileHeaderByte::Units]);

                // The version of the SEGY format.
                getBigEndian<int16_t>(
                  0x0100, &header_buffer[SEGYFileHeaderByte::SEGYFormat]);

                // We always deal with fixed traces at present.
                getBigEndian<int16_t>(
                  0x0001, &header_buffer[SEGYFileHeaderByte::FixedTrace]);

                // We do not support text extensions at present.
                getBigEndian<int16_t>(
                  0x0000, &header_buffer[SEGYFileHeaderByte::Extensions]);

                // Write the header from the buffer
                obj->writeHO(header_buffer.data());
            }
            else {
                obj->writeHO(NULL);
            }
        }
    }
}

//////////////////////////       Member functions      /////////////////////////

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
          "The SEG-Y Interval " + std::to_string(inc_) + " is not normal.",
          PIOL_VERBOSITY_NONE);
        return;
    }

    if (inc != inc_) {
        inc           = inc_;
        state.writeHO = true;
    }
}

/*! Template function for writing SEG-Y traces and parameters, random and
 *  contiguous.
 *  @tparam T The type of offset (pointer or size_t)
 *  @param[in] obj The object-layer object.
 *  @param[in] ns The number of samples per trace.
 *  @param[in] offset The offset(s). If T == size_t * this is an array,
 *                    otherwise its a single offset.
 *  @param[in] sz The number of traces to write
 *  @param[in] trc Pointer to trace array.
 *  @param[in] prm Pointer to parameter structure.
 *  @param[in] skip Skip \c skip entries in the parameter structure
 */
template<typename T>
void writeTraceT(
  ObjectInterface* obj,
  const size_t ns,
  T offset,
  const size_t sz,
  trace_t* trc,
  const Param* prm,
  const size_t skip)
{
    uchar* tbuf = reinterpret_cast<uchar*>(trc);

    if (trc != TRACE_NULL && trc != nullptr) {
        for (size_t i = 0; i < ns * sz; i++) {
            reverse4Bytes(&tbuf[i * sizeof(float)]);
        }
    }

    if (prm == PIOL_PARAM_NULL) {
        obj->writeDODF(offset, ns, sz, tbuf);
    }
    else {
        const size_t blockSz =
          (trc == TRACE_NULL ? SEGSz::getMDSz() : SEGSz::getDOSz(ns));
        std::vector<uchar> alloc(blockSz * sz);
        uchar* buf = (sz ? alloc.data() : nullptr);
        SEGY_utils::insertParam(sz, prm, buf, blockSz - SEGSz::getMDSz(), skip);

        if (trc == TRACE_NULL) {
            obj->writeDOMD(offset, ns, sz, buf);
        }
        else {
            for (size_t i = 0; i < sz; i++) {
                std::copy(
                  &tbuf[i * SEGSz::getDFSz(ns)],
                  &tbuf[(i + 1) * SEGSz::getDFSz(ns)],
                  &buf[i * SEGSz::getDOSz(ns) + SEGSz::getMDSz()]);
            }
            obj->writeDO(offset, ns, sz, buf);
        }
    }

    if (trc != TRACE_NULL && trc != nullptr) {
        for (size_t i = 0; i < ns * sz; i++) {
            reverse4Bytes(&tbuf[i * sizeof(float)]);
        }
    }
}

void WriteSEGY::writeTrace(
  const size_t offset,
  const size_t sz,
  trace_t* trc,
  const Param* prm,
  const size_t skip)
{
    if (!nsSet) {
        piol->log->record(
          name, Log::Layer::File, Log::Status::Error,
          "The number of samples per trace (ns) has not been set. The output is probably erroneous.",
          PIOL_VERBOSITY_NONE);
    }

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
    if (!nsSet) {
        piol->log->record(
          name, Log::Layer::File, Log::Status::Error,
          "The number of samples per trace (ns) has not been set. The output is probably erroneous.",
          PIOL_VERBOSITY_NONE);
    }

    writeTraceT(obj.get(), ns, offset, sz, trc, prm, skip);
    state.stalent = true;
    if (sz) {
        nt = std::max(offset[sz - 1LU] + 1LU, nt);
    }
}

}  // namespace PIOL
