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
#include <fftw3.h>
#include <complex>
#include "global.hh"
#include "set/set.hh"
#include "ops/temporalfilter.hh"
#include "share/api.hh"
namespace PIOL { namespace File {

/********************************************** Core *****************************************/

/********************************************* Non-Core **************************************/
size_t filterOrder(trace_t cornerP, trace_t cornerS, trace_t fs)
{
    trace_t OmegaP = trace_t(2)*std::tan(PI*cornerP)/fs;
    trace_t OmegaS = trace_t(2)*std::tan(PI*cornerS)/fs;
    return std::ceil(.5*std::log((10*10-1)/(std::pow(10,.3)-1))/std::log(OmegaS/OmegaP));
}
void expandPoly (std::complex<trace_t> * coef, size_t nvx, trace_t * poly)
{
    std::complex<trace_t> vecTemp;
    std::vector<std::complex<trace_t>> vecXpnd(nvx+1LU);
    vecXpnd[0LU]=-coef[0LU];
    vecXpnd[1LU]=trace_t(1);
    for (size_t i = 1LU; i < nvx; i++)
    {
        vecXpnd[i+1LU]=trace_t(1);
        for (size_t j = 0LU; j< i; j++)
            vecXpnd[i-j]=vecXpnd[i-j]*-coef[i]+vecXpnd[i-j-1LU];
        vecXpnd[0]=vecXpnd[0]*-coef[i];
    }
    for (size_t i=0LU; i< nvx+1LU; i++)
        poly[i]=vecXpnd[i].real();
}
void lowpass(size_t N, std::complex<trace_t> * z, std::complex<trace_t> * p, trace_t & k, trace_t cf1)
{
    std::complex<trace_t> pprodBL(1,0);
    for (size_t i = 0LU; i < N; i++)
    {
        pprodBL *= trace_t(4) - p[i]*cf1;
        p[i]=(trace_t(4)+(p[i]*cf1))/(trace_t(4)-(p[i]*cf1));
        z[i]=trace_t(-1);
    }
    k*=std::pow(cf1,N)/pprodBL.real();
}
void highpass(size_t N, std::complex<trace_t> * z, std::complex<trace_t> * p, trace_t & k, trace_t cf1)
{
    std::complex<trace_t> pprod(1,0);
    std::complex<trace_t> pprodBL(1,0);
    for (size_t i = 0LU; i< N; i++)
    {
        pprod *= - p[i];
        pprodBL *= trace_t(4) - cf1/p[i];
        p[i] =(trace_t(4)+ cf1/p[i])/(trace_t(4) - cf1/p[i]);
        z[i] = trace_t(1);
    }
    k *= (trace_t(1)/pprod.real())*(std::pow(trace_t(4), trace_t(N))/pprodBL.real());
}
void bandpass(size_t N, std::complex<trace_t> * z, std::complex<trace_t> * p , trace_t & k, trace_t cf1, trace_t cf2)
{

    trace_t bndCntr = cf2 - cf1;
    trace_t bndLen = std::sqrt(cf1*cf2);
    std::complex<trace_t> pprodBL(1,0);
    std::complex<trace_t> bndLen2;
    bndLen2=bndLen*bndLen;
    for (size_t i = 0LU; i<N; i++)
    {
        p[i]*=bndCntr/trace_t(2);
        p[N+i] = p[i] -  std::sqrt(p[i]*p[i] - bndLen*bndLen);
        p[i] += std::sqrt(p[i]*p[i] - bndLen*bndLen);
        z[i] = trace_t(1);
        z[N+i] = trace_t(-1);
    }
    for (size_t i = 0LU; i<2LU*N; i++)
    {
        pprodBL *= trace_t(4) - p[i];
        p[i]= (trace_t(4)+p[i])/(trace_t(4)-p[i]);
    }
    k *= std::pow(bndCntr,N)*(std::pow(trace_t(4),trace_t(N))*pprodBL.real())/(pprodBL.real()*pprodBL.real() + pprodBL.imag()*pprodBL.imag());
}
void bandstop(size_t N, std::complex<trace_t> * z, std::complex<trace_t> * p , trace_t & k, trace_t cf1, trace_t cf2)
{
    trace_t bndCntr = cf2 - cf1;
    trace_t bndLen = std::sqrt(cf1*cf2);
    std::complex<trace_t> bndLen2;
    bndLen2=bndLen*bndLen;
    for (size_t i = 0LU; i<N; i++)
    {
        p[i]=(bndCntr/trace_t(2))/p[i];
        p[N+i] = p[i] -  std::sqrt(p[i]*p[i] - bndLen2);
        p[i] += std::sqrt(p[i]*p[i] - bndLen2);
        z[i] = I*bndLen;
        z[N+i] = -z[i];
    }
    std::complex<trace_t> zPrdct(1,0);
    std::complex<trace_t> pPrdct(1,0);
    for (size_t i = 0LU; i<N*2LU; i++)
    {
        zPrdct*=trace_t(4.0)-z[i];
        pPrdct*=trace_t(4.0)-p[i];
        z[i]=(trace_t(4)+z[i])/(trace_t(4) - z[i]);
        p[i]=(trace_t(4)+p[i])/(trace_t(4) - p[i]);
    }
    k = (zPrdct.real()*pPrdct.real()+zPrdct.imag()*pPrdct.imag())/(pPrdct.real()*pPrdct.real()+pPrdct.imag()*pPrdct.imag());
}
void makeFilter(FltrType type, trace_t * numer, trace_t * denom, size_t N, trace_t fs, trace_t cf1, trace_t cf2)
{
    size_t tN = (cf2 == 0) ? N : N*2;
    trace_t Wn = 4 * std::tan(PI*(cf1/(fs*.5))/2);
    trace_t Wn2 = 4 * std::tan(PI*(cf2/(fs*.5))/2);
    std::vector<std::complex<trace_t>> z(tN);
    std::vector<std::complex<trace_t>> p(tN);
    trace_t k = trace_t(1);
    for (size_t i = 0; i<N; i++)
        p[i]=-exp(I*PI*(-trace_t(N)+1+2*trace_t(i))/(2*trace_t(N)));

    switch(type)
    {
        default:
        case FltrType::Lowpass :
            lowpass(N, z.data(), p.data(),k, Wn);
            break;
        case FltrType::Highpass :
            highpass(N, z.data(), p.data(), k, Wn);
            break;
        case FltrType::Bandpass :
            bandpass(N, z.data(), p.data(), k, Wn, Wn2);
            break;
        case FltrType::Bandstop :
            bandstop(N, z.data(), p.data(), k, Wn, Wn2);
            break;
    }
    expandPoly(z.data(), tN, numer);
    expandPoly(p.data(), tN, denom);
    trace_t ahold;
    for (size_t i=0LU; i< tN+1LU; i++)
        numer[i]*=k;
    for (size_t i = 0LU; i< (tN+1LU)/2LU; i++)
    {
        ahold =denom[i];
        denom[i]=denom[tN-i];
        denom[tN-i]=ahold;
    }
}

void temporalFilter(size_t nt, size_t ns, trace_t * trc, trace_t fs, FltrType type, FltrDmn domain,  PadType pad,size_t nw, size_t winCntr, std::vector<trace_t> corner)
{
    std::sort(corner.begin(), corner.end());
    switch (type)
    {
        default:
        case FltrType::Lowpass :
        {
            std::vector<trace_t> c={corner[0LU], 0};
            size_t N= filterOrder(corner[0LU], corner[1LU], fs);
            temporalFilter(nt, ns, trc, fs, type, domain, pad, nw, winCntr,c,N);
            break;
        }
        case FltrType::Highpass :
        {
            std::vector<trace_t> c ={corner[1LU], 0};
            size_t N = filterOrder(corner[1LU], corner[0LU],fs);
            temporalFilter(nt, ns, trc, fs, type, domain, pad, nw, winCntr,c, N);
            break;
        }
        case FltrType::Bandpass :
        {
            std::vector<trace_t> c = {corner[1], corner[2]};
            temporalFilter(nt, ns, trc, fs, type, domain, pad, nw, winCntr,c, std::max(filterOrder(corner[1],corner[0],fs),filterOrder(corner[2],corner[3],fs)));
            break;
        }
        case FltrType::Bandstop :
        {
            std::vector<trace_t> c = {corner[0], corner[3]};
            temporalFilter(nt, ns, trc, fs, type, domain, pad, nw, winCntr, c, std::max(filterOrder(corner[0],corner[1],fs),filterOrder(corner[3],corner[2],fs)));
            break;

        }
    }
}
FltrPad getPad(PadType type)
{
    switch (type)
    {
        default :
        case PadType::Zero :
            return [](trace_t * trc, size_t N, size_t nw, size_t j){return trace_t(0); };
            break;
        case PadType::Symmetric :
            return [](trace_t * trc, size_t N, size_t nw, size_t j){return (j<=nw) ? trc[N-j] : trc[2*(nw+N) -j];};
            break;
        case PadType::Replicate :
            return [](trace_t * trc, size_t N, size_t nw, size_t j){return (j<=nw) ? trc[0] : trc[nw];};
            break;
        case PadType::Cyclic :
            return [](trace_t * trc, size_t N, size_t nw, size_t j){return (j<=nw) ? trc[nw -(N-j)] : trc[j-nw -N]; };
            break;
    }
}
void filterFreq(size_t nss, trace_t * trcX, trace_t fs, size_t N,trace_t * numer, trace_t * denom, FltrPad padding)
{
//TODO: Generalize fftwf for other data types besides floats
    auto frequency = new fftwf_complex[nss];
    auto freqFilter = new fftwf_complex[nss];
    fftwf_plan planFFT = fftwf_plan_dft_r2c_1d(nss, trcX, frequency, FFTW_MEASURE);
    fftwf_execute(planFFT);
    fftwf_destroy_plan(planFFT);
    std::complex<trace_t> b(0,0);
    std::complex<trace_t> a(0,0);
    for (size_t i = 0; i<nss/2 + 1; i++)
    {
        b=(0,0);
        a=(0,0);
        for (size_t j = 0; j < N + 1; j++)
        {
            b.real(b.real() + numer[j]*std::cos(-fs*trace_t(j)*trace_t(i)/nss));
            b.imag(b.imag() + numer[j]*std::sin(-fs*trace_t(j)*trace_t(i)/nss));
            a.real(a.real() + denom[j]*std::cos(-fs*trace_t(j)*trace_t(i)/nss));
            a.imag(a.imag() + denom[j]*std::sin(-fs*trace_t(j)*trace_t(i)/nss));
        }
        std::complex<trace_t> H = b/a;
        freqFilter[i][0]=frequency[i][0]*std::fabs(H.real())-frequency[i][1]*std::fabs(H.imag());
        freqFilter[i][1]=frequency[i][1]*std::fabs(H.real())+frequency[i][0]*std::fabs(H.imag());
        trcX[i]=0;
    }
    fftwf_plan planIFFT =fftwf_plan_dft_c2r_1d(nss, freqFilter, trcX,FFTW_MEASURE);
    fftwf_execute(planIFFT);
    fftwf_destroy_plan(planIFFT);
    for (size_t i =0; i< nss; i++)
        trcX[i] /=nss;
    delete[] frequency;
    delete[] freqFilter;
}
void IIR(size_t N, size_t ns, trace_t * b, trace_t * a, trace_t * x, trace_t * y, trace_t * zi)
{
    y[0]= b[0]*x[0]+zi[0];
    for (size_t i=1; i< N; i++)
    {
        y[i] = b[0]*x[i]+zi[i];
        for (size_t j = 1; j < i+1; j++)
            y[i]+=b[j]*x[i-j]-a[j]*y[i-j];
    }
    for (size_t i = N; i < ns; i++)
    {
        y[i]= b[0]*x[i];
        for (size_t j =1; j < N+1; j++)
            y[i] +=b[j]*x[i-j]-a[j]*y[i-j];
    }
}
void filterTime(size_t nw, trace_t * trcOrgnl, size_t Nt, trace_t * numer, trace_t * denom, FltrPad padding)
{
    std::vector<trace_t> trcX (nw+6*(Nt+1));
    for (size_t i =0; i < 3*(Nt+1); i++)
    {
        trcX[i]=padding(trcOrgnl,3*(Nt+1),nw,i);
        trcX[i+3*Nt+nw]=padding(trcOrgnl, 3*(Nt+1), nw-1, i+3*Nt+nw);
    }
    for (size_t i = 0; i<nw; i++)
        trcX[i+3*(Nt+1)]=trcOrgnl[i];
    std::vector<trace_t> zi(Nt);
    std::vector<trace_t> ziF(Nt);
    std::vector<trace_t> trcY(nw +6*(Nt+1));
    trace_t B =0;
    trace_t Imin =trace_t(1);
    for (size_t i =1; i< Nt+1; i++)
    {
        B += numer[i]-denom[i]*numer[0];
        Imin += denom[i];
    }
    zi[0]=B/Imin;
    trace_t a = trace_t(1);
    trace_t c = trace_t(0);
    for (size_t i = 1; i < Nt; i++)
    {
        a += denom[i];
        c += numer[i]-denom[i]*numer[0];
        zi[i]=a *zi[0]-c;
    }
    for (size_t i = 0; i < Nt; i++)
        ziF[i]=zi[i]*trcX[0];
    IIR(Nt, nw+6*(Nt+1), numer, denom, trcX.data(), trcY.data(), ziF.data());
    for (size_t i = 0; i < nw+6*(Nt+1); i++)
    {
        trcX[i]=trcY[nw+6*(1+Nt) - i-1];
        trcY[nw+6*(Nt+1)-1 - i]=0;
    }
    for (size_t i = 0; i < Nt; i++)
        zi[i] *= trcX[0];
    IIR(Nt, nw+6*(Nt+1), numer, denom, trcX.data(), trcY.data(), zi.data());
    for (size_t i = 0; i < nw; i++)
        trcOrgnl[i]=trcY[nw+3*(Nt+1) - 1 - i];
}
void temporalFilter(size_t nt, size_t ns, trace_t * trc, trace_t fs, FltrType type, FltrDmn domain, PadType pad, size_t nw, size_t winCntr,  std::vector<trace_t> corners, size_t N)
{
    nw = (nw == 0U) ? ns : nw;
    winCntr = (winCntr == 0U) ?  ns/2 : winCntr;
    nw = (nw > ns) ? ns : nw;
    nw = (nw % 2 == 1) ? nw : nw - 1;
    assert(winCntr < ns && "Window Center is larger than trace length");
    size_t tail= (corners[1]==trace_t(0)) ? 1LU : 2LU;
    size_t Nt = N*tail;
    std::vector<trace_t> numer(Nt+1);
    std::vector<trace_t> denom(Nt+1);
    makeFilter(type, numer.data(),denom.data(), N, fs, corners[0], corners[1]);
    for (size_t i = 0; i < nt; i++)
    {
        std::vector<trace_t> trcOrgnl(nw);
        for (size_t j = 0; j<nw; j++)
            trcOrgnl[j]=trc[i*ns + (winCntr - nw/2)+j];
        switch(domain)
        {
            case FltrDmn::Time :
                filterTime(nw, trcOrgnl.data(),Nt, numer.data(), denom.data(), getPad(pad));
                break;
            case FltrDmn::Freq :
                filterFreq(nw, trcOrgnl.data(),fs,Nt,  numer.data(), denom.data(), getPad(pad));
                break;
        }
        for (size_t j = 0; j < nw; j++)
            trc[i*ns+(winCntr - nw/2) + j] =trcOrgnl[j];
    }
}
}}
