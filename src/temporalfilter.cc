////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Meghan Fisher - meghan.fisher@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date May 2017
/// @brief The Temporal Bandpass operation
/// @details The algorithm finds some type of moving average (RMS, RMS with
///          Triangle window, Mean Abs, and Median) to trace amplitudes for
///          visualization purposes. It can be applied to traces independantly
///          or applies the same scalar to all traces at the same height.
////////////////////////////////////////////////////////////////////////////////

#include "global.hh"
#include "ops/temporalfilter.hh"
#include "share/api.hh"
#include "share/units.hh"

#include <algorithm>
#include <assert.h>
#include <complex>
#include <fftw3.h>
#include <math.h>
#include <vector>

namespace PIOL {
namespace File {

// TODO: Use complex literals when newer compilers are available
//       (Current is gcc (GCC) 5.3.0)
/// The imaginary number, i = sqrt(-1).
const cmtrace_t I = cmtrace_t(0_t, 1_t);

// TODO: Use this when intel supports it
// using namespace std::complex_literals;
/************************************ Core ************************************/

/********************************** Non-Core **********************************/
size_t filterOrder(const trace_t cornerP, const trace_t cornerS)
{
    // unfortunately the standard doesnt require math functions to provide
    // constexpr functions
    static const trace_t val =
      geom_t(0.5 * std::log(99.0 / (std::pow(10.0, 0.3) - 1.0)));

    return std::ceil(
      val
      / std::log(
          std::tan(Math::pi_t * cornerS) / std::tan(Math::pi_t * cornerP)));
}

void expandPoly(const cmtrace_t* coef, const size_t nvx, trace_t* poly)
{
    cmtrace_t vecTemp;
    std::vector<cmtrace_t> vecXpnd(nvx + 1LU);

    vecXpnd[0LU] = -coef[0LU];
    vecXpnd[1LU] = 1_t;

    for (size_t i = 1LU; i < nvx; i++) {
        vecXpnd[i + 1LU] = 1_t;
        for (size_t j = 0LU; j < i; j++) {
            vecXpnd[i - j] = vecXpnd[i - j] * -coef[i] + vecXpnd[i - j - 1LU];
        }
        vecXpnd[0] = vecXpnd[0] * -coef[i];
    }

    for (size_t i = 0LU; i < nvx + 1LU; i++) {
        poly[i] = vecXpnd[i].real();
    }
}

/// Common filter division: (4 + x)/(4 - x)
/// @param[in] x The \c x value
/// @return (4+x)/(4-x)
inline cmtrace_t filDiv(cmtrace_t x)
{
    return (4_t + x) / (4_t - x);
}

trace_t lowpass(size_t N, cmtrace_t* z, cmtrace_t* p, trace_t cf1)
{
    cmtrace_t pprodBL(1.0, 0.0);
    for (size_t i = 0LU; i < N; i++) {
        pprodBL *= 4_t - p[i] * cf1;
        p[i] = filDiv(p[i] * cf1);
        z[i] = -1_t;
    }

    return std::pow(cf1, N) / pprodBL.real();
}

trace_t highpass(size_t N, cmtrace_t* z, cmtrace_t* p, trace_t cf1)
{
    cmtrace_t pprod(1, 0);
    cmtrace_t pprodBL(1, 0);
    for (size_t i = 0LU; i < N; i++) {
        pprod *= -p[i];
        pprodBL *= 4_t - cf1 / p[i];
        p[i] = filDiv(cf1 / p[i]);
        z[i] = 1_t;
    }
    return (1_t / pprod.real()) * (std::pow(4_t, trace_t(N)) / pprodBL.real());
}

trace_t bandpass(size_t N, cmtrace_t* z, cmtrace_t* p, trace_t cf1, trace_t cf2)
{
    trace_t bndCntr = cf2 - cf1;
    trace_t bndLen  = cf1 * cf2;

    cmtrace_t pprodBL(1, 0);

    for (size_t i = 0LU; i < N; i++) {
        p[i] *= bndCntr / 2_t;
        p[N + i] = p[i] - std::sqrt(p[i] * p[i] - bndLen);
        p[i] += std::sqrt(p[i] * p[i] - bndLen);
        z[i]     = 1_t;
        z[N + i] = -1_t;
    }

    for (size_t i = 0LU; i < 2LU * N; i++) {
        pprodBL *= 4_t - p[i];
        p[i] = filDiv(p[i]);
    }

    return std::pow(bndCntr, N) * std::pow(4_t, trace_t(N)) * pprodBL.real()
           / std::norm(pprodBL);
}

trace_t bandstop(size_t N, cmtrace_t* z, cmtrace_t* p, trace_t cf1, trace_t cf2)
{
    trace_t bndCntr = cf2 - cf1;
    trace_t bndLen  = std::sqrt(cf1 * cf2);
    trace_t bndLen2 = cf1 * cf2;

    for (size_t i = 0LU; i < N; i++) {
        p[i]     = (bndCntr / 2_t) / p[i];
        p[N + i] = p[i] - std::sqrt(p[i] * p[i] - bndLen2);
        p[i] += std::sqrt(p[i] * p[i] - bndLen2);
        z[i]     = I * bndLen;
        z[N + i] = -z[i];
    }

    cmtrace_t zPrdct(1, 0);
    cmtrace_t pPrdct(1, 0);

    for (size_t i = 0LU; i < N * 2LU; i++) {
        zPrdct *= 4_t - z[i];
        z[i] = filDiv(z[i]);
        pPrdct *= 4_t - p[i];
        p[i] = filDiv(p[i]);
    }

    return (zPrdct.real() * pPrdct.real() + zPrdct.imag() * pPrdct.imag())
           / std::norm(pPrdct);
}

void makeFilter(
  FltrType type,
  trace_t* numer,
  trace_t* denom,
  llint N,
  trace_t fs,
  trace_t cf1,
  trace_t cf2)
{
    size_t tN  = (cf2 == 0) ? N : N * 2LU;
    trace_t Wn = 4_t * std::tan(Math::pi_t * (cf1 / (fs * 0.5_t)) / 2_t);
    trace_t W2 = 4_t * std::tan(Math::pi_t * (cf2 / (fs * 0.5_t)) / 2_t);

    std::vector<cmtrace_t> z(tN);
    std::vector<cmtrace_t> p(tN);

    // Determines the analogue zero-pole-gain representation of filter for given
    // number of poles
    for (llint i = 0; i < N; i++) {
        p[i] =
          -exp(I * Math::pi_t * trace_t(1LL + 2LL * i - N) / trace_t(2LL * N));
    }

    // Determines the analogue zero-pole-gain for given filter type at specific
    // frequency(ies)
    trace_t k;
    switch (type) {
        default:
        case FltrType::Lowpass:
            k = lowpass(N, z.data(), p.data(), Wn);
            break;

        case FltrType::Highpass:
            k = highpass(N, z.data(), p.data(), Wn);
            break;

        case FltrType::Bandpass:
            k = bandpass(N, z.data(), p.data(), Wn, W2);
            break;

        case FltrType::Bandstop:
            k = bandstop(N, z.data(), p.data(), Wn, W2);
            break;
    }

    expandPoly(z.data(), tN, numer);
    expandPoly(p.data(), tN, denom);

    for (size_t i = 0LU; i < tN + 1LU; i++) {
        numer[i] *= k;
    }

    for (size_t i = 0LU; i < (tN + 1LU) / 2LU; i++) {
        std::swap(denom[i], denom[tN - i]);
    }
}

void temporalFilter(
  size_t nt,
  size_t ns,
  trace_t* trc,
  trace_t fs,
  FltrType type,
  FltrDmn domain,
  PadType pad,
  size_t nw,
  size_t winCntr,
  std::vector<trace_t> corner)
{
    std::sort(corner.begin(), corner.end());
    switch (type) {
        default:
        case FltrType::Lowpass: {
            std::vector<trace_t> c = {corner[0LU], 0LU};
            size_t N               = filterOrder(corner[0LU], corner[1LU]);
            temporalFilter(
              nt, ns, trc, fs, type, domain, pad, nw, winCntr, c, N);
        } break;

        case FltrType::Highpass: {
            std::vector<trace_t> c = {corner[1LU], 0LU};
            size_t N               = filterOrder(corner[1LU], corner[0LU]);
            temporalFilter(
              nt, ns, trc, fs, type, domain, pad, nw, winCntr, c, N);
        } break;

        case FltrType::Bandpass: {
            std::vector<trace_t> c = {corner[1LU], corner[2LU]};
            temporalFilter(
              nt, ns, trc, fs, type, domain, pad, nw, winCntr, c,
              std::max(
                filterOrder(corner[1], corner[0]),
                filterOrder(corner[2], corner[3])));
        } break;

        case FltrType::Bandstop: {
            std::vector<trace_t> c = {corner[0LU], corner[3LU]};
            temporalFilter(
              nt, ns, trc, fs, type, domain, pad, nw, winCntr, c,
              std::max(
                filterOrder(corner[0], corner[1]),
                filterOrder(corner[3], corner[2])));
        } break;
    }
}

FltrPad getPad(PadType type)
{
    switch (type) {
        default:
        case PadType::Zero:
            return [](trace_t*, size_t, size_t, size_t) { return 0.0_t; };
            break;

        case PadType::Symmetric:
            return [](trace_t* trc, size_t N, size_t nw, size_t j) {
                return (j <= nw ? trc[N - j] : trc[2 * (nw + N) - j]);
            };
            break;

        case PadType::Replicate:
            return [](trace_t* trc, size_t, size_t nw, size_t j) {
                return (j <= nw ? trc[0] : trc[nw]);
            };
            break;

        case PadType::Cyclic:
            return [](trace_t* trc, size_t N, size_t nw, size_t j) {
                return (j <= nw ? trc[nw - (N - j)] : trc[j - nw - N]);
            };
            break;
    }
}

void filterFreq(
  size_t nss,
  trace_t* trcX,
  trace_t fs,
  size_t N,
  trace_t* numer,
  trace_t* denom,
  FltrPad)
{
    // TODO: Generalize fftwf for other data types besides floats
    // TODO: If creating + destroying plans becomes a bottleneck, re-use the
    //       plans
    std::vector<cmtrace_t> frequency(nss);
    std::vector<cmtrace_t> frequency1(nss);
    fftwf_plan planFFT = fftwf_plan_dft_r2c_1d(
      nss, trcX, reinterpret_cast<fftwf_complex*>(frequency.data()),
      FFTW_MEASURE);
    fftwf_execute(planFFT);
    fftwf_destroy_plan(planFFT);

    for (size_t i = 0; i < nss / 2LU + 1LU; i++) {
        cmtrace_t a = 0, b = 0;
        for (size_t j = 0; j < N + 1LU; j++) {
            cmtrace_t val =
              std::exp(-I * cmtrace_t(fs * trace_t(j * i) / trace_t(nss)));
            b += numer[j] * val;
            a += denom[j] * val;
        }
        cmtrace_t H = b / a;
        frequency[i] *= cmtrace_t(std::fabs(H.real()), std::fabs(H.imag()));
    }

    fftwf_plan planIFFT = fftwf_plan_dft_c2r_1d(
      nss, reinterpret_cast<fftwf_complex*>(frequency.data()), trcX,
      FFTW_MEASURE);
    fftwf_execute(planIFFT);
    fftwf_destroy_plan(planIFFT);

    for (size_t i = 0; i < nss; i++) {
        trcX[i] /= nss;
    }
}

/// Infinite Impulse Response
/// @param[in] N  Feedforwrd / Feedback filter order
/// @param[in] ns Number of samples
/// @param[in] b  Feedforward filter coefficients
///               (pointer to array of size \c N)
/// @param[in] a  Feedback filter coefficients
///               (pointer to array of size \c N)
/// @param[in] x  The input signal  (pointer to array of size \c ns)
/// @param[in] y  The output signal (pointer to array of size \c ns)
/// @param[in] zi Z-transform coefficients
/// @todo Document Z-transform coefficients
void IIR(
  size_t N,
  size_t ns,
  trace_t* b,
  trace_t* a,
  trace_t* x,
  trace_t* y,
  trace_t* zi)
{
    y[0] = b[0] * x[0] + zi[0];
    for (size_t i = 1LU; i < N; i++) {
        y[i] = b[0] * x[i] + zi[i];
        for (size_t j = 1LU; j < i + 1LU; j++)
            y[i] += b[j] * x[i - j] - a[j] * y[i - j];
    }

    for (size_t i = N; i < ns; i++) {
        y[i] = b[0] * x[i];
        for (size_t j = 1; j < N + 1LU; j++)
            y[i] += b[j] * x[i - j] - a[j] * y[i - j];
    }
}

void filterTime(
  size_t nw,
  trace_t* trcOrgnl,
  size_t numTail,
  trace_t* numer,
  trace_t* denom,
  FltrPad padding)
{
    std::vector<trace_t> trcX(nw + 6LU * (numTail + 1LU));

    for (size_t i = 0LU; i < 3LU * (numTail + 1LU); i++) {
        trcX[i] = padding(trcOrgnl, 3LU * (numTail + 1LU), nw, i);
        trcX[i + 3LU * numTail + nw] = padding(
          trcOrgnl, 3LU * (numTail + 1LU), nw - 1LU, i + 3LU * numTail + nw);
    }

    for (size_t i = 0; i < nw; i++) {
        trcX[i + 3LU * (numTail + 1LU)] = trcOrgnl[i];
    }

    std::vector<trace_t> zi(numTail);
    std::vector<trace_t> ziF(numTail);
    std::vector<trace_t> trcY(nw + 6LU * (numTail + 1LU));

    trace_t B    = 0;
    trace_t Imin = 1_t;
    for (size_t i = 1LU; i < numTail + 1LU; i++) {
        B += numer[i] - denom[i] * numer[0];
        Imin += denom[i];
    }

    // Applies a bilinear transform to convert analgue filter to digital filter
    // numer and denom generated by makeFilter.
    zi[0]     = B / Imin;
    trace_t a = 1_t;
    trace_t c = 0_t;
    for (size_t i = 1; i < numTail; i++) {
        a += denom[i];
        c += numer[i] - denom[i] * numer[0];
        zi[i] = a * zi[0] - c;
    }

    for (size_t i = 0; i < numTail; i++) {
        ziF[i] = zi[i] * trcX[0];
    }

    IIR(
      numTail, nw + 6LU * (numTail + 1LU), numer, denom, trcX.data(),
      trcY.data(), ziF.data());

    for (size_t i = 0; i < nw + 6 * (numTail + 1); i++) {
        trcX[i] = trcY[nw + 6LU * (1LU + numTail) - i - 1LU];
        trcY[nw + 6 * (numTail + 1) - 1 - i] = 0;
    }

    for (size_t i = 0; i < numTail; i++) {
        zi[i] *= trcX[0];
    }

    IIR(
      numTail, nw + 6LU * (numTail + 1LU), numer, denom, trcX.data(),
      trcY.data(), zi.data());

    for (size_t i = 0; i < nw; i++) {
        trcOrgnl[i] = trcY[nw + 3LU * (numTail + 1LU) - i - 1LU];
    }
}

void temporalFilter(
  size_t nt,
  size_t ns,
  trace_t* trc,
  trace_t fs,
  FltrType type,
  FltrDmn domain,
  PadType pad,
  size_t nw,
  size_t winCntr,
  std::vector<trace_t> corners,
  size_t N)
{
    nw      = (!nw || nw > ns ? ns : nw);
    nw      = (nw % 2 ? nw : nw - 1);
    winCntr = (!winCntr ? ns / 2LU : winCntr);

    assert(winCntr < ns && "Window Center is larger than trace length");
    size_t tail    = (!corners[1] ? 1LU : 2LU);
    size_t numTail = N * tail;
    std::vector<trace_t> numer(numTail + 1);
    std::vector<trace_t> denom(numTail + 1);
    makeFilter(type, numer.data(), denom.data(), N, fs, corners[0], corners[1]);
    for (size_t i = 0; i < nt; i++) {
        std::vector<trace_t> trcOrgnl(nw);
        for (size_t j = 0; j < nw; j++)
            trcOrgnl[j] = trc[i * ns + (winCntr - nw / 2) + j];
        switch (domain) {
            case FltrDmn::Time:
                filterTime(
                  nw, trcOrgnl.data(), numTail, numer.data(), denom.data(),
                  getPad(pad));
                break;
            case FltrDmn::Freq:
                filterFreq(
                  nw, trcOrgnl.data(), fs, numTail, numer.data(), denom.data(),
                  getPad(pad));
                break;
        }
        for (size_t j = 0; j < nw; j++)
            trc[i * ns + (winCntr - nw / 2LL) + j] = trcOrgnl[j];
    }
}

}  // namespace File
}  // namespace PIOL
