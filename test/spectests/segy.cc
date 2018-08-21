#include "tglobal.hh"

#include "ExSeisDat/PIOL/segy_utils.hh"

using namespace exseis::PIOL;

TEST(SEGYSizes, All)
{
    ASSERT_EQ(3600U, SEGY_utils::getHOSz());
    ASSERT_EQ(240U, SEGY_utils::getMDSz());
    ASSERT_EQ(
      3600U + (1111U * 4U + 240U) * 3333U, SEGY_utils::getDOLoc(3333, 1111));
}
