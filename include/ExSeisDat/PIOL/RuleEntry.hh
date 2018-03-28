////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The \c RuleEntry class, representing where and how in a file a
///        particular trace parameter should be written.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_RULEENTRY_HH
#define EXSEISDAT_PIOL_RULEENTRY_HH

#include <cstddef>

namespace PIOL {
namespace File {

/*! An instantiation of this structure corresponds to a single metadata rule
 */
struct RuleEntry {
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

    /// A counter showing this is the numth rule for the given data type.
    size_t num;

    /// The memory location for the primary data.
    size_t loc;

    /*! The constructor for storing the rule number and location.
     *  @param[in] num_ The numth rule of the given type for indexing
     *  @param[in] loc_ The location of the primary data
     */
    RuleEntry(const size_t num_, const size_t loc_) : num(num_), loc(loc_) {}

    /*! Virtual destructor to allow overriding by derived classes.
     */
    virtual ~RuleEntry() = default;

    /*! Pure virtual function to return the minimum location stored, in derived
     *  cases, more data can be stored than just loc
     *  @return the minimum location
     */
    virtual size_t min(void) = 0;

    /*! Pure virtual function to return the maximum location stored up to, in
     *  derived cases, more data can be stored than just loc
     *  @return the maximum location
     */
    virtual size_t max(void) = 0;

    /*! Pure virtual function to return the datatype.
     *  @return The MdType associated with the derived class.
     */
    virtual MdType type(void) = 0;
};

} // namespace File
} // namespace PIOL

#endif // EXSEISDAT_PIOL_RULEENTRY_HH
