#include "exseisdat/piol/Meta.h"
#include "exseisdat/piol/Meta.hh"

#include "gtest/gtest.h"

#include <type_traits>

namespace {
// Equality operator comparing exseis::piol::Meta and exseis_Meta
constexpr bool operator==(exseis::piol::Meta lhs, exseis_Meta rhs)
{
    return static_cast<exseis_Meta>(lhs) == rhs;
}
}  // namespace


// Test C and C++ API for the Meta enum are equivalent.
TEST(Meta, C_API)
{
    using exseis::piol::Meta;

    // Check the types are equivalent
    static_assert(
      std::is_same<std::underlying_type<Meta>::type, exseis_Meta>::value,
      "exseis::piol::Meta and exseis_Meta don't have the same type!");

    // Check the values are equivalent
    // Use a compiler flag to check the switch covers the enum.
    switch (Meta::Copy) {
        case Meta::Copy:
            static_assert(
              Meta::Copy == exseis_meta_Copy,
              "exseis::piol::Meta::Copy and exseis_meta_Copy should have the same value!");
            break;

        case Meta::ltn:
            static_assert(
              Meta::ltn == exseis_meta_ltn,
              "exseis::piol::Meta::ltn and exseis_meta_ltn should have the same value!");
            break;

        case Meta::gtn:
            static_assert(
              Meta::gtn == exseis_meta_gtn,
              "exseis::piol::Meta::gtn and exseis_meta_gtn should have the same value!");
            break;

        case Meta::tnl:
            static_assert(
              Meta::tnl == exseis_meta_tnl,
              "exseis::piol::Meta::tnl and exseis_meta_tnl should have the same value!");
            break;

        case Meta::tnr:
            static_assert(
              Meta::tnr == exseis_meta_tnr,
              "exseis::piol::Meta::tnr and exseis_meta_tnr should have the same value!");
            break;

        case Meta::tn:
            static_assert(
              Meta::tn == exseis_meta_tn,
              "exseis::piol::Meta::tn and exseis_meta_tn should have the same value!");
            break;

        case Meta::tne:
            static_assert(
              Meta::tne == exseis_meta_tne,
              "exseis::piol::Meta::tne and exseis_meta_tne should have the same value!");
            break;

        case Meta::ns:
            static_assert(
              Meta::ns == exseis_meta_ns,
              "exseis::piol::Meta::ns and exseis_meta_ns should have the same value!");
            break;

        case Meta::sample_interval:
            static_assert(
              Meta::sample_interval == exseis_meta_sample_interval,
              "exseis::piol::Meta::sample_interval and exseis_meta_sample_interval should have the same value!");
            break;

        case Meta::Tic:
            static_assert(
              Meta::Tic == exseis_meta_Tic,
              "exseis::piol::Meta::Tic and exseis_meta_Tic should have the same value!");
            break;

        case Meta::SrcNum:
            static_assert(
              Meta::SrcNum == exseis_meta_SrcNum,
              "exseis::piol::Meta::SrcNum and exseis_meta_SrcNum should have the same value!");
            break;

        case Meta::ShotNum:
            static_assert(
              Meta::ShotNum == exseis_meta_ShotNum,
              "exseis::piol::Meta::ShotNum and exseis_meta_ShotNum should have the same value!");
            break;

        case Meta::VStack:
            static_assert(
              Meta::VStack == exseis_meta_VStack,
              "exseis::piol::Meta::VStack and exseis_meta_VStack should have the same value!");
            break;

        case Meta::HStack:
            static_assert(
              Meta::HStack == exseis_meta_HStack,
              "exseis::piol::Meta::HStack and exseis_meta_HStack should have the same value!");
            break;

        case Meta::Offset:
            static_assert(
              Meta::Offset == exseis_meta_Offset,
              "exseis::piol::Meta::Offset and exseis_meta_Offset should have the same value!");
            break;

        case Meta::RGElev:
            static_assert(
              Meta::RGElev == exseis_meta_RGElev,
              "exseis::piol::Meta::RGElev and exseis_meta_RGElev should have the same value!");
            break;

        case Meta::SSElev:
            static_assert(
              Meta::SSElev == exseis_meta_SSElev,
              "exseis::piol::Meta::SSElev and exseis_meta_SSElev should have the same value!");
            break;

        case Meta::SDElev:
            static_assert(
              Meta::SDElev == exseis_meta_SDElev,
              "exseis::piol::Meta::SDElev and exseis_meta_SDElev should have the same value!");
            break;

        case Meta::WtrDepSrc:
            static_assert(
              Meta::WtrDepSrc == exseis_meta_WtrDepSrc,
              "exseis::piol::Meta::WtrDepSrc and exseis_meta_WtrDepSrc should have the same value!");
            break;

        case Meta::WtrDepRcv:
            static_assert(
              Meta::WtrDepRcv == exseis_meta_WtrDepRcv,
              "exseis::piol::Meta::WtrDepRcv and exseis_meta_WtrDepRcv should have the same value!");
            break;

        case Meta::x_src:
            static_assert(
              Meta::x_src == exseis_meta_x_src,
              "exseis::piol::Meta::x_src and exseis_meta_x_src should have the same value!");
            break;

        case Meta::y_src:
            static_assert(
              Meta::y_src == exseis_meta_y_src,
              "exseis::piol::Meta::y_src and exseis_meta_y_src should have the same value!");
            break;

        case Meta::x_rcv:
            static_assert(
              Meta::x_rcv == exseis_meta_x_rcv,
              "exseis::piol::Meta::x_rcv and exseis_meta_x_rcv should have the same value!");
            break;

        case Meta::y_rcv:
            static_assert(
              Meta::y_rcv == exseis_meta_y_rcv,
              "exseis::piol::Meta::y_rcv and exseis_meta_y_rcv should have the same value!");
            break;

        case Meta::xCmp:
            static_assert(
              Meta::xCmp == exseis_meta_xCmp,
              "exseis::piol::Meta::xCmp and exseis_meta_xCmp should have the same value!");
            break;

        case Meta::yCmp:
            static_assert(
              Meta::yCmp == exseis_meta_yCmp,
              "exseis::piol::Meta::yCmp and exseis_meta_yCmp should have the same value!");
            break;

        case Meta::coordinate_scalar:
            static_assert(
              Meta::coordinate_scalar == exseis_meta_coordinate_scalar,
              "exseis::piol::Meta::coordinate_scalar and exseis_meta_coordinate_scalar should have the same value!");
            break;

        case Meta::il:
            static_assert(
              Meta::il == exseis_meta_il,
              "exseis::piol::Meta::il and exseis_meta_il should have the same value!");
            break;

        case Meta::xl:
            static_assert(
              Meta::xl == exseis_meta_xl,
              "exseis::piol::Meta::xl and exseis_meta_xl should have the same value!");
            break;

        case Meta::TransUnit:
            static_assert(
              Meta::TransUnit == exseis_meta_TransUnit,
              "exseis::piol::Meta::TransUnit and exseis_meta_TransUnit should have the same value!");
            break;

        case Meta::TraceUnit:
            static_assert(
              Meta::TraceUnit == exseis_meta_TraceUnit,
              "exseis::piol::Meta::TraceUnit and exseis_meta_TraceUnit should have the same value!");
            break;

        case Meta::dsdr:
            static_assert(
              Meta::dsdr == exseis_meta_dsdr,
              "exseis::piol::Meta::dsdr and exseis_meta_dsdr should have the same value!");
            break;

        case Meta::Misc1:
            static_assert(
              Meta::Misc1 == exseis_meta_Misc1,
              "exseis::piol::Meta::Misc1 and exseis_meta_Misc1 should have the same value!");
            break;

        case Meta::Misc2:
            static_assert(
              Meta::Misc2 == exseis_meta_Misc2,
              "exseis::piol::Meta::Misc2 and exseis_meta_Misc2 should have the same value!");
            break;

        case Meta::Misc3:
            static_assert(
              Meta::Misc3 == exseis_meta_Misc3,
              "exseis::piol::Meta::Misc3 and exseis_meta_Misc3 should have the same value!");
            break;

        case Meta::Misc4:
            static_assert(
              Meta::Misc4 == exseis_meta_Misc4,
              "exseis::piol::Meta::Misc4 and exseis_meta_Misc4 should have the same value!");
            break;
    }
}
