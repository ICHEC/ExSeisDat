#include "objsegytest.hh"

TEST_F(ObjSpecTest, SEGYHORead1)
{
    read_ho_pattern_test(7, 107);
}

TEST_F(ObjSpecTest, SEGYHORead2)
{
    read_ho_pattern_test(10, 46);
}

TEST_F(ObjSpecTest, SEGYHORead3)
{
    read_ho_pattern_test(0, 0);
}

TEST_F(ObjSpecTest, SEGYReadSingle1)
{
    read_test<Block::TRACE_METADATA>(10U, 1U, 200, 13, 117);
    read_test<Block::TRACE_DATA>(10U, 1U, 200, 13, 117);
    read_test<Block::TRACE>(10U, 1U, 200, 13, 117);
}

TEST_F(ObjSpecTest, SEGYReadSingle2)
{
    read_test<Block::TRACE_METADATA>(10U, 1U, 200, 13, 13);
    read_test<Block::TRACE_DATA>(10U, 1U, 200, 13, 13);
    read_test<Block::TRACE>(10U, 1U, 200, 13, 13);
}

TEST_F(ObjSpecTest, SEGYReadZeroNt)
{
    read_test<Block::TRACE_METADATA>(10U, 0U, 2000);
    read_test<Block::TRACE_DATA>(10U, 0U, 2000);
    read_test<Block::TRACE>(10U, 0U, 2000);
}

TEST_F(ObjSpecTest, SEGYReadZeroNs)
{
    read_test<Block::TRACE_METADATA>(10U, 100U, 0U);
    read_test<Block::TRACE_DATA>(10U, 100U, 0U);
    read_test<Block::TRACE>(10U, 100U, 0U);
}

TEST_F(ObjSpecTest, SEGYRead)
{
    read_test<Block::TRACE_METADATA>(10U, 100U, 2000);
    read_test<Block::TRACE_DATA>(10U, 100U, 2000);
    read_test<Block::TRACE>(10U, 100U, 2000);
}

TEST_F(ObjSpecTest, FarmSEGYBigRead)
{
    read_test<Block::TRACE_METADATA>(10U, 300000, 5000);
    read_test<Block::TRACE_DATA>(10U, 300000, 5000);
    read_test<Block::TRACE>(10U, 300000, 5000);
}

// random read

TEST_F(ObjSpecTest, SEGYRandomReadSingle1)
{
    auto vec = get_random_vec(1U, 1337);
    read_random_test<Block::TRACE_METADATA>(200, vec, 117);
    read_random_test<Block::TRACE_DATA>(200, vec, 117);
    read_random_test<Block::TRACE>(200, vec, 117);
}

TEST_F(ObjSpecTest, SEGYRandomReadSingle2)
{
    auto vec = get_random_vec(1U, 1337);
    read_random_test<Block::TRACE_METADATA>(200, vec, 13);
    read_random_test<Block::TRACE_DATA>(200, vec, 13);
    read_random_test<Block::TRACE>(200, vec, 13);
}

TEST_F(ObjSpecTest, SEGYRandomReadZeroNt)
{
    auto vec = get_random_vec(0U, 1337);
    read_random_test<Block::TRACE_METADATA>(2000, vec);
    read_random_test<Block::TRACE_DATA>(2000, vec);
    read_random_test<Block::TRACE>(2000, vec);
}

TEST_F(ObjSpecTest, SEGYRandomReadZeroNs)
{
    auto vec = get_random_vec(100U, 1337);
    read_random_test<Block::TRACE_METADATA>(0U, vec);
    read_random_test<Block::TRACE_DATA>(0U, vec);
    read_random_test<Block::TRACE>(0U, vec);
}

TEST_F(ObjSpecTest, SEGYRandomRead)
{
    auto vec = get_random_vec(100U, 1337);
    read_random_test<Block::TRACE_METADATA>(2000, vec);
    read_random_test<Block::TRACE_DATA>(2000, vec);
    read_random_test<Block::TRACE>(2000, vec);
}

TEST_F(ObjSpecTest, FarmSEGYRandomBigRead)
{
    auto vec = get_random_vec(300000U, 1337);
    read_random_test<Block::TRACE_METADATA>(5000, vec);
    read_random_test<Block::TRACE_DATA>(5000, vec);
    read_random_test<Block::TRACE>(5000, vec);
}
