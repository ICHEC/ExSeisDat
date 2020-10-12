////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The \c RuleEntry class, representing where and how in a file a
///        particular trace parameter should be written.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEIS_PIOL_METADATA_RULES_RULE_ENTRY_HH
#define EXSEIS_PIOL_METADATA_RULES_RULE_ENTRY_HH

#include <cstddef>
#include <memory>

namespace exseis {
inline namespace piol {
inline namespace metadata {
inline namespace rules {

/*! @brief An instantiation of this structure corresponds to a single metadata
 * rule
 */
class Rule_entry {
  public:
    /// The type of data
    enum class MdType : size_t {
        /// Long int data
        Long,

        /// Short int data
        Short,

        /// Floating point data
        Float,

        /// For indexing purposes
        Index,

        /// Copy all relevant headers. Not file format agnostic.
        Copy
    };

    /// The memory location for the primary data.
    size_t loc;

    /*! @brief The constructor for storing the rule number and location.
     *
     *  @param[in] loc The location of the primary data
     */
    Rule_entry(size_t loc) : loc(loc) {}

    /*! @brief Virtual destructor.
     */
    virtual ~Rule_entry() = default;

    /*! @brief Return the minimum location stored, in derived cases, more data
     *         can be stored than just loc.
     *
     *  @return the minimum location
     */
    virtual size_t min() const = 0;

    /*! @brief Return the maximum location stored up to, in derived cases, more
     *         data can be stored than just loc
     *  @return the maximum location
     */
    virtual size_t max() const = 0;

    /*! @brief Return the datatype.
     *  @return The MdType associated with the derived class.
     */
    virtual MdType type() const = 0;

    /// @brief Return a duplicate of the RuleEntry.
    ///
    /// @returns A pointer to a duplicate of the current instance.
    ///
    virtual std::unique_ptr<Rule_entry> clone() const = 0;

    /// @brief Query the amount of memory used by the current instance.
    ///
    /// @return The amount of memory used by the current instance.
    virtual size_t memory_usage() const = 0;
};

}  // namespace rules
}  // namespace metadata
}  // namespace piol
}  // namespace exseis

#endif  // EXSEIS_PIOL_METADATA_RULES_RULE_ENTRY_HH
