#include "objsegytest.hh"

TEST_F(ObjIntegTest, SEGYHOWrite1)
{
    make_real_segy<true>(temp_file());
    should_write_file_header_pattern<false>(0, 107);
}

TEST_F(ObjIntegTest, SEGYHOWrite2)
{
    make_real_segy<true>(temp_file());
    should_write_file_header_pattern<false>(0, 46);
}

TEST_F(ObjIntegTest, SEGYHOWrite3)
{
    make_real_segy<true>(temp_file());
    should_write_file_header_pattern<false>(0, 0);
}

TEST_F(ObjIntegTest, SEGYWriteSingle1)
{
    make_real_segy<true>(temp_file());
    write_test<Block::TRACE_METADATA, false>(10U, 1U, 200, 0, 117);
    write_test<Block::TRACE_DATA, false>(10U, 1U, 200, 0, 117);
    write_test<Block::TRACE, false>(10U, 1U, 200, 0, 117);
}

TEST_F(ObjIntegTest, SEGYWriteSingle2)
{
    make_real_segy<true>(temp_file());
    write_test<Block::TRACE_METADATA, false>(10U, 1U, 200, 0, 13);
    write_test<Block::TRACE_DATA, false>(10U, 1U, 200, 0, 13);
    write_test<Block::TRACE, false>(10U, 1U, 200, 0, 13);
}

TEST_F(ObjIntegTest, SEGYWriteZeroNt)
{
    make_real_segy<true>(temp_file());
    write_test<Block::TRACE_METADATA, false>(10U, 0U, 2000);
    write_test<Block::TRACE_DATA, false>(10U, 0U, 200);
    write_test<Block::TRACE, false>(10U, 0U, 200);
}

TEST_F(ObjIntegTest, SEGYWriteZeroNs)
{
    make_real_segy<true>(temp_file());
    write_test<Block::TRACE_METADATA, false>(10U, 100U, 0U);
    write_test<Block::TRACE_DATA, false>(10U, 100U, 0U);
    write_test<Block::TRACE, false>(10U, 100U, 0U);
}

TEST_F(ObjIntegTest, SEGYWrite)
{
    make_real_segy<true>(temp_file());
    write_test<Block::TRACE_METADATA, false>(10U, 100U, 2000);
    write_test<Block::TRACE_DATA, false>(10U, 100U, 2000);
    write_test<Block::TRACE, false>(10U, 100U, 2000);
}

TEST_F(ObjIntegTest, FarmSEGYBigWrite)
{
    make_real_segy<true>(temp_file());
    write_test<Block::TRACE_METADATA, false>(10U, 300000, 5000);
    write_test<Block::TRACE_DATA, false>(10U, 300000, 5000);
    write_test<Block::TRACE, false>(10U, 300000, 5000);
}

// random write
TEST_F(ObjIntegTest, SEGYRandomWriteSingle1)
{
    make_real_segy<true>(temp_file());
    auto vec = get_random_vec(1U, 1337);
    write_random_test<Block::TRACE_METADATA, false>(200, vec, 117);
    write_random_test<Block::TRACE_DATA, false>(200, vec, 117);
    write_random_test<Block::TRACE, false>(200, vec, 117);
}

TEST_F(ObjIntegTest, SEGYRandomWriteSingle2)
{
    make_real_segy<true>(temp_file());
    auto vec = get_random_vec(1U, 1337);
    write_random_test<Block::TRACE_METADATA, false>(200, vec, 13);
    write_random_test<Block::TRACE_DATA, false>(200, vec, 13);
    write_random_test<Block::TRACE, false>(200, vec, 13);
}

TEST_F(ObjIntegTest, SEGYRandomWriteZeroNt)
{
    make_real_segy<true>(temp_file());
    auto vec = get_random_vec(0U, 1337);
    write_random_test<Block::TRACE_METADATA, false>(2000, vec);
    write_random_test<Block::TRACE_DATA, false>(2000, vec);
    write_random_test<Block::TRACE, false>(2000, vec);
}

TEST_F(ObjIntegTest, SEGYRandomWriteZeroNs)
{
    make_real_segy<true>(temp_file());
    auto vec = get_random_vec(100U, 1337);
    write_random_test<Block::TRACE_METADATA, false>(0U, vec);
    write_random_test<Block::TRACE_DATA, false>(0U, vec);
    write_random_test<Block::TRACE, false>(0U, vec);
}

TEST_F(ObjIntegTest, SEGYRandomWrite)
{
    make_real_segy<true>(temp_file());
    auto vec = get_random_vec(100U, 1337);
    write_random_test<Block::TRACE_METADATA, false>(2000, vec);
    write_random_test<Block::TRACE_DATA, false>(2000, vec);
    write_random_test<Block::TRACE, false>(2000, vec);
}

TEST_F(ObjIntegTest, FarmSEGYRandomBigWrite)
{
    make_real_segy<true>(temp_file());
    auto vec = get_random_vec(300000U, 1337);
    write_random_test<Block::TRACE_METADATA, false>(5000U, vec);
    write_random_test<Block::TRACE_DATA, false>(5000U, vec);
    write_random_test<Block::TRACE, false>(5000U, vec);
}
