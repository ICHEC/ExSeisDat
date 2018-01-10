#ifndef PIOLWRAPTESTSPRINTERS_INCLUDE_GUARD
#define PIOLWRAPTESTSPRINTERS_INCLUDE_GUARD

#include <iostream>
#include <memory>

extern "C" struct PIOL_CoordElem;

::std::ostream& operator<<(::std::ostream& os, const PIOL_CoordElem& value);

namespace PIOL {

namespace File {
    struct Rule;
    struct Param;

    ::std::ostream& operator<<(::std::ostream& os, PIOL::File::Param* value);
    ::std::ostream& operator<<(::std::ostream& os, const PIOL::File::Param* value);
} // namespace File
} // namespace PIOL

namespace std {
    ::std::ostream& operator<<(::std::ostream& os, const std::shared_ptr<PIOL::File::Rule>& value);
} // namespace std

#endif // PIOLWRAPTESTSPRINTERS_INCLUDE_GUARD
