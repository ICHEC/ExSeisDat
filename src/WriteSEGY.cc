////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief
/// @details WriteSEGY functions
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/piol/WriteSEGY.hh"

#include "exseisdat/piol/ObjectSEGY.hh"
#include "exseisdat/piol/mpi/MPI_Binary_file.hh"
#include "exseisdat/piol/segy/utils.hh"
#include "exseisdat/utils/encoding/number_encoding.hh"

#include <cmath>
#include <cstring>
#include <limits>

using namespace exseis::utils;
using namespace exseis::piol::segy;

namespace exseis {
namespace piol {

//////////////////////      Constructor & Destructor      //////////////////////

WriteSEGY::WriteSEGY(
    std::shared_ptr<ExSeisPIOL> piol,
    const std::string name,
    const WriteSEGY::Options& options) :
    WriteSEGY(
        piol,
        name,
        options,
        std::make_shared<ObjectSEGY>(
            piol,
            name,
            std::make_shared<MPI_Binary_file>(piol, name, FileMode::Write)))
{
}

WriteSEGY::WriteSEGY(
    std::shared_ptr<ExSeisPIOL> piol,
    std::string name,
    const WriteSEGY::Options& options,
    std::shared_ptr<ObjectInterface> obj) :
    m_piol(piol),
    m_name(name),
    m_obj(obj),
    m_sample_interval_factor(options.sample_interval_factor)
{
}

WriteSEGY::~WriteSEGY(void)
{
    // TODO: On error this can be a source of a deadlock
    if (!m_piol->log->has_error()) {
        calc_nt();

        if (m_state.resize) {
            m_obj->set_file_size(segy::get_file_size(m_nt, m_ns));
        }

        if (m_state.should_write_file_header) {
            // Write file header on rank 0.
            if (m_piol->comm->get_rank() == 0) {
                // The buffer to build the header in
                std::vector<unsigned char> header_buffer(
                    segy::segy_binary_file_header_size());

                // Write the text header into the start of the buffer.
                std::copy(
                    std::begin(m_text), std::end(m_text),
                    std::begin(header_buffer));

                // Write ns, the number format, and the interval
                const std::array<unsigned char, 2> be_ns =
                    to_big_endian(static_cast<int16_t>(m_ns));
                std::copy(
                    std::begin(be_ns), std::end(be_ns),
                    &header_buffer[SEGYFileHeaderByte::num_sample]);

                const std::array<unsigned char, 2> be_format =
                    to_big_endian(static_cast<int16_t>(SEGYNumberFormat::IEEE));
                std::copy(
                    std::begin(be_format), std::end(be_format),
                    &header_buffer[SEGYFileHeaderByte::type]);

                const std::array<unsigned char, 2> be_interval =
                    to_big_endian(static_cast<int16_t>(std::lround(
                        m_sample_interval / m_sample_interval_factor)));
                std::copy(
                    std::begin(be_interval), std::end(be_interval),
                    &header_buffer[SEGYFileHeaderByte::interval]);

                // Currently these are hard-coded entries:
                // The unit system.
                const std::array<unsigned char, 2> be_units =
                    to_big_endian<int16_t>(0x0001);
                std::copy(
                    std::begin(be_units), std::end(be_units),
                    &header_buffer[SEGYFileHeaderByte::units]);

                // The version of the SEGY format.
                const std::array<unsigned char, 2> be_segy_format =
                    to_big_endian<int16_t>(0x0100);
                std::copy(
                    std::begin(be_segy_format), std::end(be_segy_format),
                    &header_buffer[SEGYFileHeaderByte::segy_format]);

                // We always deal with fixed traces at present.
                const std::array<unsigned char, 2> be_fixed_trace =
                    to_big_endian<int16_t>(0x0001);
                std::copy(
                    std::begin(be_fixed_trace), std::end(be_fixed_trace),
                    &header_buffer[SEGYFileHeaderByte::fixed_trace]);

                // We do not support text extensions at present.
                const auto be_extensions = to_big_endian<int16_t>(0x0000);
                std::copy(
                    std::begin(be_extensions), std::end(be_extensions),
                    &header_buffer[SEGYFileHeaderByte::extensions]);

                // Write the header from the buffer
                m_obj->should_write_file_header(header_buffer.data());
            }
            else {
                m_obj->should_write_file_header(nullptr);
            }
        }
    }
}

const std::string& WriteSEGY::file_name() const
{
    return m_name;
}

//////////////////////////       Member functions      /////////////////////////

size_t WriteSEGY::calc_nt(void)
{
    if (m_state.stalent) {
        m_nt            = m_piol->comm->max(m_nt);
        m_state.stalent = false;
        m_state.resize  = true;
    }
    return m_nt;
}

void WriteSEGY::write_text(const std::string text)
{
    if (m_text != text) {
        m_text = text;
        m_text.resize(segy::segy_text_header_size());
        m_state.should_write_file_header = true;
    }
}

void WriteSEGY::write_ns(const size_t ns)
{
    if (ns > std::numeric_limits<int16_t>::max()) {
        m_piol->log->add_entry(exseis::utils::Log_entry{
            exseis::utils::Status::Error, "Ns value is too large for SEG-Y",
            exseis::utils::Verbosity::none,
            EXSEISDAT_SOURCE_POSITION("exseis::piol::WriteSEGY::write_ns")});

        return;
    }

    if (m_ns != ns) {
        m_ns                             = ns;
        m_state.resize                   = true;
        m_state.should_write_file_header = true;
    }
    m_is_ns_set = true;
}

void WriteSEGY::write_nt(const size_t nt)
{
    if (m_nt != nt) {
        m_nt           = nt;
        m_state.resize = true;
    }
    m_state.stalent = false;
}

void WriteSEGY::write_sample_interval(
    const exseis::utils::Floating_point sample_interval)
{
    if (std::isnormal(sample_interval) == false) {
        m_piol->log->add_entry(exseis::utils::Log_entry{
            exseis::utils::Status::Error,
            "The SEG-Y Interval " + std::to_string(sample_interval)
                + " is not normal.",
            exseis::utils::Verbosity::none,
            EXSEISDAT_SOURCE_POSITION(
                "exseis::piol::WriteSEGY::write_sample_interval")});

        return;
    }

    if (m_sample_interval < sample_interval
        || m_sample_interval > sample_interval) {
        m_sample_interval                = sample_interval;
        m_state.should_write_file_header = true;
    }
}

/*! Template function for writing SEG-Y traces and parameters, random and
 *  contiguous.
 *  @tparam T The type of offset (pointer or size_t)
 *  @param[in] obj The object-layer object.
 *  @param[in] ns The number of samples per trace.
 *  @param[in] offset The offset(s). If T == size_t * this is an array,
 *                    otherwise its a single offset.
 *  @param[in] number_of_traces The number of traces to write
 *  @param[in] trc Pointer to trace array.
 *  @param[in] prm Pointer to parameter structure.
 *  @param[in] skip Skip \c skip entries in the parameter structure
 */
template<typename T>
void write_trace_t(
    ObjectInterface* obj,
    const size_t ns,
    T offset,
    const size_t number_of_traces,
    exseis::utils::Trace_value* trc,
    const Trace_metadata* prm,
    const size_t skip)
{
    unsigned char* tbuf = reinterpret_cast<unsigned char*>(trc);


    // Convert from host-endianness to SEGY endianness for writing

    if (trc != nullptr) {
        for (size_t i = 0; i < ns * number_of_traces; i++) {
            const auto be_trc_i_bytes =
                to_big_endian<exseis::utils::Trace_value>(trc[i]);

            unsigned char* trc_i_bytes =
                reinterpret_cast<unsigned char*>(&trc[i]);

            std::copy(
                std::begin(be_trc_i_bytes), std::end(be_trc_i_bytes),
                trc_i_bytes);
        }
    }


    if (prm == nullptr) {
        obj->write_trace_data(offset, ns, number_of_traces, tbuf);
    }
    else {
        const size_t block_sz =
            (trc == nullptr ? segy::segy_trace_header_size() :
                              segy::segy_trace_size(ns));

        std::vector<unsigned char> alloc(block_sz * number_of_traces);
        unsigned char* buf = (number_of_traces ? alloc.data() : nullptr);

        segy::insert_trace_metadata(
            number_of_traces, *prm, buf,
            block_sz - segy::segy_trace_header_size(), skip);
        if (trc == nullptr) {
            obj->write_trace_metadata(offset, ns, number_of_traces, buf);
        }
        else {
            for (size_t i = 0; i < number_of_traces; i++) {
                std::copy(
                    &tbuf[i * segy::segy_trace_data_size(ns)],
                    &tbuf[(i + 1) * segy::segy_trace_data_size(ns)],
                    &buf
                        [i * segy::segy_trace_size(ns)
                         + segy::segy_trace_header_size()]);
            }

            obj->write_trace(offset, ns, number_of_traces, buf);
        }
    }


    // Convert back from SEGY endianness to native endianness, as it was
    // before calling this routine

    if (trc != nullptr) {
        for (size_t i = 0; i < ns * number_of_traces; i++) {
            const exseis::utils::Trace_value be_trc_i = trc[i];

            const unsigned char* be_trc_i_bytes =
                reinterpret_cast<const unsigned char*>(&be_trc_i);

            trc[i] = from_big_endian<exseis::utils::Trace_value>(
                be_trc_i_bytes[0], be_trc_i_bytes[1], be_trc_i_bytes[2],
                be_trc_i_bytes[3]);
        }
    }
}


void WriteSEGY::write_trace(
    const size_t offset,
    const size_t number_of_traces,
    exseis::utils::Trace_value* trc,
    const Trace_metadata* prm,
    const size_t skip)
{
    if (!m_is_ns_set) {
        m_piol->log->add_entry(exseis::utils::Log_entry{
            exseis::utils::Status::Error,
            "The number of samples per trace (ns) has not been set. The output is probably erroneous.",
            exseis::utils::Verbosity::none,
            EXSEISDAT_SOURCE_POSITION("exseis::piol::WriteSEGY::write_trace")});
    }

    write_trace_t(m_obj.get(), m_ns, offset, number_of_traces, trc, prm, skip);
    m_state.stalent = true;
    m_nt            = std::max(offset + number_of_traces, m_nt);
}

void WriteSEGY::write_trace_non_contiguous(
    const size_t number_of_traces,
    const size_t* offset,
    exseis::utils::Trace_value* trc,
    const Trace_metadata* prm,
    const size_t skip)
{
    if (!m_is_ns_set) {
        m_piol->log->add_entry(exseis::utils::Log_entry{
            exseis::utils::Status::Error,
            "The number of samples per trace (ns) has not been set. The output is probably erroneous.",
            exseis::utils::Verbosity::none,
            EXSEISDAT_SOURCE_POSITION(
                "exseis::piol::WriteSEGY::write_trace_non_contiguous")});
    }

    write_trace_t(m_obj.get(), m_ns, offset, number_of_traces, trc, prm, skip);
    m_state.stalent = true;
    if (number_of_traces != 0) {
        m_nt = std::max(offset[number_of_traces - 1LU] + 1LU, m_nt);
    }
}

void WriteSEGY::write_param(
    const size_t offset,
    const size_t sz,
    const Trace_metadata* prm,
    const size_t skip)
{
    write_trace(offset, sz, nullptr, prm, skip);
}

void WriteSEGY::write_param_non_contiguous(
    const size_t sz,
    const size_t* offset,
    const Trace_metadata* prm,
    const size_t skip)
{
    write_trace_non_contiguous(sz, offset, nullptr, prm, skip);
}

}  // namespace piol
}  // namespace exseis
