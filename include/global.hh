/*
    Insert trivial typdefs etc into this file.
*/
#ifndef PIOLGLOBAL_INCLUDE_GUARD
#define PIOLGLOBAL_INCLUDE_GUARD
#include <cstdint>
#include <memory>
typedef unsigned char uchar;    //!< The type to use for accessing individual bytes
typedef int64_t llint;          //!< A large signed integer type.
typedef float trace_t;          //!< Real type for traces, use "*_t" convention
typedef double geom_t;          //!< Real type for geometry
typedef double unit_t;          //!< Real type for geometry
typedef const std::size_t csize_t;

#include "anc/log.hh"
#include "anc/piol.hh"

//TODO: Document
enum class FileMode : size_t
{
    Read,
    Write,
    ReadWrite,
    Test
};

namespace PIOL {
namespace Data { class Interface; }
namespace Obj { class Interface; }
namespace File { class Interface; }
}
#endif
