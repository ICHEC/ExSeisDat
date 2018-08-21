#ifndef PIOLWRAPTESTSPRINTERS_INCLUDE_GUARD
#define PIOLWRAPTESTSPRINTERS_INCLUDE_GUARD

#include <iostream>
#include <memory>

extern "C" struct PIOL_CoordElem;

::std::ostream& operator<<(::std::ostream& os, const PIOL_CoordElem& value);

namespace exseis {
namespace PIOL {

struct Rule;
struct Param;

::std::ostream& operator<<(::std::ostream& os, Param* value);
::std::ostream& operator<<(::std::ostream& os, const Param* value);

}  // namespace PIOL
}  // namespace exseis

namespace std {

::std::ostream& operator<<(
  ::std::ostream& os, const std::shared_ptr<exseis::PIOL::Rule>& value);

}  // namespace std

#endif  // PIOLWRAPTESTSPRINTERS_INCLUDE_GUARD
