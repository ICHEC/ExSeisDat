#include "mockrule.hh"
#include "mockset.hh"
#include "setwraptests.hh"
#include "wraptesttools.hh"

#include "exseisdat/utils/signal_processing/Gain_function.h"
#include "exseisdat/utils/signal_processing/Gain_function.hh"

#include "exseisdat/utils/signal_processing/Taper_function.h"
#include "exseisdat/utils/signal_processing/Taper_function.hh"

using namespace testing;
using namespace exseis::utils;
using namespace exseis::piol;
using namespace exseis::flow;

void test_piol_set(
    std::shared_ptr<ExSeis*> piol,
    std::shared_ptr<Trace_metadata*> trace_metadata)
{
    auto set_ptr = std::make_shared<Set*>();

    EXPECT_CALL(mock_rule(), Rule_ctor(_, _, _, _, _));
    EXPECT_CALL(
        mock_set(), ctor(_, GetEqDeref(piol), "Test_Set_pattern*.segy", _, _))
        .WillOnce(SaveArg<0>(set_ptr));

    EXPECT_CALL(
        mock_set(), get_min_max(EqDeref(set_ptr), Meta::Copy, Meta::Copy, _))
        .WillOnce(DoAll(
            WithArg<3>(Invoke([](auto coord_elem) {
                EXPECT_FLOAT_EQ(coord_elem->val, 800.0);
                EXPECT_EQ(coord_elem->num, 810u);

                coord_elem->val = 820.0;
                coord_elem->num = 830.0;
            })),
            CheckInOutParam(CoordElem{820.0, 830})));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());

    const SortType sort_types[] = {
        SortType::SrcRcv,   SortType::SrcOff,  SortType::SrcROff,
        SortType::RcvOff,   SortType::RcvROff, SortType::LineOff,
        SortType::LineROff, SortType::OffLine, SortType::ROffLine};

    for (auto sort_type : sort_types) {
        EXPECT_CALL(mock_set(), sort(EqDeref(set_ptr), sort_type));
    }

    EXPECT_CALL(mock_set(), sort(EqDeref(set_ptr), Matcher<CompareP>(_)))
        .WillOnce(DoAll(
            CheckInOutParam(std::make_pair(840, 850)),
            WithArg<1>(Invoke([=](auto fn) {
                bool val = fn(**trace_metadata, 840, 850);
                EXPECT_TRUE(val);
            }))));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(mock_set(), sort(EqDeref(set_ptr), Matcher<CompareP>(_)))
        .WillOnce(DoAll(
            CheckInOutParam(std::make_pair(860, 870)),
            WithArg<1>(Invoke([=](auto fn) {
                bool val = fn(**trace_metadata, 860, 870);
                EXPECT_FALSE(val);
            }))));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());

    const std::pair<Taper_function, Taper_function> taper_types[] = {
        {exseis_linear_taper, linear_taper},
        {exseis_cosine_taper, cosine_taper},
        {exseis_cosine_square_taper, exseis_cosine_square_taper}};
    for (auto taper_type : taper_types) {
        EXPECT_CALL(
            mock_set(),
            taper(
                EqDeref(set_ptr), AnyOf(taper_type.first, taper_type.second),
                880, 890));
    }

    EXPECT_CALL(
        mock_set(), output(EqDeref(set_ptr), "Test_Set_output_name.segy"));

    EXPECT_CALL(mock_set(), text(EqDeref(set_ptr), "Test Set text"));

    EXPECT_CALL(mock_set(), summary(EqDeref(set_ptr)));

    EXPECT_CALL(mock_set(), add(EqDeref(set_ptr), "Test Set add"));

    typedef std::pair<Gain_function, Gain_function> Gain_function_pair;
    const Gain_function_pair agc_types[] = {
        std::make_pair(exseis_rectangular_rms_gain, rectangular_rms_gain),
        std::make_pair(exseis_triangular_rms_gain, triangular_rms_gain),
        std::make_pair(exseis_mean_abs_gain, mean_abs_gain),
        std::make_pair(exseis_median_gain, median_gain)};

    for (auto agc_type : agc_types) {
        EXPECT_CALL(
            mock_set(),
            agc(EqDeref(set_ptr), AnyOf(agc_type.first, agc_type.second), 900,
                910.0));
    }

    EXPECT_CALL(mock_set(), dtor(EqDeref(set_ptr)));
    EXPECT_CALL(mock_rule(), Rule_dtor(_));
}
