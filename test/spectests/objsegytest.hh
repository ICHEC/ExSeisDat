#ifndef EXSEISDAT_TEST_SPECTESTS_OBJSEGYTEST_HH
#define EXSEISDAT_TEST_SPECTESTS_OBJSEGYTEST_HH

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "tglobal.hh"

#include "exseisdat/piol/CommunicatorMPI.hh"
#include "exseisdat/piol/ExSeis.hh"
#include "exseisdat/piol/ObjectInterface.hh"
#include "exseisdat/piol/ObjectSEGY.hh"
#include "exseisdat/piol/mpi/MPI_Binary_file.hh"
#include "exseisdat/piol/segy/utils.hh"
#include "exseisdat/utils/typedefs.hh"

#include "exseisdat/utils/encoding/number_encoding.hh"

#include <memory>
#include <string>

using namespace testing;
using namespace exseis::piol;

class MockData : public Binary_file {
  public:
    MOCK_CONST_METHOD0(get_file_size, size_t(void));

    MOCK_CONST_METHOD3(read, void(const size_t, const size_t, void*));
    MOCK_CONST_METHOD5(
        read_noncontiguous,
        void(const size_t, const size_t, const size_t, const size_t, void*));
    MOCK_CONST_METHOD4(
        read_noncontiguous_irregular,
        void(const size_t, const size_t, const size_t*, void*));

    MOCK_CONST_METHOD3(write, void(const size_t, const size_t, const void*));
    MOCK_CONST_METHOD5(
        write_noncontiguous,
        void(
            const size_t,
            const size_t,
            const size_t,
            const size_t,
            const void*));
    MOCK_CONST_METHOD4(
        write_noncontiguous_irregular,
        void(const size_t, const size_t, const size_t*, const void*));

    // TODO: This method is not tested
    MOCK_CONST_METHOD1(set_file_size, void(const size_t));
    MOCK_CONST_METHOD0(is_open, bool());
};

enum class Block { TRACE_METADATA, TRACE_DATA, TRACE };

class ObjTest : public Test {
  protected:
    std::shared_ptr<ExSeis> m_piol   = ExSeis::make();
    std::shared_ptr<MockData> m_mock = nullptr;
    ObjectInterface* m_obj           = nullptr;

    template<bool WRITE>
    void make_real_segy(std::string name)
    {
        if (m_obj != nullptr) {
            delete m_obj;
        }

        auto data = std::make_shared<MPI_Binary_file>(
            m_piol, name, (WRITE ? FileMode::ReadWrite : FileMode::Read));
        m_piol->assert_ok();
        m_obj = new ObjectSEGY(m_piol, name, data);
        m_piol->assert_ok();
    }

    std::string make_segy(std::string name = nonexistant_filename())
    {
        if (m_obj != nullptr) {
            delete m_obj;
        }
        m_mock = std::make_shared<MockData>();
        m_piol->assert_ok();
        m_obj = new ObjectSEGY(m_piol, name, m_mock);
        m_piol->assert_ok();

        return name;
    }

    ~ObjTest()
    {
        if (m_obj != nullptr) {
            delete m_obj;
        }
    }

    void segy_file_size_test(size_t sz)
    {
        EXPECT_CALL(*m_mock, get_file_size()).WillOnce(Return(sz));
        m_piol->assert_ok();
        EXPECT_EQ(sz, m_obj->get_file_size());
    }

    template<bool UseMock = true>
    void read_ho_pattern_test(size_t off, unsigned char magic)
    {
        const size_t extra = 20U;
        std::vector<unsigned char> c_ho;
        if (UseMock) {
            c_ho.resize(segy::segy_binary_file_header_size());
            for (size_t i = 0U; i < segy::segy_binary_file_header_size(); i++) {
                c_ho[i] = get_pattern(off + i);
            }
            EXPECT_CALL(
                *m_mock, read(0U, segy::segy_binary_file_header_size(), _))
                .WillOnce(WithArg<2>(Invoke([&](void* buffer) {
                    std::copy(
                        std::begin(c_ho), std::end(c_ho),
                        static_cast<unsigned char*>(buffer));
                })));
        }

        std::vector<unsigned char> ho(
            segy::segy_binary_file_header_size() + 2 * extra);
        for (auto i = 0U; i < extra; i++) {
            ho[i] = ho[ho.size() - extra + i] = magic;
        }

        m_obj->read_ho(&ho[extra]);

        m_piol->assert_ok();

        for (auto i = 0U; i < segy::segy_binary_file_header_size(); i++) {
            ASSERT_EQ(get_pattern(off + i), ho[extra + i]) << "Pattern " << i;
        }
        for (auto i = 0U; i < extra; i++) {
            ASSERT_EQ(magic, ho[ho.size() - extra + i])
                << "Pattern Extra " << i;
        }
    }

    template<bool UseMock = true>
    void should_write_file_header_pattern(size_t off, unsigned char magic)
    {
        if (UseMock && m_mock == nullptr) {
            std::cerr << "Using UseMock when not initialised: LOC: " << __LINE__
                      << std::endl;
            return;
        }
        const size_t extra = 20U;
        std::vector<unsigned char> c_ho(segy::segy_binary_file_header_size());
        for (size_t i = 0U; i < segy::segy_binary_file_header_size(); i++) {
            c_ho[i] = get_pattern(off + i);
        }

        if (UseMock) {
            EXPECT_CALL(
                *m_mock, write(0U, segy::segy_binary_file_header_size(), _))
                .WillOnce(
                    check2(c_ho.data(), segy::segy_binary_file_header_size()));
        }

        std::vector<unsigned char> ho(
            segy::segy_binary_file_header_size() + 2 * extra);
        for (auto i = 0U; i < extra; i++) {
            ho[i] = ho[ho.size() - extra + i] = magic;
        }

        for (auto i = 0U; i < segy::segy_binary_file_header_size(); i++) {
            ho[i + extra] = c_ho[i];
        }

        m_obj->should_write_file_header(&ho[extra]);
        m_piol->assert_ok();
        if (UseMock) {
            read_ho_pattern_test<UseMock>(off, magic);
        }
    }

    template<Block Type, bool UseMock = true>
    void read_test(
        const size_t offset,
        const size_t nt,
        const size_t ns,
        const size_t poff   = 0,
        unsigned char magic = 0)
    {
        SCOPED_TRACE("read_test " + std::to_string(size_t(Type)));
        if (UseMock && m_mock == nullptr) {
            std::cerr << "Using UseMock when not initialised: LOC: " << __LINE__
                      << std::endl;
            return;
        }
        const size_t extra = 20U;
        size_t bsz =
            (Type == Block::TRACE_METADATA ?
                 segy::segy_trace_header_size() :
                 (Type == Block::TRACE_DATA ? segy::segy_trace_data_size(ns) :
                                              segy::segy_trace_size(ns)));
        auto loc_func =
            (Type != Block::TRACE_DATA ? segy::segy_trace_location<float> :
                                         segy::segy_trace_data_location<float>);
        size_t step = nt * bsz;
        std::vector<unsigned char> trnew(step + 2U * extra);

        std::vector<unsigned char> tr;
        if (UseMock) {
            tr.resize(step);
            for (size_t i = 0U; i < nt; i++) {
                for (size_t j = 0U; j < bsz; j++) {
                    size_t pos      = poff + loc_func(offset + i, ns) + j;
                    tr[i * bsz + j] = get_pattern(pos % 0x100);
                }
            }
            if (Type == Block::TRACE) {
                EXPECT_CALL(*m_mock, read(loc_func(offset, ns), nt * bsz, _))
                    .WillOnce(WithArg<2>(Invoke([&](void* buffer) {
                        std::copy(
                            std::begin(tr), std::end(tr),
                            static_cast<unsigned char*>(buffer));
                    })));
            }
            else {
                EXPECT_CALL(
                    *m_mock, read_noncontiguous(
                                 loc_func(offset, ns), bsz,
                                 segy::segy_trace_size(ns), nt, _))
                    .WillOnce(WithArg<4>(Invoke([&](void* buffer) {
                        std::copy(
                            std::begin(tr), std::end(tr),
                            static_cast<unsigned char*>(buffer));
                    })));
            }
        }

        for (size_t i = 0U; i < extra; i++) {
            trnew[i] = trnew[trnew.size() - extra + i] = magic;
        }

        switch (Type) {
            case Block::TRACE_DATA:
                m_obj->read_trace_data(offset, ns, nt, &trnew[extra]);
                break;

            case Block::TRACE_METADATA:
                m_obj->read_trace_metadata(offset, ns, nt, &trnew[extra]);
                break;

            default:
            case Block::TRACE:
                m_obj->read_trace(offset, ns, nt, &trnew[extra]);
                break;
        }
        m_piol->assert_ok();

        size_t tcnt = 0;
        for (size_t i = 0U; i < nt; i++) {
            for (size_t j = 0U; j < bsz; j++, tcnt++) {
                size_t pos = poff + loc_func(offset + i, ns) + j;
                ASSERT_EQ(trnew[extra + i * bsz + j], get_pattern(pos % 0x100))
                    << i << " " << j;
            }
        }
        for (size_t i = 0U; i < extra; i++, tcnt += 2U) {
            ASSERT_EQ(trnew[i], magic);
            ASSERT_EQ(trnew[trnew.size() - extra + i], magic);
        }
        ASSERT_EQ(tcnt, step + 2U * extra);
    }

    template<Block Type, bool UseMock = true>
    void write_test(
        const size_t offset,
        const size_t nt,
        const size_t ns,
        const size_t poff   = 0,
        unsigned char magic = 0)
    {
        SCOPED_TRACE("write_test " + std::to_string(size_t(Type)));

        const size_t extra = 20U;
        size_t bsz =
            (Type == Block::TRACE_METADATA ?
                 segy::segy_trace_header_size() :
                 (Type == Block::TRACE_DATA ? segy::segy_trace_data_size(ns) :
                                              segy::segy_trace_size(ns)));
        auto loc_func =
            (Type != Block::TRACE_DATA ? segy::segy_trace_location<float> :
                                         segy::segy_trace_data_location<float>);

        size_t step = nt * bsz;
        std::vector<unsigned char> tr;
        std::vector<unsigned char> trnew(step + 2U * extra);

        if (UseMock) {
            tr.resize(step);
            for (size_t i = 0U; i < nt; i++) {
                for (size_t j = 0U; j < bsz; j++) {
                    size_t pos      = poff + loc_func(offset + i, ns) + j;
                    tr[i * bsz + j] = get_pattern(pos % 0x100);
                }
            }
            if (Type == Block::TRACE) {
                EXPECT_CALL(*m_mock, write(loc_func(offset, ns), nt * bsz, _))
                    .WillOnce(check2(tr.data(), tr.size()));
            }
            else {
                EXPECT_CALL(
                    *m_mock, write_noncontiguous(
                                 loc_func(offset, ns), bsz,
                                 segy::segy_trace_size(ns), nt, _))
                    .WillOnce(check4(tr.data(), tr.size()));
            }
        }
        for (size_t i = 0U; i < nt; i++) {
            for (size_t j = 0U; j < bsz; j++) {
                size_t pos = poff + loc_func(offset + i, ns) + j;
                trnew[extra + i * bsz + j] = get_pattern(pos % 0x100);
            }
        }

        for (size_t i = 0U; i < extra; i++) {
            trnew[i] = trnew[trnew.size() - extra + i] = magic;
        }

        switch (Type) {
            case Block::TRACE_DATA:
                m_obj->write_trace_data(offset, ns, nt, &trnew[extra]);
                break;

            case Block::TRACE_METADATA:
                m_obj->write_trace_metadata(offset, ns, nt, &trnew[extra]);
                break;

            default:
            case Block::TRACE:
                m_obj->write_trace(offset, ns, nt, &trnew[extra]);
                break;
        }

        if (!UseMock) {
            read_test<Type, UseMock>(offset, nt, ns, poff, magic);
        }
    }

    template<Block Type, bool UseMock = true>
    void read_random_test(
        const size_t ns,
        const std::vector<size_t>& offset,
        unsigned char magic = 0)
    {
        SCOPED_TRACE("read_random_test " + std::to_string(size_t(Type)));
        size_t nt = offset.size();
        if (UseMock && m_mock == nullptr) {
            std::cerr << "Using UseMock when not initialised: LOC: " << __LINE__
                      << std::endl;
            return;
        }
        const size_t extra = 20U;
        size_t bsz =
            (Type == Block::TRACE_METADATA ?
                 segy::segy_trace_header_size() :
                 (Type == Block::TRACE_DATA ? segy::segy_trace_data_size(ns) :
                                              segy::segy_trace_size(ns)));
        auto loc_func =
            (Type != Block::TRACE_DATA ? segy::segy_trace_location<float> :
                                         segy::segy_trace_data_location<float>);

        size_t step = nt * bsz;
        std::vector<unsigned char> trnew(step + 2U * extra);
        std::vector<unsigned char> tr;
        if (UseMock) {
            tr.resize(step);
            for (size_t i = 0U; i < nt; i++) {
                for (size_t j = 0U; j < bsz; j++) {
                    size_t pos      = loc_func(offset[i], ns) + j;
                    tr[i * bsz + j] = get_pattern(pos % 0x100);
                }
            }

            if (Type != Block::TRACE_DATA || bsz > 0) {
                EXPECT_CALL(
                    *m_mock, read_noncontiguous_irregular(bsz, nt, _, _))
                    .WillOnce(WithArg<3>(Invoke([&](void* buffer) {
                        std::copy(
                            std::begin(tr), std::end(tr),
                            static_cast<unsigned char*>(buffer));
                    })))
                    .RetiresOnSaturation();
            }
        }

        for (size_t i = 0U; i < extra; i++) {
            trnew[i] = trnew[trnew.size() - extra + i] = magic;
        }

        switch (Type) {
            case Block::TRACE_DATA:
                m_obj->read_trace_data(offset.data(), ns, nt, &trnew[extra]);
                break;

            case Block::TRACE_METADATA:
                m_obj->read_trace_metadata(
                    offset.data(), ns, nt, &trnew[extra]);
                break;

            default:
            case Block::TRACE:
                m_obj->read_trace(offset.data(), ns, nt, &trnew[extra]);
                break;
        }

        size_t tcnt = 0;
        for (size_t i = 0U; i < nt; i++) {
            for (size_t j = 0U; j < bsz; j++, tcnt++) {
                size_t pos = loc_func(offset[i], ns) + j;
                ASSERT_EQ(trnew[extra + i * bsz + j], get_pattern(pos % 0x100))
                    << i << " " << j;
            }
        }
        for (size_t i = 0U; i < extra; i++, tcnt += 2U) {
            ASSERT_EQ(trnew[i], magic);
            ASSERT_EQ(trnew[trnew.size() - extra + i], magic);
        }
        ASSERT_EQ(tcnt, step + 2U * extra);
    }

    template<Block Type, bool UseMock = true>
    void write_random_test(
        const size_t ns,
        const std::vector<size_t>& offset,
        unsigned char magic = 0)
    {
        SCOPED_TRACE("write_random_test " + std::to_string(size_t(Type)));
        size_t nt          = offset.size();
        const size_t extra = 20U;
        size_t bsz =
            (Type == Block::TRACE_METADATA ?
                 segy::segy_trace_header_size() :
                 (Type == Block::TRACE_DATA ? segy::segy_trace_data_size(ns) :
                                              segy::segy_trace_size(ns)));
        auto loc_func =
            (Type != Block::TRACE_DATA ? segy::segy_trace_location<float> :
                                         segy::segy_trace_data_location<float>);
        size_t step = nt * bsz;
        std::vector<unsigned char> tr;
        std::vector<unsigned char> trnew(step + 2U * extra);

        if (UseMock) {
            tr.resize(step);
            for (size_t i = 0U; i < nt; i++) {
                for (size_t j = 0U; j < bsz; j++) {
                    size_t pos      = loc_func(offset[i], ns) + j;
                    tr[i * bsz + j] = get_pattern(pos % 0x100);
                }
            }
            if (Type != Block::TRACE_DATA || bsz > 0) {
                EXPECT_CALL(
                    *m_mock, write_noncontiguous_irregular(bsz, nt, _, _))
                    .WillOnce(check3(tr.data(), step))
                    .RetiresOnSaturation();
            }
        }

        for (size_t i = 0U; i < nt; i++) {
            for (size_t j = 0U; j < bsz; j++) {
                size_t pos                 = loc_func(offset[i], ns) + j;
                trnew[extra + i * bsz + j] = get_pattern(pos % 0x100);
            }
        }
        for (size_t i = 0U; i < extra; i++) {
            trnew[i] = trnew[i + trnew.size() - extra] = magic;
        }

        switch (Type) {
            case Block::TRACE_DATA:
                m_obj->write_trace_data(offset.data(), ns, nt, &trnew[extra]);
                break;

            case Block::TRACE_METADATA:
                m_obj->write_trace_metadata(
                    offset.data(), ns, nt, &trnew[extra]);
                break;

            default:
            case Block::TRACE:
                m_obj->write_trace(offset.data(), ns, nt, &trnew[extra]);
                break;
        }
        if (!UseMock) {
            read_random_test<Type, UseMock>(ns, offset, magic);
        }
    }
};

class ObjSpecTest : public ObjTest {
  public:
    ObjSpecTest() : ObjTest() { make_segy(); }
};

typedef ObjTest ObjIntegTest;

#endif  // EXSEISDAT_TEST_SPECTESTS_OBJSEGYTEST_HH
