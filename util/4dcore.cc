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

struct _TempPair
{
    geom_t rs;
    size_t tn;
    _TempPair(geom_t rs_, size_t tn_) : rs(rs_), tn(tn_)
    {
    }
    bool operator <(const _TempPair & p) const
    {
        return (p.rs < rs || (p.rs == rs && p.tn < tn));
    }
    void operator =(const _TempPair & p)
    {
        rs = p.rs;
        tn = p.tn;
    }
};

void update(size_t offset, Coords * local, Coords * other, vec<size_t> & min, vec<geom_t> & minrs)
{
    size_t sz = local->sz;
    //For the vectorisation
    geom_t * lxS = local->xSrc;
    geom_t * lyS = local->ySrc;
    geom_t * lxR = local->xRcv;
    geom_t * lyR = local->yRcv;

    geom_t * rxS = other->xSrc;
    geom_t * ryS = other->ySrc;
    geom_t * rxR = other->xRcv;
    geom_t * ryR = other->yRcv;

    size_t * tn = other->tn;

    for (size_t i = 0; i < sz; i++)                         //Loop through every file1 trace
    {
//        size_t lmin = min[i];                               //temporary variables are improving optimisation potential
//        geom_t lminrs = minrs[i];

/*        #pragma omp simd aligned(rxS:ALIGN) aligned(ryS:ALIGN) aligned(rxR:ALIGN) aligned(ryR:ALIGN) \
                        aligned(tn:ALIGN) reduction(min, lmin)
        for (size_t j = 0U; j < other->allocSz; j++)        //loop through a multiple of the alignment
            {
                geom_t dval = dsr(lxS[i], lyS[i], lxR[i], lyR[i],
                                  rxS[j], ryS[j], rxR[j], ryR[j]);
                lmin = (dval < lminrs ? tn[j] : lmin);      //Update min if applicable
                lminrs = std::min(dval, lminrs);            //Update minrs if applicable
            }
        min[i] = lmin;
        minrs[i] = lminrs;
*/
        _TempPair lm(minrs[i], min[i]);

        #pragma omp declare reduction(minpair : _TempPair : \
        omp_out = omp_in < omp_out ? omp_in : omp_out \
        initializer (omp_priv=_TempPair(std::numeric_limits<geom_t>::max(), 0U))

        #pragma omp simd aligned(rxS:ALIGN) aligned(ryS:ALIGN) aligned(rxR:ALIGN) aligned(ryR:ALIGN) \
                        aligned(tn:ALIGN) reduction(minpair:lm)
        for (size_t j = 0U; j < other->allocSz; j++)        //loop through a multiple of the alignment
            {
                geom_t dval = dsr(lxS[i], lyS[i], lxR[i], lyR[i],
                                  rxS[j], ryS[j], rxR[j], ryR[j]);
                _TempPair tmp(dval, tn[j]);
                lm = (lm < tmp ? lm : tmp);
                //lm = std::min(lm, tmp);
            }
        min[i] = lm.tn;
        minrs[i] = lm.rs;
    }
}
}}

