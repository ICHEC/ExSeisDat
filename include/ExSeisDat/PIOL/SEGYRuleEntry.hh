////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Instances of the \c RuleEntry class necessary for reading/writing
///        SEGY trace parameters.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_SEGYRULEENTRY_HH
#define EXSEISDAT_PIOL_SEGYRULEENTRY_HH

#include "ExSeisDat/PIOL/RuleEntry.hh"
#include "ExSeisDat/PIOL/segy_utils.hh"

#include <algorithm>

namespace PIOL {

/*! The Long rule entry structure for the SEG-Y format.
 */
struct SEGYLongRuleEntry : public RuleEntry {
    /*! The constructor.
     *  @param[in] num_ The numth entry for indexing purposes
     *  @param[in] loc_  The location of the primary data
     */
    SEGYLongRuleEntry(size_t num_, Tr loc_) : RuleEntry(num_, size_t(loc_)) {}

    /*! Return the minimum location stored, i.e loc just loc
     *  @return the minimum location
     */
    size_t min(void) { return loc; }

    /*! Return the maximum location stored up to, including the size of the data
     *  stored
     *  @return the maximum location plus 4 bytes to store an int32_t
     */
    size_t max(void) { return loc + 4U; }

    /*! Return the datatype associated with the entry.
     *  @return \c MdType::Long
     */
    MdType type(void) { return MdType::Long; }
};

/*! The Copy rule entry structure for the SEG-Y format.
 */
struct SEGYCopyRuleEntry : public RuleEntry {
    /*! The constructor (empty).
     */
    SEGYCopyRuleEntry(void) : RuleEntry(0U, 0U) {}

    /*! Return the minimum location stored, i.e 0
     *  @return 0U
     */
    size_t min(void) { return 0U; }

    /*! Return the size of the trace header
     *  @return the size of the trace header
     */
    size_t max(void) { return SEGY_utils::getMDSz(); }

    /*! Return the datatype associated with the entry.
     *  @return \c MdType::Copy
     */
    MdType type(void) { return MdType::Copy; }
};

/*! The Index rule entry structure. For indexing without modifying what
 *  will be stored.
 */
struct SEGYIndexRuleEntry : public RuleEntry {
    /*! The constructor.
     *  @param[in] num_ The numth entry for indexing purposes
     */
    SEGYIndexRuleEntry(size_t num_) : RuleEntry(num_, 0U) {}

    /*! Return 0. nothing stored
     *  @return Return 0
     */
    size_t min(void) { return 0; }

    /*! Return 0. nothing stored
     *  @return Return 0
     */
    size_t max(void) { return 0; }

    /*! Return the datatype associated with the entry.
     *  @return \c MdType::Index
     */
    MdType type(void) { return MdType::Index; }
};

/*! The Short rule entry structure for the SEG-Y format.
 */
struct SEGYShortRuleEntry : public RuleEntry {
    /*! The constructor.
     *  @param[in] num_ The numth entry for indexing purposes
     *  @param[in] loc_  The location of the primary data
     */
    SEGYShortRuleEntry(size_t num_, Tr loc_) : RuleEntry(num_, size_t(loc_)) {}

    /*! Return the minimum location stored, i.e loc
     *  @return the minimum location
     */
    size_t min(void) { return loc; }

    /*! Return the maximum location stored up to, including the size of the data
     *  stored
     *  @return the maximum location plus 2 bytes to store an int16_t
     */
    size_t max(void) { return loc + 2U; }

    /*! Return the datatype associated with the entry.
     *  @return \c MdType::Short
     */
    MdType type(void) { return MdType::Short; }
};

/*! The Float rule entry structure for the SEG-Y format.
 */
struct SEGYFloatRuleEntry : public RuleEntry {
    /// The location of the scaler field.
    size_t scalLoc;

    /*! The constructor.
     *  @param[in] num_ The numth entry for indexing purposes
     *  @param[in] loc_  The location of the primary data
     *  @param[in] scalLoc_ The location of the scaler field.
     */
    SEGYFloatRuleEntry(size_t num_, Tr loc_, Tr scalLoc_) :
        RuleEntry(num_, size_t(loc_)),
        scalLoc(size_t(scalLoc_))
    {
    }

    /*! Return the minimum location stored
     *  @return the minimum location
     */
    size_t min(void) { return std::min(scalLoc, loc); }

    /*! Return the maximum location stored up to, including the size of the data
     *  stored
     *  @return the maximum location. If the scaler is in a location higher than
     *          the the primary data store then the location + 2U is returned,
     *          otherwise the primary location + 4U is returned.
     */
    size_t max(void) { return std::max(scalLoc + 2U, loc + 4U); }

    /*! Return the datatype associated with the entry.
     *  @return \c MdType::Float
     */
    MdType type(void) { return MdType::Float; }
};

}  // namespace PIOL

#endif  // EXSEISDAT_PIOL_SEGYRULEENTRY_HH
