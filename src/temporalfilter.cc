////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief   The Temporal Bandpass operation
/// @details The algorithm finds some type of moving average (RMS, RMS with
///          Triangle window, Mean Abs, and Median) to trace amplitudes for
///          visualization purposes. It can be applied to traces independantly
///          or applies the same scalar to all traces at the same height.
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/operations/temporalfilter.hh"
#include "ExSeisDat/utils/typedefs.h"

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <complex>
#include <fftw3.h>
#include <vector>

using namespace exseis::utils;

namespace exseis {
namespace PIOL {

// TODO: Use complex literals when newer compilers are available
//       (Current is gcc (GCC) 5.3.0)
/// The imaginary number, i = sqrt(-1).
const Complex_trace_value I = Complex_trace_value(0, 1);

/// The circle constant, pi.
const exseis::utils::Trace_value pi = 3.14159265358979323846264338327950288;

// TODO: Use this when intel supports it
// using namespace std::complex_literals;
/************************************ Core ************************************/

/********************************** Non-Core **********************************/
size_t filterOrder(
  const exseis::utils::Trace_value cornerP,
  const exseis::utils::Trace_value cornerS)
{
    // unfortunately the standard doesnt require math functions to provide
    // constexpr functions
    static const exseis::utils::Trace_value val = exseis::utils::Floating_point(
      0.5 * std::log(99.0 / (std::pow(10.0, 0.3) - 1.0)));

    return std::ceil(
      val / std::log(std::tan(pi * cornerS) / std::tan(pi * cornerP)));
}

void expandPoly(
  const Complex_trace_value* coef,
  const size_t nvx,
  exseis::utils::Trace_value* poly)
{
    std::vector<Complex_trace_value> vecXpnd(nvx + 1LU);

    vecXpnd[0LU] = -coef[0LU];
    vecXpnd[1LU] = 1;

    for (size_t i = 1LU; i < nvx; i++) {
        vecXpnd[i + 1LU] = 1;
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
inline Complex_trace_value filDiv(Complex_trace_value x)
{
    return (exseis::utils::Trace_value(4) + x)
           / (exseis::utils::Trace_value(4) - x);
}

exseis::utils::Trace_value lowpass(
  size_t N,
  Complex_trace_value* z,
  Complex_trace_value* p,
  exseis::utils::Trace_value cf1)
{
    Complex_trace_value pprodBL(1.0, 0.0);
    for (size_t i = 0LU; i < N; i++) {
        pprodBL *= exseis::utils::Trace_value(4) - p[i] * cf1;
        p[i] = filDiv(p[i] * cf1);
        z[i] = -1;
    }

    return std::pow(cf1, N) / pprodBL.real();
}

exseis::utils::Trace_value highpass(
  size_t N,
  Complex_trace_value* z,
  Complex_trace_value* p,
  exseis::utils::Trace_value cf1)
{
    Complex_trace_value pprod(1, 0);
    Complex_trace_value pprodBL(1, 0);
    for (size_t i = 0LU; i < N; i++) {
        pprod *= -p[i];
        pprodBL *= exseis::utils::Trace_value(4) - cf1 / p[i];
        p[i] = filDiv(cf1 / p[i]);
        z[i] = 1;
    }
    return (1 / pprod.real())
           * (std::pow(
                exseis::utils::Trace_value(4), exseis::utils::Trace_value(N))
              / pprodBL.real());
}

exseis::utils::Trace_value bandpass(
  size_t N,
  Complex_trace_value* z,
  Complex_trace_value* p,
  exseis::utils::Trace_value cf1,
  exseis::utils::Trace_value cf2)
{
    exseis::utils::Trace_value bndCntr = cf2 - cf1;
    exseis::utils::Trace_value bndLen  = cf1 * cf2;

    Complex_trace_value pprodBL(1, 0);

    for (size_t i = 0LU; i < N; i++) {
        p[i] *= bndCntr / 2;
        p[N + i] = p[i] - std::sqrt(p[i] * p[i] - bndLen);
        p[i] += std::sqrt(p[i] * p[i] - bndLen);
        z[i]     = 1;
        z[N + i] = -1;
    }

    for (size_t i = 0LU; i < 2LU * N; i++) {
        pprodBL *= exseis::utils::Trace_value(4) - p[i];
        p[i] = filDiv(p[i]);
    }

    return std::pow(bndCntr, N)
           * std::pow(
               exseis::utils::Trace_value(4), exseis::utils::Trace_value(N))
           * pprodBL.real() / std::norm(pprodBL);
}

exseis::utils::Trace_value bandstop(
  size_t N,
  Complex_trace_value* z,
  Complex_trace_value* p,
  exseis::utils::Trace_value cf1,
  exseis::utils::Trace_value cf2)
{
    exseis::utils::Trace_value bndCntr = cf2 - cf1;
    exseis::utils::Trace_value bndLen  = std::sqrt(cf1 * cf2);
    exseis::utils::Trace_value bndLen2 = cf1 * cf2;

    for (size_t i = 0LU; i < N; i++) {
        p[i]     = (bndCntr / 2) / p[i];
        p[N + i] = p[i] - std::sqrt(p[i] * p[i] - bndLen2);
        p[i] += std::sqrt(p[i] * p[i] - bndLen2);
        z[i]     = I * bndLen;
        z[N + i] = -z[i];
    }

    Complex_trace_value zPrdct(1, 0);
    Complex_trace_value pPrdct(1, 0);

    for (size_t i = 0LU; i < N * 2LU; i++) {
        zPrdct *= exseis::utils::Trace_value(4) - z[i];
        z[i] = filDiv(z[i]);
        pPrdct *= exseis::utils::Trace_value(4) - p[i];
        p[i] = filDiv(p[i]);
    }

    return (zPrdct.real() * pPrdct.real() + zPrdct.imag() * pPrdct.imag())
           / std::norm(pPrdct);
}

void makeFilter(
  FltrType type,
  exseis::utils::Trace_value* numer,
  exseis::utils::Trace_value* denom,
  exseis::utils::Integer N,
  exseis::utils::Trace_value fs,
  exseis::utils::Trace_value cf1,
  exseis::utils::Trace_value cf2)
{
    size_t tN = (cf2 == 0) ? N : N * 2LU;
    exseis::utils::Trace_value Wn =
      4 * std::tan(pi * (cf1 / (fs * exseis::utils::Trace_value(0.5))) / 2);
    exseis::utils::Trace_value W2 =
      4 * std::tan(pi * (cf2 / (fs * exseis::utils::Trace_value(0.5))) / 2);

    std::vector<Complex_trace_value> z(tN);
    std::vector<Complex_trace_value> p(tN);

    // Determines the analogue zero-pole-gain representation of filter for given
    // number of poles
    for (exseis::utils::Integer i = 0; i < N; i++) {
        p[i] = -exp(
          I * pi * exseis::utils::Trace_value(1LL + 2LL * i - N)
          / exseis::utils::Trace_value(2LL * N));
    }

    // Determines the analogue zero-pole-gain for given filter type at specific
    // frequency(ies)
    exseis::utils::Trace_value k;
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
  exseis::utils::Trace_value* trc,
  exseis::utils::Trace_value fs,
  FltrType type,
  FltrDmn domain,
  PadType pad,
  size_t nw,
  size_t winCntr,
  std::vector<exseis::utils::Trace_value> corner)
{
    std::sort(corner.begin(), corner.end());
    switch (type) {
        default:
        case FltrType::Lowpass: {
            std::vector<exseis::utils::Trace_value> c = {corner[0LU], 0LU};
            size_t N = filterOrder(corner[0LU], corner[1LU]);
            temporalFilter(
              nt, ns, trc, fs, type, domain, pad, nw, winCntr, c, N);
        } break;

        case FltrType::Highpass: {
            std::vector<exseis::utils::Trace_value> c = {corner[1LU], 0LU};
            size_t N = filterOrder(corner[1LU], corner[0LU]);
            temporalFilter(
              nt, ns, trc, fs, type, domain, pad, nw, winCntr, c, N);
        } break;

        case FltrType::Bandpass: {
            std::vector<exseis::utils::Trace_value> c = {corner[1LU],
                                                         corner[2LU]};
            temporalFilter(
              nt, ns, trc, fs, type, domain, pad, nw, winCntr, c,
              std::max(
                filterOrder(corner[1], corner[0]),
                filterOrder(corner[2], corner[3])));
        } break;

        case FltrType::Bandstop: {
            std::vector<exseis::utils::Trace_value> c = {corner[0LU],
                                                         corner[3LU]};
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
            return [](exseis::utils::Trace_value*, size_t, size_t, size_t) {
                return 0;
            };
            break;

        case PadType::Symmetric:
            return [](
                     exseis::utils::Trace_value* trc, size_t N, size_t nw,
                     size_t j) {
                return (j <= nw ? trc[N - j] : trc[2 * (nw + N) - j]);
            };
            break;

        case PadType::Replicate:
            return
              [](exseis::utils::Trace_value* trc, size_t, size_t nw, size_t j) {
                  return (j <= nw ? trc[0] : trc[nw]);
              };
            break;

        case PadType::Cyclic:
            return [](
                     exseis::utils::Trace_value* trc, size_t N, size_t nw,
                     size_t j) {
                return (j <= nw ? trc[nw - (N - j)] : trc[j - nw - N]);
            };
            break;
    }
}

void filterFreq(
  size_t nss,
  exseis::utils::Trace_value* trcX,
  exseis::utils::Trace_value fs,
  size_t N,
  exseis::utils::Trace_value* numer,
  exseis::utils::Trace_value* denom,
  FltrPad)
{
    // TODO: Generalize fftwf for other data types besides floats
    // TODO: If creating + destroying plans becomes a bottleneck, re-use the
    //       plans
    std::vector<Complex_trace_value> frequency(nss);
    std::vector<Complex_trace_value> frequency1(nss);
    fftwf_plan planFFT = fftwf_plan_dft_r2c_1d(
      nss, trcX, reinterpret_cast<fftwf_complex*>(frequency.data()),
      FFTW_MEASURE);
    fftwf_execute(planFFT);
    fftwf_destroy_plan(planFFT);

    for (size_t i = 0; i < nss / 2LU + 1LU; i++) {
        Complex_trace_value a = 0, b = 0;
        for (size_t j = 0; j < N + 1LU; j++) {
            Complex_trace_value val = std::exp(
              -I
              * Complex_trace_value(
                  fs * exseis::utils::Trace_value(j * i)
                  / exseis::utils::Trace_value(nss)));
            b += numer[j] * val;
            a += denom[j] * val;
        }
        Complex_trace_value H = b / a;
        frequency[i] *=
          Complex_trace_value(std::fabs(H.real()), std::fabs(H.imag()));
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
  exseis::utils::Trace_value* b,
  exseis::utils::Trace_value* a,
  exseis::utils::Trace_value* x,
  exseis::utils::Trace_value* y,
  exseis::utils::Trace_value* zi)
{
    y[0] = b[0] * x[0] + zi[0];
    for (size_t i = 1LU; i < N; i++) {
        y[i] = b[0] * x[i] + zi[i];
        for (size_t j = 1LU; j < i + 1LU; j++) {
            y[i] += b[j] * x[i - j] - a[j] * y[i - j];
        }
    }

    for (size_t i = N; i < ns; i++) {
        y[i] = b[0] * x[i];
        for (size_t j = 1; j < N + 1LU; j++) {
            y[i] += b[j] * x[i - j] - a[j] * y[i - j];
        }
    }
}

void filterTime(
  size_t nw,
  exseis::utils::Trace_value* trcOrgnl,
  size_t numTail,
  exseis::utils::Trace_value* numer,
  exseis::utils::Trace_value* denom,
  FltrPad padding)
{
    std::vector<exseis::utils::Trace_value> trcX(nw + 6LU * (numTail + 1LU));

    for (size_t i = 0LU; i < 3LU * (numTail + 1LU); i++) {
        trcX[i] = padding(trcOrgnl, 3LU * (numTail + 1LU), nw, i);
        trcX[i + 3LU * numTail + nw] = padding(
          trcOrgnl, 3LU * (numTail + 1LU), nw - 1LU, i + 3LU * numTail + nw);
    }

    for (size_t i = 0; i < nw; i++) {
        trcX[i + 3LU * (numTail + 1LU)] = trcOrgnl[i];
    }

    std::vector<exseis::utils::Trace_value> zi(numTail);
    std::vector<exseis::utils::Trace_value> ziF(numTail);
    std::vector<exseis::utils::Trace_value> trcY(nw + 6LU * (numTail + 1LU));

    exseis::utils::Trace_value B    = 0;
    exseis::utils::Trace_value Imin = 1;
    for (size_t i = 1LU; i < numTail + 1LU; i++) {
        B += numer[i] - denom[i] * numer[0];
        Imin += denom[i];
    }

    // Applies a bilinear transform to convert analgue filter to digital filter
    // numer and denom generated by makeFilter.
    zi[0]                        = B / Imin;
    exseis::utils::Trace_value a = 1;
    exseis::utils::Trace_value c = 0;
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
  exseis::utils::Trace_value* trc,
  exseis::utils::Trace_value fs,
  FltrType type,
  FltrDmn domain,
  PadType pad,
  size_t nw,
  size_t winCntr,
  std::vector<exseis::utils::Trace_value> corners,
  size_t N)
{
    nw      = (!nw || nw > ns ? ns : nw);
    nw      = (nw % 2 ? nw : nw - 1);
    winCntr = (!winCntr ? ns / 2LU : winCntr);

    assert(winCntr < ns && "Window Center is larger than trace length");
    size_t tail    = (!corners[1] ? 1LU : 2LU);
    size_t numTail = N * tail;
    std::vector<exseis::utils::Trace_value> numer(numTail + 1);
    std::vector<exseis::utils::Trace_value> denom(numTail + 1);
    makeFilter(type, numer.data(), denom.data(), N, fs, corners[0], corners[1]);

    for (size_t i = 0; i < nt; i++) {

        std::vector<exseis::utils::Trace_value> trcOrgnl(nw);

        for (size_t j = 0; j < nw; j++) {
            trcOrgnl[j] = trc[i * ns + (winCntr - nw / 2) + j];
        }
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

        for (size_t j = 0; j < nw; j++) {
            trc[i * ns + (winCntr - nw / 2LL) + j] = trcOrgnl[j];
        }
    }
}

}  // namespace PIOL
}  // namespace exseis
