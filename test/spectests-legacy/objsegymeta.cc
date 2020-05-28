#include "objsegytest.hh"

TEST_F(ObjSpecTest, TestBypassConstructor)
{
    auto filename = make_segy();
    EXPECT_EQ(m_piol, m_obj->piol());
    EXPECT_EQ(filename, m_obj->name());
    EXPECT_EQ(m_mock_io_driver, m_obj->data());
}

TEST_F(ObjIntegTest, zeroSEGYFileSize)
{
    make_real_segy<false>(zero_file());
    m_piol->assert_ok();
    EXPECT_NE(nullptr, m_obj->data());
    EXPECT_EQ(0U, m_obj->get_file_size());
    m_piol->assert_ok();
}

TEST_F(ObjIntegTest, SmallSEGYFileSize)
{
    make_real_segy<false>(small_file());
    m_piol->assert_ok();
    EXPECT_NE(nullptr, m_obj->data());
    EXPECT_EQ(small_size, m_obj->get_file_size());
    m_piol->assert_ok();
}

TEST_F(ObjIntegTest, BigSEGYFileSize)
{
    make_real_segy<false>(large_file());
    m_piol->assert_ok();
    EXPECT_NE(nullptr, m_obj->data());
    EXPECT_EQ(large_size, m_obj->get_file_size());
    m_piol->assert_ok();
}

TEST_F(ObjSpecTest, ZeroSEGYFileSize)
{
    make_segy();
    segy_file_size_test(0U);
}

TEST_F(ObjSpecTest, SmallSEGYFileSize)
{
    make_segy();
    segy_file_size_test(40U * prefix(2U));
}

TEST_F(ObjSpecTest, BigSEGYFileSize)
{
    make_segy();
    segy_file_size_test(8U * prefix(4U));
}
