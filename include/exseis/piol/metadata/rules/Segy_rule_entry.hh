////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Instances of the \c Rule_entry class necessary for reading/writing
///        SEGY trace parameters.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEIS_PIOL_METADATA_RULES_SEGY_RULE_ENTRY_SEGY_RULE_ENTRY_HH
#define EXSEIS_PIOL_METADATA_RULES_SEGY_RULE_ENTRY_SEGY_RULE_ENTRY_HH

#include "exseis/piol/metadata/rules/Rule_entry.hh"
#include "exseis/piol/segy/Trace_header_offsets.hh"
#include "exseis/piol/segy/utils.hh"

#include <algorithm>


namespace exseis {
inline namespace piol {
inline namespace metadata {
inline namespace rules {


/// @brief The Long rule entry structure for the SEG-Y format.
///
struct Segy_long_rule_entry : public Rule_entry {
    /// @brief The constructor.
    ///
    /// @param[in] loc  The location of the primary data
    ///
    Segy_long_rule_entry(segy::Trace_header_offsets loc) :
        Rule_entry(size_t(loc))
    {
    }

    /// @brief Return the minimum location stored, i.e loc just loc
    ///
    /// @return the minimum location
    ///
    size_t min() const override { return loc; }

    /// @brief Return the maximum location stored up to, including the size of
    ///        the data stored
    ///
    /// @return the maximum location plus 4 bytes to store an int32_t
    ///
    size_t max() const override { return loc + 4U; }

    /// @brief Return the datatype associated with the entry.
    ///
    /// @return \c MdType::Long
    ///
    MdType type() const override { return MdType::Long; }

    /// @copydoc Rule_entry::clone
    std::unique_ptr<Rule_entry> clone() const override
    {
        return std::make_unique<Segy_long_rule_entry>(
            static_cast<segy::Trace_header_offsets>(loc));
    }

    /// @copydoc Rule_entry::memory_usage()
    size_t memory_usage() const override { return sizeof *this; }
};

/// @brief The Copy rule entry structure for the SEG-Y format.
///
struct Segy_copy_rule_entry : public Rule_entry {
    /// @brief The constructor (empty).
    ///
    Segy_copy_rule_entry() : Rule_entry(0U) {}

    /// @brief Return the minimum location stored, i.e 0
    ///
    /// @return 0U
    ///
    size_t min() const override { return 0U; }

    /// @brief Return the size of the trace header
    ///
    /// @return the size of the trace header
    ///
    size_t max() const override { return segy::segy_trace_header_size(); }

    /// @brief Return the datatype associated with the entry.
    ///
    /// @return \c MdType::Copy
    ///
    MdType type() const override { return MdType::Copy; }

    /// @copydoc Rule_entry::clone
    std::unique_ptr<Rule_entry> clone() const override
    {
        return std::make_unique<Segy_copy_rule_entry>();
    }

    /// @copydoc Rule_entry::memory_usage()
    size_t memory_usage() const override { return sizeof *this; }
};

/// @brief The Index rule entry structure. For indexing without modifying what
///        will be stored.
///
struct Segy_index_rule_entry : public Rule_entry {
    /// @brief The constructor.
    ///
    Segy_index_rule_entry() : Rule_entry(0U) {}

    /// @brief Return 0. nothing stored
    ///
    /// @return Return 0
    ///
    size_t min() const override { return 0; }

    /// @brief Return 0. nothing stored
    ///
    /// @return Return 0
    ///
    size_t max() const override { return 0; }

    /// @brief Return the datatype associated with the entry.
    ///
    /// @return \c MdType::Index
    ///
    MdType type() const override { return MdType::Index; }

    /// @copydoc Rule_entry::clone
    std::unique_ptr<Rule_entry> clone() const override
    {
        return std::make_unique<Segy_index_rule_entry>();
    }

    /// @copydoc Rule_entry::memory_usage()
    size_t memory_usage() const override { return sizeof *this; }
};

/// @brief The Short rule entry structure for the SEG-Y format.
///
struct Segy_short_rule_entry : public Rule_entry {
    /// @brief The constructor.
    ///
    /// @param[in] loc  The location of the primary data
    ///
    Segy_short_rule_entry(segy::Trace_header_offsets loc) :
        Rule_entry(size_t(loc))
    {
    }

    /// @brief Return the minimum location stored, i.e loc
    ///
    /// @return the minimum location
    ///
    size_t min() const override { return loc; }

    /// @brief Return the maximum location stored up to, including the size of
    ///        the data stored
    ///
    /// @return the maximum location plus 2 bytes to store an int16_t
    ///
    size_t max() const override { return loc + 2U; }

    /// @brief Return the datatype associated with the entry.
    ///
    /// @return \c MdType::Short
    ///
    MdType type() const override { return MdType::Short; }

    /// @copydoc Rule_entry::clone
    std::unique_ptr<Rule_entry> clone() const override
    {
        return std::make_unique<Segy_short_rule_entry>(
            static_cast<segy::Trace_header_offsets>(loc));
    }

    /// @copydoc Rule_entry::memory_usage()
    size_t memory_usage() const override { return sizeof *this; }
};

/// @brief The Float rule entry structure for the SEG-Y format.
///
struct Segy_float_rule_entry : public Rule_entry {
    /// @brief The location of the scaler field.
    size_t scalar_location;

    /// @brief The constructor.
    ///
    /// @param[in] loc  The location of the primary data
    /// @param[in] scalar_location The location of the scaler field.
    ///
    Segy_float_rule_entry(
        segy::Trace_header_offsets loc,
        segy::Trace_header_offsets scalar_location) :
        Rule_entry(size_t(loc)), scalar_location(size_t(scalar_location))
    {
    }

    /// @brief Return the minimum location stored
    ///
    /// @return the minimum location
    ///
    size_t min() const override { return std::min(scalar_location, loc); }

    /// @brief Return the maximum location stored up to, including the size of
    ///        the data stored.
    ///
    /// @return the maximum location. If the scaler is in a location higher than
    ///         the the primary data store then the location + 2U is returned,
    ///         otherwise the primary location + 4U is returned.
    ///
    size_t max() const override
    {
        return std::max(scalar_location + 2U, loc + 4U);
    }

    /// @brief Return the datatype associated with the entry.
    ///
    /// @return \c MdType::Float
    ///
    MdType type() const override { return MdType::Float; }

    /// @copydoc Rule_entry::clone
    std::unique_ptr<Rule_entry> clone() const override
    {
        return std::make_unique<Segy_float_rule_entry>(
            static_cast<segy::Trace_header_offsets>(loc),
            static_cast<segy::Trace_header_offsets>(scalar_location));
    }

    /// @copydoc Rule_entry::memory_usage()
    size_t memory_usage() const override { return sizeof *this; }
};

}  // namespace rules
}  // namespace metadata
}  // namespace piol
}  // namespace exseis

#endif  // EXSEIS_PIOL_METADATA_RULES_SEGY_RULE_ENTRY_SEGY_RULE_ENTRY_HH
