#include <memory>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "global.hh"
#include "tglobal.hh"
#include "share/segy.hh"
#include "share/datatype.hh"
#include "anc/cmpi.hh"
#include "data/datampiio.hh"
#define private public
#define protected public
#include "object/object.hh"
#include "object/objsegy.hh"
#undef private
#undef protected

using namespace testing;
using namespace PIOL;

class MockData : public Data::Interface
{
    public :
    MockData(const std::shared_ptr<ExSeisPIOL> piol_, const std::string name_) : Data::Interface(piol_, name_)
    {
    }

    MOCK_CONST_METHOD0(getFileSz, size_t(void));
    MOCK_CONST_METHOD3(read, void(csize_t, csize_t, uchar *));
    MOCK_CONST_METHOD5(read, void(csize_t, csize_t, csize_t, csize_t, uchar *));
    MOCK_CONST_METHOD3(write, void(csize_t, csize_t, const uchar *));
    MOCK_CONST_METHOD5(write, void(csize_t, csize_t, csize_t, csize_t, const uchar *));
    // TODO: This method is not tested
    MOCK_CONST_METHOD1(setFileSz, void(csize_t));
};

class ObjSpecTest : public Test
{
    protected :
    std::shared_ptr<ExSeisPIOL> piol;
    const Obj::SEGYOpt segyOpt;
    Data::MPIIOOpt dataOpt;
    Comm::MPIOpt opt;
    std::shared_ptr<MockData> mock;
    Obj::Interface * obj;

    ObjSpecTest()
    {
        mock = nullptr;
        obj = nullptr;
        opt.initMPI = false;
        piol = std::make_shared<ExSeisPIOL>(opt);
    }
    template <bool WriteTest>
    void makeRealSEGY(std::string name)
    {
        if (obj != nullptr)
            delete obj;

        if (WriteTest)
            dataOpt.mode = FileMode::Test;
        obj = new Obj::SEGY(piol, name, segyOpt, dataOpt);
        piol->isErr();
    }

    void makeSEGY(std::string name = notFile)
    {
        if (obj != nullptr)
            delete obj;
        mock = std::make_shared<MockData>(piol, notFile);
        obj = new Obj::SEGY(piol, name, segyOpt, mock);
        piol->isErr();
    }

    ~ObjSpecTest()
    {
        if (obj != nullptr)
            delete obj;
    }

    void SEGYFileSizeTest(size_t sz)
    {
        EXPECT_CALL(*mock, getFileSz()).Times(Exactly(1)).WillOnce(Return(sz));
        piol->isErr();
        EXPECT_EQ(sz, obj->getFileSz());
    }

    template <bool MOCK=true>
    void readHOPatternTest(size_t off, uchar magic)
    {
        csize_t extra = 20U;
        std::vector<uchar> cHo;
        if (MOCK)
        {
            cHo.resize(SEGSz::getHOSz());
            for (size_t i = 0U; i < SEGSz::getHOSz(); i++)
                cHo[i] = getPattern(off + i);
                EXPECT_CALL(*mock, read(0U, SEGSz::getHOSz(), _))
                            .Times(Exactly(1))
                            .WillOnce(SetArrayArgument<2>(cHo.begin(), cHo.end()));
        }

        std::vector<uchar> ho(SEGSz::getHOSz() + 2*extra);
        for (auto i = 0U; i < extra; i++)
            ho[i] = ho[ho.size()-extra+i] = magic;

        obj->readHO(&ho[extra]);

        piol->isErr();

        for (auto i = 0U; i < SEGSz::getHOSz(); i++)
            ASSERT_EQ(getPattern(off + i), ho[extra+i]) << "Pattern " << i;
        for (auto i = 0U; i < extra; i++)
            ASSERT_EQ(magic, ho[ho.size()-extra+i]) << "Pattern Extra " << i;
    }

    template <bool MOCK = true>
    void writeHOPattern(size_t off, uchar magic)
    {
        if (MOCK && mock == nullptr)
        {
            std::cerr << "Using Mock when not initialised: LOC: " << __LINE__ << std::endl;
            return;
        }
        csize_t extra = 20U;
        std::vector<uchar> cHo(SEGSz::getHOSz());
        for (size_t i = 0U; i < SEGSz::getHOSz(); i++)
            cHo[i] = getPattern(off + i);

        if (MOCK)
            EXPECT_CALL(*mock, write(0U, SEGSz::getHOSz(), _))
                        .Times(Exactly(1))
                        .WillOnce(check2(cHo.data(), SEGSz::getHOSz()));

        std::vector<uchar> ho(SEGSz::getHOSz() + 2*extra);
        for (auto i = 0U; i < extra; i++)
            ho[i] = ho[ho.size()-extra+i] = magic;

        for (auto i = 0U; i < extra; i++)
            ho[i+extra] = cHo[i];

        obj->writeHO(&ho[extra]);
        piol->isErr();
        if (MOCK)
            readHOPatternTest<MOCK>(off, magic);
    }

    void SEGYReadHOTest(std::vector<uchar> magic)
    {
        const size_t extra = 1025U;
        std::vector<uchar> ho(extra + SEGSz::getHOSz() + extra);

        for (size_t j = 0U; j < magic.size(); j++)
        {
            for (size_t i = 0U; i < extra; i++)
                ho[i] = ho[ho.size() - extra + i] = magic[j];

            obj->readHO(ho.data() + extra);

            //Overrun checks
            for (size_t i = 0U; i < extra; i++)
            {
                ASSERT_EQ(magic[j], ho[i]);
                ASSERT_EQ(magic[j], ho[ho.size() - extra + i]);
            }
            for (size_t i = 0U; i < SEGSz::getHOSz(); i++)
                ASSERT_EQ(getPattern(i), ho[i+extra]);
        }
    }

    template <bool DOMD, bool MOCK = true>
    void readTest(csize_t offset, csize_t nt, csize_t ns, csize_t poff = 0, uchar magic = 0)
    {
        if (MOCK && mock == nullptr)
        {
            std::cerr << "Using Mock when not initialised: LOC: " << __LINE__ << std::endl;
            return;
        }
        const size_t extra = 20U;
        size_t bsz = (DOMD ? SEGSz::getMDSz() : SEGSz::getDFSz(ns));
        size_t step = nt * bsz;
        std::vector<uchar> trnew(step + 2U*extra);

        std::vector<uchar> tr;
        if (MOCK)
        {
            tr.resize(step);
            for (size_t i = 0U; i < tr.size(); i++)
                tr[i] = getPattern(poff + i + (i / bsz) * SEGSz::getDOSz(ns));
        }

        for (size_t i = 0U; i < extra; i++)
            trnew[i] = trnew[trnew.size()-extra+i] = magic;

        if (DOMD)
        {
            if (MOCK)
                EXPECT_CALL(*mock, read(SEGSz::getDOLoc<float>(offset, ns), SEGSz::getMDSz(), SEGSz::getDOSz(ns), nt, _))
                            .Times(Exactly(1)).WillOnce(SetArrayArgument<4>(tr.begin(), tr.end()));
            obj->readDOMD(offset, ns, nt, &trnew[extra]);
        }
        else
        {
            if (MOCK)
                EXPECT_CALL(*mock, read(SEGSz::getDODFLoc<float>(offset, ns), SEGSz::getDFSz(ns), SEGSz::getDOSz(ns), nt, _))
                            .Times(Exactly(1)).WillOnce(SetArrayArgument<4>(tr.begin(), tr.end()));
            obj->readDODF(offset, ns, nt, &trnew[extra]);
        }

        for (size_t i = 0U; i < tr.size(); i++)
            ASSERT_EQ(trnew[extra+i], getPattern(poff + i + (i / bsz) * SEGSz::getDOSz(ns)));

        for (size_t i = 0U; i < extra; i++)
        {
            ASSERT_EQ(trnew[i], magic);
            ASSERT_EQ(trnew[trnew.size()-extra+i], magic);
        }
    }

    template <bool DOMD, bool MOCK = true>
    void writeTest(csize_t offset, csize_t nt, csize_t ns, csize_t poff = 0, uchar magic = 0)
    {
        const size_t extra = 20U;
        size_t bsz = (DOMD ? SEGSz::getMDSz() : SEGSz::getDFSz(ns));
        size_t step = nt * bsz;
        std::vector<uchar> tr;
        std::vector<uchar> trnew(step + 2U*extra);

        if (MOCK)
        {
            tr.resize(step);
            for (size_t i = 0U; i < tr.size(); i++)
                trnew[i+extra] = tr[i] = getPattern(poff + i + (i / bsz) * SEGSz::getDOSz(ns));
        }
        else
            for (size_t i = 0U; i < tr.size(); i++)
                trnew[i+extra] = tr[i] = getPattern(poff + i + (i / bsz) * SEGSz::getDOSz(ns));

        for (size_t i = 0U; i < extra; i++)
            trnew[i] = trnew[trnew.size()-extra+i] = magic;

        if (DOMD)
        {
            if (MOCK)
                EXPECT_CALL(*mock, write(SEGSz::getDOLoc<float>(offset, ns), SEGSz::getMDSz(), SEGSz::getDOSz(ns), nt, _))
                              .Times(Exactly(1)).WillOnce(check4(tr, tr.size()));
            obj->writeDOMD(offset, ns, nt, &trnew[extra]);
        }
        else
        {
            if (MOCK)
                EXPECT_CALL(*mock, write(SEGSz::getDODFLoc<float>(offset, ns), SEGSz::getDFSz(ns), SEGSz::getDOSz(ns), nt, _))
                              .Times(Exactly(1)).WillOnce(check4(tr, tr.size()));
            obj->writeDODF(offset, ns, nt, &trnew[extra]);
        }

        if (!MOCK)
            readTest<DOMD, MOCK>(offset, nt, ns, poff, magic);
    }
};

typedef ObjSpecTest ObjIntegTest;

