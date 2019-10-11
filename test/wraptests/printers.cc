#include "printers.hh"

#include "exseisdat/piol/metadata/Trace_metadata.hh"
#include "exseisdat/piol/operations/minmax.hh"


namespace exseis {
namespace piol {
inline namespace operations {

::std::ostream& operator<<(::std::ostream& os, const PIOL_CoordElem& value)
{
    os << "exseis::piol::CoordElem { "
       << ".val = " << value.val << ", "
       << ".num = " << value.num << " }";
    return os;
}

}  // namespace operations
}  // namespace piol
}  // namespace exseis


namespace exseis {
namespace piol {
inline namespace metadata {

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

}  // namespace metadata
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
