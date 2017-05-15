#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "tglobal.hh"
#include <cmath>
#include <vector>
#define private public
#define protected public
#include "file/file.hh"
#include "set/set.hh"
#include "set.hh"
#undef private
#undef protected
#include <iostream>
namespace PIOL {
extern std::pair<size_t, size_t> decompose(size_t sz, size_t numRank, size_t rank);     //TODO: TEMP!
}

using namespace PIOL;
using namespace testing;

class MockFile : public File::ReadInterface
{
    public :
    MOCK_CONST_METHOD0(readNs, size_t(void));
    MOCK_METHOD0(readNt, size_t(void));
    MOCK_CONST_METHOD0(readInc, geom_t(void));

    MOCK_METHOD1(writeText, void(const std::string));
    MOCK_METHOD1(writeNs, void(const csize_t));
    MOCK_METHOD1(writeNt, void(const csize_t));
    MOCK_METHOD1(writeInc, void(const geom_t));

    MOCK_CONST_METHOD4(readParam, void(csize_t, csize_t, File::Param *, csize_t));
    MOCK_CONST_METHOD4(readParam, void(csize_t, csize_t *, File::Param *, csize_t));
    MOCK_CONST_METHOD5(readTrace, void(csize_t, csize_t, trace_t *, File::Param *, csize_t));
    MOCK_CONST_METHOD5(readTrace, void(csize_t, csize_t *, trace_t *, File::Param *, csize_t));
    MOCK_CONST_METHOD5(readTraceNonMono, void(csize_t, csize_t *, trace_t *, File::Param *, csize_t));
};

ACTION_P(cpyprm, src)
{
   *arg2 = *src;
}

extern void muting(size_t nt, size_t ns, trace_t * trc, size_t mute);
extern void taperMan(size_t nt, size_t ns, trace_t * trc, std::function<trace_t(trace_t wt, trace_t ramp)> func, size_t nTailLft, size_t nTailRt);

struct SetTest : public Test
{
    std::shared_ptr<ExSeisPIOL> piol;
    std::unique_ptr<Set> set;
    std::deque<File::Param> prm;
    Comm::MPI::Opt opt;
    const double pi = M_PI;

    SetTest(void)
    {
        opt.initMPI = false;
        piol = std::make_shared<ExSeisPIOL>(opt);
        set = nullptr;
    }
    void init(size_t numFile, size_t numNs, size_t numInc, size_t srtCnt, bool linear)
    {
        if (set.get() != nullptr)
            set.release();
        set = std::make_unique<Set>(piol);
        for (size_t j = 0; j < numNs; j++)
            for (size_t k = 0; k < numInc; k++)
                for (size_t i = 0; i < numFile; i++)
                {
                    auto mock = std::make_unique<MockFile>();
                    size_t nt = 1000U + i;
                    EXPECT_CALL(*mock, readNt()).WillRepeatedly(Return(nt));
                    EXPECT_CALL(*mock, readNs()).WillRepeatedly(Return(1000U + i));
                    EXPECT_CALL(*mock, readInc()).WillRepeatedly(Return(1000. + geom_t(i)));

                    auto dec = decompose(nt, piol->comm->getNumRank(), piol->comm->getRank());
                    prm.emplace_back(dec.second);
                    File::Param * tprm = &prm.back();

                    if (linear)
                        for (size_t l = 0; l < dec.second; l++)
                        {
                            setPrm(l, Meta::xSrc, 2000. - geom_t(dec.first + l), tprm);
                            setPrm(l, Meta::ySrc, 2000. - geom_t(dec.first + l), tprm);
                            setPrm(l, Meta::xRcv, 2000. + geom_t(dec.first + l), tprm);
                            setPrm(l, Meta::yRcv, 2000. + geom_t(dec.first + l), tprm);
                            setPrm(l, Meta::xCmp, 2000. - geom_t(dec.first + l), tprm);
                            setPrm(l, Meta::yCmp, 2000. - geom_t(dec.first + l), tprm);
                            setPrm(l, Meta::il, 2000U + dec.first + l, tprm);
                            setPrm(l, Meta::xl, 2000U + dec.first + l, tprm);
                            setPrm(l, Meta::tn, l+dec.first, tprm);

                            auto xS = 2000. - geom_t((dec.first + l) % (nt / 10U));
                            auto yR = 2000. + geom_t((dec.first + l) / (nt / 10U));
                            setPrm(l, Meta::Offset, (xS-2000.)*(xS-2000.) + (2000.-yR)*(2000.-yR), tprm);
                        }
                    else
                        for (size_t l = 0; l < dec.second; l++)
                        {
                            auto xS = 2000U - (dec.first + l) % (nt / 10U);
                            auto yR = 2000U + (dec.first + l) / (nt / 10U);
                            setPrm(l, Meta::xSrc, xS, tprm);
                            setPrm(l, Meta::ySrc, 2000., tprm);
                            setPrm(l, Meta::xRcv, 2000., tprm);
                            setPrm(l, Meta::yRcv, yR, tprm);
                            setPrm(l, Meta::xCmp, xS, tprm);
                            setPrm(l, Meta::yCmp, yR, tprm);
                            setPrm(l, Meta::il, 4000U - xS, tprm);
                            setPrm(l, Meta::xl, yR, tprm);
                            setPrm(l, Meta::tn, l+dec.first, tprm);
                            setPrm(l, Meta::Offset, (xS-2000.)*(xS-2000.) + (2000.-yR)*(2000.-yR), tprm);
                        }
                    EXPECT_CALL(*mock, readParam(dec.second, An<csize_t *>(), _, _))
                                    .Times(Exactly(srtCnt))
                                    .WillRepeatedly(cpyprm(&prm.back()));

                    set->add(std::move(mock));
                }
    }


    void init(size_t numFile, size_t nt, size_t inactive)
    {
        constexpr size_t NOT_IN_OUTPUT = std::numeric_limits<size_t>::max();
        srand(1337);
        if (set.get() != nullptr)
            set.release();
        set = std::make_unique<Set>(piol);
        for (size_t i = 0; i < numFile; i++)
        {
            auto mock = std::make_unique<MockFile>();
            EXPECT_CALL(*mock, readNt()).WillRepeatedly(Return(nt));
            EXPECT_CALL(*mock, readNs()).WillRepeatedly(Return(1000U));
            EXPECT_CALL(*mock, readInc()).WillRepeatedly(Return(1000.));

            set->add(std::move(mock));

            llint range = (nt / inactive);
            for (size_t j = 0; j < inactive; j++)
            {
                size_t randj = range*j + (rand() % range);
                set->file[i]->lst[randj] = NOT_IN_OUTPUT;
            }
        }
    }

    void taperTest(size_t nt, size_t ns, size_t mute, std::function<trace_t(trace_t, trace_t)> func, TaperType type, size_t nTailLft, size_t nTailRt)
    {
       if (set.get() != nullptr)
            set.release();
        set = std::make_unique<Set>(piol);
        std::vector<trace_t> trc(nt * ns);
        std::vector<trace_t> trcMan(nt * ns);
        File::Param p(nt);
        std::fill(trc.begin(), trc.end(), 1.0f);
        if (mute != 0)
            muting(nt, ns, trc.data(),  mute);
        trcMan = trc;

        if (nTailRt == 0)
            set->taper(type, nTailLft);
        else
            set->taper(type, nTailLft, nTailRt);
        set->modify(ns, &p, trc.data());

        taperMan(nt, ns, trcMan.data(), func, nTailLft, nTailRt);
        for (size_t i = 0; i < nt; i++)
            for (size_t j = 0; j < ns; j++)
            {
                EXPECT_FLOAT_EQ(trc[i*ns+j],trcMan[i*ns+j]);
            }
    }
    void agcTest(size_t nt, size_t ns, AGCType type, std::function<trace_t(size_t, trace_t *)> func, size_t window, trace_t normR)
    {
        if (set.get() != nullptr)
            set.release();
        set = std::make_unique<Set>(piol);
        std::vector<trace_t> trc(nt*ns);
        std::vector<trace_t> trcMan(nt*ns);
        File::Param p(nt);

        for (size_t i = 0; i < nt; i++)
            for (size_t j = 0; j < ns; j++)
            {
                trc[i*ns + j] = float(j);
                std::cout<<trc[i*ns+j]<<std::endl;//float(j)*std::exp(-.05*float(j))*std::cos(pi*2*float(j));
            }
       trcMan = trc;
       set->agc(type, window, normR);
       set->modify(ns, &p, trc.data());
       size_t win;
       for (size_t i = 0; i < nt; i++)
           for (size_t j = 0; j < ns; j++)
           {
               if (j < window - 1)
               {
                   win = j + 1;
                   std::cout<<win<<std::endl;
               }
               else if ((ns - j) < window)
               {
                   win = ns - j;
                   std::cout<<win<<std::endl;
               }
               else
               {
                   win = window;
               }
               std::cout<<"Man win: "<<win<<std::endl;
               std::vector<trace_t> trcWin(trcMan.begin() + (i*ns +j), trcMan.end() + (i*ns + j + win));
              // trcMan[i*ns+j] = normR/func(win, trcWin.data());
               EXPECT_FLOAT_EQ(trc[i*ns+j], normR/func(win,trcWin.data()));
           }
    }
};

