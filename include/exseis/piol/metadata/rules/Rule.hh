////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The state of this file is temporarily SEG-Y specific, when new
///        formats are investigated this file should be split into a
///        format-agnostic component and a SEG-Y specific component.
/// @details The SEG-Y implementation of metadata is dynamic to reflect the
///          actual usage by geophysicists where SEG-Y metadata fields are
///          routinely used for other purposes.  as such, the SEG-Y standard is
///          treated as a default series of rules which can be removed and
///          replaced. A rule specifies a specific type of metadata and
///          particular locations associated with that rule. For example, each
///          Float rule requires two locations to be specified, a location for a
///          corresponding integer and a location for a scaling factor.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEIS_PIOL_METADATA_RULES_RULE_HH
#define EXSEIS_PIOL_METADATA_RULES_RULE_HH

#include "exseis/piol/metadata/Trace_metadata_key.hh"
#include "exseis/piol/metadata/rules/Rule_entry.hh"
#include "exseis/piol/segy/Trace_header_offsets.hh"

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace exseis {
inline namespace piol {
inline namespace metadata {
inline namespace rules {

namespace detail {

/// @brief Hash class for Trace_metadata_key
/// This is a workaround for some compilers which don't properly support
/// std::hash for enum types
class Trace_metadata_key_hash :
    public std::hash<std::underlying_type<Trace_metadata_key>::type> {
    using Base = std::hash<std::underlying_type<Trace_metadata_key>::type>;

  public:
    /// @brief Forward a Trace_metadata_key type to the base hasher
    result_type operator()(Trace_metadata_key key) const
    {
        return static_cast<const Base&>(*this)(
            static_cast<std::underlying_type<Trace_metadata_key>::type>(key));
    }
};

}  // namespace detail


/// @brief Rules associated with trace parameters
///
/// @details The structure which holds the rules associated with the trace
///          parameters in a file. These rules describe how to interpret the
///          metadata and also how to index the parameter structure of arrays.
///
class Rule {
  public:
    /// @brief Number of copy rules. either 0 or 1.
    size_t num_copy = 0;

    /// @brief The starting byte position in the SEG-Y header.
    size_t start = 0;

    /// @brief The end byte position (+ 1) in the SEG-Y header.
    size_t end = 0;

    /// @brief A structure for storing the state of the extent and header buffer
    ///        processing.
    struct State_flags {
        /// @brief Flag marking if the extent calculation is stale.
        bool badextent = true;

        /// @brief Flag marking if the full header buffer is processed.
        bool fullextent = true;
    };

    /// @brief The State_flags instance for the Rule instance.
    State_flags flag;

    /// @brief The type of the unordered map which stores all current rules.
    /// A map ensures there are no duplicates.
    ///
    using Rule_entry_map = std::unordered_map<
        Trace_metadata_key,
        std::unique_ptr<Rule_entry>,
        detail::Trace_metadata_key_hash>;

    /// @brief The map storing all the current rules.
    Rule_entry_map rule_entry_map;

    /// @brief The copy constructor
    ///
    /// @param[in] rule The Rule to copy.
    ///
    Rule(const Rule& rule) : Rule(rule.rule_entry_map, rule.flag.fullextent) {}

    /// @brief The copy assignment operator
    ///
    /// @param[in] rhs The Rule to copy.
    ///
    /// @returns The current instance, set to the same value as `rhs`.
    ///
    Rule& operator=(const Rule& rhs);


    /// @brief Test equality of two rules
    ///
    /// @param[in] rhs The value to test against this.
    ///
    /// @returns True if equal, false otherwise.
    ///
    bool operator==(const Rule& rhs) const
    {
        return this->rule_entry_map == rhs.rule_entry_map;
    }

    /// @brief The constructor for creating a Rule structure with
    ///        default rules in place or no rules in place.
    ///
    /// @param[in] full     Whether the extents are set to the default size or
    ///                     calculated dynamically.
    /// @param[in] defaults Whether the default SEG-Y rules should be set.
    /// @param[in] extras   Whether maximum amount of rules should be set.
    ///                     Useful when copying files through the library.
    ///
    Rule(bool full, bool defaults, bool extras = false);

    /// @brief The constructor for supplying a list of Meta entries which
    ///        have default locations associated with them.
    ///
    /// @param[in] m        A list of meta entries with default entries. Entries
    ///                     without defaults will be ignored.
    /// @param[in] full     Whether the extents are set to the default size or
    ///                     calculated dynamically.
    /// @param[in] defaults Whether the default SEG-Y rules should be set.
    /// @param[in] extras   Whether maximum amount of rules should be set.
    ///                     Useful when copying files through the library.
    ///
    Rule(
        const std::vector<Trace_metadata_key>& m,
        bool full     = true,
        bool defaults = false,
        bool extras   = false);

    /// @brief The constructor for creating a Rule structure with
    ///        default rules in place or no rules in place.
    ///
    /// @param[in] rule_entry_map   An unordered map to initialise the internal
    ///                             rule_entry_map object with.
    /// @param[in] full             Whether the extents are set to the default
    ///                             size or calculated dynamically.
    ///
    Rule(const Rule_entry_map& rule_entry_map, bool full = true);

    /// @brief The destructor. Deallocates the memory associated with the rule entries.
    ///
    ~Rule();

    /// @brief Add a pre-defined rule.
    ///
    /// @param[in] m The Meta entry.
    ///
    /// @return Return true if the rule was added, otherwise false
    ///
    bool add_rule(Trace_metadata_key m);

    /// @brief Add all rules from the given argument
    ///
    /// @param[in] r Another rule pointer.
    ///
    /// @return Return true if no errors
    ///
    bool add_rule(const Rule& r);

    /// @brief Add a rule for longs.
    ///
    /// @param[in] m    The Meta entry.
    /// @param[in] loc  The location in the SEG-Y Trace Metadata (4 bytes).
    ///
    void add_long(Trace_metadata_key m, segy::Trace_header_offsets loc);

    /// @brief Add a rule for floats.
    ///
    /// @param[in] m                The Meta entry.
    /// @param[in] loc              The location in the SEG-Y Trace Metadata for
    ///                             the primary data to be stored (4 bytes).
    /// @param[in] scalar_location  The location in the SEG-Y Trace Metadata for
    ///                             the scaler to be stored (2 bytes).
    ///
    void add_segy_float(
        Trace_metadata_key m,
        segy::Trace_header_offsets loc,
        segy::Trace_header_offsets scalar_location);

    /// @brief Add a rule for floats.
    ///
    /// @param[in] m    The Meta entry.
    /// @param[in] loc  The location in the SEG-Y Trace Metadata for the primary
    ///                 data to be stored (2 bytes).
    ///
    void add_short(Trace_metadata_key m, segy::Trace_header_offsets loc);

    /// @brief Add a rule for an index.
    ///
    /// @param[in] m The Meta entry.
    ///
    void add_index(Trace_metadata_key m);

    /// @brief Add a rule to buffer the original trace header.
    ///
    void add_copy();

    /// @brief Remove a rule based on the meta entry.
    ///
    /// @param[in] m The meta entry.
    ///
    void rm_rule(Trace_metadata_key m);

    /// @brief Return the size of the buffer space required for the metadata
    ///        items when converting to SEG-Y.
    ///
    /// @return Return the size.
    ///
    size_t extent();

    /// @overload
    /// @copydoc Rule::extent()
    size_t extent() const;

    /// @brief Estimate of the total memory used
    ///
    /// @return Return estimate in bytes.
    ///
    size_t memory_usage() const;

    /// @brief How much memory will each set of parameters require?
    ///
    /// @return Amount of memory in bytes.
    ///
    size_t memory_usage_per_header() const;

    /// @brief Get the rule entry associated with a particular meta entry.
    ///
    /// @details If there is no rule entry associated with the meta entry, this
    ///          function returns a nullptr.
    ///
    /// @param[in] entry The meta entry.
    ///
    /// @return The associated rule entry, or nullptr if there is none.
    ///
    const Rule_entry* get_entry(Trace_metadata_key entry) const;
};

}  // namespace rules
}  // namespace metadata
}  // namespace piol
}  // namespace exseis

#endif  // EXSEIS_PIOL_METADATA_RULES_RULE_HH
