#include "tglobal.hh"

#include "exseisdat/piol/segy/utils.hh"

using namespace exseis::piol;

TEST(SEGYSizes, All)
{
    ASSERT_EQ(3600U, segy::segy_binary_file_header_size());
    ASSERT_EQ(240U, segy::segy_trace_header_size());
    ASSERT_EQ(
      3600U + (1111U * 4U + 240U) * 3333U,
      segy::segy_trace_location(3333, 1111));
}
