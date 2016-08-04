/*
    Insert trivial typdefs etc into this file.
*/
#ifndef PIOLGLOBAL_INCLUDE_GUARD
#define PIOLGLOBAL_INCLUDE_GUARD
#include <cstdint>
#define MODERN_CPP !DEFINED(__cplusplus) || __STDC_VERSION__ < 201103L

typedef unsigned char uchar;    //!< The type to use for accessing individual bytes
typedef int64_t llint;          //!< A large signed integer type.
typedef float trace_t;          //!< Real type for traces, use "*_t" convention
typedef double geom_t;          //!< Real type for geometry
typedef double unit_t;          //!< Real type for geometry
typedef const std::size_t csize_t;

#include "anc/piol.hh"
#include "anc/log.hh"

#include <iostream>
#warning remove this

namespace PIOL { namespace Data {
class Interface;
struct Opt;
}}

namespace PIOL { namespace Obj {
class Interface;
struct Opt;
}}

namespace PIOL { namespace File {
class Interface;
struct Opt;
}}
#endif
