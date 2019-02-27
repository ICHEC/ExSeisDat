////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_OPOPT_HH
#define EXSEISDAT_FLOW_OPOPT_HH

#include "exseisdat/flow/FuncOpt.hh"

#include <algorithm>
#include <initializer_list>
#include <vector>

namespace exseis {
namespace flow {
namespace detail {

/*! A structure to hold operation options.
 */
class OpOpt {
  private:
    /// A list of the function options.
    std::vector<FuncOpt> m_option_list;

  public:
    /*! Empty constructor.
     */
    OpOpt(void) {}

    /*! Initialise the options list with an initialiser list.
     *  @param[in] list An initialiser list of options.
     */
    OpOpt(std::initializer_list<FuncOpt> list) : m_option_list(list) {}

    /*! Check if an option is present in the list.
     *  @param[in] opt The function option.
     *  @return Return true if the option is present in the list.
     */
    bool check(FuncOpt opt)
    {
        auto it = std::find(m_option_list.begin(), m_option_list.end(), opt);
        return it != m_option_list.end();
    }

    /*! Add an option to the list.
     *  @param[in] opt The function option.
     */
    void add(FuncOpt opt) { m_option_list.push_back(opt); }
};

}  // namespace detail
}  // namespace flow
}  // namespace exseis

#endif  // EXSEISDAT_FLOW_OPOPT_HH
