#include "printers.hh"

#include "ExSeisDat/PIOL/Param.h"
#include "ExSeisDat/PIOL/operations/minmax.h"

::std::ostream& operator<<(::std::ostream& os, const PIOL_CoordElem& value)
{
    os << "exseis::PIOL::CoordElem { "
       << ".val = " << value.val << ", "
       << ".num = " << value.num << " }";
    return os;
}

namespace exseis {
namespace PIOL {

::std::ostream& operator<<(::std::ostream& os, Param* value)
{
    os << "exseis::PIOL::Param* " << static_cast<void*>(value);
    return os;
}

::std::ostream& operator<<(::std::ostream& os, const Param* value)
{
    os << "const exseis::PIOL::Param* " << static_cast<const void*>(value);
    return os;
}

}  // namespace PIOL
}  // namespace exseis

namespace std {

::std::ostream& operator<<(
  ::std::ostream& os, const std::shared_ptr<exseis::PIOL::Rule>& value)
{
    os << "std::shared_ptr<exseis::PIOL::Rule> "
       << static_cast<void*>(value.get());
    return os;
}

}  // namespace std
