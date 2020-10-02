#include "exseisdat/piol/segy/Trace_header_offsets.h"
#include "exseisdat/piol/segy/Trace_header_offsets.hh"

#include "gtest/gtest.h"

#include <type_traits>

namespace {
// Equality operator comparing exseis::piol::Trace_header_offsets and
// exseis::piol::segy::exseis_segy_Trace_header_offsets
constexpr bool operator==(
    exseis::piol::segy::Trace_header_offsets lhs,
    exseis::piol::segy::exseis_segy_Trace_header_offsets rhs)
{
    return static_cast<exseis::piol::segy::exseis_segy_Trace_header_offsets>(
               lhs)
           == rhs;
}
}  // namespace

TEST(Trace_header_offsets, C_API)
{
    // Test C and C++ enums have the same type.
    static_assert(
        std::is_same<
            std::underlying_type<
                exseis::piol::segy::Trace_header_offsets>::type,
            exseis::piol::segy::exseis_segy_Trace_header_offsets>::value,
        "exseis::piol::segy::Trace_header_offsets and segy::exseis_segy_Trace_header_offsets should have the same type!");

    using exseis::piol::segy::Trace_header_offsets;

    // Test C and C++ enums have the same values.
    // Use a compiler flag to test the switch covers the enum.
    switch (Trace_header_offsets::SeqNum) {
        case Trace_header_offsets::SeqNum:
            static_assert(
                Trace_header_offsets::SeqNum
                    == exseis::piol::segy::exseis_segy_tr_SeqNum,
                "exseis::piol::SeqNum and segy::exseis_segy_tr_SeqNum should have the same value!");
            break;

        case Trace_header_offsets::SeqFNum:
            static_assert(
                Trace_header_offsets::SeqFNum
                    == exseis::piol::segy::exseis_segy_tr_SeqFNum,
                "exseis::piol::Trace_header_offsets::SeqFNum and segy::exseis_segy_tr_SeqFNum should have the same value!");
            break;

        case Trace_header_offsets::ORF:
            static_assert(
                Trace_header_offsets::ORF
                    == exseis::piol::segy::exseis_segy_tr_ORF,
                "exseis::piol::Trace_header_offsets::ORF and segy::exseis_segy_tr_ORF should have the same value!");
            break;

        case Trace_header_offsets::TORF:
            static_assert(
                Trace_header_offsets::TORF
                    == exseis::piol::segy::exseis_segy_tr_TORF,
                "exseis::piol::Trace_header_offsets::TORF and segy::exseis_segy_tr_TORF should have the same value!");
            break;

        case Trace_header_offsets::ENSrcNum:
            static_assert(
                Trace_header_offsets::ENSrcNum
                    == exseis::piol::segy::exseis_segy_tr_ENSrcNum,
                "exseis::piol::Trace_header_offsets::ENSrcNum and segy::exseis_segy_tr_ENSrcNum should have the same value!");
            break;

        case Trace_header_offsets::SeqNumEns:
            static_assert(
                Trace_header_offsets::SeqNumEns
                    == exseis::piol::segy::exseis_segy_tr_SeqNumEns,
                "exseis::piol::Trace_header_offsets::SeqNumEns and segy::exseis_segy_tr_SeqNumEns should have the same value!");
            break;

        case Trace_header_offsets::TIC:
            static_assert(
                Trace_header_offsets::TIC
                    == exseis::piol::segy::exseis_segy_tr_TIC,
                "exseis::piol::Trace_header_offsets::TIC and segy::exseis_segy_tr_TIC should have the same value!");
            break;

        case Trace_header_offsets::VStackCnt:
            static_assert(
                Trace_header_offsets::VStackCnt
                    == exseis::piol::segy::exseis_segy_tr_VStackCnt,
                "exseis::piol::Trace_header_offsets::VStackCnt and segy::exseis_segy_tr_VStackCnt should have the same value!");
            break;

        case Trace_header_offsets::HStackCnt:
            static_assert(
                Trace_header_offsets::HStackCnt
                    == exseis::piol::segy::exseis_segy_tr_HStackCnt,
                "exseis::piol::Trace_header_offsets::HStackCnt and segy::exseis_segy_tr_HStackCnt should have the same value!");
            break;

        case Trace_header_offsets::CDist:
            static_assert(
                Trace_header_offsets::CDist
                    == exseis::piol::segy::exseis_segy_tr_CDist,
                "exseis::piol::Trace_header_offsets::CDist and segy::exseis_segy_tr_CDist should have the same value!");
            break;

        case Trace_header_offsets::RcvElv:
            static_assert(
                Trace_header_offsets::RcvElv
                    == exseis::piol::segy::exseis_segy_tr_RcvElv,
                "exseis::piol::Trace_header_offsets::RcvElv and segy::exseis_segy_tr_RcvElv should have the same value!");
            break;

        case Trace_header_offsets::SurfElvSrc:
            static_assert(
                Trace_header_offsets::SurfElvSrc
                    == exseis::piol::segy::exseis_segy_tr_SurfElvSrc,
                "exseis::piol::Trace_header_offsets::SurfElvSrc and segy::exseis_segy_tr_SurfElvSrc should have the same value!");
            break;

        case Trace_header_offsets::SrcDpthSurf:
            static_assert(
                Trace_header_offsets::SrcDpthSurf
                    == exseis::piol::segy::exseis_segy_tr_SrcDpthSurf,
                "exseis::piol::Trace_header_offsets::SrcDpthSurf and segy::exseis_segy_tr_SrcDpthSurf should have the same value!");
            break;

        case Trace_header_offsets::DtmElvRcv:
            static_assert(
                Trace_header_offsets::DtmElvRcv
                    == exseis::piol::segy::exseis_segy_tr_DtmElvRcv,
                "exseis::piol::Trace_header_offsets::DtmElvRcv and segy::exseis_segy_tr_DtmElvRcv should have the same value!");
            break;

        case Trace_header_offsets::DtmElvSrc:
            static_assert(
                Trace_header_offsets::DtmElvSrc
                    == exseis::piol::segy::exseis_segy_tr_DtmElvSrc,
                "exseis::piol::Trace_header_offsets::DtmElvSrc and segy::exseis_segy_tr_DtmElvSrc should have the same value!");
            break;

        case Trace_header_offsets::WtrDepSrc:
            static_assert(
                Trace_header_offsets::WtrDepSrc
                    == exseis::piol::segy::exseis_segy_tr_WtrDepSrc,
                "exseis::piol::Trace_header_offsets::WtrDepSrc and segy::exseis_segy_tr_WtrDepSrc should have the same value!");
            break;

        case Trace_header_offsets::WtrDepRcv:
            static_assert(
                Trace_header_offsets::WtrDepRcv
                    == exseis::piol::segy::exseis_segy_tr_WtrDepRcv,
                "exseis::piol::Trace_header_offsets::WtrDepRcv and segy::exseis_segy_tr_WtrDepRcv should have the same value!");
            break;

        case Trace_header_offsets::ScaleElev:
            static_assert(
                Trace_header_offsets::ScaleElev
                    == exseis::piol::segy::exseis_segy_tr_ScaleElev,
                "exseis::piol::Trace_header_offsets::ScaleElev and segy::exseis_segy_tr_ScaleElev should have the same value!");
            break;

        case Trace_header_offsets::ScaleCoord:
            static_assert(
                Trace_header_offsets::ScaleCoord
                    == exseis::piol::segy::exseis_segy_tr_ScaleCoord,
                "exseis::piol::Trace_header_offsets::ScaleCoord and segy::exseis_segy_tr_ScaleCoord should have the same value!");
            break;

        case Trace_header_offsets::x_src:
            static_assert(
                Trace_header_offsets::x_src
                    == exseis::piol::segy::exseis_segy_tr_x_src,
                "exseis::piol::Trace_header_offsets::x_src and segy::exseis_segy_tr_x_src should have the same value!");
            break;

        case Trace_header_offsets::y_src:
            static_assert(
                Trace_header_offsets::y_src
                    == exseis::piol::segy::exseis_segy_tr_y_src,
                "exseis::piol::Trace_header_offsets::y_src and segy::exseis_segy_tr_y_src should have the same value!");
            break;

        case Trace_header_offsets::x_rcv:
            static_assert(
                Trace_header_offsets::x_rcv
                    == exseis::piol::segy::exseis_segy_tr_x_rcv,
                "exseis::piol::Trace_header_offsets::x_rcv and segy::exseis_segy_tr_x_rcv should have the same value!");
            break;

        case Trace_header_offsets::y_rcv:
            static_assert(
                Trace_header_offsets::y_rcv
                    == exseis::piol::segy::exseis_segy_tr_y_rcv,
                "exseis::piol::Trace_header_offsets::y_rcv and segy::exseis_segy_tr_y_rcv should have the same value!");
            break;

        case Trace_header_offsets::UpSrc:
            static_assert(
                Trace_header_offsets::UpSrc
                    == exseis::piol::segy::exseis_segy_tr_UpSrc,
                "exseis::piol::Trace_header_offsets::UpSrc and segy::exseis_segy_tr_UpSrc should have the same value!");
            break;

        case Trace_header_offsets::UpRcv:
            static_assert(
                Trace_header_offsets::UpRcv
                    == exseis::piol::segy::exseis_segy_tr_UpRcv,
                "exseis::piol::Trace_header_offsets::UpRcv and segy::exseis_segy_tr_UpRcv should have the same value!");
            break;

        case Trace_header_offsets::Ns:
            static_assert(
                Trace_header_offsets::Ns
                    == exseis::piol::segy::exseis_segy_tr_Ns,
                "exseis::piol::Trace_header_offsets::Ns and segy::exseis_segy_tr_Ns should have the same value!");
            break;

        case Trace_header_offsets::sample_interval:
            static_assert(
                Trace_header_offsets::sample_interval
                    == exseis::piol::segy::exseis_segy_tr_sample_interval,
                "exseis::piol::Trace_header_offsets::sample_interval and segy::exseis_segy_tr_sample_interval should have the same value!");
            break;

        case Trace_header_offsets::xCmp:
            static_assert(
                Trace_header_offsets::xCmp
                    == exseis::piol::segy::exseis_segy_tr_xCmp,
                "exseis::piol::Trace_header_offsets::xCmp and segy::exseis_segy_tr_xCmp should have the same value!");
            break;

        case Trace_header_offsets::yCmp:
            static_assert(
                Trace_header_offsets::yCmp
                    == exseis::piol::segy::exseis_segy_tr_yCmp,
                "exseis::piol::Trace_header_offsets::yCmp and segy::exseis_segy_tr_yCmp should have the same value!");
            break;

        case Trace_header_offsets::il:
            static_assert(
                Trace_header_offsets::il
                    == exseis::piol::segy::exseis_segy_tr_il,
                "exseis::piol::Trace_header_offsets::il and segy::exseis_segy_tr_il should have the same value!");
            break;

        case Trace_header_offsets::xl:
            static_assert(
                Trace_header_offsets::xl
                    == exseis::piol::segy::exseis_segy_tr_xl,
                "exseis::piol::Trace_header_offsets::xl and segy::exseis_segy_tr_xl should have the same value!");
            break;

        case Trace_header_offsets::ShotNum:
            static_assert(
                Trace_header_offsets::ShotNum
                    == exseis::piol::segy::exseis_segy_tr_ShotNum,
                "exseis::piol::Trace_header_offsets::ShotNum and segy::exseis_segy_tr_ShotNum should have the same value!");
            break;

        case Trace_header_offsets::ShotScal:
            static_assert(
                Trace_header_offsets::ShotScal
                    == exseis::piol::segy::exseis_segy_tr_ShotScal,
                "exseis::piol::Trace_header_offsets::ShotScal and segy::exseis_segy_tr_ShotScal should have the same value!");
            break;

        case Trace_header_offsets::ValMeas:
            static_assert(
                Trace_header_offsets::ValMeas
                    == exseis::piol::segy::exseis_segy_tr_ValMeas,
                "exseis::piol::Trace_header_offsets::ValMeas and segy::exseis_segy_tr_ValMeas should have the same value!");
            break;

        case Trace_header_offsets::TransConst:
            static_assert(
                Trace_header_offsets::TransConst
                    == exseis::piol::segy::exseis_segy_tr_TransConst,
                "exseis::piol::Trace_header_offsets::TransConst and segy::exseis_segy_tr_TransConst should have the same value!");
            break;

        case Trace_header_offsets::TransExp:
            static_assert(
                Trace_header_offsets::TransExp
                    == exseis::piol::segy::exseis_segy_tr_TransExp,
                "exseis::piol::Trace_header_offsets::TransExp and segy::exseis_segy_tr_TransExp should have the same value!");
            break;

        case Trace_header_offsets::TransUnit:
            static_assert(
                Trace_header_offsets::TransUnit
                    == exseis::piol::segy::exseis_segy_tr_TransUnit,
                "exseis::piol::Trace_header_offsets::TransUnit and segy::exseis_segy_tr_TransUnit should have the same value!");
            break;

        case Trace_header_offsets::TimeScal:
            static_assert(
                Trace_header_offsets::TimeScal
                    == exseis::piol::segy::exseis_segy_tr_TimeScal,
                "exseis::piol::Trace_header_offsets::TimeScal and segy::exseis_segy_tr_TimeScal should have the same value!");
            break;

        case Trace_header_offsets::SrcMeas:
            static_assert(
                Trace_header_offsets::SrcMeas
                    == exseis::piol::segy::exseis_segy_tr_SrcMeas,
                "exseis::piol::Trace_header_offsets::SrcMeas and segy::exseis_segy_tr_SrcMeas should have the same value!");
            break;

        case Trace_header_offsets::SrcMeasExp:
            static_assert(
                Trace_header_offsets::SrcMeasExp
                    == exseis::piol::segy::exseis_segy_tr_SrcMeasExp,
                "exseis::piol::Trace_header_offsets::SrcMeasExp and segy::exseis_segy_tr_SrcMeasExp should have the same value!");
            break;
    }
}
