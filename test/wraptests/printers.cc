#include "printers.hh"

#include "ExSeisDat/PIOL/Param.h"
#include "ExSeisDat/PIOL/operations/minmax.h"

::std::ostream& operator<<(::std::ostream& os, const PIOL_CoordElem& value)
{
    os << "PIOL::CoordElem { "
       << ".val = " << value.val << ", "
       << ".num = " << value.num << " }";
    return os;
}

namespace PIOL {

::std::ostream& operator<<(::std::ostream& os, PIOL::Param* value)
{
    os << "PIOL::Param* " << static_cast<void*>(value);
    return os;
}

::std::ostream& operator<<(::std::ostream& os, const PIOL::Param* value)
{
    os << "const PIOL::Param* " << static_cast<const void*>(value);
    return os;
}

}  // namespace PIOL

namespace std {

::std::ostream& operator<<(
  ::std::ostream& os, const std::shared_ptr<PIOL::Rule>& value)
{
    os << "std::shared_ptr<PIOL::Rule> " << static_cast<void*>(value.get());
    return os;
}

}  // namespace std
