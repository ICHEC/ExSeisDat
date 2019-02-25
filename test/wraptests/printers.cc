#include "printers.hh"

#include "exseisdat/piol/Trace_metadata.hh"
#include "exseisdat/piol/operations/minmax.hh"

::std::ostream& operator<<(::std::ostream& os, const PIOL_CoordElem& value)
{
    os << "exseis::piol::CoordElem { "
       << ".val = " << value.val << ", "
       << ".num = " << value.num << " }";
    return os;
}

namespace exseis {
namespace piol {

::std::ostream& operator<<(::std::ostream& os, Trace_metadata* value)
{
    os << "exseis::piol::Trace_metadata* " << static_cast<void*>(value);
    return os;
}

::std::ostream& operator<<(::std::ostream& os, const Trace_metadata* value)
{
    os << "const exseis::piol::Trace_metadata* "
       << static_cast<const void*>(value);
    return os;
}

}  // namespace piol
}  // namespace exseis

namespace std {

::std::ostream& operator<<(
    ::std::ostream& os, const std::shared_ptr<exseis::piol::Rule>& value)
{
    os << "std::shared_ptr<exseis::piol::Rule> "
       << static_cast<void*>(value.get());
    return os;
}

}  // namespace std
