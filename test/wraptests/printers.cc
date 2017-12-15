#include "printers.hh"

#include "share/param.hh"
#include "share/api.hh"
#include "file/dynsegymd.hh"

::std::ostream& operator<<(::std::ostream& os, const PIOL_CoordElem& value) {
    os << "PIOL::CoordElem { "
        << ".val = " << value.val << ", "
        << ".num = " << value.num
        << " }";
}

namespace PIOL {

namespace File {
    class Rule;

    ::std::ostream& operator<<(::std::ostream& os, PIOL::File::Param* value) {
        os << "PIOL::File::Param* " << static_cast<void*>(value);
        return os;
    }

    ::std::ostream& operator<<(::std::ostream& os, const PIOL::File::Param* value) {
        os << "const PIOL::File::Param* " << static_cast<const void*>(value);
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
