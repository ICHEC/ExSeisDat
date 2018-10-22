#include "exseisdat/piol/Tr.h"
#include "exseisdat/piol/Tr.hh"

#include "gtest/gtest.h"

#include <type_traits>

namespace {
// Equality operator comparing exseis::piol::Tr and exseis_Tr
constexpr bool operator==(exseis::piol::Tr lhs, exseis_Tr rhs)
{
    return static_cast<exseis_Tr>(lhs) == rhs;
}
}  // namespace

TEST(Tr, C_API)
{

    // Test C and C++ enums have the same type.
    static_assert(
      std::is_same<
        std::underlying_type<exseis::piol::Tr>::type, exseis_Tr>::value,
      "exseis::piol::Tr and exseis_Tr should have the same type!");

    using exseis::piol::Tr;

    // Test C and C++ enums have the same values.
    // Use a compiler flag to test the switch covers the enum.
    switch (Tr::SeqNum) {
        case Tr::SeqNum:
            static_assert(
              Tr::SeqNum == exseis_tr_SeqNum,
              "exseis::piol::SeqNum and exseis_tr_SeqNum should have the same value!");
            break;

        case Tr::SeqFNum:
            static_assert(
              Tr::SeqFNum == exseis_tr_SeqFNum,
              "exseis::piol::Tr::SeqFNum and exseis_tr_SeqFNum should have the same value!");
            break;

        case Tr::ORF:
            static_assert(
              Tr::ORF == exseis_tr_ORF,
              "exseis::piol::Tr::ORF and exseis_tr_ORF should have the same value!");
            break;

        case Tr::TORF:
            static_assert(
              Tr::TORF == exseis_tr_TORF,
              "exseis::piol::Tr::TORF and exseis_tr_TORF should have the same value!");
            break;

        case Tr::ENSrcNum:
            static_assert(
              Tr::ENSrcNum == exseis_tr_ENSrcNum,
              "exseis::piol::Tr::ENSrcNum and exseis_tr_ENSrcNum should have the same value!");
            break;

        case Tr::SeqNumEns:
            static_assert(
              Tr::SeqNumEns == exseis_tr_SeqNumEns,
              "exseis::piol::Tr::SeqNumEns and exseis_tr_SeqNumEns should have the same value!");
            break;

        case Tr::TIC:
            static_assert(
              Tr::TIC == exseis_tr_TIC,
              "exseis::piol::Tr::TIC and exseis_tr_TIC should have the same value!");
            break;

        case Tr::VStackCnt:
            static_assert(
              Tr::VStackCnt == exseis_tr_VStackCnt,
              "exseis::piol::Tr::VStackCnt and exseis_tr_VStackCnt should have the same value!");
            break;

        case Tr::HStackCnt:
            static_assert(
              Tr::HStackCnt == exseis_tr_HStackCnt,
              "exseis::piol::Tr::HStackCnt and exseis_tr_HStackCnt should have the same value!");
            break;

        case Tr::CDist:
            static_assert(
              Tr::CDist == exseis_tr_CDist,
              "exseis::piol::Tr::CDist and exseis_tr_CDist should have the same value!");
            break;

        case Tr::RcvElv:
            static_assert(
              Tr::RcvElv == exseis_tr_RcvElv,
              "exseis::piol::Tr::RcvElv and exseis_tr_RcvElv should have the same value!");
            break;

        case Tr::SurfElvSrc:
            static_assert(
              Tr::SurfElvSrc == exseis_tr_SurfElvSrc,
              "exseis::piol::Tr::SurfElvSrc and exseis_tr_SurfElvSrc should have the same value!");
            break;

        case Tr::SrcDpthSurf:
            static_assert(
              Tr::SrcDpthSurf == exseis_tr_SrcDpthSurf,
              "exseis::piol::Tr::SrcDpthSurf and exseis_tr_SrcDpthSurf should have the same value!");
            break;

        case Tr::DtmElvRcv:
            static_assert(
              Tr::DtmElvRcv == exseis_tr_DtmElvRcv,
              "exseis::piol::Tr::DtmElvRcv and exseis_tr_DtmElvRcv should have the same value!");
            break;

        case Tr::DtmElvSrc:
            static_assert(
              Tr::DtmElvSrc == exseis_tr_DtmElvSrc,
              "exseis::piol::Tr::DtmElvSrc and exseis_tr_DtmElvSrc should have the same value!");
            break;

        case Tr::WtrDepSrc:
            static_assert(
              Tr::WtrDepSrc == exseis_tr_WtrDepSrc,
              "exseis::piol::Tr::WtrDepSrc and exseis_tr_WtrDepSrc should have the same value!");
            break;

        case Tr::WtrDepRcv:
            static_assert(
              Tr::WtrDepRcv == exseis_tr_WtrDepRcv,
              "exseis::piol::Tr::WtrDepRcv and exseis_tr_WtrDepRcv should have the same value!");
            break;

        case Tr::ScaleElev:
            static_assert(
              Tr::ScaleElev == exseis_tr_ScaleElev,
              "exseis::piol::Tr::ScaleElev and exseis_tr_ScaleElev should have the same value!");
            break;

        case Tr::ScaleCoord:
            static_assert(
              Tr::ScaleCoord == exseis_tr_ScaleCoord,
              "exseis::piol::Tr::ScaleCoord and exseis_tr_ScaleCoord should have the same value!");
            break;

        case Tr::x_src:
            static_assert(
              Tr::x_src == exseis_tr_x_src,
              "exseis::piol::Tr::x_src and exseis_tr_x_src should have the same value!");
            break;

        case Tr::y_src:
            static_assert(
              Tr::y_src == exseis_tr_y_src,
              "exseis::piol::Tr::y_src and exseis_tr_y_src should have the same value!");
            break;

        case Tr::x_rcv:
            static_assert(
              Tr::x_rcv == exseis_tr_x_rcv,
              "exseis::piol::Tr::x_rcv and exseis_tr_x_rcv should have the same value!");
            break;

        case Tr::y_rcv:
            static_assert(
              Tr::y_rcv == exseis_tr_y_rcv,
              "exseis::piol::Tr::y_rcv and exseis_tr_y_rcv should have the same value!");
            break;

        case Tr::UpSrc:
            static_assert(
              Tr::UpSrc == exseis_tr_UpSrc,
              "exseis::piol::Tr::UpSrc and exseis_tr_UpSrc should have the same value!");
            break;

        case Tr::UpRcv:
            static_assert(
              Tr::UpRcv == exseis_tr_UpRcv,
              "exseis::piol::Tr::UpRcv and exseis_tr_UpRcv should have the same value!");
            break;

        case Tr::Ns:
            static_assert(
              Tr::Ns == exseis_tr_Ns,
              "exseis::piol::Tr::Ns and exseis_tr_Ns should have the same value!");
            break;

        case Tr::sample_interval:
            static_assert(
              Tr::sample_interval == exseis_tr_sample_interval,
              "exseis::piol::Tr::sample_interval and exseis_tr_sample_interval should have the same value!");
            break;

        case Tr::xCmp:
            static_assert(
              Tr::xCmp == exseis_tr_xCmp,
              "exseis::piol::Tr::xCmp and exseis_tr_xCmp should have the same value!");
            break;

        case Tr::yCmp:
            static_assert(
              Tr::yCmp == exseis_tr_yCmp,
              "exseis::piol::Tr::yCmp and exseis_tr_yCmp should have the same value!");
            break;

        case Tr::il:
            static_assert(
              Tr::il == exseis_tr_il,
              "exseis::piol::Tr::il and exseis_tr_il should have the same value!");
            break;

        case Tr::xl:
            static_assert(
              Tr::xl == exseis_tr_xl,
              "exseis::piol::Tr::xl and exseis_tr_xl should have the same value!");
            break;

        case Tr::ShotNum:
            static_assert(
              Tr::ShotNum == exseis_tr_ShotNum,
              "exseis::piol::Tr::ShotNum and exseis_tr_ShotNum should have the same value!");
            break;

        case Tr::ShotScal:
            static_assert(
              Tr::ShotScal == exseis_tr_ShotScal,
              "exseis::piol::Tr::ShotScal and exseis_tr_ShotScal should have the same value!");
            break;

        case Tr::ValMeas:
            static_assert(
              Tr::ValMeas == exseis_tr_ValMeas,
              "exseis::piol::Tr::ValMeas and exseis_tr_ValMeas should have the same value!");
            break;

        case Tr::TransConst:
            static_assert(
              Tr::TransConst == exseis_tr_TransConst,
              "exseis::piol::Tr::TransConst and exseis_tr_TransConst should have the same value!");
            break;

        case Tr::TransExp:
            static_assert(
              Tr::TransExp == exseis_tr_TransExp,
              "exseis::piol::Tr::TransExp and exseis_tr_TransExp should have the same value!");
            break;

        case Tr::TransUnit:
            static_assert(
              Tr::TransUnit == exseis_tr_TransUnit,
              "exseis::piol::Tr::TransUnit and exseis_tr_TransUnit should have the same value!");
            break;

        case Tr::TimeScal:
            static_assert(
              Tr::TimeScal == exseis_tr_TimeScal,
              "exseis::piol::Tr::TimeScal and exseis_tr_TimeScal should have the same value!");
            break;

        case Tr::SrcMeas:
            static_assert(
              Tr::SrcMeas == exseis_tr_SrcMeas,
              "exseis::piol::Tr::SrcMeas and exseis_tr_SrcMeas should have the same value!");
            break;

        case Tr::SrcMeasExp:
            static_assert(
              Tr::SrcMeasExp == exseis_tr_SrcMeasExp,
              "exseis::piol::Tr::SrcMeasExp and exseis_tr_SrcMeasExp should have the same value!");
            break;
    }
}
