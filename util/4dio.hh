/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \date January 2017
 *   \brief
 *   \details
 *//*******************************************************************************************/

#ifndef FOURDBIN4DIO_INCLUDE_GUARD
#define FOURDBIN4DIO_INCLUDE_GUARD
#include "4d.hh"
namespace PIOL { namespace FOURD {
/*! This structure is for holding ALIGN aligned memory containing the coordinates.
 */
struct Coords
{
    size_t sz;
    geom_t * xSrc;
    geom_t * ySrc;
    geom_t * xRcv;
    geom_t * yRcv;
    size_t * tn;
    const size_t align = ALIGN;
    size_t allocSz;
    Coords(size_t sz_) : sz(sz_)
    {
        allocSz = ((sz + align) / align) * align;

        //posix_memalign() guarantees the memory allocated is alligned according to the alignment
        //value
        posix_memalign(reinterpret_cast<void **>(&xSrc), align, allocSz * sizeof(geom_t));
        posix_memalign(reinterpret_cast<void **>(&ySrc), align, allocSz * sizeof(geom_t));
        posix_memalign(reinterpret_cast<void **>(&xRcv), align, allocSz * sizeof(geom_t));
        posix_memalign(reinterpret_cast<void **>(&yRcv), align, allocSz * sizeof(geom_t));
        posix_memalign(reinterpret_cast<void **>(&tn), align, allocSz * sizeof(size_t));
        for (size_t i = 0; i < allocSz; i++)
            xSrc[i] = ySrc[i] = xRcv[i] = yRcv[i] = std::numeric_limits<geom_t>::max();
    }

    ~Coords(void)
    {
        if (xSrc)
            free(xSrc);
        if (ySrc)
            free(ySrc);
        if (xRcv)
            free(xRcv);
        if (yRcv)
            free(yRcv);
    }
};

extern void getCoords(ExSeisPIOL * piol, File::Interface * file, size_t offset, Coords * coords);
extern void selectDupe(ExSeisPIOL * piol, std::shared_ptr<File::Rule> rule, File::Direct & dst, File::Direct & src, vec<size_t> & list, vec<geom_t> & minrs);
extern void select(ExSeisPIOL * piol, std::shared_ptr<File::Rule> rule, File::Direct & dst, File::Direct & src, vec<size_t> & list, vec<geom_t> & minrs);
}}
#endif
