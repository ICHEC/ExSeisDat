#include <memory>
#include <string>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "cppfileapi.hh"
#include "global.hh"
#include "tglobal.hh"
#include "share/segy.hh"
#include "share/datatype.hh"
#include "anc/mpi.hh"
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
    MockData(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_) : Data::Interface(piol_, name_)
    {
    }

    MOCK_CONST_METHOD0(getFileSz, size_t(void));
    MOCK_CONST_METHOD3(read, void(csize_t, csize_t, uchar *));
    MOCK_CONST_METHOD5(read, void(csize_t, csize_t, csize_t, csize_t, uchar *));
    MOCK_CONST_METHOD4(read, void(csize_t, csize_t, csize_t *, uchar *));
    MOCK_CONST_METHOD3(write, void(csize_t, csize_t, const uchar *));
    MOCK_CONST_METHOD5(write, void(csize_t, csize_t, csize_t, csize_t, const uchar *));
    MOCK_CONST_METHOD4(write, void(csize_t, csize_t, csize_t *, const uchar *));
    // TODO: This method is not tested
    MOCK_CONST_METHOD1(setFileSz, void(csize_t));
};

enum class Block
{
    DOMD,
    DODF,
    DO
};

class ObjTest : public Test
{
    protected :
    std::shared_ptr<ExSeis> piol = ExSeis::New();
    std::shared_ptr<MockData> mock = nullptr;
    Obj::Interface * obj = nullptr;

    template <bool WRITE>
    void makeRealSEGY(std::string name)
    {
        if (obj != nullptr)
            delete obj;

        auto data = std::make_shared<Data::MPIIO>(piol, name, (WRITE ? FileMode::Test : FileMode::Read));
        piol->isErr();
        obj = new Obj::SEGY(piol, name, data, (WRITE ? FileMode::Test : FileMode::Read));
        piol->isErr();
    }

    void makeSEGY(std::string name = notFile)
    {
        if (obj != nullptr)
            delete obj;
        mock = std::make_shared<MockData>(piol, notFile);
        piol->isErr();
        obj = new Obj::SEGY(piol, name, mock);
        piol->isErr();
    }

    ~ObjTest()
    {
        if (obj != nullptr)
            delete obj;
    }

    void SEGYFileSizeTest(size_t sz)
    {
        EXPECT_CALL(*mock, getFileSz()).WillOnce(Return(sz));
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
                        .WillOnce(check2(cHo.data(), SEGSz::getHOSz()));

        std::vector<uchar> ho(SEGSz::getHOSz() + 2*extra);
        for (auto i = 0U; i < extra; i++)
            ho[i] = ho[ho.size()-extra+i] = magic;

        for (auto i = 0U; i < SEGSz::getHOSz(); i++)
            ho[i+extra] = cHo[i];

        obj->writeHO(&ho[extra]);
        piol->isErr();
        if (MOCK)
            readHOPatternTest<MOCK>(off, magic);
    }

    template <Block Type, bool MOCK = true>
    void readTest(csize_t offset, csize_t nt, csize_t ns, csize_t poff = 0, uchar magic = 0)
    {
        SCOPED_TRACE("readTest " + std::to_string(size_t(Type)));
        if (MOCK && mock == nullptr)
        {
            std::cerr << "Using Mock when not initialised: LOC: " << __LINE__ << std::endl;
            return;
        }
        const size_t extra = 20U;
        size_t bsz = (Type == Block::DOMD ? SEGSz::getMDSz() : (Type == Block::DODF ? SEGSz::getDFSz(ns) : SEGSz::getDOSz(ns)));
        auto locFunc = (Type != Block::DODF ? SEGSz::getDOLoc<float> : SEGSz::getDODFLoc<float>);
        size_t step = nt * bsz;
        std::vector<uchar> trnew(step + 2U*extra);

        std::vector<uchar> tr;
        if (MOCK)
        {
            tr.resize(step);
            for (size_t i = 0U; i < nt; i++)
                for (size_t j = 0U; j < bsz; j++)
                {
                    size_t pos = poff + locFunc(offset + i, ns) + j;
                    tr[i*bsz+j] = getPattern(pos % 0x100);
                }
            if (Type == Block::DO)
                EXPECT_CALL(*mock, read(locFunc(offset, ns), nt*bsz, _))
                        .WillOnce(SetArrayArgument<2>(tr.begin(), tr.end()));
            else
                EXPECT_CALL(*mock, read(locFunc(offset, ns), bsz, SEGSz::getDOSz(ns), nt, _))
                        .WillOnce(SetArrayArgument<4>(tr.begin(), tr.end()));
        }

        for (size_t i = 0U; i < extra; i++)
            trnew[i] = trnew[trnew.size()-extra+i] = magic;

        switch (Type)
        {
            case Block::DODF :
                obj->readDODF(offset, ns, nt, &trnew[extra]);
            break;
            case Block::DOMD :
                obj->readDOMD(offset, ns, nt, &trnew[extra]);
            break;
            default :
            case Block::DO :
                obj->readDO(offset, ns, nt, &trnew[extra]);
            break;
        }
        piol->isErr();

        size_t tcnt = 0;
        for (size_t i = 0U; i < nt; i++)
            for (size_t j = 0U; j < bsz; j++, tcnt++)
            {
                size_t pos = poff + locFunc(offset + i, ns) + j;
                ASSERT_EQ(trnew[extra+i*bsz+j], getPattern(pos % 0x100)) << i << " " << j;
            }
        for (size_t i = 0U; i < extra; i++, tcnt += 2U)
        {
            ASSERT_EQ(trnew[i], magic);
            ASSERT_EQ(trnew[trnew.size()-extra+i], magic);
        }
        ASSERT_EQ(tcnt, step + 2U*extra);
    }

    template <Block Type, bool MOCK = true>
    void writeTest(csize_t offset, csize_t nt, csize_t ns, csize_t poff = 0, uchar magic = 0)
    {
        SCOPED_TRACE("writeTest " + std::to_string(size_t(Type)));

        const size_t extra = 20U;
        size_t bsz = (Type == Block::DOMD ? SEGSz::getMDSz() : (Type == Block::DODF ? SEGSz::getDFSz(ns) : SEGSz::getDOSz(ns)));
        auto locFunc = (Type != Block::DODF ? SEGSz::getDOLoc<float> : SEGSz::getDODFLoc<float>);

        size_t step = nt * bsz;
        std::vector<uchar> tr;
        std::vector<uchar> trnew(step + 2U*extra);

        if (MOCK)
        {
            tr.resize(step);
            for (size_t i = 0U; i < nt; i++)
                for (size_t j = 0U; j < bsz; j++)
                {
                    size_t pos = poff + locFunc(offset + i, ns) + j;
                    tr[i*bsz+j] = getPattern(pos % 0x100);
                }
            if (Type == Block::DO)
                EXPECT_CALL(*mock, write(locFunc(offset, ns), nt*bsz, _))
                        .WillOnce(check2(tr, tr.size()));
            else
                EXPECT_CALL(*mock, write(locFunc(offset, ns), bsz, SEGSz::getDOSz(ns), nt, _))
                        .WillOnce(check4(tr, tr.size()));
        }
        for (size_t i = 0U; i < nt; i++)
            for (size_t j = 0U; j < bsz; j++)
            {
                size_t pos = poff + locFunc(offset + i, ns) + j;
                trnew[extra + i*bsz+j] = getPattern(pos % 0x100);
            }

        for (size_t i = 0U; i < extra; i++)
            trnew[i] = trnew[trnew.size()-extra+i] = magic;

        switch (Type)
        {
            case Block::DODF :
                obj->writeDODF(offset, ns, nt, &trnew[extra]);
            break;
            case Block::DOMD :
                obj->writeDOMD(offset, ns, nt, &trnew[extra]);
            break;
            default :
            case Block::DO :
                obj->writeDO(offset, ns, nt, &trnew[extra]);
            break;
        }

        if (!MOCK)
            readTest<Type, MOCK>(offset, nt, ns, poff, magic);
    }

    template <Block Type, bool MOCK = true>
    void readRandomTest(csize_t ns, const std::vector<size_t> & offset, uchar magic = 0)
    {
        SCOPED_TRACE("readRandomTest " + std::to_string(size_t(Type)));
        size_t nt = offset.size();
        if (MOCK && mock == nullptr)
        {
            std::cerr << "Using Mock when not initialised: LOC: " << __LINE__ << std::endl;
            return;
        }
        const size_t extra = 20U;
        size_t bsz = (Type == Block::DOMD ? SEGSz::getMDSz() : (Type == Block::DODF ? SEGSz::getDFSz(ns) : SEGSz::getDOSz(ns)));
        auto locFunc = (Type != Block::DODF ? SEGSz::getDOLoc<float> : SEGSz::getDODFLoc<float>);

        size_t step = nt * bsz;
        std::vector<uchar> trnew(step + 2U*extra);
        std::vector<uchar> tr;
        if (MOCK)
        {
            tr.resize(step);
            for (size_t i = 0U; i < nt; i++)
                for (size_t j = 0U; j < bsz; j++)
                {
                    size_t pos = locFunc(offset[i], ns) + j;
                    tr[i*bsz+j] = getPattern(pos % 0x100);
                }

            if (Type != Block::DODF || bsz > 0)
                EXPECT_CALL(*mock, read(bsz, nt, _, _))
                               .WillOnce(SetArrayArgument<3>(tr.begin(), tr.end()))
                               .RetiresOnSaturation();

        }

        for (size_t i = 0U; i < extra; i++)
            trnew[i] = trnew[trnew.size()-extra+i] = magic;

        switch (Type)
        {
            case Block::DODF :
                obj->readDODF(offset.data(), ns, nt, &trnew[extra]);
            break;
            case Block::DOMD :
                obj->readDOMD(offset.data(), ns, nt, &trnew[extra]);
            break;
            default :
            case Block::DO :
                obj->readDO(offset.data(), ns, nt, &trnew[extra]);
            break;
        }

        size_t tcnt = 0;
        for (size_t i = 0U; i < nt; i++)
            for (size_t j = 0U; j < bsz; j++, tcnt++)
            {
                size_t pos = locFunc(offset[i], ns) + j;
                ASSERT_EQ(trnew[extra+i*bsz+j], getPattern(pos % 0x100)) << i << " " << j;
            }
        for (size_t i = 0U; i < extra; i++, tcnt += 2U)
        {
            ASSERT_EQ(trnew[i], magic);
            ASSERT_EQ(trnew[trnew.size()-extra+i], magic);
        }
        ASSERT_EQ(tcnt, step + 2U*extra);
    }

    template <Block Type, bool MOCK = true>
    void writeRandomTest(csize_t ns, const std::vector<size_t> & offset, uchar magic = 0)
    {
        SCOPED_TRACE("writeRandomTest " + std::to_string(size_t(Type)));
        size_t nt = offset.size();
        const size_t extra = 20U;
        size_t bsz = (Type == Block::DOMD ? SEGSz::getMDSz() : (Type == Block::DODF ? SEGSz::getDFSz(ns) : SEGSz::getDOSz(ns)));
        auto locFunc = (Type != Block::DODF ? SEGSz::getDOLoc<float> : SEGSz::getDODFLoc<float>);
        size_t step = nt * bsz;
        std::vector<uchar> tr;
        std::vector<uchar> trnew(step + 2U*extra);

        if (MOCK)
        {
            tr.resize(step);
            for (size_t i = 0U; i < nt; i++)
                for (size_t j = 0U; j < bsz; j++)
                {
                    size_t pos = locFunc(offset[i], ns) + j;
                    tr[i*bsz+j] = getPattern(pos % 0x100);
                }
            if (Type != Block::DODF || bsz > 0)
                EXPECT_CALL(*mock, write(bsz, nt, _, _))
                               .WillOnce(check3(tr.data(), step))
                               .RetiresOnSaturation();
        }

        for (size_t i = 0U; i < nt; i++)
            for (size_t j = 0U; j < bsz; j++)
            {
                size_t pos = locFunc(offset[i], ns) + j;
                trnew[extra + i*bsz+j] = getPattern(pos % 0x100);
            }
        for (size_t i = 0U; i < extra; i++)
            trnew[i] = trnew[i+trnew.size()-extra] = magic;

        switch (Type)
        {
            case Block::DODF :
                obj->writeDODF(offset.data(), ns, nt, &trnew[extra]);
            break;
            case Block::DOMD :
                obj->writeDOMD(offset.data(), ns, nt, &trnew[extra]);
            break;
            default :
            case Block::DO :
                obj->writeDO(offset.data(), ns, nt, &trnew[extra]);
            break;
        }
        if (!MOCK)
            readRandomTest<Type, MOCK>(ns, offset, magic);
    }
};

class ObjSpecTest : public ObjTest
{
    public :
    ObjSpecTest() : ObjTest()
    {
        makeSEGY();
    }
};
typedef ObjTest ObjIntegTest;

