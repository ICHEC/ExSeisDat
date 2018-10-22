#ifndef EXSEISDAT_TEST_WRAPTESTS_PRINTERS_HH
#define EXSEISDAT_TEST_WRAPTESTS_PRINTERS_HH

#include <iostream>
#include <memory>

extern "C" struct PIOL_CoordElem;

::std::ostream& operator<<(::std::ostream& os, const PIOL_CoordElem& value);

namespace exseis {
namespace piol {

class Rule;
class Trace_metadata;

::std::ostream& operator<<(::std::ostream& os, Trace_metadata* value);
::std::ostream& operator<<(::std::ostream& os, const Trace_metadata* value);

}  // namespace piol
}  // namespace exseis

namespace std {

::std::ostream& operator<<(
  ::std::ostream& os, const std::shared_ptr<exseis::piol::Rule>& value);

}  // namespace std

#endif  // EXSEISDAT_TEST_WRAPTESTS_PRINTERS_HH
