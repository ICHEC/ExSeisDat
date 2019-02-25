#include "exseisdat/piol/operations/SortType.h"
#include "exseisdat/piol/operations/SortType.hh"

#include "gtest/gtest.h"

#include <type_traits>


namespace {
// Equality operator comparing exseis::piol::SortType and exseis_SortType
constexpr bool operator==(exseis::piol::SortType lhs, exseis_SortType rhs)
{
    return static_cast<exseis_SortType>(lhs) == rhs;
}
}  // namespace


// Test C and C++ API for the SortType enum are equivalent.
TEST(SortType, C_API)
{
    using exseis::piol::SortType;

    // Check the types are equivalent
    static_assert(
        std::is_same<
            std::underlying_type<SortType>::type, exseis_SortType>::value,
        "exseis::piol::SortType and exseis_SortType don't have the same type!");

    // Check the values are equivalent
    // Use a compiler flag to check the switch covers the enum.
    switch (SortType::SrcRcv) {

        case SortType::SrcRcv:
            static_assert(
                SortType::SrcRcv == exseis_sorttype_SrcRcv,
                "exseis::piol::SortType::SrcRcv and exseis_sorttype_SrcRcv should have the same value!");
            break;

        case SortType::SrcOff:
            static_assert(
                SortType::SrcOff == exseis_sorttype_SrcOff,
                "exseis::piol::SortType::SrcOff and exseis_sorttype_SrcOff should have the same value!");
            break;

        case SortType::SrcROff:
            static_assert(
                SortType::SrcROff == exseis_sorttype_SrcROff,
                "exseis::piol::SortType::SrcROff and exseis_sorttype_SrcROff should have the same value!");
            break;

        case SortType::RcvOff:
            static_assert(
                SortType::RcvOff == exseis_sorttype_RcvOff,
                "exseis::piol::SortType::RcvOff and exseis_sorttype_RcvOff should have the same value!");
            break;

        case SortType::RcvROff:
            static_assert(
                SortType::RcvROff == exseis_sorttype_RcvROff,
                "exseis::piol::SortType::RcvROff and exseis_sorttype_RcvROff should have the same value!");
            break;

        case SortType::LineOff:
            static_assert(
                SortType::LineOff == exseis_sorttype_LineOff,
                "exseis::piol::SortType::LineOff and exseis_sorttype_LineOff should have the same value!");
            break;

        case SortType::LineROff:
            static_assert(
                SortType::LineROff == exseis_sorttype_LineROff,
                "exseis::piol::SortType::LineROff and exseis_sorttype_LineROff should have the same value!");
            break;

        case SortType::OffLine:
            static_assert(
                SortType::OffLine == exseis_sorttype_OffLine,
                "exseis::piol::SortType::OffLine and exseis_sorttype_OffLine should have the same value!");
            break;

        case SortType::ROffLine:
            static_assert(
                SortType::ROffLine == exseis_sorttype_ROffLine,
                "exseis::piol::SortType::ROffLine and exseis_sorttype_ROffLine should have the same value!");
            break;
    }
}
