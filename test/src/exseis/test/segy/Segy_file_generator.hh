#ifndef EXSEIS_TEST_SEGY_FILE_GENERATOR
#define EXSEIS_TEST_SEGY_FILE_GENERATOR

#include "exseis/piol/segy/Trace_header_offsets.hh"
#include "exseis/piol/segy/utils.hh"
#include "exseis/utils/encoding/character_encoding.hh"
#include "exseis/utils/encoding/number_encoding.hh"

#include <algorithm>
#include <array>
#include <type_traits>
#include <utility>
#include <vector>

#include "exseis/test/IBM_number.hh"
#include "exseis/test/Index_hash.hh"

namespace exseis {
namespace test {
namespace segy {

class Text_header {
  public:
    enum class Encoding { ascii, ebcdic };

    struct Native {
        std::array<unsigned char, exseis::segy::segy_text_header_size()> ascii;
        Encoding encoding;
    };

    using Binary =
        std::array<unsigned char, exseis::segy::segy_text_header_size()>;

  private:
    Native m_native;
    Binary m_binary;

  public:
    Native native() const { return m_native; }
    Binary binary() const { return m_binary; }

    Text_header(Encoding e)
    {
        for (size_t i = 0; i < size(); i++) {
            m_native.ascii[i] = ascii_pattern(i);
            m_native.encoding = e;

            switch (e) {
                case Encoding::ascii:
                    m_binary[i] = ascii_pattern(i);
                    break;

                case Encoding::ebcdic:
                    m_binary[i] = ebcdic_pattern(i);
                    break;

                default:
                    assert(false && "Unknown encoding!");
                    break;
            }
        }
    }

    static unsigned char ascii_pattern(uint64_t i)
    {
        auto ascii = exseis::test::Index_hash::get<unsigned char>(i);
        while (!exseis::is_printable_ascii(ascii)) {
            // Some exceptions to being printable!
            switch (ascii) {
                case '\n':
                    break;
                case '\r':
                    break;
                case '\t':
                    break;
            };

            uint32_t ascii32 = ascii + 1801;  // add some prime
            ascii            = static_cast<unsigned char>(ascii32);
        }

        return ascii;
    }

    static unsigned char ebcdic_pattern(uint64_t i)
    {
        return exseis::to_ebcdic_from_ascii(ascii_pattern(i));
    }

    static constexpr size_t size() { return Binary{}.size(); }
};


class Binary_header {
  public:
    struct Native {
        int16_t sample_interval;    // 3217-3218
        int16_t samples_per_trace;  // 3221-3222
        exseis::segy::Segy_number_format number_format;
    };

    using Binary = std::array<
        unsigned char,
        exseis::segy::segy_binary_file_header_size()
            - exseis::segy::segy_text_header_size()>;

  private:
    Native m_native;
    Binary m_binary;

  public:
    Native native() const { return m_native; }
    Binary binary() const { return m_binary; }

    static constexpr size_t size() { return Binary{}.size(); }

    Binary_header(
        int16_t samples_per_trace,
        exseis::segy::Segy_number_format number_format) :
        m_binary{}
    {
        using Offset = exseis::segy::Segy_file_header_byte;

        // Use some(!) non-zero number for the sample_interval
        m_native.sample_interval = write_header<int16_t>(
            std::max<int16_t>(samples_per_trace, 1), Offset::interval);

        m_native.samples_per_trace =
            write_header<int16_t>(samples_per_trace, Offset::num_sample);

        m_native.number_format =
            write_header<int16_t>(number_format, Offset::type);
    }

  private:
    template<typename Value, typename NativeValue>
    NativeValue write_header(NativeValue value, size_t offset)
    {
        static_assert(
            sizeof(Value) == sizeof(NativeValue),
            "Expect Value and VativeValue to be the same size");

        // All Segy_file_header_byte values defined as absolute offsets from
        // the start of the file, which includes the text header!
        constexpr auto text_header_size = 3200;

        const auto value_be = exseis::to_big_endian(static_cast<Value>(value));
        std::copy(
            value_be.cbegin(), value_be.cend(),
            m_binary.data() + offset - text_header_size);

        return value;
    }
};


class Trace {
  public:
    struct Native {
        int32_t line_trace_index = 0;  //  1- 4
        int32_t file_trace_index = 0;  //  5- 8
        int32_t ofr_number       = 0;  //  9-12
        int32_t ofr_trace_index  = 0;  // 13-16

        int16_t coordinate_scalar = 0;  // 71-72
        double src_x              = 0;  // 73-76
        double src_y              = 0;  // 77-80
        double rcv_x              = 0;  // 81-84
        double rcv_y              = 0;  // 85-88

        int16_t number_of_samples = 0;  // 115-116
        int16_t sample_interval   = 0;  // 117-118

        double cmp_x       = 0;  // 181-184
        double cmp_y       = 0;  // 185-188
        int32_t in_line    = 0;  // 189-192
        int32_t cross_line = 0;  // 193-196

        std::vector<float> samples;  // 241-end
        exseis::segy::Segy_number_format number_format;
    };

    using Binary = std::vector<unsigned char>;

  private:
    Native m_native;
    Binary m_binary;

  public:
    Native native() const { return m_native; }
    Binary binary() const { return m_binary; }

    Trace(
        int32_t trace_index,
        int16_t number_of_samples,
        exseis::segy::Segy_number_format number_format)
    {

        // Pre-setup some values needed for hashes etc.
        m_native.file_trace_index  = trace_index + 1;  // 1-indexed
        m_native.number_of_samples = number_of_samples;
        m_native.samples.resize(number_of_samples);
        m_native.number_format = number_format;

        m_binary.resize(
            trace_header_size()
            + number_of_samples * bytes_per_sample(number_format));

        init_metadata();
        init_data();
    }

    static constexpr size_t trace_header_size() { return 240; }

  private:
    template<typename T>
    T hash(size_t index) const
    {
        // Set hash offset to the byte offset from the first trace in the file
        const size_t offset =
            m_native.file_trace_index * m_native.number_of_samples * 4;
        return exseis::test::Index_hash::get<T>(index + offset);
    }

    template<typename T>
    T hash(exseis::segy::Trace_header_offsets offset) const
    {
        return hash<T>(static_cast<size_t>(offset) - 1);
    }

    // Set Trace metadata
    void init_metadata()
    {
        using Offsets = exseis::segy::Trace_header_offsets;

        m_native.line_trace_index = write_header<int32_t>(
            m_native.file_trace_index, Offsets::line_trace_index);

        m_native.file_trace_index = write_header<int32_t>(
            m_native.file_trace_index, Offsets::file_trace_index);

        m_native.ofr_number = write_header<int32_t>(Offsets::ORF);
        m_native.ofr_trace_index =
            write_header<int32_t>(Offsets::ofr_trace_index);

        m_native.coordinate_scalar =
            write_header<int16_t>(1, Offsets::coordinate_scalar);

        const double coordinate_scalar =
            static_cast<double>(m_native.coordinate_scalar);
        m_native.src_x =
            write_header<int32_t>(Offsets::source_x) * coordinate_scalar;
        m_native.src_y =
            write_header<int32_t>(Offsets::source_y) * coordinate_scalar;
        m_native.rcv_x =
            write_header<int32_t>(Offsets::receiver_x) * coordinate_scalar;
        m_native.rcv_y =
            write_header<int32_t>(Offsets::receiver_y) * coordinate_scalar;

        m_native.number_of_samples = write_header<int16_t>(
            m_native.number_of_samples, Offsets::number_of_samples);

        m_native.sample_interval =
            write_header<int16_t>(Offsets::sample_interval);

        m_native.cmp_x =
            write_header<int32_t>(Offsets::cdp_x) * coordinate_scalar;
        m_native.cmp_y =
            write_header<int32_t>(Offsets::cdp_y) * coordinate_scalar;
        m_native.in_line    = write_header<int32_t>(Offsets::il);
        m_native.cross_line = write_header<int32_t>(Offsets::xl);
    }

    template<typename Value>
    Value write_header(Value value, exseis::segy::Trace_header_offsets offset)
    {
        const auto value_be = exseis::to_big_endian(value);
        std::copy(
            value_be.cbegin(), value_be.cend(),
            std::next(std::begin(m_binary), static_cast<size_t>(offset) - 1));

        return value;
    }

    template<typename Value>
    Value write_header(exseis::segy::Trace_header_offsets offset)
    {
        return write_header(hash<Value>(offset), offset);
    }

    void init_data()
    {
        // Set Trace data
        assert(
            bytes_per_sample(m_native.number_format) == 4
            && "Unexpected number format! Only testing IEEE_fp32 and IBM_fp32.");

        for (int16_t i = 0; i < m_native.number_of_samples; i++) {
            m_native.samples[i] = samples_pattern_ieee(i);

            auto sample_binary_be = ([&] {
                switch (m_native.number_format) {
                    case exseis::segy::Segy_number_format::IEEE_fp32:
                        return exseis::to_big_endian(samples_pattern_ieee(i));

                    case exseis::segy::Segy_number_format::IBM_fp32:
                        return samples_pattern_ibm(i);

                    case exseis::segy::Segy_number_format::TC4:
                    case exseis::segy::Segy_number_format::TC2:
                    case exseis::segy::Segy_number_format::FPG:
                    case exseis::segy::Segy_number_format::NA1:
                    case exseis::segy::Segy_number_format::NA2:
                    case exseis::segy::Segy_number_format::TC1:
                    default:
                        assert(
                            false && "Untested value of Segy_number_format!");
                        return std::array<unsigned char, 4>{};
                }
            }());

            std::copy(
                sample_binary_be.cbegin(), sample_binary_be.cend(),
                m_binary.begin() + trace_header_size()
                    + sample_binary_be.size() * i);
        }
    }

  public:
    exseis::test::IBM_number samples_pattern(size_t i) const
    {
        uint32_t hash = this->hash<uint32_t>(i + trace_header_size());

        // Use first bit for sign
        const int sign = (hash & 0x01) == 0 ? 1 : -1;
        hash           = hash >> 1;


        // Could use next 7 bits for exponent [-64, 63]
        // For IEEE representability, need exponent [-32, 31]
        // Only need 6 bits then!
        // Take the 6 bits for [0, 63], then bias it
        const int8_t exponent = (hash & 0x3FU) - 32;
        hash                  = hash >> 8;

        // Use next 24 bits for significand
        uint32_t significand = hash & 0xFFFFFFU;

        // Need to ensure at most 3 top-bits are 0.
        if (significand <= 0x0FFFFFU) significand += 0x100000U;

        return {sign, exponent, significand};
    }

    float samples_pattern_ieee(size_t i) const
    {
        return samples_pattern(i).native();
    }

    std::array<unsigned char, 4> samples_pattern_ibm(size_t i) const
    {
        return samples_pattern(i).binary();
    }

    static constexpr size_t bytes_per_sample(
        exseis::segy::Segy_number_format number_format)
    {
        switch (number_format) {
            case exseis::segy::Segy_number_format::IBM_fp32:
                return 4;
            case exseis::segy::Segy_number_format::IEEE_fp32:
                return 4;

            case exseis::segy::Segy_number_format::TC4:
            case exseis::segy::Segy_number_format::TC2:
            case exseis::segy::Segy_number_format::FPG:
            case exseis::segy::Segy_number_format::NA1:
            case exseis::segy::Segy_number_format::NA2:
            case exseis::segy::Segy_number_format::TC1:
            default:
                assert(false && "Unsupported number format!");
                return -1;
        }
    }

    size_t size() const { return m_binary.size(); }
};


class Segy_file {
  public:
    struct Native {
        Text_header::Native text_header;
        Binary_header::Native binary_header;
        std::vector<Trace::Native> traces;
    };

    using Binary = std::vector<unsigned char>;

  private:
    Native m_native;
    Binary m_binary;

  public:
    Native native() const { return m_native; }
    Binary binary() const { return m_binary; }

    Segy_file(
        Text_header::Encoding text_encoding,
        exseis::segy::Segy_number_format number_format,
        int32_t number_of_traces,
        int16_t samples_per_trace)
    {
        Text_header text_header{text_encoding};
        Binary_header binary_header{samples_per_trace, number_format};

        std::vector<Trace> traces;
        for (int32_t i = 0; i < number_of_traces; i++) {
            traces.emplace_back(i, samples_per_trace, number_format);
        }

        std::vector<Trace::Native> traces_native;
        for (const auto& trace : traces) {
            traces_native.push_back(trace.native());
        }

        m_native =
            Native{text_header.native(), binary_header.native(), traces_native};

        const auto text_header_binary = text_header.binary();
        m_binary.insert(
            m_binary.end(), text_header_binary.cbegin(),
            text_header_binary.cend());

        const auto binary_header_binary = binary_header.binary();
        m_binary.insert(
            m_binary.end(), binary_header_binary.cbegin(),
            binary_header_binary.cend());

        for (const auto& trace : traces) {
            const auto trace_binary = trace.binary();
            m_binary.insert(
                m_binary.end(), trace_binary.cbegin(), trace_binary.cend());
        }
    }

    size_t size() const { return m_binary.size(); }
};


}  // namespace segy
}  // namespace test
}  // namespace exseis

#endif  // EXSEIS_TEST_SEGY_FILE_GENERATOR
