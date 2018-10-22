#include "datampiiotest.hh"

#include <sys/stat.h>

TEST_F(MPIIOTest, SetZeroFileSz)
{
    make_test_sz(0);
}

// Takes about 3 seconds on Fionn
TEST_F(MPIIOTest, FarmSetNormalFileSz)
{
    make_test_sz(2 * prefix(3));
}

TEST_F(MPIIOTest, FarmSetLargeFileSz)
{
    make_test_sz(10 * prefix(3));
}

TEST_F(MPIIOTest, WriteContigZero)
{
    make_mpiio<true>(temp_file());
    const size_t nt = 0;
    const size_t ns = 0;
    write_small_blocks<false>(nt, ns);
    m_piol->assert_ok();
}

TEST_F(MPIIOTest, WriteContigSSS)
{
    make_mpiio<true>(temp_file());
    const size_t nt = 400;
    const size_t ns = 261;
    write_small_blocks<false>(nt, ns);
    m_piol->assert_ok();
}

TEST_F(MPIIOTest, WriteContigSLS)
{
    make_mpiio<true>(temp_file());
    const size_t nt = 40U;
    const size_t ns = 40000U;
    write_small_blocks<false>(nt, ns, 1000U);
    m_piol->assert_ok();
}

TEST_F(MPIIOTest, FarmWriteContigSLM)
{
    make_mpiio<true>(temp_file());
    const size_t nt = 40000U;
    const size_t ns = 40000U;
    write_small_blocks<false>(nt, ns);
    m_piol->assert_ok();
}

TEST_F(MPIIOTest, FarmWriteContigSLL)
{
    make_mpiio<true>(temp_file());
    const size_t nt = 2000000U;
    const size_t ns = 1000U;
    write_small_blocks<false>(nt, ns);
    m_piol->assert_ok();
}

TEST_F(MPIIOTest, FarmWriteContigLLM)
{
    make_mpiio<true>(temp_file());
    const size_t nt = 40000U;
    const size_t ns = 40000U;
    write_big_blocks<false>(nt, ns);
    m_piol->assert_ok();
}

TEST_F(MPIIOTest, FarmWriteContigMLL)
{
    make_mpiio<true>(temp_file());
    const size_t nt = 2000000U;
    const size_t ns = 1000U;
    write_big_blocks<false>(nt, ns);
    m_piol->assert_ok();
}

TEST_F(MPIIOTest, WriteBlocksZero)
{
    make_mpiio<true>(temp_file());
    const size_t nt = 0;
    const size_t ns = 0;
    write_small_blocks<true>(nt, ns);
    m_piol->assert_ok();
}

TEST_F(MPIIOTest, WriteBlocksSSS)
{
    make_mpiio<true>(temp_file());
    const size_t nt = 400;
    const size_t ns = 261;
    write_small_blocks<true>(nt, ns);
    m_piol->assert_ok();
}

TEST_F(MPIIOTest, WriteBlocksSLS)
{
    make_mpiio<true>(temp_file());
    const size_t nt = 40U;
    const size_t ns = 40000U;
    write_small_blocks<true>(nt, ns, 1000U);
    m_piol->assert_ok();
}

TEST_F(MPIIOTest, FarmWriteBlocksSLM)
{
    make_mpiio<true>(temp_file());
    const size_t nt = 40000U;
    const size_t ns = 40000U;
    write_small_blocks<true>(nt, ns);
    m_piol->assert_ok();
}

TEST_F(MPIIOTest, FarmWriteBlocksSLL)
{
    make_mpiio<true>(temp_file());
    const size_t nt = 2000000U;
    const size_t ns = 1000U;
    write_small_blocks<true>(nt, ns);
    m_piol->assert_ok();
}

TEST_F(MPIIOTest, FarmWriteBlocksLLM)
{
    make_mpiio<true>(temp_file());
    const size_t nt = 40000U;
    const size_t ns = 40000U;
    write_big_blocks<true>(nt, ns);
    m_piol->assert_ok();
}

TEST_F(MPIIOTest, FarmWriteBlocksMLL)
{
    make_mpiio<true>(temp_file());
    const size_t nt = 2000000U;
    const size_t ns = 1000U;
    write_big_blocks<true>(nt, ns);
    m_piol->assert_ok();
}

////////Lists///////
TEST_F(MPIIOTest, WriteListZero)
{
    make_mpiio<true>(temp_file());
    const size_t nt = 0;
    const size_t ns = 0;
    write_list(nt, ns);
    m_piol->assert_ok();
}

TEST_F(MPIIOTest, WriteListSmall)
{
    make_mpiio<true>(temp_file());
    const size_t nt = 400;
    const size_t ns = 261;

    write_list(nt, ns);
    m_piol->assert_ok();
}

TEST_F(MPIIOTest, FarmWriteListLarge)
{
    make_mpiio<true>(temp_file());
    const size_t nt = 2000000U;
    const size_t ns = 1000U;
    write_list(nt, ns);
    m_piol->assert_ok();
}
