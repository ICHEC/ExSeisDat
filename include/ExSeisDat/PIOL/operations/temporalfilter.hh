////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Meghan Fisher - meghan.fisher@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date May 2017
/// @brief
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_OPERATIONS_TEMPORALFILTER_HH
#define EXSEISDAT_PIOL_OPERATIONS_TEMPORALFILTER_HH

#include "ExSeisDat/utils/typedefs.h"

#include <complex>
#include <functional>
#include <vector>

namespace exseis {
namespace PIOL {

using namespace exseis::utils::typedefs;

/// An enum class of the different types of filters.
enum class FltrType : int {
    /// Create Lowpass IIR Butterworth filter
    Lowpass,

    /// Create Highpass IIR Butterworth filter
    Highpass,

    /// Create Bandpass IIR Butterworth filter
    Bandpass,

    /// Create Bandstop IIR Butterworth filter
    Bandstop
};

/// An enum class of the different types of filtering domains.
enum class FltrDmn : int {
    /// Filter in time domain
    Time,

    /// Filter in frequency domain
    Freq
};

/// An enum class of the different types of trace padding functions.
enum class PadType : int {
    /// Pad using zeros
    Zero,

    /// Pad using reflection of trace
    Symmetric,

    /// Pad using closest value in trace
    Replicate,

    /// Pad using values from other end of trace
    Cyclic
};


/// Function interface for filter padding functions.
using FltrPad = std::function<exseis::utils::Trace_value(
  exseis::utils::Trace_value*, size_t, size_t, size_t)>;

/// Complex type for traces
using Complex_trace_value = std::complex<exseis::utils::Trace_value>;


/*! Determines the filter order if given passband and stopband frequecnies
 *  @param[in] cornerP Passband corner
 *  @param[in] cornerS Stopband corner
 *  @return Filter order
 */
size_t filterOrder(
  exseis::utils::Trace_value cornerP, exseis::utils::Trace_value cornerS);

/*! Expands a series of polynomials of the form (z-b0)(z-b1)...(z-bn)
 *  @param[in] coef Vector of b coefficients
 *  @param[in] nvx  Number of b coefficients
 *  @param[in] poly Expanded polynomial coefficients
 */
void expandPoly(
  const Complex_trace_value* coef,
  size_t nvx,
  exseis::utils::Trace_value* poly);

/*! Creates a digital Butterworth lowpass filter for a given corner in
 *  zero/pole/gain form
 *  @param[in] N   Filter order
 *  @param[in] z   Vector of filter zeros
 *  @param[in] p   Vector of filter poles
 *  @param[in] cf1 Corner passband frequency (Hz)
 *  @return DOCUMENT ME
 *  @todo   DOCUMENT return type
 */
exseis::utils::Trace_value lowpass(
  size_t N,
  Complex_trace_value* z,
  Complex_trace_value* p,
  exseis::utils::Trace_value cf1);

/*! Creates a digital Butterworth highpass filter for a given corner in
 *  zero/pole/gain form
 *  @param[in] N   Filter order
 *  @param[in] z   Vector of filter zeros
 *  @param[in] p   Vector of filter poles
 *  @param[in] cf1 Corner passband frequency (Hz)
 *  @return DOCUMENT ME
 *  @todo DOCUMENT return type
 */
exseis::utils::Trace_value highpass(
  size_t N,
  Complex_trace_value* z,
  Complex_trace_value* p,
  exseis::utils::Trace_value cf1);

/*! Creates a digital Butterworth bandpass filter for a given corner in
 *  zero/pole/gain form
 *  @param[in] N   Filter order
 *  @param[in] z   Vector of filter zeros
 *  @param[in] p   Vector of filter poles
 *  @param[in] cf1 Left corner passband frequency (Hz)
 *  @param[in] cf2 Right corner passband frequecy (Hz)
 *  @return DOCUMENT ME
 *  @todo DOCUMENT return type
 */
exseis::utils::Trace_value bandpass(
  size_t N,
  Complex_trace_value* z,
  Complex_trace_value* p,
  exseis::utils::Trace_value cf1,
  exseis::utils::Trace_value cf2);

/*! Creates a digital Butterworth bandstop filter for a given corner in
 *  zero/pole/gain form
 *  @param[in] N   Filter order
 *  @param[in] z   Vector of filter zeros
 *  @param[in] p   Vector of filter poles
 *  @param[in] cf1 Left corner passband frequency (Hz)
 *  @param[in] cf2 Right corner passband frequecy (Hz)
 *  @return DOCUMENT ME
 *  @todo DOCUMENT return type
 */
exseis::utils::Trace_value bandstop(
  size_t N,
  Complex_trace_value* z,
  Complex_trace_value* p,
  exseis::utils::Trace_value cf1,
  exseis::utils::Trace_value cf2);

/*! Creates a discrete, digital Butterworth filter for a given corner in
 *  polynomial transfer function form
 *  @param[in] type  Type of filter (lowpass, highpass, bandstop, bandpass)
 *  @param[in] numer Array of polynomial coefficiences in the numerator of
 *                   filter transfer function
 *  @param[in] denom Array of polynomial coefficiences in the denominator of
 *                   filter transfer function
 *  @param[in] N     Filter order
 *  @param[in] fs    Sampling frequency
 *  @param[in] cf1   Left corner passband frequency (Hz)
 *  @param[in] cf2   Right corner passband frequency (Hz)
 */
void makeFilter(
  FltrType type,
  exseis::utils::Trace_value* numer,
  exseis::utils::Trace_value* denom,
  exseis::utils::Integer N,
  exseis::utils::Trace_value fs,
  exseis::utils::Trace_value cf1,
  exseis::utils::Trace_value cf2);

/*! Get the pattern for padding traces for filtering
 *  @param[in] type Type of padding
 *  @return function for padding trace
 */
FltrPad getPad(PadType type);

/*! Filter trace in frequency domain
 *  @param[in] nss     Number of Subtrace Samples
 *  @param[in] trcX    Unfiltered windowed and padded trace
 *  @param[in] fs      Sampling frequency
 *  @param[in] N       Filter Order
 *  @param[in] numer   Array of polynomial coefficiences in the numerator of
 *                     filter transfer function
 *  @param[in] denom   Array of polynomial coefficiences in the denominator of
 *                     filter transfer function
 *  @param[in] padding Funtion for padding trace
 */
void filterFreq(
  size_t nss,
  exseis::utils::Trace_value* trcX,
  exseis::utils::Trace_value fs,
  size_t N,
  exseis::utils::Trace_value* numer,
  exseis::utils::Trace_value* denom,
  FltrPad padding);

/*! Filter trace in time domain
 *  @param[in] nw       Size of filter window
 *  @param[in] trcOrgnl Unfiltered windowed and padded trace
 *  @param[in] N        Filter Order
 *  @param[in] numer    Array of polynomial coefficiences in the numerator of
 *                      filter transfer function
 *  @param[in] denom    Array of polynomial coefficiences in the denominator of
 *                      filter transfer function
 *  @param[in] padding  Funtion for padding trace
 */
void filterTime(
  size_t nw,
  exseis::utils::Trace_value* trcOrgnl,
  size_t N,
  exseis::utils::Trace_value* numer,
  exseis::utils::Trace_value* denom,
  FltrPad padding);

/*! Temporally filter traces when given passband and stopband frequencies
 *  @param[in] nt      Number of traces
 *  @param[in] ns      Number of samples per trace
 *  @param[in] trc     Traces
 *  @param[in] fs      Sampling frequency
 *  @param[in] type    Type of filter
 *  @param[in] domain  Domain to filter in (frequency or time)
 *  @param[in] pad     Type of trace padding
 *  @param[in] nw      Size of filter window
 *  @param[in] winCntr Center of filter window
 *  @param[in] corners Vector of corner frequencies (Hz)
 */
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
  std::vector<exseis::utils::Trace_value> corners);

/*! Temporally filter traces when given passband frequencies and filter Order
 *  @param[in] nt      Number of traces
 *  @param[in] ns      Number of samples per trace
 *  @param[in] trc     Traces
 *  @param[in] fs      Sampling Frequency
 *  @param[in] type    Type of filter
 *  @param[in] domain  Domain to filter in (frequency or time)
 *  @param[in] pad     Type of trace padding
 *  @param[in] nw      Size of filter window
 *  @param[in] winCntr Center of filter window
 *  @param[in] N       Filter order
 *  @param[in] corners Vector of corner frequencies (Hz)
 */
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
  size_t N);

}  // namespace PIOL
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_OPERATIONS_TEMPORALFILTER_HH
