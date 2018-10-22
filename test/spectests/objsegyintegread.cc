#include "objsegytest.hh"

TEST_F(ObjIntegTest, SEGYHORead1)
{
    make_real_segy<false>(plarge_file());
    read_ho_pattern_test<false>(0, 107);
}

TEST_F(ObjIntegTest, SEGYHORead2)
{
    make_real_segy<false>(plarge_file());
    read_ho_pattern_test<false>(0, 46);
}

TEST_F(ObjIntegTest, SEGYHORead3)
{
    make_real_segy<false>(plarge_file());
    read_ho_pattern_test<false>(0, 0);
}

TEST_F(ObjIntegTest, SEGYReadSingle1)
{
    make_real_segy<false>(plarge_file());
    read_test<Block::TRACE_METADATA, false>(10U, 1U, 200, 0, 13);
    read_test<Block::TRACE_DATA, false>(10U, 1U, 200, 0, 13);
    read_test<Block::TRACE, false>(10U, 1U, 200, 0, 13);
}

TEST_F(ObjIntegTest, SEGYReadSingle2)
{
    make_real_segy<false>(plarge_file());
    read_test<Block::TRACE_METADATA, false>(10U, 1U, 200, 0, 117);
    read_test<Block::TRACE_DATA, false>(10U, 1U, 200, 0, 117);
    read_test<Block::TRACE, false>(10U, 1U, 200, 0, 117);
}

TEST_F(ObjIntegTest, SEGYReadZeroNt)
{
    make_real_segy<false>(plarge_file());
    read_test<Block::TRACE_METADATA, false>(10U, 0U, 2000);
    read_test<Block::TRACE_DATA, false>(10U, 0U, 2000);
    read_test<Block::TRACE, false>(10U, 0U, 2000);
}

TEST_F(ObjIntegTest, SEGYReadZeroNs)
{
    make_real_segy<false>(plarge_file());
    read_test<Block::TRACE_METADATA, false>(10U, 100U, 0U);
    read_test<Block::TRACE_DATA, false>(10U, 100U, 0U);
    read_test<Block::TRACE, false>(10U, 100U, 0U);
}

TEST_F(ObjIntegTest, SEGYRead)
{
    make_real_segy<false>(plarge_file());
    read_test<Block::TRACE_METADATA, false>(10U, 100U, 2000);
    read_test<Block::TRACE_DATA, false>(10U, 100U, 2000);
    read_test<Block::TRACE, false>(10U, 100U, 2000);
}

TEST_F(ObjIntegTest, FarmSEGYBigRead)
{
    make_real_segy<false>(plarge_file());
    read_test<Block::TRACE_METADATA, false>(10U, 300000, 5000);
    read_test<Block::TRACE_DATA, false>(10U, 300000, 5000);
    read_test<Block::TRACE, false>(10U, 300000, 5000);
}

// Random reads
TEST_F(ObjIntegTest, SEGYRandomReadSingle1)
{
    make_real_segy<false>(plarge_file());
    auto vec = get_random_vec(1U, 1337);
    read_random_test<Block::TRACE_METADATA, false>(200U, vec, 117);
    read_random_test<Block::TRACE_DATA, false>(200U, vec, 117);
    read_random_test<Block::TRACE, false>(200U, vec, 117);
}

TEST_F(ObjIntegTest, SEGYRandomReadSingle2)
{
    make_real_segy<false>(plarge_file());
    auto vec = get_random_vec(1U, 1337);
    read_random_test<Block::TRACE_METADATA, false>(200U, vec, 13);
    read_random_test<Block::TRACE_DATA, false>(200U, vec, 13);
    read_random_test<Block::TRACE, false>(200U, vec, 13);
}

TEST_F(ObjIntegTest, SEGYRandomReadZeroNt)
{
    make_real_segy<false>(plarge_file());
    auto vec = get_random_vec(0, 1337);
    read_random_test<Block::TRACE_METADATA, false>(2000, vec);
    read_random_test<Block::TRACE_DATA, false>(2000, vec);
    read_random_test<Block::TRACE, false>(2000, vec);
}

TEST_F(ObjIntegTest, SEGYRandomReadZeroNs)
{
    make_real_segy<false>(plarge_file());
    auto vec = get_random_vec(100U, 1337);
    read_random_test<Block::TRACE_METADATA, false>(0U, vec);
    read_random_test<Block::TRACE_DATA, false>(0U, vec);
    read_random_test<Block::TRACE, false>(0U, vec);
}

TEST_F(ObjIntegTest, SEGYRandomRead)
{
    make_real_segy<false>(plarge_file());
    auto vec = get_random_vec(100U, 1337);
    read_random_test<Block::TRACE_METADATA, false>(2000U, vec);
    read_random_test<Block::TRACE_DATA, false>(2000U, vec);
    read_random_test<Block::TRACE, false>(2000U, vec);
}

TEST_F(ObjIntegTest, FarmSEGYRandomBigRead)
{
    make_real_segy<false>(plarge_file());
    auto vec = get_random_vec(300000, 1337);
    read_random_test<Block::TRACE_METADATA, false>(5000U, vec);
    read_random_test<Block::TRACE_DATA, false>(5000U, vec);
    read_random_test<Block::TRACE, false>(5000U, vec);
}
