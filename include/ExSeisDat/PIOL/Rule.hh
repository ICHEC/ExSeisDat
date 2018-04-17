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
#ifndef EXSEISDAT_PIOL_RULE_HH
#define EXSEISDAT_PIOL_RULE_HH

#include "ExSeisDat/PIOL/Meta.h"
#include "ExSeisDat/PIOL/RuleEntry.hh"
#include "ExSeisDat/PIOL/Tr.h"

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace exseis {
namespace PIOL {

/*! This function exists to address a defect in enum usage in a map
 *  that is present in the intel and older GNU compilers.
 */
struct EnumHash {
    /*! This overload describes how to convert from the enum to a size_t
     *  @tparam T The enum type
     *  @param[in] t The enum value
     *  @return Return a cast to size_t
     */
    template<typename T>
    size_t operator()(T t) const
    {
        return static_cast<size_t>(t);
    }
};


/*! The structure which holds the rules associated with the trace parameters in
 *  a file.  These rules describe how to interpret the metadata and also how to
 *  index the parameter structure of arrays.
 */
struct Rule {
    /// Number of long rules.
    size_t numLong = 0;

    /// Number of float rules.
    size_t numFloat = 0;

    /// Number of short rules.
    size_t numShort = 0;

    /// Number of index rules.
    size_t numIndex = 0;

    /// Number of copy rules. either 0 or 1.
    size_t numCopy = 0;

    /// The starting byte position in the SEG-Y header.
    size_t start;

    /// The end byte position (+ 1) in the SEG-Y header.
    size_t end;

    /// A structure for storing the state of the extent and header buffer
    /// processing.
    struct StateFlags {
        /// Flag marking if the extent calculation is stale.
        uint32_t badextent;

        /// Flag marking if the full header buffer is processed.
        uint32_t fullextent;
    };

    /// The StateFlags instance for the Rule instance.
    StateFlags flag;

    /*! The type of the unordered map which stores all current rules.
     *  A map ensures there are no duplicates. */
    typedef std::unordered_map<Meta, RuleEntry*, EnumHash> RuleMap;

    /// The map storing all the current rules.
    RuleMap translate;

    /*! The constructor for creating a Rule structure with
     *  default rules in place or no rules in place.
     *  @param[in] full Whether the extents are set to the default size or
     *                  calculated dynamically.
     *  @param[in] defaults Whether the default SEG-Y rules should be set.
     *  @param[in] extras Whether maximum amount of rules should be set. Useful
     *                    when copying files through the library.
     */
    Rule(bool full, bool defaults, bool extras = false);

    /*! The constructor for supplying a list of Meta entries which
     *  have default locations associated with them.
     *  @param[in] m A list of meta entries with default entries. Entries
     *               without defaults will be ignored.
     *  @param[in] full Whether the extents are set to the default size or
     *                  calculated dynamically.
     *  @param[in] defaults Whether the default SEG-Y rules should be set.
     *  @param[in] extras Whether maximum amount of rules should be set. Useful
     *                    when copying files through the library.
     */
    Rule(
      const std::vector<Meta>& m,
      bool full     = true,
      bool defaults = false,
      bool extras   = false);

    /*! The constructor for creating a Rule structure with
     *  default rules in place or no rules in place.
     *  @param[in] translate_ An unordered map to initialise the internal
     *                        translate object with.
     *  @param[in] full Whether the extents are set to the default size or
     *                  calculated dynamically.
     */
    Rule(RuleMap translate_, bool full = true);

    /*! The destructor. Deallocates the memory associated with the rule entries.
     */
    ~Rule(void);

    /*! Add a pre-defined rule.
     *  @param[in] m The Meta entry.
     *  @return Return true if the rule was added, otherwise false
     */
    bool addRule(Meta m);

    /*! Add all rules from the given argument
     *  @param[in] r Another rule pointer.
     *  @return Return true if no errors
     */
    bool addRule(const Rule& r);

    /*! Add a rule for longs.
     *  @param[in] m The Meta entry.
     *  @param[in] loc The location in the SEG-Y DOMD (4 bytes).
     */
    void addLong(Meta m, Tr loc);

    /*! Add a rule for floats.
     *  @param[in] m The Meta entry.
     *  @param[in] loc The location in the SEG-Y DOMD for the primary data to be
     *                 stored (4 bytes).
     *  @param[in] scalLoc The location in the SEG-Y DOMD for the scaler to be
     *                     stored (2 bytes).
     */
    void addSEGYFloat(Meta m, Tr loc, Tr scalLoc);

    /*! Add a rule for floats.
     *  @param[in] m The Meta entry.
     *  @param[in] loc The location in the SEG-Y DOMD for the primary data to be
     *                 stored (2 bytes).
     */
    void addShort(Meta m, Tr loc);

    /*! Add a rule for an index.
     *  @param[in] m The Meta entry.
     */
    void addIndex(Meta m);

    /*! Add a rule to buffer the original trace header.
     */
    void addCopy(void);

    /*! Remove a rule based on the meta entry.
     *  @param[in] m The meta entry.
     */
    void rmRule(Meta m);

    /*! Return the size of the buffer space required for the metadata items when
     *  converting to SEG-Y.
     *  @return Return the size.
     */
    size_t extent(void);

    /*! Estimate of the total memory used
     *  @return Return estimate in bytes.
     */
    size_t memUsage(void) const;

    /*! How much memory will each set of parameters require?
     *  @return Amount of memory in bytes.
     */
    size_t paramMem() const;

    /*! Get the rule entry associated with a particular meta entry.
     *  @param[in] entry The meta entry.
     *  @return The associated rule entry.
     */
    RuleEntry* getEntry(Meta entry);
};

}  // namespace PIOL
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_RULE_HH
