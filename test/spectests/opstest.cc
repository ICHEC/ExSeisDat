#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "tglobal.hh"

#include "segymdextra.hh"

#include "exseisdat/piol/configuration/ExSeis.hh"
#include "exseisdat/piol/operations/minmax.hh"
#include "exseisdat/piol/operations/sort_operations/sort.hh"
#include "exseisdat/utils/communicator/Communicator_mpi.hh"
#include "exseisdat/utils/signal_processing/AGC.h"
#include "exseisdat/utils/signal_processing/mute.h"
#include "exseisdat/utils/signal_processing/taper.h"
#include "exseisdat/utils/signal_processing/temporalfilter.hh"

using namespace testing;
using namespace exseis::piol;
using exseis::utils::Communicator_mpi;

struct OpsTest : public Test {
    std::shared_ptr<ExSeis> piol = ExSeis::make();
};

/*! Get the min and the max of a set of parameters passed. This is a parallel
 *  operation. It is the collective min and max across all processes (which also
 *  must all call this file).
 *  @param[in,out] piol The PIOL object
 *  @param[in] offset The starting trace number (local).
 *  @param[in] lnt The local number of traces to process.
 *  @param[in] coord
 *  @param[out] minmax An array of structures containing the minimum item.x,
 *                     maximum item.x, minimum item.y, maximum item.y and their
 *                     respective trace numbers.
 */
void get_min_max(
    ExSeisPIOL* piol,
    size_t offset,
    size_t lnt,
    const coord_t* coord,
    CoordElem* minmax)
{
    auto xlam = [](const coord_t& a) -> exseis::utils::Floating_point {
        return a.x;
    };
    auto ylam = [](const coord_t& a) -> exseis::utils::Floating_point {
        return a.y;
    };
    get_min_max<coord_t>(piol, offset, lnt, coord, xlam, ylam, minmax);
}

TEST_F(OpsTest, get_min_maxSimple)
{
    std::vector<coord_t> coord(1000);
    for (int i = 0; i < 1000; i++) {
        coord[i] = {1500. + i, 1300. - i};
    }

    std::vector<CoordElem> minmax(4);
    for (size_t offset = 0; offset < 300000; offset += 1 + offset * 10) {
        get_min_max(
            piol.get(), offset, coord.size(), coord.data(), minmax.data());
        piol->assert_ok();
        ASSERT_EQ(offset, minmax[0].num);
        ASSERT_EQ(offset + 999, minmax[1].num);
        ASSERT_EQ(offset + 999, minmax[2].num);
        ASSERT_EQ(offset, minmax[3].num);

        if (sizeof(exseis::utils::Floating_point) == sizeof(double)) {
            ASSERT_DOUBLE_EQ(minmax[0].val, 1500.);  // min x
            ASSERT_DOUBLE_EQ(minmax[1].val, 2499.);  // max x
            ASSERT_DOUBLE_EQ(minmax[2].val, 301.);   // min x
            ASSERT_DOUBLE_EQ(minmax[3].val, 1300.);  // max x
        }
        else {
            ASSERT_FLOAT_EQ(minmax[0].val, 1500.);  // min x
            ASSERT_FLOAT_EQ(minmax[1].val, 2499.);  // max x
            ASSERT_FLOAT_EQ(minmax[2].val, 301.);   // min x
            ASSERT_FLOAT_EQ(minmax[3].val, 1300.);  // max x
        }
    }
}

TEST_F(
    OpsTest,
    get_min_maxFail1)  // These fails won't surive a multi-processor example
{
    std::vector<coord_t> coord(1000);
    for (int i = 0; i < 1000; i++) {
        coord[i] = {1500. + i, 1300. - i};
    }

    std::vector<CoordElem> minmax(4);
    get_min_max(piol.get(), 10, coord.size(), coord.data(), NULL);
    piol->assert_ok();
}

TEST_F(
    OpsTest,
    get_min_maxFail2)  // These fails won't surive a multi-processor example
{
    std::vector<CoordElem> minmax(4);
    get_min_max(piol.get(), 10, 0, NULL, minmax.data());
    piol->assert_ok();
}

TEST_F(
    OpsTest,
    get_min_maxFail3)  // These fails won't surive a multi-processor example
{
    std::vector<coord_t> coord(1000);
    for (int i = 0; i < 1000; i++) {
        coord[i] = {1500. + i, 1300. - i};
    }

    std::vector<CoordElem> minmax(4);
    get_min_max(piol.get(), 10, 0, coord.data(), minmax.data());
    piol->assert_ok();
}

template<bool Y, bool Min>
void test_min_max(
    const std::vector<coord_t>& coord, const std::vector<CoordElem>& minmax)
{
    exseis::utils::Floating_point val =
        exseis::utils::Floating_point(Min ? 1 : -1)
        * std::numeric_limits<exseis::utils::Floating_point>::infinity();
    size_t tn = 0;
    for (size_t i = 0; i < coord.size(); i++) {
        double t = (Y ? coord[i].y : coord[i].x);
        if ((t < val && Min) || (t > val && !Min)) {
            val = t;
            tn  = i;
        }
    }

    if (sizeof(double) == sizeof(exseis::utils::Floating_point)) {
        ASSERT_DOUBLE_EQ(val, minmax[2U * size_t(Y) + size_t(!Min)].val);
    }
    else {
        ASSERT_FLOAT_EQ(val, minmax[2U * size_t(Y) + size_t(!Min)].val);
    }
    ASSERT_EQ(tn, minmax[2U * size_t(Y) + size_t(!Min)].num);
}

TEST_F(OpsTest, get_min_maxRand)
{
    srand(31337);
    std::vector<CoordElem> minmax(4);
    for (size_t l = 0; l < 20; l++) {
        size_t num = rand() % 1710880;
        std::vector<coord_t> coord(num);
        for (size_t i = 0; i < num; i++) {
            coord[i] = {1.0 * rand(), 1.0 * rand()};
        }

        get_min_max(piol.get(), 0, coord.size(), coord.data(), minmax.data());
        piol->assert_ok();
        test_min_max<false, false>(coord, minmax);
        test_min_max<false, true>(coord, minmax);
        test_min_max<true, false>(coord, minmax);
        test_min_max<true, true>(coord, minmax);
    }
}

TEST_F(OpsTest, SortSrcRcvBackwards)
{
    Trace_metadata prm(200);
    for (size_t i = 0; i < prm.size(); i++) {
        prm.set_floating_point(
            i, Trace_metadata_key::x_src,
            1000.0 - (prm.size() * piol->get_rank() + i) / 20);
        prm.set_floating_point(
            i, Trace_metadata_key::y_src,
            1000.0 - (prm.size() * piol->get_rank() + i) % 20);
        prm.set_floating_point(
            i, Trace_metadata_key::x_rcv,
            1000.0 - (prm.size() * piol->get_rank() + i) / 10);
        prm.set_floating_point(
            i, Trace_metadata_key::y_rcv,
            1000.0 - (prm.size() * piol->get_rank() + i) % 10);
        prm.set_index(
            i, Trace_metadata_key::gtn, prm.size() * piol->get_rank() + i);
    }
    auto list               = sort(piol.get(), Sort_type::SrcRcv, prm);
    size_t global_list_size = piol->comm->sum(list.size());
    for (size_t i = 0; i < list.size(); i++) {
        ASSERT_EQ(
            global_list_size - (prm.size() * piol->get_rank() + i) - 1,
            list[i]);
    }
}

TEST_F(OpsTest, SortSrcRcvForwards)
{
    Trace_metadata prm(200);
    for (size_t i = 0; i < prm.size(); i++) {
        prm.set_floating_point(
            i, Trace_metadata_key::x_src,
            1000.0 + (prm.size() * piol->get_rank() + i) / 20);
        prm.set_floating_point(
            i, Trace_metadata_key::y_src,
            1000.0 + (prm.size() * piol->get_rank() + i) % 20);
        prm.set_floating_point(
            i, Trace_metadata_key::x_rcv,
            1000.0 + (prm.size() * piol->get_rank() + i) / 10);
        prm.set_floating_point(
            i, Trace_metadata_key::y_rcv,
            1000.0 + (prm.size() * piol->get_rank() + i) % 10);
        prm.set_index(
            i, Trace_metadata_key::gtn, prm.size() * piol->get_rank() + i);
    }
    auto list = sort(piol.get(), Sort_type::SrcRcv, prm);

    for (size_t i = 0; i < list.size(); i++) {
        ASSERT_EQ(prm.size() * piol->get_rank() + i, list[i]);
    }
}

TEST_F(OpsTest, SortSrcRcvRand)
{
    Trace_metadata prm(10);
    for (size_t i = 0; i < prm.size(); i++) {
        prm.set_floating_point(i, Trace_metadata_key::y_src, 1000.0);
        prm.set_floating_point(i, Trace_metadata_key::x_rcv, 1000.0);
        prm.set_floating_point(i, Trace_metadata_key::y_rcv, 1000.0);
        prm.set_index(i, Trace_metadata_key::gtn, 10 * piol->get_rank() + i);
    }
    prm.set_floating_point(
        0, Trace_metadata_key::x_src, 10.0 * piol->get_rank() + 5.0);
    prm.set_floating_point(
        1, Trace_metadata_key::x_src, 10.0 * piol->get_rank() + 3.0);
    prm.set_floating_point(
        2, Trace_metadata_key::x_src, 10.0 * piol->get_rank() + 1.0);
    prm.set_floating_point(
        3, Trace_metadata_key::x_src, 10.0 * piol->get_rank() + 4.0);
    prm.set_floating_point(
        4, Trace_metadata_key::x_src, 10.0 * piol->get_rank() + 2.0);
    prm.set_floating_point(
        5, Trace_metadata_key::x_src, 10.0 * piol->get_rank() + 9.0);
    prm.set_floating_point(
        6, Trace_metadata_key::x_src, 10.0 * piol->get_rank() + 6.0);
    prm.set_floating_point(
        7, Trace_metadata_key::x_src, 10.0 * piol->get_rank() + 8.0);
    prm.set_floating_point(
        8, Trace_metadata_key::x_src, 10.0 * piol->get_rank() + 7.0);
    prm.set_floating_point(
        9, Trace_metadata_key::x_src, 10.0 * piol->get_rank() + 0.0);

    auto list = sort(piol.get(), Sort_type::SrcRcv, prm);

    ASSERT_EQ(10 * piol->get_rank() + 5U, list[0]);
    ASSERT_EQ(10 * piol->get_rank() + 3U, list[1]);
    ASSERT_EQ(10 * piol->get_rank() + 1U, list[2]);
    ASSERT_EQ(10 * piol->get_rank() + 4U, list[3]);
    ASSERT_EQ(10 * piol->get_rank() + 2U, list[4]);
    ASSERT_EQ(10 * piol->get_rank() + 9U, list[5]);
    ASSERT_EQ(10 * piol->get_rank() + 6U, list[6]);
    ASSERT_EQ(10 * piol->get_rank() + 8U, list[7]);
    ASSERT_EQ(10 * piol->get_rank() + 7U, list[8]);
    ASSERT_EQ(10 * piol->get_rank() + 0U, list[9]);
}

TEST_F(OpsTest, FilterCheckLowpass)
{
    size_t n = 4;
    std::vector<exseis::utils::Trace_value> denom_calc(n + 1);
    std::vector<exseis::utils::Trace_value> numer_calc(n + 1);
    make_filter(
        exseis::utils::FltrType::Lowpass, numer_calc.data(), denom_calc.data(),
        n, 30.0, 1.2, 0.0);

    std::vector<exseis::utils::Trace_value> denom_ref = {
        1, -3.34406784, 4.23886395, -2.40934286, 0.5174782};

    std::vector<exseis::utils::Trace_value> numer_ref = {
        0.00018321611, 0.00073286443, 0.0010992966, 0.00073286443,
        0.00018321611};

    for (size_t i = 0; i < n + 1; i++) {
        EXPECT_FLOAT_EQ(denom_ref[i], denom_calc[i]);
        EXPECT_FLOAT_EQ(numer_ref[i], numer_calc[i]);
    }
}

TEST_F(OpsTest, FilterCheckHighpass)
{
    size_t n = 4;
    std::vector<exseis::utils::Trace_value> denom_calc(n + 1);
    std::vector<exseis::utils::Trace_value> numer_calc(n + 1);
    make_filter(
        exseis::utils::FltrType::Highpass, numer_calc.data(), denom_calc.data(),
        n, 30, 1.2, 0);

    std::vector<exseis::utils::Trace_value> denom_ref = {
        1, -3.34406784, 4.23886395, -2.40934286, 0.5174782};
    std::vector<exseis::utils::Trace_value> numer_ref = {
        0.71935955, -2.87743821, 4.31615732, -2.87743821, 0.71935955};
    for (size_t i = 0; i < n + 1; i++) {
        EXPECT_FLOAT_EQ(denom_ref[i], denom_calc[i]);
        EXPECT_FLOAT_EQ(numer_ref[i], numer_calc[i]);
    }
}

TEST_F(OpsTest, FilterCheckBandpass)
{
    size_t n = 4;
    std::vector<exseis::utils::Trace_value> denom_calc(2 * n + 1);
    std::vector<exseis::utils::Trace_value> numer_calc(2 * n + 1);
    make_filter(
        exseis::utils::FltrType::Bandpass, numer_calc.data(), denom_calc.data(),
        n, 30, 1.2, 6.5);
    std::vector<exseis::utils::Trace_value> denom_ref = {
        1.0,         -4.19317484, 8.01505053, -9.44595842, 7.69031281,
        -4.39670663, 1.68365361,  -0.3953309, 0.04619144};
    std::vector<exseis::utils::Trace_value> numer_ref = {
        0.03142168, 0.0,        -0.1256867, 0.0,       0.18853005,
        0.0,        -0.1256867, 0.0,        0.03142168};
    for (size_t i = 0; i < 2 * n + 1; i++) {
        EXPECT_FLOAT_EQ(denom_ref[i], denom_calc[i]);
        EXPECT_FLOAT_EQ(numer_ref[i], numer_calc[i]);
    }
}

TEST_F(OpsTest, FilterCheckBandstop)
{
    size_t n = 4;
    std::vector<exseis::utils::Trace_value> denom_calc(2 * n + 1);
    std::vector<exseis::utils::Trace_value> numer_calc(2 * n + 1);
    make_filter(
        exseis::utils::FltrType::Bandstop, numer_calc.data(), denom_calc.data(),
        n, exseis::utils::Trace_value(30), exseis::utils::Trace_value(1.2),
        exseis::utils::Trace_value(6.5));
    std::vector<exseis::utils::Trace_value> denom_ref = {
        1.0,         -4.19317484, 8.01505053, -9.44595842, 7.69031281,
        -4.39670663, 1.68365361,  -0.3953309, 0.04619144};
    std::vector<exseis::utils::Trace_value> numer_ref = {
        0.21261259,  -1.38519468, 4.23471188,  -7.83039071, 9.54055945,
        -7.83039071, 4.23471188,  -1.38519468, 0.21261259};
    for (size_t i = 0; i < 2 * n + 1; i++) {
        EXPECT_FLOAT_EQ(denom_ref[i], denom_calc[i]);
        EXPECT_NEAR(numer_ref[i], numer_calc[i], 5e-6);
    }
}
