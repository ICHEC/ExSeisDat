#ifndef PIOLWRAPTESTSPRINTERS_INCLUDE_GUARD
#define PIOLWRAPTESTSPRINTERS_INCLUDE_GUARD

#include <iostream>
#include <memory>

namespace PIOL {
namespace File {
    class Rule;
    class Param;

    ::std::ostream& operator<<(::std::ostream& os, PIOL::File::Param* value);
    ::std::ostream& operator<<(::std::ostream& os, const PIOL::File::Param* value);
} // namespace File
} // namespace PIOL

namespace std {
    ::std::ostream& operator<<(::std::ostream& os, const std::shared_ptr<PIOL::File::Rule>& value);
} // namespace std

#endif // PIOLWRAPTESTSPRINTERS_INCLUDE_GUARD
