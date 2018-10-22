#ifndef EXSEISDAT_TEST_SPECTESTS_TGLOBAL_HH
#define EXSEISDAT_TEST_SPECTESTS_TGLOBAL_HH

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "exseisdat/utils/typedefs.hh"

#include <cstdlib>
#include <string>

extern const size_t magic_num1;
extern const size_t small_size;
extern const size_t large_size;
extern const size_t large_segy_size;

std::string zero_file();
std::string small_file();
std::string large_file();
// Large file with a pattern
std::string plarge_file();
std::string temp_file();
std::string temp_file_segy();
std::string small_segy_file();
std::string large_segy_file();
std::string big_trace_segy_file();

/// Generate a unique filename. MPI Collective call.
std::string nonexistant_filename();

extern int32_t il_num(size_t);
extern int32_t xl_num(size_t);
extern exseis::utils::Floating_point x_num(size_t i);
extern exseis::utils::Floating_point y_num(size_t i);
extern unsigned char get_pattern(size_t);
extern std::vector<size_t> get_random_vec(size_t nt, int seed);
extern std::vector<size_t> get_random_vec(size_t nt, size_t max, int seed);


// List of TypeIdHelper<T> instances to avoid global weak variables due
// to its static data member.
class MPIIOTest;
extern template bool testing::internal::TypeIdHelper<MPIIOTest>::dummy_;

struct RuleFixList;
extern template bool testing::internal::TypeIdHelper<RuleFixList>::dummy_;

struct RuleFixEmpty;
extern template bool testing::internal::TypeIdHelper<RuleFixEmpty>::dummy_;

struct RuleFixDefault;
extern template bool testing::internal::TypeIdHelper<RuleFixDefault>::dummy_;

struct FileReadSEGYTest;
extern template bool testing::internal::TypeIdHelper<FileReadSEGYTest>::dummy_;

extern template typename testing::DefaultValue<unsigned long>::ValueProducer*
  testing::DefaultValue<unsigned long>::producer_;

struct FileWriteSEGYTest;
extern template bool testing::internal::TypeIdHelper<FileWriteSEGYTest>::dummy_;

struct SetTest;
extern template bool testing::internal::TypeIdHelper<SetTest>::dummy_;

extern template typename testing::DefaultValue<double>::ValueProducer*
  testing::DefaultValue<double>::producer_;


constexpr size_t prefix(const size_t pow)
{
    return (pow != 0 ? 1024U * prefix(pow - 1U) : 1U);
}

ACTION_P2(check0, buf, sz)
{
    for (size_t i = 0; i < sz; i++) {
        ASSERT_EQ(buf[i], arg0[i]) << "Error with byte: " << i << "\n";
    }
}

ACTION_P2(check1, buf, sz)
{
    for (size_t i = 0; i < sz; i++) {
        ASSERT_EQ(buf[i], arg1[i]) << "Error with byte: " << i << "\n";
    }
}

ACTION_P2(check2, buf, sz)
{
    for (size_t i = 0; i < sz; i++) {
        ASSERT_EQ(buf[i], arg2[i]) << "Error with byte: " << i << "\n";
    }
}

ACTION_P2(check3, buf, sz)
{
    for (size_t i = 0; i < sz; i++) {
        ASSERT_EQ(buf[i], arg3[i]) << "Error with byte: " << i << "\n";
    }
}

ACTION_P2(check4, buf, sz)
{
    for (size_t i = 0; i < sz; i++) {
        ASSERT_EQ(buf[i], arg4[i]) << "Error with byte: " << i << "\n";
    }
}

#endif  // EXSEISDAT_TEST_SPECTESTS_TGLOBAL_HH
