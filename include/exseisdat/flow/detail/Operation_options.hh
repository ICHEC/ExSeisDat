////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_DETAIL_OPERATION_OPTIONS_HH
#define EXSEISDAT_FLOW_DETAIL_OPERATION_OPTIONS_HH

#include "exseisdat/flow/detail/Function_options.hh"

#include <algorithm>
#include <initializer_list>
#include <vector>

namespace exseis {
namespace flow {
namespace detail {

/*! A structure to hold operation options.
 */
class Operation_options {
  private:
    /// A list of the function options.
    std::vector<Function_options> m_option_list;

  public:
    /*! Empty constructor.
     */
    Operation_options() {}

    /*! Initialise the options list with an initialiser list.
     *  @param[in] list An initialiser list of options.
     */
    Operation_options(std::initializer_list<Function_options> list) :
        m_option_list(list)
    {
    }

    /*! Check if an option is present in the list.
     *  @param[in] opt The function option.
     *  @return Return true if the option is present in the list.
     */
    bool check(Function_options opt)
    {
        auto it = std::find(m_option_list.begin(), m_option_list.end(), opt);
        return it != m_option_list.end();
    }

    /*! Add an option to the list.
     *  @param[in] opt The function option.
     */
    void add(Function_options opt) { m_option_list.push_back(opt); }
};

}  // namespace detail
}  // namespace flow
}  // namespace exseis

#endif  // EXSEISDAT_FLOW_DETAIL_OPERATION_OPTIONS_HH
