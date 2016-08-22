#include "global.hh"
#include <memory>
#include "file/file.hh"
namespace PIOL { namespace File {
extern geom_t xmin(Piol piol, size_t offset, size_t sz, const coord_t * val);
extern geom_t xmin(Piol piol, size_t sz, const coord_t * val);
}}
