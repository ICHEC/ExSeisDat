////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief
/// @details ReadSEGY functions
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/piol/ReadSEGY.hh"

#include "exseisdat/piol/ObjectSEGY.hh"
#include "exseisdat/piol/mpi/MPI_Binary_file.hh"

#include "exseisdat/piol/operations/sort.hh"
#include "exseisdat/piol/segy/utils.hh"
#include "exseisdat/utils/encoding/character_encoding.hh"
#include "exseisdat/utils/encoding/number_encoding.hh"

#include <algorithm>
#include <cassert>

using namespace exseis::utils;

namespace exseis {
namespace piol {

//////////////////////      Constructor & Destructor      //////////////////////

ReadSEGY::ReadSEGY(
    std::shared_ptr<ExSeisPIOL> piol,
    const std::string name,
    const ReadSEGY::Options& options) :
    ReadSEGY(
        piol,
        name,
        options,
        std::make_shared<ObjectSEGY>(
            piol,
            name,
            std::make_shared<MPI_Binary_file>(piol, name, FileMode::Read)))
{
}

ReadSEGY::ReadSEGY(
    std::shared_ptr<ExSeisPIOL> piol,
    std::string name,
    const ReadSEGY::Options& options,
    std::shared_ptr<ObjectInterface> object) :
    m_piol(piol),
    m_name(name),
    m_obj(object),
    m_sample_interval_factor(options.sample_interval_factor)
{
    ReadSEGY::read_file_headers();
}

const std::string& ReadSEGY::file_name() const
{
    return m_name;
}

void ReadSEGY::read_file_headers()
{
    using namespace segy;

    size_t ho_sz = segy::segy_binary_file_header_size();
    size_t fsz   = m_obj->get_file_size();

    // Read the global header data, if there is any.
    if (fsz >= ho_sz) {

        // Read the header into header_buffer
        auto header_buffer = std::vector<unsigned char>(ho_sz);
        m_obj->read_ho(header_buffer.data());

        // Parse the number of samples, traces, the increment and the format
        // from header_buffer.
        m_ns = static_cast<size_t>(from_big_endian<int16_t>(
            header_buffer[SEGYFileHeaderByte::num_sample + 0],
            header_buffer[SEGYFileHeaderByte::num_sample + 1]));

        m_nt = segy::get_nt(fsz, m_ns);

        m_sample_interval =
            m_sample_interval_factor
            * exseis::utils::Floating_point(from_big_endian<int16_t>(
                header_buffer[SEGYFileHeaderByte::interval + 0],
                header_buffer[SEGYFileHeaderByte::interval + 1]));

        m_number_format =
            static_cast<SEGYNumberFormat>(from_big_endian<int16_t>(
                header_buffer[SEGYFileHeaderByte::type + 0],
                header_buffer[SEGYFileHeaderByte::type + 1]));

        // Set this->m_text to the ASCII encoding of the text header data read
        // into header_buffer.
        // Determine if the current encoding is ASCII or EBCDIC from number of
        // printable ASCII or EBCDIC characters in the string.

        // Text header buffer bounds
        assert(header_buffer.size() >= segy::segy_text_header_size());
        const auto text_header_begin = std::begin(header_buffer);
        const auto text_header_end =
            text_header_begin + segy::segy_text_header_size();

        // Count printable ASCII
        const auto n_printable_ascii = std::count_if(
            text_header_begin, text_header_end, is_printable_ascii);

        // Count printable EBCDIC
        const auto n_printable_ebcdic = std::count_if(
            text_header_begin, text_header_end, is_printable_ebcdic);

        // Set text object to correct size in preparation for setting
        m_text.resize(segy::segy_text_header_size());

        if (n_printable_ascii > n_printable_ebcdic) {
            // The string is in ASCII, copy it.
            std::copy(text_header_begin, text_header_end, std::begin(m_text));
        }
        else {
            // The string is in EBCDIC, transform and copy it.
            std::transform(
                text_header_begin, text_header_end, std::begin(m_text),
                to_ascii_from_ebcdic);
        }
    }
}

size_t ReadSEGY::read_nt() const
{
    return m_nt;
}

// TODO: Unit test
void ReadSEGY::read_param(
    const size_t offset,
    const size_t sz,
    Trace_metadata* prm,
    const size_t skip) const
{
    read_trace(offset, sz, nullptr, prm, skip);
}

void ReadSEGY::read_param_non_contiguous(
    const size_t sz,
    const size_t* offsets,
    Trace_metadata* prm,
    const size_t skip) const
{
    read_trace_non_contiguous(sz, offsets, nullptr, prm, skip);
}

const std::string& ReadSEGY::read_text() const
{
    return m_text;
}

size_t ReadSEGY::read_ns() const
{
    return m_ns;
}

exseis::utils::Floating_point ReadSEGY::read_sample_interval() const
{
    return m_sample_interval;
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
void read_trace_t(
    ObjectInterface* obj,
    const segy::SEGYNumberFormat number_format,
    const size_t ns,
    const T offset,
    std::function<size_t(size_t)> offunc,
    const size_t sz,
    exseis::utils::Trace_value* trc,
    Trace_metadata* prm,
    const size_t skip)
{
    using namespace segy;

    unsigned char* tbuf = reinterpret_cast<unsigned char*>(trc);

    if (prm == nullptr) {
        obj->read_trace_data(offset, ns, sz, tbuf);
    }
    else {
        const size_t block_sz =
            (trc == nullptr ? segy::segy_trace_header_size() :
                              segy::segy_trace_size(ns));

        std::vector<unsigned char> alloc(block_sz * sz);
        unsigned char* buf = (sz ? alloc.data() : nullptr);

        if (trc == nullptr) {
            obj->read_trace_metadata(offset, ns, sz, buf);
        }
        else {
            obj->read_trace(offset, ns, sz, buf);

            for (size_t i = 0; i < sz; i++) {
                std::copy(
                    &buf
                        [i * segy::segy_trace_size(ns)
                         + segy::segy_trace_header_size()],
                    &buf[(i + 1) * segy::segy_trace_size(ns)],
                    &tbuf[i * segy::segy_trace_data_size(ns)]);
            }
        }

        extract_trace_metadata(
            sz, buf, *prm,
            (trc != nullptr ? segy::segy_trace_data_size(ns) : 0LU), skip);

        for (size_t i = 0; i < sz; i++) {
            prm->set_index(i + skip, Meta::ltn, offunc(i));
        }
    }

    if (trc != nullptr) {

        // Convert trace values from SEGY floating point format to host format
        if (number_format == SEGYNumberFormat::IBM) {
            for (size_t i = 0; i < ns * sz; i++) {
                const exseis::utils::Trace_value be_trc_i = trc[i];

                static_assert(
                    sizeof(be_trc_i) == 4, "Expect be_trc_i is of size 4.");

                const unsigned char* be_trc_i_bytes =
                    reinterpret_cast<const unsigned char*>(&be_trc_i);

                std::array<unsigned char, 4> be_bytes_array = {
                    {be_trc_i_bytes[0], be_trc_i_bytes[1], be_trc_i_bytes[2],
                     be_trc_i_bytes[3]}};

                trc[i] = from_ibm_to_float(be_bytes_array, true);
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

void ReadSEGY::read_trace(
    const size_t offset,
    const size_t sz,
    exseis::utils::Trace_value* trc,
    Trace_metadata* prm,
    const size_t skip) const
{
    size_t ntz = ((sz == 0) ? sz : (offset + sz > m_nt ? m_nt - offset : sz));
    if (offset >= m_nt && sz != 0) {
        // Nothing to be read.
        m_piol->log->add_entry(exseis::utils::Log_entry{
            exseis::utils::Status::Warning, "Zero byte read requested",
            exseis::utils::Verbosity::none,
            EXSEISDAT_SOURCE_POSITION("exseis::piol::ReadSEGY::read_trace")});
    }
    read_trace_t(
        m_obj.get(), m_number_format, m_ns, offset,
        [offset](size_t i) -> size_t { return offset + i; }, ntz, trc, prm,
        skip);
}

void ReadSEGY::read_trace_non_contiguous(
    const size_t sz,
    const size_t* offset,
    exseis::utils::Trace_value* trc,
    Trace_metadata* prm,
    const size_t skip) const
{
    read_trace_t(
        m_obj.get(), m_number_format, m_ns, offset,
        [offset](size_t i) -> size_t { return offset[i]; }, sz, trc, prm, skip);
}

void ReadSEGY::read_trace_non_monotonic(
    const size_t sz,
    const size_t* offset,
    exseis::utils::Trace_value* trc,
    Trace_metadata* prm,
    const size_t skip) const
{
    // Sort the initial offset and make a new offset without duplicates
    auto idx = get_sort_index(sz, offset);
    std::vector<size_t> nodups;
    nodups.push_back(offset[idx[0]]);
    for (size_t j = 1; j < sz; j++) {
        if (offset[idx[j - 1]] != offset[idx[j]]) {
            nodups.push_back(offset[idx[j]]);
        }
    }

    Trace_metadata sprm(prm->rules, (prm != nullptr ? nodups.size() : 0LU));
    std::vector<exseis::utils::Trace_value> strc(
        m_ns * (trc != nullptr ? nodups.size() : 0LU));

    read_trace_non_contiguous(
        nodups.size(), nodups.data(), (trc != nullptr ? strc.data() : trc),
        (prm != nullptr ? &sprm : prm), 0LU);

    if (prm != nullptr) {
        for (size_t n = 0, j = 0; j < sz; ++j) {
            if (j != 0 && offset[idx[j - 1]] != offset[idx[j]]) {
                n++;
            }

            prm->copy_entries(skip + idx[j], sprm, n);
        }
    }

    if (trc != nullptr) {
        for (size_t n = 0, j = 0; j < sz; ++j) {
            if (j != 0 && offset[idx[j - 1]] != offset[idx[j]]) {
                n++;
            }

            for (size_t k = 0; k < m_ns; k++) {
                trc[idx[j] * m_ns + k] = strc[n * m_ns + k];
            }
        }
    }
}

}  // namespace piol
}  // namespace exseis
