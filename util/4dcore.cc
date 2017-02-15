#include <algorithm>
#include "4dcore.hh"

namespace PIOL { namespace FOURD {
void initUpdate(size_t offset, Coords * local, Coords * other, vec<size_t> & min, vec<geom_t> & minrs)
{
    for (size_t i = 0; i < local->sz; i++)
    {
        minrs[i] = dsr(local->xSrc[i], local->ySrc[i], local->xRcv[i], local->yRcv[i],
                       other->xSrc[0], other->ySrc[0], other->xRcv[0], other->yRcv[0]);
        min[i] = offset;
    }
}

void update(size_t offset, Coords * local, Coords * other, vec<size_t> & min, vec<geom_t> & minrs)
{
    size_t sz = local->sz;
    //For the vectorisation
    const geom_t * lxS = local->xSrc;
    const geom_t * lyS = local->ySrc;
    const geom_t * lxR = local->xRcv;
    const geom_t * lyR = local->yRcv;

    const geom_t * rxS = other->xSrc;
    const geom_t * ryS = other->ySrc;
    const geom_t * rxR = other->xRcv;
    const geom_t * ryR = other->yRcv;

    const size_t * tn = other->tn;

    //Copy min and minrs to aligned memory
    geom_t * lminrs;
    size_t * lmin;
    size_t allocSz = ((sz + ALIGN) / ALIGN) * ALIGN;
    posix_memalign(reinterpret_cast<void **>(&lminrs), ALIGN, allocSz * sizeof(geom_t));
    posix_memalign(reinterpret_cast<void **>(&lmin), ALIGN, allocSz * sizeof(size_t));
    std::copy(min.begin(), min.begin()+sz, lmin);
    std::copy(minrs.begin(), minrs.begin()+sz, lminrs);

    #pragma omp simd aligned(rxS:ALIGN) aligned(ryS:ALIGN) aligned(rxR:ALIGN) aligned(ryR:ALIGN) \
                     aligned(lxS:ALIGN) aligned(lyS:ALIGN) aligned(lxR:ALIGN) aligned(lyR:ALIGN) \
                     aligned(lminrs:ALIGN) aligned(lmin:ALIGN) aligned(tn:ALIGN)
    for (size_t i = 0; i < allocSz; i++)                         //Loop through every file1 trace
    {
        geom_t lxs = lxS[i], lys = lyS[i], lxr = lxR[i], lyr = lyR[i];
        size_t lm = lmin[i];
        geom_t lmrs = lminrs[i];
//TODO: Try vectorise this when custom reductions are supported by the intel compiler
//The main issue is a structure like std::pair<geom_t, size_t> needs to be reduced rather than lm or lmrs individually
        for (size_t j = 0U; j < sz; j++)        //loop through a multiple of the alignment
        {
            geom_t dval = dsr(lxs, lys, lxr, lyr,
                              rxS[j], ryS[j], rxR[j], ryR[j]);
            lm = (dval < lmrs ? tn[j] : lm);      //Update min if applicable
            lmrs = std::min(dval, lmrs);            //Update minrs if applicable
        }
        lmin[i] = lm;
        lminrs[i] = lmrs;
    }

    std::copy(lmin, lmin+sz, min.begin());
    std::copy(lminrs, lminrs+sz, minrs.begin());
    free(lmin);
    free(lminrs);
}
}}

