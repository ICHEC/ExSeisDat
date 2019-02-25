////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief   The Temporal Bandpass operation
/// @details The algorithm finds some type of moving average (RMS, RMS with
///          Triangle window, Mean Abs, and Median) to trace amplitudes for
///          visualization purposes. It can be applied to traces independantly
///          or applies the same scalar to all traces at the same height.
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/piol/operations/temporalfilter.hh"
#include "exseisdat/utils/typedefs.hh"

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <complex>
#include <fftw3.h>
#include <vector>

using namespace exseis::utils;

namespace exseis {
namespace piol {

// TODO: Use complex literals when newer compilers are available
//       (Current is gcc (GCC) 5.3.0)
/// The imaginary number, one_i = sqrt(-1).
const Complex_trace_value one_i = Complex_trace_value(0, 1);

/// The circle constant, pi.
const exseis::utils::Trace_value pi = 3.14159265358979323846264338327950288;

// TODO: Use this when intel supports it
// using namespace std::complex_literals;
/************************************ Core ************************************/

/********************************** Non-Core **********************************/
size_t filter_order(
    const exseis::utils::Trace_value corner_p,
    const exseis::utils::Trace_value corner_s)
{
    // unfortunately the standard doesnt require math functions to provide
    // constexpr functions
    static const exseis::utils::Trace_value val = exseis::utils::Floating_point(
        0.5 * std::log(99.0 / (std::pow(10.0, 0.3) - 1.0)));

    return std::ceil(
        val / std::log(std::tan(pi * corner_s) / std::tan(pi * corner_p)));
}

void expand_poly(
    const Complex_trace_value* coef,
    const size_t nvx,
    exseis::utils::Trace_value* poly)
{
    std::vector<Complex_trace_value> vec_xpnd(nvx + 1LU);

    vec_xpnd[0LU] = -coef[0LU];
    vec_xpnd[1LU] = 1;

    for (size_t i = 1LU; i < nvx; i++) {
        vec_xpnd[i + 1LU] = 1;
        for (size_t j = 0LU; j < i; j++) {
            vec_xpnd[i - j] =
                vec_xpnd[i - j] * -coef[i] + vec_xpnd[i - j - 1LU];
        }
        vec_xpnd[0] = vec_xpnd[0] * -coef[i];
    }

    for (size_t i = 0LU; i < nvx + 1LU; i++) {
        poly[i] = vec_xpnd[i].real();
    }
}

/// Common filter division: (4 + x)/(4 - x)
/// @param[in] x The \c x value
/// @return (4+x)/(4-x)
inline Complex_trace_value fil_div(Complex_trace_value x)
{
    return (exseis::utils::Trace_value(4) + x)
           / (exseis::utils::Trace_value(4) - x);
}

exseis::utils::Trace_value lowpass(
    size_t n,
    Complex_trace_value* z,
    Complex_trace_value* p,
    exseis::utils::Trace_value cf1)
{
    Complex_trace_value pprod_bl(1.0, 0.0);
    for (size_t i = 0LU; i < n; i++) {
        pprod_bl *= exseis::utils::Trace_value(4) - p[i] * cf1;
        p[i] = fil_div(p[i] * cf1);
        z[i] = -1;
    }

    return std::pow(cf1, n) / pprod_bl.real();
}

exseis::utils::Trace_value highpass(
    size_t n,
    Complex_trace_value* z,
    Complex_trace_value* p,
    exseis::utils::Trace_value cf1)
{
    Complex_trace_value pprod(1, 0);
    Complex_trace_value pprod_bl(1, 0);
    for (size_t i = 0LU; i < n; i++) {
        pprod *= -p[i];
        pprod_bl *= exseis::utils::Trace_value(4) - cf1 / p[i];
        p[i] = fil_div(cf1 / p[i]);
        z[i] = 1;
    }
    return (1 / pprod.real())
           * (std::pow(
                  exseis::utils::Trace_value(4), exseis::utils::Trace_value(n))
              / pprod_bl.real());
}

exseis::utils::Trace_value bandpass(
    size_t n,
    Complex_trace_value* z,
    Complex_trace_value* p,
    exseis::utils::Trace_value cf1,
    exseis::utils::Trace_value cf2)
{
    exseis::utils::Trace_value bnd_cntr = cf2 - cf1;
    exseis::utils::Trace_value bnd_len  = cf1 * cf2;

    Complex_trace_value pprod_bl(1, 0);

    for (size_t i = 0LU; i < n; i++) {
        p[i] *= bnd_cntr / 2;
        p[n + i] = p[i] - std::sqrt(p[i] * p[i] - bnd_len);
        p[i] += std::sqrt(p[i] * p[i] - bnd_len);
        z[i]     = 1;
        z[n + i] = -1;
    }

    for (size_t i = 0LU; i < 2LU * n; i++) {
        pprod_bl *= exseis::utils::Trace_value(4) - p[i];
        p[i] = fil_div(p[i]);
    }

    return std::pow(bnd_cntr, n)
           * std::pow(
               exseis::utils::Trace_value(4), exseis::utils::Trace_value(n))
           * pprod_bl.real() / std::norm(pprod_bl);
}

exseis::utils::Trace_value bandstop(
    size_t n,
    Complex_trace_value* z,
    Complex_trace_value* p,
    exseis::utils::Trace_value cf1,
    exseis::utils::Trace_value cf2)
{
    exseis::utils::Trace_value bnd_cntr = cf2 - cf1;
    exseis::utils::Trace_value bnd_len  = std::sqrt(cf1 * cf2);
    exseis::utils::Trace_value bnd_len2 = cf1 * cf2;

    for (size_t i = 0LU; i < n; i++) {
        p[i]     = (bnd_cntr / 2) / p[i];
        p[n + i] = p[i] - std::sqrt(p[i] * p[i] - bnd_len2);
        p[i] += std::sqrt(p[i] * p[i] - bnd_len2);
        z[i]     = one_i * bnd_len;
        z[n + i] = -z[i];
    }

    Complex_trace_value z_prdct(1, 0);
    Complex_trace_value p_prdct(1, 0);

    for (size_t i = 0LU; i < n * 2LU; i++) {
        z_prdct *= exseis::utils::Trace_value(4) - z[i];
        z[i] = fil_div(z[i]);
        p_prdct *= exseis::utils::Trace_value(4) - p[i];
        p[i] = fil_div(p[i]);
    }

    return (z_prdct.real() * p_prdct.real() + z_prdct.imag() * p_prdct.imag())
           / std::norm(p_prdct);
}

void make_filter(
    FltrType type,
    exseis::utils::Trace_value* numer,
    exseis::utils::Trace_value* denom,
    exseis::utils::Integer n,
    exseis::utils::Trace_value fs,
    exseis::utils::Trace_value cf1,
    exseis::utils::Trace_value cf2)
{
    size_t t_n = (cf2 == 0) ? n : n * 2LU;
    exseis::utils::Trace_value wn =
        4 * std::tan(pi * (cf1 / (fs * exseis::utils::Trace_value(0.5))) / 2);
    exseis::utils::Trace_value w2 =
        4 * std::tan(pi * (cf2 / (fs * exseis::utils::Trace_value(0.5))) / 2);

    std::vector<Complex_trace_value> z(t_n);
    std::vector<Complex_trace_value> p(t_n);

    // Determines the analogue zero-pole-gain representation of filter for given
    // number of poles
    for (exseis::utils::Integer i = 0; i < n; i++) {
        p[i] = -exp(
            one_i * pi * exseis::utils::Trace_value(1LL + 2LL * i - n)
            / exseis::utils::Trace_value(2LL * n));
    }

    // Determines the analogue zero-pole-gain for given filter type at specific
    // frequency(ies)
    exseis::utils::Trace_value k;
    switch (type) {
        default:
        case FltrType::Lowpass:
            k = lowpass(n, z.data(), p.data(), wn);
            break;

        case FltrType::Highpass:
            k = highpass(n, z.data(), p.data(), wn);
            break;

        case FltrType::Bandpass:
            k = bandpass(n, z.data(), p.data(), wn, w2);
            break;

        case FltrType::Bandstop:
            k = bandstop(n, z.data(), p.data(), wn, w2);
            break;
    }

    expand_poly(z.data(), t_n, numer);
    expand_poly(p.data(), t_n, denom);

    for (size_t i = 0LU; i < t_n + 1LU; i++) {
        numer[i] *= k;
    }

    for (size_t i = 0LU; i < (t_n + 1LU) / 2LU; i++) {
        std::swap(denom[i], denom[t_n - i]);
    }
}

void temporal_filter(
    size_t nt,
    size_t ns,
    exseis::utils::Trace_value* trc,
    exseis::utils::Trace_value fs,
    FltrType type,
    FltrDmn domain,
    PadType pad,
    size_t nw,
    size_t win_cntr,
    std::vector<exseis::utils::Trace_value> corner)
{
    std::sort(corner.begin(), corner.end());
    switch (type) {
        default:
        case FltrType::Lowpass: {
            std::vector<exseis::utils::Trace_value> c = {corner[0LU], 0LU};
            size_t n = filter_order(corner[0LU], corner[1LU]);
            temporal_filter(
                nt, ns, trc, fs, type, domain, pad, nw, win_cntr, c, n);
        } break;

        case FltrType::Highpass: {
            std::vector<exseis::utils::Trace_value> c = {corner[1LU], 0LU};
            size_t n = filter_order(corner[1LU], corner[0LU]);
            temporal_filter(
                nt, ns, trc, fs, type, domain, pad, nw, win_cntr, c, n);
        } break;

        case FltrType::Bandpass: {
            std::vector<exseis::utils::Trace_value> c = {corner[1LU],
                                                         corner[2LU]};
            temporal_filter(
                nt, ns, trc, fs, type, domain, pad, nw, win_cntr, c,
                std::max(
                    filter_order(corner[1], corner[0]),
                    filter_order(corner[2], corner[3])));
        } break;

        case FltrType::Bandstop: {
            std::vector<exseis::utils::Trace_value> c = {corner[0LU],
                                                         corner[3LU]};
            temporal_filter(
                nt, ns, trc, fs, type, domain, pad, nw, win_cntr, c,
                std::max(
                    filter_order(corner[0], corner[1]),
                    filter_order(corner[3], corner[2])));
        } break;
    }
}

FltrPad get_pad(PadType type)
{
    switch (type) {
        default:
        case PadType::Zero:
            return [](exseis::utils::Trace_value*, size_t, size_t, size_t) {
                return 0;
            };
            break;

        case PadType::Symmetric:
            return [](exseis::utils::Trace_value* trc, size_t n, size_t nw,
                      size_t j) {
                return (j <= nw ? trc[n - j] : trc[2 * (nw + n) - j]);
            };
            break;

        case PadType::Replicate:
            return [](exseis::utils::Trace_value* trc, size_t, size_t nw,
                      size_t j) { return (j <= nw ? trc[0] : trc[nw]); };
            break;

        case PadType::Cyclic:
            return [](exseis::utils::Trace_value* trc, size_t n, size_t nw,
                      size_t j) {
                return (j <= nw ? trc[nw - (n - j)] : trc[j - nw - n]);
            };
            break;
    }
}

void filter_freq(
    size_t nss,
    exseis::utils::Trace_value* trc_x,
    exseis::utils::Trace_value fs,
    size_t n,
    exseis::utils::Trace_value* numer,
    exseis::utils::Trace_value* denom,
    FltrPad)
{
    // TODO: Generalize fftwf for other data types besides floats
    // TODO: If creating + destroying plans becomes a bottleneck, re-use the
    //       plans
    std::vector<Complex_trace_value> frequency(nss);
    std::vector<Complex_trace_value> frequency1(nss);
    fftwf_plan plan_fft = fftwf_plan_dft_r2c_1d(
        nss, trc_x, reinterpret_cast<fftwf_complex*>(frequency.data()),
        FFTW_MEASURE);
    fftwf_execute(plan_fft);
    fftwf_destroy_plan(plan_fft);

    for (size_t i = 0; i < nss / 2LU + 1LU; i++) {
        Complex_trace_value a = 0, b = 0;
        for (size_t j = 0; j < n + 1LU; j++) {
            Complex_trace_value val = std::exp(
                -one_i
                * Complex_trace_value(
                    fs * exseis::utils::Trace_value(j * i)
                    / exseis::utils::Trace_value(nss)));
            b += numer[j] * val;
            a += denom[j] * val;
        }
        Complex_trace_value h = b / a;
        frequency[i] *=
            Complex_trace_value(std::fabs(h.real()), std::fabs(h.imag()));
    }

    fftwf_plan plan_ifft = fftwf_plan_dft_c2r_1d(
        nss, reinterpret_cast<fftwf_complex*>(frequency.data()), trc_x,
        FFTW_MEASURE);
    fftwf_execute(plan_ifft);
    fftwf_destroy_plan(plan_ifft);

    for (size_t i = 0; i < nss; i++) {
        trc_x[i] /= nss;
    }
}

/// Infinite Impulse Response
/// @param[in] n  Feedforward / Feedback filter order
/// @param[in] ns Number of samples
/// @param[in] b  Feedforward filter coefficients
///               (pointer to array of size \c n)
/// @param[in] a  Feedback filter coefficients
///               (pointer to array of size \c n)
/// @param[in] x  The input signal  (pointer to array of size \c ns)
/// @param[in] y  The output signal (pointer to array of size \c ns)
/// @param[in] zi Z-transform coefficients
/// @todo Document Z-transform coefficients
static void iir(
    size_t n,
    size_t ns,
    exseis::utils::Trace_value* b,
    exseis::utils::Trace_value* a,
    exseis::utils::Trace_value* x,
    exseis::utils::Trace_value* y,
    exseis::utils::Trace_value* zi)
{
    y[0] = b[0] * x[0] + zi[0];
    for (size_t i = 1LU; i < n; i++) {
        y[i] = b[0] * x[i] + zi[i];
        for (size_t j = 1LU; j < i + 1LU; j++) {
            y[i] += b[j] * x[i - j] - a[j] * y[i - j];
        }
    }

    for (size_t i = n; i < ns; i++) {
        y[i] = b[0] * x[i];
        for (size_t j = 1; j < n + 1LU; j++) {
            y[i] += b[j] * x[i - j] - a[j] * y[i - j];
        }
    }
}

void filter_time(
    size_t nw,
    exseis::utils::Trace_value* trc_orgnl,
    size_t num_tail,
    exseis::utils::Trace_value* numer,
    exseis::utils::Trace_value* denom,
    FltrPad padding)
{
    std::vector<exseis::utils::Trace_value> trc_x(nw + 6LU * (num_tail + 1LU));

    for (size_t i = 0LU; i < 3LU * (num_tail + 1LU); i++) {
        trc_x[i] = padding(trc_orgnl, 3LU * (num_tail + 1LU), nw, i);
        trc_x[i + 3LU * num_tail + nw] = padding(
            trc_orgnl, 3LU * (num_tail + 1LU), nw - 1LU,
            i + 3LU * num_tail + nw);
    }

    for (size_t i = 0; i < nw; i++) {
        trc_x[i + 3LU * (num_tail + 1LU)] = trc_orgnl[i];
    }

    std::vector<exseis::utils::Trace_value> zi(num_tail);
    std::vector<exseis::utils::Trace_value> zi_f(num_tail);
    std::vector<exseis::utils::Trace_value> trc_y(nw + 6LU * (num_tail + 1LU));

    exseis::utils::Trace_value b    = 0;
    exseis::utils::Trace_value imin = 1;
    for (size_t i = 1LU; i < num_tail + 1LU; i++) {
        b += numer[i] - denom[i] * numer[0];
        imin += denom[i];
    }

    // Applies a bilinear transform to convert analgue filter to digital filter
    // numer and denom generated by make_filter.
    zi[0]                        = b / imin;
    exseis::utils::Trace_value a = 1;
    exseis::utils::Trace_value c = 0;
    for (size_t i = 1; i < num_tail; i++) {
        a += denom[i];
        c += numer[i] - denom[i] * numer[0];
        zi[i] = a * zi[0] - c;
    }

    for (size_t i = 0; i < num_tail; i++) {
        zi_f[i] = zi[i] * trc_x[0];
    }

    iir(num_tail, nw + 6LU * (num_tail + 1LU), numer, denom, trc_x.data(),
        trc_y.data(), zi_f.data());

    for (size_t i = 0; i < nw + 6 * (num_tail + 1); i++) {
        trc_x[i] = trc_y[nw + 6LU * (1LU + num_tail) - i - 1LU];
        trc_y[nw + 6 * (num_tail + 1) - 1 - i] = 0;
    }

    for (size_t i = 0; i < num_tail; i++) {
        zi[i] *= trc_x[0];
    }

    iir(num_tail, nw + 6LU * (num_tail + 1LU), numer, denom, trc_x.data(),
        trc_y.data(), zi.data());

    for (size_t i = 0; i < nw; i++) {
        trc_orgnl[i] = trc_y[nw + 3LU * (num_tail + 1LU) - i - 1LU];
    }
}

void temporal_filter(
    size_t nt,
    size_t ns,
    exseis::utils::Trace_value* trc,
    exseis::utils::Trace_value fs,
    FltrType type,
    FltrDmn domain,
    PadType pad,
    size_t nw,
    size_t win_cntr,
    std::vector<exseis::utils::Trace_value> corners,
    size_t n)
{
    nw       = ((nw == 0 || nw > ns) ? ns : nw);
    nw       = ((nw % 2) != 0 ? nw : nw - 1);
    win_cntr = ((win_cntr == 0) ? ns / 2LU : win_cntr);

    assert(win_cntr < ns && "Window Center is larger than trace length");
    size_t tail     = ((corners[1] == 0) ? 1LU : 2LU);
    size_t num_tail = n * tail;
    std::vector<exseis::utils::Trace_value> numer(num_tail + 1);
    std::vector<exseis::utils::Trace_value> denom(num_tail + 1);
    make_filter(
        type, numer.data(), denom.data(), n, fs, corners[0], corners[1]);

    for (size_t i = 0; i < nt; i++) {

        std::vector<exseis::utils::Trace_value> trc_orgnl(nw);

        for (size_t j = 0; j < nw; j++) {
            trc_orgnl[j] = trc[i * ns + (win_cntr - nw / 2) + j];
        }
        switch (domain) {
            case FltrDmn::Time:
                filter_time(
                    nw, trc_orgnl.data(), num_tail, numer.data(), denom.data(),
                    get_pad(pad));
                break;

            case FltrDmn::Freq:
                filter_freq(
                    nw, trc_orgnl.data(), fs, num_tail, numer.data(),
                    denom.data(), get_pad(pad));
                break;
        }

        for (size_t j = 0; j < nw; j++) {
            trc[i * ns + (win_cntr - nw / 2LL) + j] = trc_orgnl[j];
        }
    }
}

}  // namespace piol
}  // namespace exseis
