/************************************************************************ *******************//*!
 *   \file
 *   \author Meghan Fisher - meghan.fisher@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date May 2017
 *   \brief The Temporal Bandpass operation
 *   \details The algorithm finds some type of moving average (RMS, RMS with Triangle window,
 *   Mean Abs, and Median) to trace amplitudes for visualization purposes. It can be applied to
 *   traces independantly or applies the same scalar to all traces at the same height.
*//*******************************************************************************************/
#include <vector>
#include <algorithm>
#include <math.h>
#include <assert.h>
#include <complex>
#include "global.hh"
#include "share/units.hh"
#include "share/api.hh"

namespace PIOL { namespace File {

//TODO: Use complex literals when newer compilers are available (Current is gcc (GCC) 5.3.0)
const cmtrace_t I(0_t, 1_t);

//TODO: Use this when intel supports it
//using namespace std::complex_literals;
/********************************************** Core *****************************************/

/********************************************* Non-Core **************************************/
size_t filterOrder(const trace_t cornerP, const trace_t cornerS)
{
    //unfortunately the standard doesnt require math functions to provide constexpr functions
    static const trace_t val = geom_t(0.5 * std::log(99.0 / (std::pow(10.0, 0.3) - 1.0)));
    return std::ceil(val / std::log(std::tan(Math::pi_t*cornerS) / std::tan(Math::pi_t*cornerP)));
}

void expandPoly(const cmtrace_t * coef, csize_t nvx, trace_t * poly)
{
    cmtrace_t vecTemp;
    std::vector<cmtrace_t> vecXpnd(nvx+1LU);

    vecXpnd[0LU] = -coef[0LU];
    vecXpnd[1LU] = 1_t;

    for (size_t i = 1LU; i < nvx; i++)
    {
        vecXpnd[i+1LU] = 1_t;
        for (size_t j = 0LU; j < i; j++)
            vecXpnd[i-j] = vecXpnd[i-j] * -coef[i] + vecXpnd[i-j-1LU];
        vecXpnd[0] = vecXpnd[0] * -coef[i];
    }
    for (size_t i = 0LU; i< nvx + 1LU; i++)
        poly[i] = vecXpnd[i].real();
}

//(4 + x)/(4 - x)
inline cmtrace_t filDiv(cmtrace_t x)
{
    return (4_t + x) / (4_t - x);
}

trace_t lowpass(size_t N, cmtrace_t * z, cmtrace_t * p, trace_t cf1)
{
    cmtrace_t pprodBL(1.0, 0.0);
    for (size_t i = 0LU; i < N; i++)
    {
        pprodBL *= 4_t - p[i]*cf1;
        p[i] = filDiv(p[i]*cf1);
        z[i] = -1_t;
    }

    return std::pow(cf1, N) / pprodBL.real();
}

trace_t highpass(size_t N, cmtrace_t * z, cmtrace_t * p, trace_t cf1)
{
    cmtrace_t pprod(1, 0);
    cmtrace_t pprodBL(1, 0);
    for (size_t i = 0LU; i < N; i++)
    {
        pprod *= -p[i];
        pprodBL *= 4_t - cf1/p[i];
        p[i] = filDiv(cf1/p[i]);
        z[i] = 1_t;
    }
    return (1_t / pprod.real()) * (std::pow(4_t, trace_t(N)) / pprodBL.real());
}

trace_t bandpass(size_t N, cmtrace_t * z, cmtrace_t * p, trace_t cf1, trace_t cf2)
{
    trace_t bndCntr = cf2 - cf1;
    trace_t bndLen = cf1*cf2;

    cmtrace_t pprodBL(1, 0);

    for (size_t i = 0LU; i < N; i++)
    {
        p[i] *= bndCntr / 2_t;
        p[N+i] = p[i] -  std::sqrt(p[i]*p[i] - bndLen);
        p[i] += std::sqrt(p[i]*p[i] - bndLen);
        z[i] = 1_t;
        z[N+i] = -1_t;
    }

    for (size_t i = 0LU; i < 2LU*N; i++)
    {
        pprodBL *= 4_t - p[i];
        p[i] = filDiv(p[i]);
    }

    return std::pow(bndCntr,N) * std::pow(4_t, trace_t(N)) * pprodBL.real() / std::norm(pprodBL);
}

trace_t bandstop(size_t N, cmtrace_t * z, cmtrace_t * p, trace_t cf1, trace_t cf2)
{
    trace_t bndCntr = cf2 - cf1;
    trace_t bndLen = std::sqrt(cf1*cf2);
    trace_t bndLen2 = cf1*cf2;

    for (size_t i = 0LU; i<N; i++)
    {
        p[i] = (bndCntr/2_t)/p[i];
        p[N+i] = p[i] -  std::sqrt(p[i]*p[i] - bndLen2);
        p[i] += std::sqrt(p[i]*p[i] - bndLen2);
        z[i] = I*bndLen;
        z[N+i] = -z[i];
    }

    cmtrace_t zPrdct(1, 0);
    cmtrace_t pPrdct(1, 0);

    for (size_t i = 0LU; i<N*2LU; i++)
    {
        zPrdct *= 4_t - z[i];
        z[i] = filDiv(z[i]);
        pPrdct *= 4_t - p[i];
        p[i] = filDiv(p[i]);
    }

    return (zPrdct.real()*pPrdct.real()+zPrdct.imag()*pPrdct.imag()) / std::norm(pPrdct);
}

void makeFilter(FltrType type, trace_t * numer, trace_t * denom, llint N, trace_t fs, trace_t cf1, trace_t cf2)
{
    size_t tN = (cf2 == 0) ? N : N*2LU;
    trace_t Wn = 4_t * std::tan(Math::pi_t*(cf1/(fs*0.5_t))/2_t);
    trace_t W2 = 4_t * std::tan(Math::pi_t*(cf2/(fs*0.5_t))/2_t);

    std::vector<cmtrace_t> z(tN);
    std::vector<cmtrace_t> p(tN);

    for (llint i = 0; i < N; i++)
        p[i] = -exp(I*Math::pi_t*trace_t(1LL+2LL*i-N)/trace_t(2LL*N));
    trace_t k;
    switch (type)
    {
        default:
        case FltrType::Lowpass :
            k = lowpass(N, z.data(), p.data(), Wn);
        break;
        case FltrType::Highpass :
            k = highpass(N, z.data(), p.data(), Wn);
        break;
        case FltrType::Bandpass :
            k = bandpass(N, z.data(), p.data(), Wn, W2);
        break;
        case FltrType::Bandstop :
            k = bandstop(N, z.data(), p.data(), Wn, W2);
        break;
    }

    expandPoly(z.data(), tN, numer);
    expandPoly(p.data(), tN, denom);

    for (size_t i = 0LU; i < tN+1LU; i++)
        numer[i] *= k;
    for (size_t i = 0LU; i < (tN+1LU)/2LU; i++)
        std::swap(denom[i], denom[tN-i]);
}
}}
