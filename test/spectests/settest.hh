#ifndef EXSEISDAT_TEST_SPECTESTS_SETTEST_HH
#define EXSEISDAT_TEST_SPECTESTS_SETTEST_HH

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "tglobal.hh"

#include "exseisdat/flow/Set.hh"
#include "exseisdat/piol/ExSeis.hh"
#include "exseisdat/piol/ObjectSEGY.hh"
#include "exseisdat/piol/ReadSEGY.hh"
#include "exseisdat/piol/WriteSEGY.hh"
#include "exseisdat/piol/mpi/MPI_Binary_file.hh"
#include "exseisdat/utils/decomposition/block_decomposition.hh"

#include <numeric>

using namespace testing;
using namespace exseis::utils;
using namespace exseis::piol;
using namespace exseis::flow;
using namespace exseis::flow::detail;

class MockFile : public ReadInterface {
  public:
    MOCK_CONST_METHOD0(file_name, const std::string&(void));
    MOCK_METHOD0(read_file_headers, void(void));

    MOCK_CONST_METHOD0(read_text, const std::string&(void));
    MOCK_CONST_METHOD0(read_ns, size_t(void));
    MOCK_CONST_METHOD0(read_nt, size_t(void));
    MOCK_CONST_METHOD0(
        read_sample_interval, exseis::utils::Floating_point(void));

    MOCK_CONST_METHOD4(
        read_param,
        void(const size_t, const size_t, Trace_metadata*, const size_t));

    MOCK_CONST_METHOD4(
        read_param_non_contiguous,
        void(const size_t, const size_t*, Trace_metadata*, const size_t));

    MOCK_CONST_METHOD5(
        read_trace,
        void(
            const size_t,
            const size_t,
            exseis::utils::Trace_value*,
            Trace_metadata*,
            const size_t));

    MOCK_CONST_METHOD5(
        read_trace_non_contiguous,
        void(
            const size_t,
            const size_t*,
            exseis::utils::Trace_value*,
            Trace_metadata*,
            const size_t));

    MOCK_CONST_METHOD5(
        read_trace_non_monotonic,
        void(
            const size_t,
            const size_t*,
            exseis::utils::Trace_value*,
            Trace_metadata*,
            const size_t));
};

ACTION_P(cpyprm, src)
{
    *arg2 = *src;
}

void muting(size_t nt, size_t ns, exseis::utils::Trace_value* trc, size_t mute);

void mute_man(
    size_t nt,
    size_t ns,
    exseis::utils::Trace_value* trc,
    Taper_function func,
    size_t mute_size_at_begin,
    size_t taper_size_at_begin,
    size_t taper_size_at_end,
    size_t mute_size_at_end);

void taper_man(
    size_t nt,
    size_t ns,
    exseis::utils::Trace_value* trc,
    Taper_function func,
    size_t taper_size_at_begin,
    size_t taper_size_at_end);


template<class T>
std::shared_ptr<T> make_test(std::shared_ptr<ExSeisPIOL> piol, std::string name)
{
    return std::make_shared<T>(piol, name);
}

struct Set_public : public Set {
    using Set::Set;

    using Set::calc_func;
    using Set::m_file;
    using Set::m_func;
    using Set::m_outfix;
};

struct SetTest : public Test {
    std::shared_ptr<ExSeis> piol    = ExSeis::make();
    std::unique_ptr<Set_public> set = nullptr;
    std::deque<Trace_metadata> prm;
    CommunicatorMPI::Opt opt;

    void init(
        size_t num_file, size_t num_ns, size_t num_inc, size_t, bool linear)
    {
        if (set.get() != nullptr) {
            set.release();
        }
        set = std::make_unique<Set_public>(piol);
        for (size_t j = 0; j < num_ns; j++) {
            for (size_t k = 0; k < num_inc; k++) {
                for (size_t i = 0; i < num_file; i++) {
                    auto mock = std::make_unique<MockFile>();
                    size_t nt = 1000U + i;
                    EXPECT_CALL(*mock, read_nt()).WillRepeatedly(Return(nt));
                    EXPECT_CALL(*mock, read_ns())
                        .WillRepeatedly(Return(1000U + i));
                    EXPECT_CALL(*mock, read_sample_interval())
                        .WillRepeatedly(
                            Return(1000. + exseis::utils::Floating_point(i)));

                    auto dec = block_decomposition(
                        nt, piol->comm->get_num_rank(), piol->comm->get_rank());
                    prm.emplace_back(dec.local_size);
                    Trace_metadata& tprm = prm.back();

                    if (linear) {
                        for (size_t l = 0; l < dec.local_size; l++) {
                            tprm.set_floating_point(
                                l, Meta::x_src,
                                2000.
                                    - exseis::utils::Floating_point(
                                        dec.global_offset + l));
                            tprm.set_floating_point(
                                l, Meta::y_src,
                                2000.
                                    - exseis::utils::Floating_point(
                                        dec.global_offset + l));
                            tprm.set_floating_point(
                                l, Meta::x_rcv,
                                2000.
                                    + exseis::utils::Floating_point(
                                        dec.global_offset + l));
                            tprm.set_floating_point(
                                l, Meta::y_rcv,
                                2000.
                                    + exseis::utils::Floating_point(
                                        dec.global_offset + l));
                            tprm.set_floating_point(
                                l, Meta::xCmp,
                                2000.
                                    - exseis::utils::Floating_point(
                                        dec.global_offset + l));
                            tprm.set_floating_point(
                                l, Meta::yCmp,
                                2000.
                                    - exseis::utils::Floating_point(
                                        dec.global_offset + l));
                            tprm.set_integer(
                                l, Meta::il, 2000U + dec.global_offset + l);
                            tprm.set_integer(
                                l, Meta::xl, 2000U + dec.global_offset + l);
                            tprm.set_integer(
                                l, Meta::tn, l + dec.global_offset);

                            auto x_s =
                                2000.
                                - exseis::utils::Floating_point(
                                    (dec.global_offset + l) % (nt / 10U));
                            auto y_r =
                                2000.
                                + exseis::utils::Floating_point(
                                    (dec.global_offset + l) / (nt / 10U));
                            tprm.set_integer(
                                l, Meta::Offset,
                                (x_s - 2000.) * (x_s - 2000.)
                                    + (2000. - y_r) * (2000. - y_r));
                        }
                    }
                    else {
                        for (size_t l = 0; l < dec.local_size; l++) {
                            auto x_s =
                                2000U - (dec.global_offset + l) % (nt / 10U);
                            auto y_r =
                                2000U + (dec.global_offset + l) / (nt / 10U);
                            tprm.set_floating_point(l, Meta::x_src, x_s);
                            tprm.set_floating_point(l, Meta::y_src, 2000.);
                            tprm.set_floating_point(l, Meta::x_rcv, 2000.);
                            tprm.set_floating_point(l, Meta::y_rcv, y_r);
                            tprm.set_floating_point(l, Meta::xCmp, x_s);
                            tprm.set_floating_point(l, Meta::yCmp, y_r);
                            tprm.set_integer(l, Meta::il, 4000U - x_s);
                            tprm.set_integer(l, Meta::xl, y_r);
                            tprm.set_integer(
                                l, Meta::tn, l + dec.global_offset);
                            tprm.set_integer(
                                l, Meta::Offset,
                                (x_s - 2000.) * (x_s - 2000.)
                                    + (2000. - y_r) * (2000. - y_r));
                        }
                    }
                    EXPECT_CALL(
                        *mock, read_param_non_contiguous(
                                   dec.local_size, An<const size_t*>(), _, 0))
                        .Times(Exactly(1U))
                        .WillRepeatedly(cpyprm(&prm.back()));

                    set->add(std::move(mock));
                }
            }
        }
    }

    void init(size_t num_file, size_t nt, size_t)
    {
        srand(1337);
        set.reset(new Set_public(piol));
        for (size_t i = 0; i < num_file; i++) {
            auto mock = std::make_unique<MockFile>();
            EXPECT_CALL(*mock, read_nt()).WillRepeatedly(Return(nt));
            EXPECT_CALL(*mock, read_ns()).WillRepeatedly(Return(1000U));
            EXPECT_CALL(*mock, read_sample_interval())
                .WillRepeatedly(Return(1000.));

            set->add(std::move(mock));

            // exseis::utils::Integer range = (nt / inactive);
            //
            // #warning Randomly delete traces from the lists
            // for (size_t j = 0; j < inactive; j++)
            // {
            //     size_t randj = range*j + (rand() % range);
            //     set->file[i]->lst[randj] = NOT_IN_OUTPUT;
            // }
        }
    }

    void mute_test(
        size_t nt,
        size_t ns,
        Taper_function trace_function,
        size_t mute_size_at_begin,
        size_t taper_size_at_begin,
        size_t taper_size_at_end,
        size_t mute_size_at_end)
    {

        set.reset(new Set_public(piol));

        auto mock = std::make_unique<MockFile>();
        std::vector<exseis::utils::Trace_value> trc(nt * ns);
        std::vector<exseis::utils::Trace_value> trc_man(nt * ns);

        Trace_metadata trace_metadata(nt);

        std::fill(trc.begin(), trc.end(), 1.0f);

        trc_man = trc;
        std::vector<size_t> offsets(nt);
        std::iota(offsets.begin(), offsets.end(), 0U);


        EXPECT_CALL(*mock, read_nt()).WillRepeatedly(Return(nt));
        EXPECT_CALL(*mock, read_ns()).WillRepeatedly(Return(ns));
        EXPECT_CALL(*mock, read_sample_interval())
            .WillRepeatedly(Return(0.004));

        EXPECT_CALL(
            *mock, read_trace_non_contiguous(
                       _, A<const size_t*>(), A<exseis::utils::Trace_value*>(),
                       A<Trace_metadata*>(), 0U))
            .Times(Exactly(1U))
            .WillRepeatedly(WithArgs<0, 1, 2>(
                Invoke([trc = trc, ns = ns](
                           size_t nt, const size_t* offsets,
                           exseis::utils::Trace_value* trace_buffer) {
                    int rank = -1;
                    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
                    for (size_t i = 0; i < nt; i++) {
                        for (size_t j = 0; j < ns; j++) {
                            trace_buffer[i * ns + j] = trc[offsets[i] * ns + j];
                        }
                    }
                })));
        set->add(std::move(mock));

        set->mute(
            trace_function, mute_size_at_begin, taper_size_at_begin,
            taper_size_at_end, mute_size_at_end);

        auto filename      = temp_file_segy();
        auto base_filename = filename.substr(0, filename.find_last_of("."));

        set->m_outfix = base_filename;
        set.reset();

        piol->barrier();

        std::string name = filename;
        auto in          = make_test<ReadSEGY>(piol, name);
        in->read_trace(0U, in->read_nt(), trc.data());

        mute_man(
            nt, ns, trc_man.data(), trace_function, mute_size_at_begin,
            taper_size_at_begin, taper_size_at_end, mute_size_at_end);


        for (size_t i = 0; i < nt; i++) {
            for (size_t j = 0; j < ns; j++) {
                EXPECT_FLOAT_EQ(trc[i * ns + j], trc_man[i * ns + j]);
            }
        }
    }


    void taper_test(
        size_t nt,
        size_t ns,
        Taper_function correct_trace_function,
        Taper_function test_trace_function,
        size_t taper_size_at_begin,
        size_t taper_size_at_end)
    {
        set.reset(new Set_public(piol));
        auto mock = std::make_unique<MockFile>();

        std::vector<exseis::utils::Trace_value> trc(nt * ns);
        std::vector<exseis::utils::Trace_value> trc_man(nt * ns);
        std::fill(trc.begin(), trc.end(), 1.0f);
        trc_man = trc;

        EXPECT_CALL(*mock, read_nt()).WillRepeatedly(Return(nt));
        EXPECT_CALL(*mock, read_ns()).WillRepeatedly(Return(ns));
        EXPECT_CALL(*mock, read_sample_interval())
            .WillRepeatedly(Return(0.004));

        EXPECT_CALL(
            *mock, read_trace_non_contiguous(
                       _, A<const size_t*>(), A<exseis::utils::Trace_value*>(),
                       A<Trace_metadata*>(), 0U))
            .Times(Exactly(1U))
            .WillRepeatedly(WithArgs<0, 1, 2>(
                Invoke([trc = trc, ns = ns](
                           size_t nt, const size_t* offsets,
                           exseis::utils::Trace_value* trace_buffer) {
                    int rank = -1;
                    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
                    for (size_t i = 0; i < nt; i++) {
                        for (size_t j = 0; j < ns; j++) {
                            trace_buffer[i * ns + j] = trc[offsets[i] * ns + j];
                        }
                    }
                })));
        set->add(std::move(mock));

        set->taper(test_trace_function, taper_size_at_begin, taper_size_at_end);

        auto filename      = temp_file_segy();
        auto base_filename = filename.substr(0, filename.find_last_of("."));

        set->m_outfix = base_filename;
        set.reset();

        piol->barrier();

        std::string name = filename;
        auto in          = make_test<ReadSEGY>(piol, name);
        ASSERT_EQ(nt, in->read_nt());
        in->read_trace(0U, in->read_nt(), trc.data());

        taper_man(
            nt, ns, trc_man.data(), correct_trace_function, taper_size_at_begin,
            taper_size_at_end);

        for (size_t i = 0; i < nt; i++) {
            for (size_t j = 0; j < ns; j++) {
                EXPECT_FLOAT_EQ(trc[i * ns + j], trc_man[i * ns + j]);
            }
        }
    }

    void agc_test(
        size_t nt,
        size_t ns,
        Gain_function type,
        std::function<exseis::utils::Trace_value(
            size_t, exseis::utils::Trace_value*, size_t)> agc_func,
        size_t window,
        exseis::utils::Trace_value target_amplitude)
    {
        set.reset(new Set_public(piol));
        auto mock = std::make_unique<MockFile>();

        std::vector<exseis::utils::Trace_value> trc(nt * ns);
        std::vector<exseis::utils::Trace_value> trc_man(nt * ns);

        for (size_t i = 0; i < nt; i++) {
            for (size_t j = 0; j < ns; j++) {
                trc[i * ns + j] = j;  //*pow(-1.0f,j);
            }
        }

        trc_man = trc;

        EXPECT_CALL(*mock, read_nt()).WillRepeatedly(Return(nt));
        EXPECT_CALL(*mock, read_ns()).WillRepeatedly(Return(ns));
        EXPECT_CALL(*mock, read_sample_interval())
            .WillRepeatedly(Return(0.004));

        EXPECT_CALL(
            *mock, read_trace_non_contiguous(
                       _, A<const size_t*>(), A<exseis::utils::Trace_value*>(),
                       A<Trace_metadata*>(), 0U))
            .WillRepeatedly(WithArgs<0, 1, 2>(
                Invoke([trc = trc, ns = ns](
                           size_t nt, const size_t* offsets,
                           exseis::utils::Trace_value* trace_buffer) {
                    for (size_t i = 0; i < nt; i++) {
                        for (size_t j = 0; j < ns; j++) {
                            trace_buffer[i * ns + j] = trc[offsets[i] * ns + j];
                        }
                    }
                })));
        set->add(std::move(mock));

        set->agc(type, window, target_amplitude);

        auto filename      = temp_file_segy();
        auto base_filename = filename.substr(0, filename.find_last_of("."));

        set->m_outfix = base_filename;
        set.reset();

        std::string name = filename;

        auto in = make_test<ReadSEGY>(piol, name);
        in->read_trace(0U, in->read_nt(), trc.data());

        size_t win;
        size_t win_str;
        size_t win_cntr;
        for (size_t i = 0; i < nt; i++) {
            for (size_t j = 0; j < ns; j++) {
                if (j < (window / 2U) + 1) {
                    win      = j + 1 + (window / 2U);
                    win_str  = i * ns;
                    win_cntr = j;
                }
                else if ((ns - j) < (window / 2U) + 1) {
                    win      = ns - j + (window / 2U);
                    win_str  = i * ns + j - window / 2U;
                    win_cntr = window / 2U;
                }
                else {
                    win      = window;
                    win_str  = i * ns + j - window / 2U;
                    win_cntr = window / 2U;
                }
                std::vector<exseis::utils::Trace_value> trc_win(
                    trc_man.begin() + win_str, trc_man.begin() + win_str + win);
                ASSERT_FLOAT_EQ(
                    trc[i * ns + j],
                    trc_man[i * ns + j] * target_amplitude
                        / agc_func(win, trc_win.data(), win_cntr))
                    << i << " " << j << std::endl;
            }
        }
    }

    void filter_test(
        FltrType type,
        FltrDmn domain,
        std::vector<exseis::utils::Trace_value> corners,
        const std::vector<exseis::utils::Trace_value>& trc_ref,
        size_t nt = 1LU)
    {
        ASSERT_EQ(trc_ref.size(), static_cast<size_t>(59));
        exseis::utils::Trace_value pi = std::acos(-1);
        size_t n                      = 3;
        size_t ns                     = trc_ref.size() / nt;
        set.reset(new Set_public(piol));

        std::vector<exseis::utils::Trace_value> trc(trc_ref.size());

        for (size_t i = 0; i < nt; i++) {
            for (size_t j = 0; j < ns; j++) {
                trc[i * ns + j] =
                    std::sin(
                        exseis::utils::Trace_value(4.8) * pi
                        * (exseis::utils::Trace_value(j))
                        / exseis::utils::Trace_value(ns))
                    + exseis::utils::Trace_value(1.5)
                          * std::cos(
                              36 * pi * (exseis::utils::Trace_value(j))
                              / exseis::utils::Trace_value(ns))
                    + exseis::utils::Trace_value(0.5)
                          * std::sin(
                              48 * pi * (exseis::utils::Trace_value(j))
                              / exseis::utils::Trace_value(ns));
            }
        }


        set.reset(new Set_public(piol));
        auto mock = std::make_unique<MockFile>();

        EXPECT_CALL(*mock, read_nt()).WillRepeatedly(Return(nt));
        EXPECT_CALL(*mock, read_ns()).WillRepeatedly(Return(ns));
        EXPECT_CALL(*mock, read_sample_interval())
            .WillRepeatedly(Return(0.004));

        EXPECT_CALL(
            *mock, read_trace_non_contiguous(
                       _, A<const size_t*>(), A<exseis::utils::Trace_value*>(),
                       A<Trace_metadata*>(), 0U))
            .WillRepeatedly(WithArgs<0, 1, 2>(
                Invoke([trc = trc, ns = ns](
                           size_t nt, const size_t* offsets,
                           exseis::utils::Trace_value* trace_buffer) {
                    for (size_t i = 0; i < nt; i++) {
                        for (size_t j = 0; j < ns; j++) {
                            trace_buffer[i * ns + j] = trc[offsets[i] * ns + j];
                        }
                    }
                })));
        set->add(std::move(mock));

        set->temporal_filter(type, domain, PadType::Zero, 30, n, corners);

        auto filename      = temp_file_segy();
        auto base_filename = filename.substr(0, filename.find_last_of("."));

        set->m_outfix = base_filename;
        set.reset();

        std::string name = filename;

        auto in = make_test<ReadSEGY>(piol, name);

        in->read_trace(0U, in->read_nt(), trc.data());

        for (size_t i = 0; i < nt * ns; i++) {
            ASSERT_NEAR(
                trc[i], trc_ref[i],
                (__GNUC__ ? exseis::utils::Trace_value(0.00011) :
                            exseis::utils::Trace_value(.00001)));
        }
    }
};

#endif  // EXSEISDAT_TEST_SPECTESTS_SETTEST_HH
