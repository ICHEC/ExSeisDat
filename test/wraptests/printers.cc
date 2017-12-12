#include "printers.hh"

namespace PIOL {
namespace File {
    class Rule;

    ::std::ostream& operator<<(::std::ostream& os, PIOL::File::Param* value) {
        os << "PIOL::File::Param* " << static_cast<void*>(value);
        return os;
    }
} // namespace File
} // namespace PIOL

namespace std {
    ::std::ostream& operator<<(::std::ostream& os, const std::shared_ptr<PIOL::File::Rule>& value) {
        os << "std::shared_ptr<PIOL::File::Rule> " << static_cast<void*>(value.get());
        return os;
    }
} // namespace std
