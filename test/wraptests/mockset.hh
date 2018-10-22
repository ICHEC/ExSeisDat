#ifndef EXSEISDAT_TEST_WRAPTESTS_MOCKSET_HH
#define EXSEISDAT_TEST_WRAPTESTS_MOCKSET_HH

#include "googletest_variable_instances.hh"
#include "gmock/gmock.h"

#include "exseisdat/flow/Set.hh"

namespace exseis {
namespace flow {

class MockSet;
::testing::StrictMock<MockSet>& mock_set();

class MockSet {
  public:
    MOCK_METHOD5(
      ctor,
      void(
        Set*,
        std::shared_ptr<exseis::piol::ExSeisPIOL> piol_,
        std::string pattern,
        std::string outfix_,
        std::shared_ptr<exseis::piol::Rule> rule_));

    MOCK_METHOD3(
      ctor,
      void(
        Set*,
        std::shared_ptr<exseis::piol::ExSeisPIOL> piol_,
        std::shared_ptr<exseis::piol::Rule> rule_));

    MOCK_METHOD1(dtor, void(Set*));

    MOCK_METHOD2(sort, void(Set*, exseis::piol::CompareP sortFunc));

    MOCK_METHOD3(
      sort,
      void(
        Set*,
        std::shared_ptr<exseis::piol::Rule> r,
        exseis::piol::CompareP sortFunc));

    MOCK_METHOD2(output, std::vector<std::string>(Set*, std::string oname));

    MOCK_METHOD4(
      get_min_max,
      void(
        Set*,
        exseis::piol::MinMaxFunc<exseis::piol::Trace_metadata> xlam,
        exseis::piol::MinMaxFunc<exseis::piol::Trace_metadata> ylam,
        exseis::piol::CoordElem* minmax));

    MOCK_METHOD4(
      taper,
      void(
        Set*,
        exseis::utils::Taper_function taper_function,
        size_t nTailLft,
        size_t nTailRt));

    MOCK_METHOD4(
      agc,
      void(
        Set*,
        exseis::utils::Gain_function agcFunc,
        size_t window,
        exseis::utils::Trace_value target_amplitude));

    MOCK_METHOD2(text, void(Set*, std::string outmsg_));

    MOCK_CONST_METHOD1(summary, void(const Set*));

    MOCK_METHOD2(
      add_impl, void(Set*, std::unique_ptr<exseis::piol::ReadInterface>& in));

    void add(Set* set, std::unique_ptr<exseis::piol::ReadInterface> in)
    {
        add_impl(set, in);
    }

    MOCK_METHOD2(add, void(Set*, std::string name));

    MOCK_METHOD5(
      toAngle,
      void(
        Set*,
        std::string vmName,
        const size_t v_bin,
        const size_t output_traces_per_gather,
        exseis::utils::Floating_point output_sample_interval));

    MOCK_METHOD2(sort, void(Set*, exseis::piol::SortType type));

    MOCK_METHOD4(
      get_min_max,
      void(
        Set*,
        exseis::piol::Meta m1,
        exseis::piol::Meta m2,
        exseis::piol::CoordElem* minmax));

    MOCK_METHOD8(
      temporal_filter,
      void(
        Set*,
        exseis::piol::FltrType type,
        exseis::piol::FltrDmn domain,
        exseis::piol::PadType pad,
        exseis::utils::Trace_value fs,
        std::vector<exseis::utils::Trace_value> corners,
        size_t nw,
        size_t winCntr));

    MOCK_METHOD9(
      temporal_filter,
      void(
        Set*,
        exseis::piol::FltrType type,
        exseis::piol::FltrDmn domain,
        exseis::piol::PadType pad,
        exseis::utils::Trace_value fs,
        size_t N,
        std::vector<exseis::utils::Trace_value> corners,
        size_t nw,
        size_t winCntr));
};

}  // namespace flow
}  // namespace exseis

#endif  // EXSEISDAT_TEST_WRAPTESTS_MOCKSET_HH
