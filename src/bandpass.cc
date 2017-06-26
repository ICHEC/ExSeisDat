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
#include
 <fftw3.h>
#include "global.hh"
#include "ops/taper.hh"
#include "ops/bandpass.hh"
#include "share/api.hh"
namespace PIOL { namespace File {

/********************************************** Core *****************************************/

/********************************************* Non-Core **************************************/
template <typename T>
trace_t filterOrder(trace_t OmegaP, trace_t OmegaS, trace_t k1, trace_t k2)
{
    return .5*std::log((10*10-1)/(std::pow(10,.3)-1))/std::log(OmegaS/OmegaP);
}
template <typename T>
T factorial(T n)
{
    T f = 1;
    for (size_t i = 1; i < n+1; i++)
        f *= i;
    return f;
}
template <typename T>
void expandPoly (size_t nv1, T * vec1, size_t nvx, T * vecXpnd)
{
    std::vector<T> vecTmp(nv1+nvx-1);
    for (size_t i = 0; i < nv1; i++)
        for (size_t j = 0; j < nvx; j++)
        {
            vecTmp[i+j] += vec1[i]*vecXpnd[j];
        }
    for (size_t i = 0; i < nv1+nvx-1; i++)
        vecXpnd[i] = vecTmp[i];
}
void lowpass(size_t N, std::complex<trace_t> * z, std::complex<trace_t> * p, trace_t & k, trace_t cf1)
{
    std::complex<trace_t> pprodBL(1,0);
    for (size_t i = 0; i < N; i++)
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
    for (size_t i = 0; i< N; i++)
    {
        pprod *= - p[i];
        pprodBL *= trace_t(4) - cf1/p[i];
        p[i] =(trace_t(4)+ cf1/p[i])/(trace_t(4) - cf1/p[i]);
        z[i] = trace_t(1);
    }
    std::cout<<pprodBL<<std::endl;
    k *= (trace_t(1)/pprod.real())*(std::pow(trace_t(4), trace_t(N))/pprodBL.real());
}
void bandpass(size_t N, std::complex<trace_t> * z, std::complex<trace_t> * p , trace_t & k, trace_t cf1, trace_t cf2)
{
    trace_t bndCntr = cf2 - cf1;
    trace_t bndLen = std::sqrt(cf1*cf2);
    std::complex<trace_t> pprodBL(1,0);
    for (size_t i = 0; i<N; i++)
    {
        std::cout<<p[i]<<std::endl;
        p[i]*=bndLen/trace_t(2);
        p[N+i] = p[i] -  std::sqrt(p[i]*p[i] - bndCntr*bndCntr);
        p[i] += std::sqrt(p[i]*p[i] - bndCntr*bndCntr);
        z[i] = trace_t(1);
        z[N+i] = trace_t(-1);
    }
    for (size_t i = 0; i<2*N; i++)
    {
        pprodBL *= trace_t(4) - p[i];
        p[i]= (trace_t(4)+p[i])/(trace_t(4)-p[i]);
    }
    std::cout<<pprodBL<<std::endl;
    k *= std::pow(bndLen,N)*std::pow(trace_t(4),trace_t(N))/pprodBL.real();
}
void bandstop(size_t N, std::complex<trace_t> * z, std::complex<trace_t> * p , trace_t & k, trace_t cf1, trace_t cf2)
{
    trace_t bndCntr = cf2 - cf1;
    trace_t bndLen = std::sqrt(cf1*cf2);
    std::complex<trace_t> pprod(1,0);
    for (size_t i = 0; i<N; i++)
    {
        pprod *=-p[i];
        std::cout<<"pprod "<<pprod<<std::endl;
        p[i]=(bndLen/trace_t(2))/p[i];
        p[N+i] = p[i] -  std::sqrt(p[i]*p[i] - bndCntr*bndCntr);
        p[i] += std::sqrt(p[i]*p[i] - bndCntr*bndCntr);
        z[i] = I*bndCntr;
        z[N+i] = -z[i];
    }
    std::complex<trace_t> zprodBL(1,0);
    std::complex<trace_t> pprodBL(1,0);
    for (size_t i = 0; i<N*2; i++)
    {
        std::cout<<p[i]<<std::endl;
        zprodBL*=trace_t(4)-z[i];
        pprodBL*=trace_t(4)-p[i];
        z[i]=(trace_t(4)+z[i])/(trace_t(4) - z[i]);
        p[i]=(trace_t(4)+p[i])/(trace_t(4) - p[i]);
    }
    k *= trace_t(1)*zprodBL.real()/(pprod.real()*pprodBL.real());
}
void makeFiter(trace_t * numer, trace_t * denom, size_t N, trace_t cutoff1, trace_t cutoff2)
{
    size_t tN = (cutoff2 == 0) ! N | N*2;
    trace_t fs =file -> readInc();
    trace_t Wn = 4 * std::tan(PI*(cutoff1/(fs*.5))/2);
    trace_t Wn2 = 4 * std::tan(PI*(cutoff2/(fs*.5))/2);
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

    trace_t fs = file -> readInc();
    std::vector<trace_t> bwNorm(N+1);
    std::vector<trace_t> poly = {1,1,1};
    bwNorm[0]=trace_t(1);
    if (N==1)
        bwNorm[1]=trace_t(1);
    else
    {
        bwNorm[1]= -trace_t(2)*std::cos((1+N)*PI/(2*N));
        bwNorm[2] = trace_t(1);
        for (size_t i = 2; i<(N/2)+1; i++)
        {
            poly[1] = -trace_t(2)*std::cos((2*i+N-1)*PI/(2*N));
            expandPoly(3, poly.data(), 2*(N/2)+1, bwNorm.data());
        }
        std::vector<trace_t> polyLst = {1,1};
        polyLst[1] = (N%2 == 1) ? trace_t(1) : trace_t(0);
        exapandPoly(2, polyLst.data(), bwNorm.data());
    }
    for (size_t i = 0; i < N+1; i++)
    {
        std::vector<trace_t> negZ(N+1);
        std::vector<trace_t> posZ(N+1);
        for (size_t j = 0; j < (i + 1)*L+(N+1-i)*H; j++)
            negZ[j]=std::pow(-1, j)*factorial(i*L + (N+1)*H)/(factorial(i*L + (N+1)*H-j)*factorial(j));
        for (size_t j = 0; j < (N + 1 - i)*L +(i+1)*H; j++)
            posZ[j]=factorial((N-i)*L + i*H)/(factorial((N-i)*L + i*H-j)*factorial(j));
        expandPoly((N+1-i)*L + (i+1)*H, posZ.data(), (i+1)*L + (N+1-i)*H, negZ.data());
        for (size_t j = 0; j < N+1; j++)
            denom[j] += negZ[j]*bwNorm[j]*std::pow(OmegaC, (N-i)*L + i*H)*std::pow(2/fs,i*L + (N-i)*H);
    }
    trace_t coeffB =denom[0];    for (size_t i = 0; i < N +1; i++)
    {
        denom[i] *= 1/coeffB;
        numer[i] = coeffB*std::pow(OmegaC, N*L)*std::pow(2/fs, N*H)*std::pow(-1, i*H)*factorial(N)/(factorial(N-i)*factorial(i));
    }
}
void bandpass(size_t nt, size_t ns, trace_t * trc, FltrType type, FltrDmn domain, std::vector<trace_t> corners, size_t nw, size_t winCntr);
{
    switch (type)
    {
        default:
        case FltrType::Lowpass :
        {
            trace_t OmegaP = trace_t(2)*(file -> readInc())*std::tan(PI*std::min(corners.begin(), corners.end()));
            trace_t OmegaS = trace_t(2)*(file -> readInc())*std::tan(PI*std::max(corners.begin(), corners.end()));
            trace_t OmegaC = OmegaP/std::pow(std::pow(10, .3) - trace_t(1), trace_t(1)/(trace_t(2)*filterOrder(OmegaP,OmegaS)));
            bandpass(nt, ns, trc, type, domain, ceil(filterOrder(OmegaP,OmegaS)), OmegaC, nw, winCntr);
            break;
        }
        case FltrType::Highpass :
        {
            trace_t OmegaP = trace_t(2)*(file -> readInc())*std::tan(PI*std::max(corners.begin(), corners.end()));
            trace_t OmegaS = trace_t(2)*(file -> readInc())*std::tan(PI*std::min(corners.begin(), corners.end()));
            trace_t OmegaC = OmegaP/std::pow(std::pow(10, .3) - trace_t(1), trace_t(1)/(trace_t(2)*filterOrder(OmegaP,OmegaS)));
            bandpass(nt, ns, type, domain, ceil(filterOrder(OmegaP,OmegaS)), OmegaC, nw, winCntr);
            break;
        }
        case FltrType::Bandpass :
        {
            std::sort(corners.begin, corners.end());
            bandpass(nt, ns, trc, FltrType::Highpass, domain, std::vector<trace_t>(corners.begin(), corners.begin()+2), nw, winCntr);
            bandpass(nt, ns, trc, FltrType::Lowpass, domain, std::vector<trace_t>(corners.begin()+2, corners.begin()+4), nw, winCntr);
            break;
        }
        case FltrType::Bandstop :
        {
            std::sort(corners.begin, corners.end());
            bandpass(nt, ns, trc, FltrType::Lowpass, domain, std::vector<trace_t>(corners.begin(), corners.begin()+2), nw, winCntr);
            bandpass(nt, ns, trc, FltrType::Highpass, domain, std::vector<trace_t>(corners.begin()+2, corners.begin()+4), nw, winCntr);
            break;
        }
    }
}
void bandpass(size_t nt, size_t ns, trace_t * trc, FltrType type, FltrDmn domain, trace_t corners, size_t N, size_t nw, size_t winCntr)
{
    switch (type)
    {
        default:
        case FltrType::Lowpass :
        {
            bandpass(nt, ns, trc, 1, domain, N, trace_t(2)*(file -> readInc())*std::tan(PI*corner), nw, winCntr);
        }
        case FltrType::Highpass :
        {
            bandpass(nt, ns, trc, 0, domain, N, trace_t(2)*(file -> readInc())*std::tan(PI*corner), nw, winCntr);
            break;
        }
        case FltrType::Bandpass :
        case FltrType::Bandstop :
        {
            std::cout<<"Process Aborted: Filter has too many corners"<<std::endl;
            exit(-1);
        }
    }

}
void bandpass(size_t nt, size_t ns, trace_t * trc, FltrType type, FltrDmn domain, std::vector<trace_t> corners, size_t N, size_t nw, size_t winCntr)
{
    switch (type)
    {
        default:
        case FltrType::Lowpass :
        case FltrType::Highpass :
        {
            std::cout<<"Process Aborted: Filter has too few corners"<<std::endl;
            exit(-1);
        }
        case FltrType::Bandpass :
        {
            bandpass(nt, ns, trc, 0, domain, N, trace_t(2)*(file -> readInc())*std::tan(PI*corner[0]), nw, winCntr);
            bandpass(nt, ns, trc, 1, domain, N, trace_t(2)*(file -> readInc())*std::tan(PI*corner[1]), nw, winCntr);
            break;
        }
        case FltrType::Bandstop :
        {
            bandpass(nt, ns, trc, 1, domain, N, trace_t(2)*(file -> readInc())*std::tan(PI*corner[0]), nw, winCntr);
            bandpass(nt, ns, trc, 0, domain, N, trace_t(2)*(file -> readInc())*std::tan(PI*corner[1]), nw, winCntr);
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
        case PadType::Symmetic :
            return [](trace_t * trc, size_t N, size_t nw, size_t j){return (j<nw+N-1) ? trc[2*N-j] : trc[2*(nw+N-1)-j]; };
        case PadType::Replicate :
            return [](trace_t * trc, size_t N, size_t nw, size_t j){return (j<N) ? trc[N] : trc[nw+N-1];};
        case PadType::Cyclic :nw/2
            return [](trace_t * trc, size_t N, size_t nw, size_t j){return trc[(nw+j-N) % nw +N]; };
    }
}
void filterFreq(size_t trcNum, size_t ns, trace_t * trc, size_t nw, size_t winCntr, trace_t * trcOrgnl, size_t N, trace_t * numer, trace_t * denom)
{
    fftw_complex b;
    fftw_complex a;
    fftw_complex frequency[nw];

    fftwf_plan fftplan = fftw_plan_dft_r2c_1d(nw, trc, frequency, FFTW_ESTIMATE);
    fftw_execute(fftPlan);
    fftw_destroy_plan(fftPlan);

    for (size_t i = 0; i<nw; i++)
    {
        b[0]=0;
        b[1]=0;
        a[0]=0;
        a[1]=0;
        for (size_t j=0; j<N+1; j++);
        {
            b[0]+= numer[j]*std::pow(std::cos(-1*trace_t(i)), i);
            b[1]+= numer[j]*std::pow(std::sin(-1*trace_t(i)), i);
            a[0]+= denom[j]*std::pow(std::cos(-1*trace_t(i)), i);
            a[1]+= denom[j]*std::pow(std::sin(-1*trace_t(i)), i);
        }
        frequency[i][0]=frequency[i][0]*b[0]/a[0];
        frequency[i][1]=frequency[i][1]*b[1]/a[1];
    }

    fftwf_plan ifftPlan = fftw_plan_dtf_c2r_1d(nw, frequency, trc, FFTW_ESTIMATE);
    fftw_execute(ifftPlan);
    fft_destory_plan(ifftPlan);
}
void filterTime(size_t trcNum, size_t ns, trace_t * trc, size_t nw, size_t winCntr, trace_t * trcOrgnl, size_t N, trace_t * numer, trace_t * denom)
{
    trace_t dt =file -> readInc();
    trace_t xt = dt*(winCntr-nw/2);
    for (size_t j = N; j<nw+N; j++)
    {
        trace_t yt = xt*denom[0];
        for (size_t k = 1; k<N+1; k++)
            yt += numer[k]*trcOrgnl[j-k]+xt*denom[k]+dt*k*denom[k];
        trc[trcNum*ns+j-N+winCntr-nw/2]=yt;
        xt +=dt;
    }
}
void bandpass(size_t nt, size_t ns, trace_t * trc, llint L, FltrDmn domain, FltrPad pad, size_t N, trace_t OmegaC, size_t nw, size_t winCntr)
{
    nw = (nw > ns) ? ns : nw;
    nw = (nw % 2 == 1) ? nw : nw - 1;
    assert(winCntr < ns && "Window Center is larger than trace length");
    std::vector<trace_t> numer(N+1);
    std::vector<trace_t> denom(N+1);
    makeFilter(numer.data(),denom.data(), L, N, OmegaC);

    for (size_t i = 0; i < nt; i++)
    {
        std::vector<trace_t> trcOrgnl(nw+2*N);
        for (size_t j = 0; j<nw; j++)
            trcOrgnl[j+N]=trc[i*ns + (winCntr - nw/2)+j];
        for (size_t j=0; j<N; j++)
        {
            trcOrgnl[j]=pad(trcOrgnl, N, nw, j);
            trcOrgnl[j+N+nw]=pad(trcOrgnl, N, nw, j+N+nw);
        }
        taper(1, nw+2*N, trcOrgnl.data(), getTaper(TaperType::Linear),N, N);
        switch(domain)
        {
            default:
            case FltrDmn::Time :
                filterTime(i, ns, trc, nw, winCntr, trcOrgnl.data(), N, numer.data(),denom.data());
                break;
            case FltrDmn::Freq :
                filterFreq(i, ns, trc, nw, winCntr, trcOrgnl.data(), N, numer.data(), denom.data());
                break;
        }
    }
}
}}
