#include "exseisdat/piol/metadata/Trace_metadata_key.h"
#include "exseisdat/piol/metadata/Trace_metadata_key.hh"

#include "gtest/gtest.h"

#include <type_traits>

namespace {
// Equality operator comparing exseis::piol::Trace_metadata_key
// and exseis::piol::exseis_Trace_metadata_key
constexpr bool operator==(
    exseis::piol::Trace_metadata_key lhs,
    exseis::piol::exseis_Trace_metadata_key rhs)
{
    return static_cast<exseis::piol::exseis_Trace_metadata_key>(lhs) == rhs;
}
}  // namespace

namespace exseis {
namespace piol {

// Test C and C++ API for the Trace_metadata_key enum are equivalent.
TEST(Trace_metadata_key, C_API)
{
    // Check the types are equivalent
    static_assert(
        std::is_same<
            std::underlying_type<Trace_metadata_key>::type,
            exseis_Trace_metadata_key>::value,
        "exseis::piol::Trace_metadata_key and exseis_Trace_metadata_key don't have the same type!");

    // Check the values are equivalent
    // Use a compiler flag to check the switch covers the enum.
    switch (Trace_metadata_key::Copy) {
        case Trace_metadata_key::Copy:
            static_assert(
                Trace_metadata_key::Copy == exseis_meta_Copy,
                "exseis::piol::Trace_metadata_key::Copy and exseis_meta_Copy should have the same value!");
            break;

        case Trace_metadata_key::ltn:
            static_assert(
                Trace_metadata_key::ltn == exseis_meta_ltn,
                "exseis::piol::Trace_metadata_key::ltn and exseis_meta_ltn should have the same value!");
            break;

        case Trace_metadata_key::gtn:
            static_assert(
                Trace_metadata_key::gtn == exseis_meta_gtn,
                "exseis::piol::Trace_metadata_key::gtn and exseis_meta_gtn should have the same value!");
            break;

        case Trace_metadata_key::tnl:
            static_assert(
                Trace_metadata_key::tnl == exseis_meta_tnl,
                "exseis::piol::Trace_metadata_key::tnl and exseis_meta_tnl should have the same value!");
            break;

        case Trace_metadata_key::tnr:
            static_assert(
                Trace_metadata_key::tnr == exseis_meta_tnr,
                "exseis::piol::Trace_metadata_key::tnr and exseis_meta_tnr should have the same value!");
            break;

        case Trace_metadata_key::tn:
            static_assert(
                Trace_metadata_key::tn == exseis_meta_tn,
                "exseis::piol::Trace_metadata_key::tn and exseis_meta_tn should have the same value!");
            break;

        case Trace_metadata_key::tne:
            static_assert(
                Trace_metadata_key::tne == exseis_meta_tne,
                "exseis::piol::Trace_metadata_key::tne and exseis_meta_tne should have the same value!");
            break;

        case Trace_metadata_key::ns:
            static_assert(
                Trace_metadata_key::ns == exseis_meta_ns,
                "exseis::piol::Trace_metadata_key::ns and exseis_meta_ns should have the same value!");
            break;

        case Trace_metadata_key::sample_interval:
            static_assert(
                Trace_metadata_key::sample_interval
                    == exseis_meta_sample_interval,
                "exseis::piol::Trace_metadata_key::sample_interval and exseis_meta_sample_interval should have the same value!");
            break;

        case Trace_metadata_key::Tic:
            static_assert(
                Trace_metadata_key::Tic == exseis_meta_Tic,
                "exseis::piol::Trace_metadata_key::Tic and exseis_meta_Tic should have the same value!");
            break;

        case Trace_metadata_key::SrcNum:
            static_assert(
                Trace_metadata_key::SrcNum == exseis_meta_SrcNum,
                "exseis::piol::Trace_metadata_key::SrcNum and exseis_meta_SrcNum should have the same value!");
            break;

        case Trace_metadata_key::ShotNum:
            static_assert(
                Trace_metadata_key::ShotNum == exseis_meta_ShotNum,
                "exseis::piol::Trace_metadata_key::ShotNum and exseis_meta_ShotNum should have the same value!");
            break;

        case Trace_metadata_key::VStack:
            static_assert(
                Trace_metadata_key::VStack == exseis_meta_VStack,
                "exseis::piol::Trace_metadata_key::VStack and exseis_meta_VStack should have the same value!");
            break;

        case Trace_metadata_key::HStack:
            static_assert(
                Trace_metadata_key::HStack == exseis_meta_HStack,
                "exseis::piol::Trace_metadata_key::HStack and exseis_meta_HStack should have the same value!");
            break;

        case Trace_metadata_key::Offset:
            static_assert(
                Trace_metadata_key::Offset == exseis_meta_Offset,
                "exseis::piol::Trace_metadata_key::Offset and exseis_meta_Offset should have the same value!");
            break;

        case Trace_metadata_key::RGElev:
            static_assert(
                Trace_metadata_key::RGElev == exseis_meta_RGElev,
                "exseis::piol::Trace_metadata_key::RGElev and exseis_meta_RGElev should have the same value!");
            break;

        case Trace_metadata_key::SSElev:
            static_assert(
                Trace_metadata_key::SSElev == exseis_meta_SSElev,
                "exseis::piol::Trace_metadata_key::SSElev and exseis_meta_SSElev should have the same value!");
            break;

        case Trace_metadata_key::SDElev:
            static_assert(
                Trace_metadata_key::SDElev == exseis_meta_SDElev,
                "exseis::piol::Trace_metadata_key::SDElev and exseis_meta_SDElev should have the same value!");
            break;

        case Trace_metadata_key::WtrDepSrc:
            static_assert(
                Trace_metadata_key::WtrDepSrc == exseis_meta_WtrDepSrc,
                "exseis::piol::Trace_metadata_key::WtrDepSrc and exseis_meta_WtrDepSrc should have the same value!");
            break;

        case Trace_metadata_key::WtrDepRcv:
            static_assert(
                Trace_metadata_key::WtrDepRcv == exseis_meta_WtrDepRcv,
                "exseis::piol::Trace_metadata_key::WtrDepRcv and exseis_meta_WtrDepRcv should have the same value!");
            break;

        case Trace_metadata_key::x_src:
            static_assert(
                Trace_metadata_key::x_src == exseis_meta_x_src,
                "exseis::piol::Trace_metadata_key::x_src and exseis_meta_x_src should have the same value!");
            break;

        case Trace_metadata_key::y_src:
            static_assert(
                Trace_metadata_key::y_src == exseis_meta_y_src,
                "exseis::piol::Trace_metadata_key::y_src and exseis_meta_y_src should have the same value!");
            break;

        case Trace_metadata_key::x_rcv:
            static_assert(
                Trace_metadata_key::x_rcv == exseis_meta_x_rcv,
                "exseis::piol::Trace_metadata_key::x_rcv and exseis_meta_x_rcv should have the same value!");
            break;

        case Trace_metadata_key::y_rcv:
            static_assert(
                Trace_metadata_key::y_rcv == exseis_meta_y_rcv,
                "exseis::piol::Trace_metadata_key::y_rcv and exseis_meta_y_rcv should have the same value!");
            break;

        case Trace_metadata_key::xCmp:
            static_assert(
                Trace_metadata_key::xCmp == exseis_meta_xCmp,
                "exseis::piol::Trace_metadata_key::xCmp and exseis_meta_xCmp should have the same value!");
            break;

        case Trace_metadata_key::yCmp:
            static_assert(
                Trace_metadata_key::yCmp == exseis_meta_yCmp,
                "exseis::piol::Trace_metadata_key::yCmp and exseis_meta_yCmp should have the same value!");
            break;

        case Trace_metadata_key::coordinate_scalar:
            static_assert(
                Trace_metadata_key::coordinate_scalar
                    == exseis_meta_coordinate_scalar,
                "exseis::piol::Trace_metadata_key::coordinate_scalar and exseis_meta_coordinate_scalar should have the same value!");
            break;

        case Trace_metadata_key::il:
            static_assert(
                Trace_metadata_key::il == exseis_meta_il,
                "exseis::piol::Trace_metadata_key::il and exseis_meta_il should have the same value!");
            break;

        case Trace_metadata_key::xl:
            static_assert(
                Trace_metadata_key::xl == exseis_meta_xl,
                "exseis::piol::Trace_metadata_key::xl and exseis_meta_xl should have the same value!");
            break;

        case Trace_metadata_key::TransUnit:
            static_assert(
                Trace_metadata_key::TransUnit == exseis_meta_TransUnit,
                "exseis::piol::Trace_metadata_key::TransUnit and exseis_meta_TransUnit should have the same value!");
            break;

        case Trace_metadata_key::TraceUnit:
            static_assert(
                Trace_metadata_key::TraceUnit == exseis_meta_TraceUnit,
                "exseis::piol::Trace_metadata_key::TraceUnit and exseis_meta_TraceUnit should have the same value!");
            break;

        case Trace_metadata_key::dsdr:
            static_assert(
                Trace_metadata_key::dsdr == exseis_meta_dsdr,
                "exseis::piol::Trace_metadata_key::dsdr and exseis_meta_dsdr should have the same value!");
            break;

        case Trace_metadata_key::Misc1:
            static_assert(
                Trace_metadata_key::Misc1 == exseis_meta_Misc1,
                "exseis::piol::Trace_metadata_key::Misc1 and exseis_meta_Misc1 should have the same value!");
            break;

        case Trace_metadata_key::Misc2:
            static_assert(
                Trace_metadata_key::Misc2 == exseis_meta_Misc2,
                "exseis::piol::Trace_metadata_key::Misc2 and exseis_meta_Misc2 should have the same value!");
            break;

        case Trace_metadata_key::Misc3:
            static_assert(
                Trace_metadata_key::Misc3 == exseis_meta_Misc3,
                "exseis::piol::Trace_metadata_key::Misc3 and exseis_meta_Misc3 should have the same value!");
            break;

        case Trace_metadata_key::Misc4:
            static_assert(
                Trace_metadata_key::Misc4 == exseis_meta_Misc4,
                "exseis::piol::Trace_metadata_key::Misc4 and exseis_meta_Misc4 should have the same value!");
            break;
    }
}

}  // namespace piol
}  // namespace exseis
