////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEIS_PIOL_METADATA_TRACE_METADATA_KEY_HH
#define EXSEIS_PIOL_METADATA_TRACE_METADATA_KEY_HH

#include <cstdint>
#include <string>

namespace exseis {
inline namespace piol {
inline namespace metadata {


/// @brief X-Macro for the default trace metadata keys
#define EXSEIS_X_TRACE_METADATA_KEYS(X)                                        \
    X(raw, "Raw copy of the headers")                                          \
    X(ltn, "Local trace number")                                               \
    X(gtn, "Global trace number.")                                             \
    X(line_trace_index, "The trace number (line)")                             \
    X(ofr_trace_index, "The Trace number (record)")                            \
    X(file_trace_index, "The trace number (file)")                             \
    X(ensemble_trace_index, "The trace number (ensemble)")                     \
    X(number_of_samples, "Number of samples in this trace")                    \
    X(sample_interval, "The increment of this trace.")                         \
    X(trace_id_code, "Trace identification code")                              \
    X(energy_source_number, "Source Number")                                   \
    X(shotpoint_number, "Shot number")                                         \
    X(vstack_count, "Number of traces stacked for this trace (vertical)")      \
    X(hstack_count, "Number of traces stacked for this trace (horizontal)")    \
    X(source_receiver_distance, "Distance from source to receiver")            \
    X(receiver_elevation, "Receiver group elevation")                          \
    X(surface_elevation_at_source, "Source surface elevation")                 \
    X(source_depth, "Source depth")                                            \
    X(water_depth_at_source, "The Water depth at source")                      \
    X(water_depth_at_receiver, "The Water depth at receiver")                  \
    X(source_x, "The source x coordinate")                                     \
    X(source_y, "The source y coordinate")                                     \
    X(receiver_x, "The receiver x coordinate")                                 \
    X(receiver_y, "The receiver y coordinate")                                 \
    X(cdp_x, "The CMP x coordinate")                                           \
    X(cdp_y, "The CMP y coordinate")                                           \
    X(coordinate_scalar, "The coordinate scalar in a SEGY file")               \
    X(il, "The inline number")                                                 \
    X(xl, "The crossline number")                                              \
    X(transduction_unit, "Unit system for transduction constant")              \
    X(trace_value_unit, "Unit system for traces")                              \
    X(dsdr,                                                                    \
      "The sum of the differences between sources and receivers of this trace and another")

/// @brief The available trace parameters
///
enum class Trace_metadata_key : uint64_t {
#define EXSEIS_DETAIL_MAKE_TRACE_METADATA_KEY(ENUM, DESC) /** DESC */ ENUM,
    EXSEIS_X_TRACE_METADATA_KEYS(EXSEIS_DETAIL_MAKE_TRACE_METADATA_KEY)
#undef EXSEIS_DETAIL_MAKE_TRACE_METADATA_KEY
};


/// @brief Get a string for a Trace_metadata_key
/// @param[in] key The Trace_metadata_key to get a string for
/// @returns A string of the enum key
inline std::string to_string(Trace_metadata_key key)
{
#define EXSEIS_DETAIL_MAKE_TO_STRING(ENUM, DESC)                               \
    case Trace_metadata_key::ENUM:                                             \
        return #ENUM;

    switch (key) {
        EXSEIS_X_TRACE_METADATA_KEYS(EXSEIS_DETAIL_MAKE_TO_STRING)

        default:
            return "Unknown Trace_metadata_key!";
    }

#undef EXSEIS_DETAIL_MAKE_TO_STRING
}


}  // namespace metadata
}  // namespace piol
}  // namespace exseis

#endif  // EXSEIS_PIOL_METADATA_TRACE_METADATA_KEY_HH
