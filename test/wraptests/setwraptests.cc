#include "mockrule.hh"
#include "mockset.hh"
#include "setwraptests.hh"
#include "wraptesttools.hh"

using namespace testing;
using namespace exseis::utils;
using namespace exseis::PIOL;
using namespace exseis::Flow;

void test_PIOL_Set(std::shared_ptr<ExSeis*> piol)
{
    auto set_ptr = std::make_shared<Set*>();

    EXPECT_CALL(mockRule(), ctor(_, _, _, _, _));
    EXPECT_CALL(
      mockSet(), ctor(_, GetEqDeref(piol), "Test_Set_pattern*.segy", _, _))
      .WillOnce(SaveArg<0>(set_ptr));

    EXPECT_CALL(
      mockSet(), getMinMax(EqDeref(set_ptr), PIOL_META_COPY, PIOL_META_COPY, _))
      .WillOnce(DoAll(
        WithArg<3>(Invoke([](auto coordElem) {
            EXPECT_FLOAT_EQ(coordElem->val, 800.0);
            EXPECT_EQ(coordElem->num, 810u);

            coordElem->val = 820.0;
            coordElem->num = 830.0;
        })),
        CheckInOutParam(CoordElem{820.0, 830})));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    const SortType sort_types[] = {
      PIOL_SORTTYPE_SrcRcv,   PIOL_SORTTYPE_SrcOff,  PIOL_SORTTYPE_SrcROff,
      PIOL_SORTTYPE_RcvOff,   PIOL_SORTTYPE_RcvROff, PIOL_SORTTYPE_LineOff,
      PIOL_SORTTYPE_LineROff, PIOL_SORTTYPE_OffLine, PIOL_SORTTYPE_ROffLine};

    for (auto sort_type : sort_types) {
        EXPECT_CALL(mockSet(), sort(EqDeref(set_ptr), sort_type));
    }

    EXPECT_CALL(mockSet(), sort(EqDeref(set_ptr), Matcher<CompareP>(_)))
      .WillOnce(DoAll(
        CheckInOutParam(std::make_pair(840, 850)),
        WithArg<1>(Invoke([](auto fn) {
            bool val = fn(NULL, 840, 850);
            EXPECT_TRUE(val);
        }))));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(mockSet(), sort(EqDeref(set_ptr), Matcher<CompareP>(_)))
      .WillOnce(DoAll(
        CheckInOutParam(std::make_pair(860, 870)),
        WithArg<1>(Invoke([](auto fn) {
            bool val = fn(NULL, 860, 870);
            EXPECT_FALSE(val);
        }))));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    const std::pair<Taper_function, Taper_function> taper_types[] = {
      {exseis_linear_taper, linear_taper},
      {exseis_cosine_taper, cosine_taper},
      {exseis_cosine_square_taper, exseis_cosine_square_taper}};
    for (auto taper_type : taper_types) {
        EXPECT_CALL(
          mockSet(), taper(
                       EqDeref(set_ptr),
                       AnyOf(taper_type.first, taper_type.second), 880, 890));
    }

    EXPECT_CALL(
      mockSet(), output(EqDeref(set_ptr), "Test_Set_output_name.segy"));

    EXPECT_CALL(mockSet(), text(EqDeref(set_ptr), "Test Set text"));

    EXPECT_CALL(mockSet(), summary(EqDeref(set_ptr)));

    EXPECT_CALL(mockSet(), add(EqDeref(set_ptr), "Test Set add"));

    typedef std::pair<Gain_function, Gain_function> Gain_function_pair;
    const Gain_function_pair agc_types[] = {
      std::make_pair(exseis_rectangular_RMS_gain, rectangular_RMS_gain),
      std::make_pair(exseis_triangular_RMS_gain, triangular_RMS_gain),
      std::make_pair(exseis_mean_abs_gain, mean_abs_gain),
      std::make_pair(exseis_median_gain, median_gain)};

    for (auto agc_type : agc_types) {
        EXPECT_CALL(
          mockSet(), AGC(
                       EqDeref(set_ptr), AnyOf(agc_type.first, agc_type.second),
                       900, 910.0));
    }

    EXPECT_CALL(mockSet(), dtor(EqDeref(set_ptr)));
    EXPECT_CALL(mockRule(), dtor(_));
}
