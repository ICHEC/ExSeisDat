////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Instances of the \c RuleEntry class necessary for reading/writing
///        SEGY trace parameters.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_SEGYRULEENTRY_HH
#define EXSEISDAT_PIOL_SEGYRULEENTRY_HH

#include "exseisdat/piol/RuleEntry.hh"
#include "exseisdat/piol/segy/utils.hh"

#include <algorithm>

namespace exseis {
namespace piol {

/*! The Long rule entry structure for the SEG-Y format.
 */
struct SEGYLongRuleEntry : public RuleEntry {
    /*! The constructor.
     *  @param[in] loc  The location of the primary data
     */
    SEGYLongRuleEntry(Tr loc) : RuleEntry(size_t(loc)) {}

    /*! Return the minimum location stored, i.e loc just loc
     *  @return the minimum location
     */
    size_t min() const override { return loc; }

    /*! Return the maximum location stored up to, including the size of the data
     *  stored
     *  @return the maximum location plus 4 bytes to store an int32_t
     */
    size_t max() const override { return loc + 4U; }

    /*! Return the datatype associated with the entry.
     *  @return \c MdType::Long
     */
    MdType type() const override { return MdType::Long; }

    /// @copydoc RuleEntry::clone
    std::unique_ptr<RuleEntry> clone() const override
    {
        return std::make_unique<SEGYLongRuleEntry>(static_cast<Tr>(loc));
    }

    /// @copydoc RuleEntry::memory_usage()
    size_t memory_usage() const override { return sizeof *this; }
};

/*! The Copy rule entry structure for the SEG-Y format.
 */
struct SEGYCopyRuleEntry : public RuleEntry {
    /*! The constructor (empty).
     */
    SEGYCopyRuleEntry() : RuleEntry(0U) {}

    /*! Return the minimum location stored, i.e 0
     *  @return 0U
     */
    size_t min() const override { return 0U; }

    /*! Return the size of the trace header
     *  @return the size of the trace header
     */
    size_t max() const override { return segy::segy_trace_header_size(); }

    /*! Return the datatype associated with the entry.
     *  @return \c MdType::Copy
     */
    MdType type() const override { return MdType::Copy; }

    /// @copydoc RuleEntry::clone
    std::unique_ptr<RuleEntry> clone() const override
    {
        return std::make_unique<SEGYCopyRuleEntry>();
    }

    /// @copydoc RuleEntry::memory_usage()
    size_t memory_usage() const override { return sizeof *this; }
};

/*! The Index rule entry structure. For indexing without modifying what
 *  will be stored.
 */
struct SEGYIndexRuleEntry : public RuleEntry {
    /*! The constructor.
     */
    SEGYIndexRuleEntry() : RuleEntry(0U) {}

    /*! Return 0. nothing stored
     *  @return Return 0
     */
    size_t min() const override { return 0; }

    /*! Return 0. nothing stored
     *  @return Return 0
     */
    size_t max() const override { return 0; }

    /*! Return the datatype associated with the entry.
     *  @return \c MdType::Index
     */
    MdType type() const override { return MdType::Index; }

    /// @copydoc RuleEntry::clone
    std::unique_ptr<RuleEntry> clone() const override
    {
        return std::make_unique<SEGYIndexRuleEntry>();
    }

    /// @copydoc RuleEntry::memory_usage()
    size_t memory_usage() const override { return sizeof *this; }
};

/*! The Short rule entry structure for the SEG-Y format.
 */
struct SEGYShortRuleEntry : public RuleEntry {
    /*! The constructor.
     *  @param[in] loc  The location of the primary data
     */
    SEGYShortRuleEntry(Tr loc) : RuleEntry(size_t(loc)) {}

    /*! Return the minimum location stored, i.e loc
     *  @return the minimum location
     */
    size_t min() const override { return loc; }

    /*! Return the maximum location stored up to, including the size of the data
     *  stored
     *  @return the maximum location plus 2 bytes to store an int16_t
     */
    size_t max() const override { return loc + 2U; }

    /*! Return the datatype associated with the entry.
     *  @return \c MdType::Short
     */
    MdType type() const override { return MdType::Short; }

    /// @copydoc RuleEntry::clone
    std::unique_ptr<RuleEntry> clone() const override
    {
        return std::make_unique<SEGYShortRuleEntry>(static_cast<Tr>(loc));
    }

    /// @copydoc RuleEntry::memory_usage()
    size_t memory_usage() const override { return sizeof *this; }
};

/*! The Float rule entry structure for the SEG-Y format.
 */
struct SEGYFloatRuleEntry : public RuleEntry {
    /// The location of the scaler field.
    size_t scalar_location;

    /*! The constructor.
     *  @param[in] loc  The location of the primary data
     *  @param[in] scalar_location The location of the scaler field.
     */
    SEGYFloatRuleEntry(Tr loc, Tr scalar_location) :
        RuleEntry(size_t(loc)),
        scalar_location(size_t(scalar_location))
    {
    }

    /*! Return the minimum location stored
     *  @return the minimum location
     */
    size_t min() const override { return std::min(scalar_location, loc); }

    /*! Return the maximum location stored up to, including the size of the data
     *  stored
     *  @return the maximum location. If the scaler is in a location higher than
     *          the the primary data store then the location + 2U is returned,
     *          otherwise the primary location + 4U is returned.
     */
    size_t max() const override
    {
        return std::max(scalar_location + 2U, loc + 4U);
    }

    /*! Return the datatype associated with the entry.
     *  @return \c MdType::Float
     */
    MdType type() const override { return MdType::Float; }

    /// @copydoc RuleEntry::clone
    std::unique_ptr<RuleEntry> clone() const override
    {
        return std::make_unique<SEGYFloatRuleEntry>(
            static_cast<Tr>(loc), static_cast<Tr>(scalar_location));
    }

    /// @copydoc RuleEntry::memory_usage()
    size_t memory_usage() const override { return sizeof *this; }
};

}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_SEGYRULEENTRY_HH
