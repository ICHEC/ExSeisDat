////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_OPOPT_HH
#define EXSEISDAT_FLOW_OPOPT_HH

#include "ExSeisDat/Flow/FuncOpt.hh"

#include <algorithm>
#include <initializer_list>
#include <vector>

namespace PIOL {

/*! A structure to hold operation options.
 */
class OpOpt {
    /// A list of the function options.
    std::vector<FuncOpt> optList;

  public:
    /*! Empty constructor.
     */
    OpOpt(void) {}

    /*! Initialise the options list with an initialiser list.
     *  @param[in] list An initialiser list of options.
     */
    OpOpt(std::initializer_list<FuncOpt> list) : optList(list) {}

    /*! Check if an option is present in the list.
     *  @param[in] opt The function option.
     *  @return Return true if the option is present in the list.
     */
    bool check(FuncOpt opt)
    {
        auto it = std::find(optList.begin(), optList.end(), opt);
        return it != optList.end();
    }

    /*! Add an option to the list.
     *  @param[in] opt The function option.
     */
    void add(FuncOpt opt) { optList.push_back(opt); }
};

}  // namespace PIOL

#endif  // EXSEISDAT_FLOW_OPOPT_HH
