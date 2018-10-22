#include "objsegytest.hh"

TEST_F(ObjSpecTest, SEGYHOWrite1)
{
    should_write_file_header_pattern(0, 107);
}

TEST_F(ObjSpecTest, SEGYHOWrite2)
{
    should_write_file_header_pattern(0, 46);
}

TEST_F(ObjSpecTest, SEGYHOWrite3)
{
    should_write_file_header_pattern(0, 0);
}

TEST_F(ObjSpecTest, SEGYWriteSingle1)
{
    write_test<Block::TRACE_METADATA>(10U, 1U, 200, 13, 13);
    write_test<Block::TRACE_DATA>(10U, 1U, 200, 13, 13);
    write_test<Block::TRACE>(10U, 1U, 200, 13, 13);
}

TEST_F(ObjSpecTest, SEGYWriteSingle2)
{
    write_test<Block::TRACE_METADATA>(10U, 1U, 200, 13, 117);
    write_test<Block::TRACE_DATA>(10U, 1U, 200, 13, 117);
    write_test<Block::TRACE>(10U, 1U, 200, 13, 117);
}

TEST_F(ObjSpecTest, SEGYWriteZeroNt)
{
    write_test<Block::TRACE_METADATA>(10U, 0U, 2000);
    write_test<Block::TRACE_DATA>(10U, 0U, 2000);
    write_test<Block::TRACE>(10U, 0U, 2000);
}

TEST_F(ObjSpecTest, SEGYWriteZeroNs)
{
    write_test<Block::TRACE_METADATA>(10U, 100U, 0U);
    write_test<Block::TRACE_DATA>(10U, 100U, 0U);
    write_test<Block::TRACE>(10U, 100U, 0U);
}

TEST_F(ObjSpecTest, SEGYWrite)
{
    write_test<Block::TRACE_METADATA>(10U, 100U, 2000);
    write_test<Block::TRACE_DATA>(10U, 100U, 2000);
    write_test<Block::TRACE>(10U, 100U, 2000);
}

TEST_F(ObjSpecTest, FarmSEGYBigWrite)
{
    write_test<Block::TRACE_METADATA>(10U, 300000, 5000);
    write_test<Block::TRACE_DATA>(10U, 300000, 5000);
    write_test<Block::TRACE>(10U, 300000, 5000);
}

// random read

TEST_F(ObjSpecTest, SEGYRandomWriteSingle1)
{
    auto vec = get_random_vec(1U, 1337);
    write_random_test<Block::TRACE_METADATA>(200, vec, 13);
    write_random_test<Block::TRACE_DATA>(200, vec, 13);
    write_random_test<Block::TRACE>(200, vec, 13);
}

TEST_F(ObjSpecTest, SEGYRandomWriteSingle2)
{
    auto vec = get_random_vec(1U, 1337);
    write_random_test<Block::TRACE_METADATA>(200, vec, 117);
    write_random_test<Block::TRACE_DATA>(200, vec, 117);
    write_random_test<Block::TRACE>(200, vec, 117);
}

TEST_F(ObjSpecTest, SEGYRandomWriteZeroNt)
{
    auto vec = get_random_vec(0U, 1337);
    write_random_test<Block::TRACE_METADATA>(2000, vec);
    write_random_test<Block::TRACE_DATA>(2000, vec);
    write_random_test<Block::TRACE>(2000, vec);
}

TEST_F(ObjSpecTest, SEGYRandomWriteZeroNs)
{
    auto vec = get_random_vec(100U, 1337);
    write_random_test<Block::TRACE_METADATA>(0U, vec);
    write_random_test<Block::TRACE_DATA>(0U, vec);
    write_random_test<Block::TRACE>(0U, vec);
}

TEST_F(ObjSpecTest, SEGYRandomWrite)
{
    auto vec = get_random_vec(100U, 1337);
    write_random_test<Block::TRACE_METADATA>(2000, vec);
    write_random_test<Block::TRACE_DATA>(2000, vec);
    write_random_test<Block::TRACE>(2000, vec);
}

TEST_F(ObjSpecTest, FarmSEGYRandomBigWrite)
{
    auto vec = get_random_vec(300000U, 1337);
    write_random_test<Block::TRACE_METADATA>(5000U, vec);
    write_random_test<Block::TRACE_DATA>(5000U, vec);
    write_random_test<Block::TRACE>(5000U, vec);
}
