////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEIS_EXSEIS_SEGY_TRACE_HEADER_OFFSETS_HH
#define EXSEIS_EXSEIS_SEGY_TRACE_HEADER_OFFSETS_HH

#include <cstddef>

namespace exseis {
inline namespace piol {
namespace segy {

/// @brief X-Macro defining the offsets in a SEGY trace header
#define EXSEIS_X_SEGY_TRACE_HEADER_OFFSETS(X)                                  \
    X(line_trace_index, 1U, int32_t, "The trace sequence number in the Line.") \
    X(file_trace_index, 5U, int32_t,                                           \
      "The trace sequence number in SEG-Y File.")                              \
    X(ORF, 9U, int32_t, "The original field record number.")                   \
    X(ofr_trace_index, 13U, int32_t, "The trace number in the ORF.")           \
    X(energy_source_number, 17U, int32_t, "The source energy number.")         \
    X(ensemble_trace_index, 25U, int32_t, "The trace number in the ensemble.") \
    X(trace_id_code, 29U, int16_t, "The trace identification number.")         \
    X(vstack_count, 31U, int16_t, "The number of traces vertically stacked.")  \
    X(hstack_count, 33U, int16_t,                                              \
      "The number of traces horizontally stacked.")                            \
    X(source_receiver_distance, 37U, int32_t,                                  \
      "The distance from source center to receiver centre.")                   \
    X(receiver_elevation, 41U, int32_t, "The receiver group elevation.")       \
    X(surface_elevation_at_source, 45U, int32_t,                               \
      "The surface elevation at the source.")                                  \
    X(source_depth, 49U, int32_t,                                              \
      "The source depth below surface (opposite of above?).")                  \
    X(DtmElvRcv, 53U, int32_t, "The datum elevation for the receiver group.")  \
    X(DtmElvSrc, 57U, int32_t, "The datum elevation for the source.")          \
    X(water_depth_at_source, 61U, int32_t, "The water depth for the source.")  \
    X(water_depth_at_receiver, 65U, int32_t,                                   \
      "The water depth for the receive group.")                                \
    X(ScaleElev, 69U, int16_t,                                                 \
      "The scale coordinate for 41-68 (elevations + depths).")                 \
    X(coordinate_scalar, 71U, int16_t,                                         \
      "The scale coordinate for 73-88 + 181-188")                              \
    X(source_x, 73U, int32_t, "The X coordinate for the source")               \
    X(source_y, 77U, int32_t, "The Y coordinate for the source")               \
    X(receiver_x, 81U, int32_t, "The X coordinate for the receive group")      \
    X(receiver_y, 85U, int32_t, "The Y coordinate for the receive group")      \
    X(UpSrc, 95U, int16_t, "The uphole time at the source (ms).")              \
    X(UpRcv, 97U, int16_t, "The uphole time at the receive group (ms).")       \
    X(number_of_samples, 115U, int16_t, "The number of samples in the trace.") \
    X(sample_interval, 117U, int16_t, "The sample interval (us).")             \
    X(cdp_x, 181U, int32_t, "The X coordinate for the CMP")                    \
    X(cdp_y, 185U, int32_t, "The Y coordinate for the CMP")                    \
    X(il, 189U, int32_t, "The Inline grid point.")                             \
    X(xl, 193U, int32_t, "The Crossline grid point.")                          \
    X(shotpoint_number, 197U, int32_t, "The source nearest to the CDP.")       \
    X(ShotScal, 201U, int16_t,                                                 \
      "The shot number scalar. (Explicitly says that 0 == 1)")                 \
    X(trace_value_unit, 203U, int16_t,                                         \
      "The unit system used for trace values.")                                \
    X(TransConst, 205U, int32_t, "The transduction constant.")                 \
    X(TransExp, 209U, int16_t, "The transduction exponent.")                   \
    X(transduction_unit, 211U, int16_t, "The transduction units")              \
    X(TimeScal, 215U, int16_t, "Scalar for time measurements.")                \
    X(SrcMeas, 225U, int32_t, "Source measurement.")                           \
    X(SrcMeasExp, 229U, int16_t, "Source measurement exponent.")

/// @brief Macro to generate the Trace_header_offsets enum
#define EXSEIS_SEGY_MAKE_TRACE_HEADER_OFFSETS_ENUM(ENUM, OFFSET, TYPE, DESC)   \
    /** DESC (TYPE) */                                                         \
    ENUM = OFFSET,

/// @brief SEG-Y Trace Header offsets
enum class Trace_header_offsets : size_t {
    EXSEIS_X_SEGY_TRACE_HEADER_OFFSETS(
        EXSEIS_SEGY_MAKE_TRACE_HEADER_OFFSETS_ENUM)
};

#undef EXSEIS_SEGY_MAKE_TRACE_HEADER_OFFSETS_ENUM


}  // namespace segy
}  // namespace piol
}  // namespace exseis

#endif  // EXSEIS_EXSEIS_SEGY_TRACE_HEADER_OFFSETS_HH
