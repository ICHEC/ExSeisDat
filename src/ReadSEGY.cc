////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief
/// @details ReadSEGY functions
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/file/ReadSEGY.hh"

#include "ExSeisDat/PIOL/file/characterconversion.hh"
#include "ExSeisDat/PIOL/file/dynsegymd.hh"
#include "ExSeisDat/PIOL/file/segymd.hh"
#include "ExSeisDat/PIOL/share/misc.hh"
#include "ExSeisDat/PIOL/share/segy.hh"
#include "ExSeisDat/PIOL/share/units.hh"

#include <algorithm>
#include <cassert>

namespace PIOL {
namespace File {

//////////////////////      Constructor & Destructor      //////////////////////
ReadSEGY::Opt::Opt(void)
{
    incFactor = SI::Micro;
}

ReadSEGY::ReadSEGY(
  std::shared_ptr<ExSeisPIOL> piol_,
  const std::string name_,
  const ReadSEGY::Opt& opt,
  std::shared_ptr<Obj::Interface> obj_) :
    ReadInterface(piol_, name_, obj_),
    format(Format::IEEE),
    incFactor(opt.incFactor)
{
    size_t hoSz = SEGSz::getHOSz();
    size_t fsz  = obj->getFileSz();

    // Read the global header data, if there is any.
    if (fsz >= hoSz) {

        // Read the header into buf
        auto buf = std::vector<uchar>(hoSz);
        obj->readHO(buf.data());

        // Read the number of samples, traces, the increment and the format
        // from buf.
        ns     = getMd(Hdr::NumSample, buf.data());
        nt     = SEGSz::getNt(fsz, ns);
        inc    = geom_t(getMd(Hdr::Increment, buf.data())) * incFactor;
        format = static_cast<Format>(getMd(Hdr::Type, buf.data()));

        // Set this->text to the ASCII encoding of the text header data read
        // into buf.
        // Determine if the current encoding is ASCII or EBCDIC from number of
        // printable ASCII or EBCDIC characters in the string.

        // Text header bounds
        assert(buf.size() > SEGSz::getTextSz());
        const auto text_header_begin = std::begin(buf);
        const auto text_header_end   = text_header_begin + SEGSz::getTextSz();

        // Count printable ASCII
        const size_t n_printable_ascii =
          std::count_if(text_header_begin, text_header_end, is_printable_ASCII);

        // Count printable EBCDIC
        const size_t n_printable_ebcdic = std::count_if(
          text_header_begin, text_header_end, is_printable_EBCDIC);

        if (n_printable_ascii > n_printable_ebcdic) {
            // The string is in ASCII, copy it.
            std::copy(
              text_header_begin, text_header_end, std::back_inserter(text));
        }
        else {
            // The string is in EBCDIC, transform and copy it.
            std::transform(
              text_header_begin, text_header_end, std::back_inserter(text),
              ebcdicToAscii);
        }
    }
}

ReadSEGY::ReadSEGY(
  std::shared_ptr<ExSeisPIOL> piol_,
  const std::string name_,
  std::shared_ptr<Obj::Interface> obj_) :
    ReadSEGY(piol_, name_, ReadSEGY::Opt(), obj_)
{
}

size_t ReadSEGY::readNt(void) const
{
    return nt;
}

/*! Template function for reading SEG-Y traces and parameters, random and
 *  contiguous.
 *  @tparam T The type of offset (pointer or size_t)
 *  @param[in] obj The object-layer object.
 *  @param[in] format The format of the trace data.
 *  @param[in] ns The number of samples per trace.
 *  @param[in] offset The offset(s). If T == size_t * this is an array,
 *                    otherwise its a single offset.
 *  @param[in] offunc A function which given the ith trace of the local process,
 *                    returns the associated trace offset.
 *  @param[in] sz The number of traces to read
 *  @param[in] trc Pointer to trace array.
 *  @param[in] prm Pointer to parameter structure.
 *  @param[in] skip Skip \c skip entries in the parameter structure
 */
template<typename T>
void readTraceT(
  Obj::Interface* obj,
  const Format format,
  const size_t ns,
  const T offset,
  std::function<size_t(size_t)> offunc,
  const size_t sz,
  trace_t* trc,
  Param* prm,
  const size_t skip)
{
    uchar* tbuf = reinterpret_cast<uchar*>(trc);

    if (prm == PIOL_PARAM_NULL) {
        obj->readDODF(offset, ns, sz, tbuf);
    }
    else {
        const size_t blockSz =
          (trc == TRACE_NULL ? SEGSz::getMDSz() : SEGSz::getDOSz(ns));

        std::vector<uchar> alloc(blockSz * sz);
        uchar* buf = (sz ? alloc.data() : nullptr);

        if (trc == TRACE_NULL) {
            obj->readDOMD(offset, ns, sz, buf);
        }
        else {
            obj->readDO(offset, ns, sz, buf);

            for (size_t i = 0; i < sz; i++) {
                std::copy(
                  &buf[i * SEGSz::getDOSz(ns) + SEGSz::getMDSz()],
                  &buf[(i + 1) * SEGSz::getDOSz(ns)],
                  &tbuf[i * SEGSz::getDFSz(ns)]);
            }
        }

        extractParam(
          sz, buf, prm, (trc != TRACE_NULL ? SEGSz::getDFSz(ns) : 0LU), skip);

        for (size_t i = 0; i < sz; i++) {
            setPrm(i + skip, PIOL_META_ltn, offunc(i), prm);
        }
    }

    if (trc != TRACE_NULL && trc != nullptr) {
        if (format == Format::IBM) {
            for (size_t i = 0; i < ns * sz; i++) {
                trc[i] = convertIBMtoIEEE(trc[i], true);
            }
        }
        else {
            for (size_t i = 0; i < ns * sz; i++) {
                reverse4Bytes(&tbuf[i * sizeof(float)]);
            }
        }
    }
}

void ReadSEGY::readTrace(
  const size_t offset,
  const size_t sz,
  trace_t* trc,
  Param* prm,
  const size_t skip) const
{
    size_t ntz = (!sz ? sz : (offset + sz > nt ? nt - offset : sz));
    if (offset >= nt && sz) {
        // Nothing to be read.
        piol->log->record(
          name, Log::Layer::File, Log::Status::Warning,
          "readParam() was called for a zero byte read", PIOL_VERBOSITY_NONE);
    }
    readTraceT(
      obj.get(), format, ns, offset,
      [offset](size_t i) -> size_t { return offset + i; }, ntz, trc, prm, skip);
}

void ReadSEGY::readTraceNonContiguous(
  const size_t sz,
  const size_t* offset,
  trace_t* trc,
  Param* prm,
  const size_t skip) const
{
    readTraceT(
      obj.get(), format, ns, offset,
      [offset](size_t i) -> size_t { return offset[i]; }, sz, trc, prm, skip);
}

void ReadSEGY::readTraceNonMonotonic(
  const size_t sz,
  const size_t* offset,
  trace_t* trc,
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

    File::Param sprm(prm->r, (prm != PIOL_PARAM_NULL ? nodups.size() : 0LU));
    std::vector<trace_t> strc(ns * (trc != TRACE_NULL ? nodups.size() : 0LU));

    readTraceNonContiguous(
      nodups.size(), nodups.data(), (trc != TRACE_NULL ? strc.data() : trc),
      (prm != PIOL_PARAM_NULL ? &sprm : prm), 0LU);

    if (prm != PIOL_PARAM_NULL)
        for (size_t n = 0, j = 0; j < sz; ++j) {
            n += (j && offset[idx[j - 1]] != offset[idx[j]]);
            cpyPrm(n, &sprm, skip + idx[j], prm);
        }

    if (trc != TRACE_NULL)
        for (size_t n = 0, j = 0; j < sz; ++j) {
            n += (j && offset[idx[j - 1]] != offset[idx[j]]);
            for (size_t k = 0; k < ns; k++)
                trc[idx[j] * ns + k] = strc[n * ns + k];
        }
}

}  // namespace File
}  // namespace PIOL
