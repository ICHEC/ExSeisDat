////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief
/// @details ReadSEGY functions
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/ReadSEGY.hh"

#include "ExSeisDat/PIOL/ObjectInterface.hh"
#include "ExSeisDat/PIOL/operations/sort.hh"
#include "ExSeisDat/PIOL/param_utils.hh"
#include "ExSeisDat/PIOL/segy_utils.hh"
#include "ExSeisDat/utils/encoding/character_encoding.hh"
#include "ExSeisDat/utils/encoding/number_encoding.hh"

#include <algorithm>
#include <cassert>

using namespace exseis::utils;

namespace exseis {
namespace PIOL {

//////////////////////      Constructor & Destructor      //////////////////////
ReadSEGY::Opt::Opt(void)
{
    const double microsecond = 1e-6;
    incFactor                = 1 * microsecond;
}

ReadSEGY::ReadSEGY(
  std::shared_ptr<ExSeisPIOL> piol_,
  const std::string name_,
  const ReadSEGY::Opt& opt,
  std::shared_ptr<ObjectInterface> obj_) :
    ReadInterface(piol_, name_, obj_),
    incFactor(opt.incFactor)
{
    using namespace SEGY_utils;

    size_t hoSz = SEGY_utils::getHOSz();
    size_t fsz  = obj->getFileSz();

    // Read the global header data, if there is any.
    if (fsz >= hoSz) {

        // Read the header into header_buffer
        auto header_buffer = std::vector<unsigned char>(hoSz);
        obj->readHO(header_buffer.data());

        // Parse the number of samples, traces, the increment and the format
        // from header_buffer.
        ns = from_big_endian<int16_t>(
          header_buffer[SEGYFileHeaderByte::NumSample + 0],
          header_buffer[SEGYFileHeaderByte::NumSample + 1]);

        nt = SEGY_utils::getNt(fsz, ns);

        inc = incFactor
              * exseis::utils::Floating_point(from_big_endian<int16_t>(
                  header_buffer[SEGYFileHeaderByte::Interval + 0],
                  header_buffer[SEGYFileHeaderByte::Interval + 1]));

        number_format = static_cast<SEGYNumberFormat>(from_big_endian<int16_t>(
          header_buffer[SEGYFileHeaderByte::Type + 0],
          header_buffer[SEGYFileHeaderByte::Type + 1]));

        // Set this->text to the ASCII encoding of the text header data read
        // into header_buffer.
        // Determine if the current encoding is ASCII or EBCDIC from number of
        // printable ASCII or EBCDIC characters in the string.

        // Text header buffer bounds
        assert(header_buffer.size() >= SEGY_utils::getTextSz());
        const auto text_header_begin = std::begin(header_buffer);
        const auto text_header_end =
          text_header_begin + SEGY_utils::getTextSz();

        // Count printable ASCII
        const auto n_printable_ascii =
          std::count_if(text_header_begin, text_header_end, is_printable_ASCII);

        // Count printable EBCDIC
        const auto n_printable_ebcdic = std::count_if(
          text_header_begin, text_header_end, is_printable_EBCDIC);

        // Set text object to correct size in preparation for setting
        text.resize(SEGY_utils::getTextSz());

        if (n_printable_ascii > n_printable_ebcdic) {
            // The string is in ASCII, copy it.
            std::copy(text_header_begin, text_header_end, std::begin(text));
        }
        else {
            // The string is in EBCDIC, transform and copy it.
            std::transform(
              text_header_begin, text_header_end, std::begin(text),
              to_ASCII_from_EBCDIC);
        }
    }
}

ReadSEGY::ReadSEGY(
  std::shared_ptr<ExSeisPIOL> piol_,
  const std::string name_,
  std::shared_ptr<ObjectInterface> obj_) :
    ReadSEGY(piol_, name_, ReadSEGY::Opt(), obj_)
{
}

size_t ReadSEGY::readNt(void) const
{
    return nt;
}

/*! Template function for reading SEG-Y traces and parameters, random and
 *  contiguous.
 *  @tparam T                The type of offset (pointer or size_t)
 *  @param[in] obj           The object-layer object.
 *  @param[in] number_format The format of the trace data.
 *  @param[in] ns            The number of samples per trace.
 *  @param[in] offset        The offset(s). If T == size_t * this is an array,
 *                           otherwise its a single offset.
 *  @param[in] offunc        A function which given the ith trace of the local
 *                           process, returns the associated trace offset.
 *  @param[in] sz            The number of traces to read
 *  @param[in] trc           Pointer to trace array.
 *  @param[in] prm           Pointer to parameter structure.
 *  @param[in] skip          Skip \c skip entries in the parameter structure
 */
template<typename T>
void readTraceT(
  ObjectInterface* obj,
  const SEGY_utils::SEGYNumberFormat number_format,
  const size_t ns,
  const T offset,
  std::function<size_t(size_t)> offunc,
  const size_t sz,
  exseis::utils::Trace_value* trc,
  Param* prm,
  const size_t skip)
{
    using namespace SEGY_utils;

    unsigned char* tbuf = reinterpret_cast<unsigned char*>(trc);

    if (prm == PIOL_PARAM_NULL) {
        obj->readDODF(offset, ns, sz, tbuf);
    }
    else {
        const size_t blockSz =
          (trc == TRACE_NULL ? SEGY_utils::getMDSz() : SEGY_utils::getDOSz(ns));

        std::vector<unsigned char> alloc(blockSz * sz);
        unsigned char* buf = (sz ? alloc.data() : nullptr);

        if (trc == TRACE_NULL) {
            obj->readDOMD(offset, ns, sz, buf);
        }
        else {
            obj->readDO(offset, ns, sz, buf);

            for (size_t i = 0; i < sz; i++) {
                std::copy(
                  &buf[i * SEGY_utils::getDOSz(ns) + SEGY_utils::getMDSz()],
                  &buf[(i + 1) * SEGY_utils::getDOSz(ns)],
                  &tbuf[i * SEGY_utils::getDFSz(ns)]);
            }
        }

        extractParam(
          sz, buf, prm, (trc != TRACE_NULL ? SEGY_utils::getDFSz(ns) : 0LU),
          skip);

        for (size_t i = 0; i < sz; i++) {
            param_utils::setPrm(i + skip, PIOL_META_ltn, offunc(i), prm);
        }
    }

    if (trc != TRACE_NULL && trc != nullptr) {

        // Convert trace values from SEGY floating point format to host format
        if (number_format == SEGYNumberFormat::IBM) {
            for (size_t i = 0; i < ns * sz; i++) {
                const exseis::utils::Floating_point be_trc_i = trc[i];

                const unsigned char* be_trc_i_bytes =
                  reinterpret_cast<const unsigned char*>(&be_trc_i);

                std::array<unsigned char, 4> be_bytes_array = {
                  {be_trc_i_bytes[0], be_trc_i_bytes[1], be_trc_i_bytes[2],
                   be_trc_i_bytes[3]}};

                trc[i] = from_IBM_to_float(be_bytes_array, true);
            }
        }
        else {
            for (size_t i = 0; i < ns * sz; i++) {
                trc[i] = from_big_endian<exseis::utils::Trace_value>(
                  tbuf[i * sizeof(exseis::utils::Trace_value) + 0],
                  tbuf[i * sizeof(exseis::utils::Trace_value) + 1],
                  tbuf[i * sizeof(exseis::utils::Trace_value) + 2],
                  tbuf[i * sizeof(exseis::utils::Trace_value) + 3]);
            }
        }
    }
}

void ReadSEGY::readTrace(
  const size_t offset,
  const size_t sz,
  exseis::utils::Trace_value* trc,
  Param* prm,
  const size_t skip) const
{
    size_t ntz = (!sz ? sz : (offset + sz > nt ? nt - offset : sz));
    if (offset >= nt && sz) {
        // Nothing to be read.
        piol->log->record(
          name, Logger::Layer::File, Logger::Status::Warning,
          "readParam() was called for a zero byte read", PIOL_VERBOSITY_NONE);
    }
    readTraceT(
      obj.get(), number_format, ns, offset,
      [offset](size_t i) -> size_t { return offset + i; }, ntz, trc, prm, skip);
}

void ReadSEGY::readTraceNonContiguous(
  const size_t sz,
  const size_t* offset,
  exseis::utils::Trace_value* trc,
  Param* prm,
  const size_t skip) const
{
    readTraceT(
      obj.get(), number_format, ns, offset,
      [offset](size_t i) -> size_t { return offset[i]; }, sz, trc, prm, skip);
}

void ReadSEGY::readTraceNonMonotonic(
  const size_t sz,
  const size_t* offset,
  exseis::utils::Trace_value* trc,
  Param* prm,
  const size_t skip) const
{
    // Sort the initial offset and make a new offset without duplicates
    auto idx = getSortIndex(sz, offset);
    std::vector<size_t> nodups;
    nodups.push_back(offset[idx[0]]);
    for (size_t j = 1; j < sz; j++)
        if (offset[idx[j - 1]] != offset[idx[j]])
            nodups.push_back(offset[idx[j]]);

    Param sprm(prm->r, (prm != PIOL_PARAM_NULL ? nodups.size() : 0LU));
    std::vector<exseis::utils::Trace_value> strc(
      ns * (trc != TRACE_NULL ? nodups.size() : 0LU));

    readTraceNonContiguous(
      nodups.size(), nodups.data(), (trc != TRACE_NULL ? strc.data() : trc),
      (prm != PIOL_PARAM_NULL ? &sprm : prm), 0LU);

    if (prm != PIOL_PARAM_NULL)
        for (size_t n = 0, j = 0; j < sz; ++j) {
            n += (j && offset[idx[j - 1]] != offset[idx[j]]);
            param_utils::cpyPrm(n, &sprm, skip + idx[j], prm);
        }

    if (trc != TRACE_NULL)
        for (size_t n = 0, j = 0; j < sz; ++j) {
            n += (j && offset[idx[j - 1]] != offset[idx[j]]);
            for (size_t k = 0; k < ns; k++)
                trc[idx[j] * ns + k] = strc[n * ns + k];
        }
}

}  // namespace PIOL
}  // namespace exseis
