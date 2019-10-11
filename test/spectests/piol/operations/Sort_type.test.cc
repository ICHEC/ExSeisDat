#include "exseisdat/piol/operations/sort_operations/SortType.h"
#include "exseisdat/piol/operations/sort_operations/Sort_type.hh"

#include "gtest/gtest.h"

#include <type_traits>


namespace {
// Equality operator comparing exseis::piol::Sort_type and
// exseis::piol::exseis_SortType
constexpr bool operator==(
    exseis::piol::Sort_type lhs, exseis::piol::exseis_SortType rhs)
{
    return static_cast<exseis::piol::exseis_SortType>(lhs) == rhs;
}
}  // namespace

namespace exseis {
namespace piol {

// Test C and C++ API for the Sort_type enum are equivalent.
TEST(Sort_type, C_API)
{
    using exseis::piol::exseis_SortType;
    using exseis::piol::Sort_type;

    // Check the types are equivalent
    static_assert(
        std::is_same<
            std::underlying_type<Sort_type>::type, exseis_SortType>::value,
        "exseis::piol::Sort_type and exseis_SortType don't have the same type!");

    // Check the values are equivalent
    // Use a compiler flag to check the switch covers the enum.
    switch (Sort_type::SrcRcv) {

        case Sort_type::SrcRcv:
            static_assert(
                Sort_type::SrcRcv == exseis_sorttype_SrcRcv,
                "exseis::piol::Sort_type::SrcRcv and exseis_sorttype_SrcRcv should have the same value!");
            break;

        case Sort_type::SrcOff:
            static_assert(
                Sort_type::SrcOff == exseis_sorttype_SrcOff,
                "exseis::piol::Sort_type::SrcOff and exseis_sorttype_SrcOff should have the same value!");
            break;

        case Sort_type::SrcROff:
            static_assert(
                Sort_type::SrcROff == exseis_sorttype_SrcROff,
                "exseis::piol::Sort_type::SrcROff and exseis_sorttype_SrcROff should have the same value!");
            break;

        case Sort_type::RcvOff:
            static_assert(
                Sort_type::RcvOff == exseis_sorttype_RcvOff,
                "exseis::piol::Sort_type::RcvOff and exseis_sorttype_RcvOff should have the same value!");
            break;

        case Sort_type::RcvROff:
            static_assert(
                Sort_type::RcvROff == exseis_sorttype_RcvROff,
                "exseis::piol::Sort_type::RcvROff and exseis_sorttype_RcvROff should have the same value!");
            break;

        case Sort_type::LineOff:
            static_assert(
                Sort_type::LineOff == exseis_sorttype_LineOff,
                "exseis::piol::Sort_type::LineOff and exseis_sorttype_LineOff should have the same value!");
            break;

        case Sort_type::LineROff:
            static_assert(
                Sort_type::LineROff == exseis_sorttype_LineROff,
                "exseis::piol::Sort_type::LineROff and exseis_sorttype_LineROff should have the same value!");
            break;

        case Sort_type::OffLine:
            static_assert(
                Sort_type::OffLine == exseis_sorttype_OffLine,
                "exseis::piol::Sort_type::OffLine and exseis_sorttype_OffLine should have the same value!");
            break;

        case Sort_type::ROffLine:
            static_assert(
                Sort_type::ROffLine == exseis_sorttype_ROffLine,
                "exseis::piol::Sort_type::ROffLine and exseis_sorttype_ROffLine should have the same value!");
            break;
    }
}

}  // namespace piol
}  // namespace exseis
