#ifndef PIOLWRAPTESTMOCKSET_HEADER_GUARD
#define PIOLWRAPTESTMOCKSET_HEADER_GUARD

#include "googletest_variable_instances.hh"
#include "gmock/gmock.h"

#include "ExSeisDat/Flow/Set.hh"

namespace exseis {
namespace Flow {

class MockSet;
::testing::StrictMock<MockSet>& mockSet();

class MockSet {
  public:
    MOCK_METHOD5(
      ctor,
      void(
        Set*,
        std::shared_ptr<exseis::PIOL::ExSeisPIOL> piol_,
        std::string pattern,
        std::string outfix_,
        std::shared_ptr<exseis::PIOL::Rule> rule_));

    MOCK_METHOD3(
      ctor,
      void(
        Set*,
        std::shared_ptr<exseis::PIOL::ExSeisPIOL> piol_,
        std::shared_ptr<exseis::PIOL::Rule> rule_));

    MOCK_METHOD1(dtor, void(Set*));

    MOCK_METHOD2(sort, void(Set*, exseis::PIOL::CompareP sortFunc));

    MOCK_METHOD3(
      sort,
      void(
        Set*,
        std::shared_ptr<exseis::PIOL::Rule> r,
        exseis::PIOL::CompareP sortFunc));

    MOCK_METHOD2(output, std::vector<std::string>(Set*, std::string oname));

    MOCK_METHOD4(
      getMinMax,
      void(
        Set*,
        exseis::PIOL::MinMaxFunc<exseis::PIOL::Param> xlam,
        exseis::PIOL::MinMaxFunc<exseis::PIOL::Param> ylam,
        exseis::PIOL::CoordElem* minmax));

    MOCK_METHOD4(
      taper,
      void(
        Set*,
        exseis::PIOL::TaperFunc tapFunc,
        size_t nTailLft,
        size_t nTailRt));

    MOCK_METHOD4(
      AGC,
      void(
        Set*,
        exseis::utils::Gain_function agcFunc,
        size_t window,
        exseis::utils::Trace_value target_amplitude));

    MOCK_METHOD2(text, void(Set*, std::string outmsg_));

    MOCK_CONST_METHOD1(summary, void(const Set*));

    MOCK_METHOD2(
      add_impl, void(Set*, std::unique_ptr<exseis::PIOL::ReadInterface>& in));

    void add(Set* set, std::unique_ptr<exseis::PIOL::ReadInterface> in)
    {
        add_impl(set, in);
    }

    MOCK_METHOD2(add, void(Set*, std::string name));

    MOCK_METHOD5(
      toAngle,
      void(
        Set*,
        std::string vmName,
        const size_t vBin,
        const size_t oGSz,
        exseis::utils::Floating_point oInc));

    MOCK_METHOD2(sort, void(Set*, exseis::PIOL::SortType type));

    MOCK_METHOD4(
      getMinMax,
      void(
        Set*,
        exseis::PIOL::Meta m1,
        exseis::PIOL::Meta m2,
        exseis::PIOL::CoordElem* minmax));

    MOCK_METHOD4(
      taper,
      void(
        Set*, exseis::PIOL::TaperType type, size_t nTailLft, size_t nTailRt));

    MOCK_METHOD8(
      temporalFilter,
      void(
        Set*,
        exseis::PIOL::FltrType type,
        exseis::PIOL::FltrDmn domain,
        exseis::PIOL::PadType pad,
        exseis::utils::Trace_value fs,
        std::vector<exseis::utils::Trace_value> corners,
        size_t nw,
        size_t winCntr));

    MOCK_METHOD9(
      temporalFilter,
      void(
        Set*,
        exseis::PIOL::FltrType type,
        exseis::PIOL::FltrDmn domain,
        exseis::PIOL::PadType pad,
        exseis::utils::Trace_value fs,
        size_t N,
        std::vector<exseis::utils::Trace_value> corners,
        size_t nw,
        size_t winCntr));
};

}  // namespace Flow
}  // namespace exseis

#endif  // PIOLWRAPTESTMOCKSET_HEADER_GUARD
