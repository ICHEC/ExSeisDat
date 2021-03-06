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
#include "ExSeisDat/utils/encoding/number_encoding.hh"

#include <cstring>
#include <limits>

using namespace exseis::utils;
using namespace exseis::PIOL::SEGY_utils;

namespace exseis {
namespace PIOL {

//////////////////////      Constructor & Destructor      //////////////////////

WriteSEGY::Opt::Opt(void)
{
    const double microsecond = 1e-6;
    incFactor                = 1 * microsecond;
}

WriteSEGY::WriteSEGY(
  std::shared_ptr<ExSeisPIOL> piol_,
  const std::string name_,
  const WriteSEGY::Opt& opt,
  std::shared_ptr<ObjectInterface> obj_) :
    WriteInterface(piol_, name_, obj_),
    incFactor(opt.incFactor)
{
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
    // TODO: On error this can be a source of a deadlock
    if (!piol->log->isErr()) {
        calcNt();

        if (state.resize) {
            obj->setFileSz(SEGY_utils::getFileSz(nt, ns));
        }

        if (state.writeHO) {
            // Write file header on rank 0.
            if (piol->comm->getRank() == 0) {
                // The buffer to build the header in
                std::vector<unsigned char> header_buffer(SEGY_utils::getHOSz());

                // Write the text header into the start of the buffer.
                std::copy(
                  std::begin(text), std::end(text), std::begin(header_buffer));

                // Write ns, the number format, and the interval
                const auto be_ns = to_big_endian<int16_t>(ns);
                std::copy(
                  std::begin(be_ns), std::end(be_ns),
                  &header_buffer[SEGYFileHeaderByte::NumSample]);

                const auto be_format = to_big_endian<int16_t>(
                  static_cast<int16_t>(SEGYNumberFormat::IEEE));
                std::copy(
                  std::begin(be_format), std::end(be_format),
                  &header_buffer[SEGYFileHeaderByte::Type]);

                const auto be_interval =
                  to_big_endian<int16_t>(std::lround(inc / incFactor));
                std::copy(
                  std::begin(be_interval), std::end(be_interval),
                  &header_buffer[SEGYFileHeaderByte::Interval]);

                // Currently these are hard-coded entries:
                // The unit system.
                const auto be_units = to_big_endian<int16_t>(0x0001);
                std::copy(
                  std::begin(be_units), std::end(be_units),
                  &header_buffer[SEGYFileHeaderByte::Units]);

                // The version of the SEGY format.
                const auto be_segy_format = to_big_endian<int16_t>(0x0100);
                std::copy(
                  std::begin(be_segy_format), std::end(be_segy_format),
                  &header_buffer[SEGYFileHeaderByte::SEGYFormat]);

                // We always deal with fixed traces at present.
                const auto be_fixed_trace = to_big_endian<int16_t>(0x0001);
                std::copy(
                  std::begin(be_fixed_trace), std::end(be_fixed_trace),
                  &header_buffer[SEGYFileHeaderByte::FixedTrace]);

                // We do not support text extensions at present.
                const auto be_extensions = to_big_endian<int16_t>(0x0000);
                std::copy(
                  std::begin(be_extensions), std::end(be_extensions),
                  &header_buffer[SEGYFileHeaderByte::Extensions]);

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
        text.resize(SEGY_utils::getTextSz());
        state.writeHO = true;
    }
}

void WriteSEGY::writeNs(const size_t ns_)
{
    if (ns_ > size_t(std::numeric_limits<int16_t>::max())) {
        piol->log->record(
          name, Logger::Layer::File, Logger::Status::Error,
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

void WriteSEGY::writeInc(const exseis::utils::Floating_point inc_)
{
    if (std::isnormal(inc_) == false) {
        piol->log->record(
          name, Logger::Layer::File, Logger::Status::Error,
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
  exseis::utils::Trace_value* trc,
  const Param* prm,
  const size_t skip)
{
    unsigned char* tbuf = reinterpret_cast<unsigned char*>(trc);


    // Convert from host-endianness to SEGY endianness for writing

    if (trc != TRACE_NULL && trc != nullptr) {
        for (size_t i = 0; i < ns * sz; i++) {
            const auto be_trc_i_bytes =
              to_big_endian<exseis::utils::Trace_value>(trc[i]);

            unsigned char* trc_i_bytes =
              reinterpret_cast<unsigned char*>(&trc[i]);

            std::copy(
              std::begin(be_trc_i_bytes), std::end(be_trc_i_bytes),
              trc_i_bytes);
        }
    }


    if (prm == PIOL_PARAM_NULL) {
        obj->writeDODF(offset, ns, sz, tbuf);
    }
    else {
        const size_t blockSz =
          (trc == TRACE_NULL ? SEGY_utils::getMDSz() : SEGY_utils::getDOSz(ns));

        std::vector<unsigned char> alloc(blockSz * sz);
        unsigned char* buf = (sz ? alloc.data() : nullptr);

        SEGY_utils::insertParam(
          sz, prm, buf, blockSz - SEGY_utils::getMDSz(), skip);

        if (trc == TRACE_NULL) {
            obj->writeDOMD(offset, ns, sz, buf);
        }
        else {
            for (size_t i = 0; i < sz; i++) {
                std::copy(
                  &tbuf[i * SEGY_utils::getDFSz(ns)],
                  &tbuf[(i + 1) * SEGY_utils::getDFSz(ns)],
                  &buf[i * SEGY_utils::getDOSz(ns) + SEGY_utils::getMDSz()]);
            }

            obj->writeDO(offset, ns, sz, buf);
        }
    }


    // Convert back from SEGY endianness to native endianness, as it was
    // before calling this routine

    if (trc != TRACE_NULL && trc != nullptr) {
        for (size_t i = 0; i < ns * sz; i++) {
            const exseis::utils::Trace_value be_trc_i = trc[i];

            const unsigned char* be_trc_i_bytes =
              reinterpret_cast<const unsigned char*>(&be_trc_i);

            trc[i] = from_big_endian<exseis::utils::Trace_value>(
              be_trc_i_bytes[0], be_trc_i_bytes[1], be_trc_i_bytes[2],
              be_trc_i_bytes[3]);
        }
    }
}


void WriteSEGY::writeTrace(
  const size_t offset,
  const size_t sz,
  exseis::utils::Trace_value* trc,
  const Param* prm,
  const size_t skip)
{
    if (!nsSet) {
        piol->log->record(
          name, Logger::Layer::File, Logger::Status::Error,
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
  exseis::utils::Trace_value* trc,
  const Param* prm,
  const size_t skip)
{
    if (!nsSet) {
        piol->log->record(
          name, Logger::Layer::File, Logger::Status::Error,
          "The number of samples per trace (ns) has not been set. The output is probably erroneous.",
          PIOL_VERBOSITY_NONE);
    }

    writeTraceT(obj.get(), ns, offset, sz, trc, prm, skip);
    state.stalent = true;
    if (sz != 0) {
        nt = std::max(offset[sz - 1LU] + 1LU, nt);
    }
}

}  // namespace PIOL
}  // namespace exseis
