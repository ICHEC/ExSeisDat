#include "exseisdat/utils/logging/Verbosity.h"
#include "exseisdat/utils/logging/Verbosity.hh"

#include "gtest/gtest.h"

#include <type_traits>

namespace {
// Equality operator comparing exseis::utils::Verbosity and exseis_Verbosity.
constexpr bool operator==(exseis::utils::Verbosity lhs, exseis_Verbosity rhs)
{
    return static_cast<exseis_Verbosity>(lhs) == rhs;
}
}  // namespace

// Check the C++ and C API verbosity enums are equivalent, i.e. have equivalent
// types and values.
TEST(Verbosity, C_API)
{
    using exseis::utils::Verbosity;

    // Check C++ and C API have the same type.
    static_assert(
        std::is_same<
            std::underlying_type<Verbosity>::type, exseis_Verbosity>::value,
        "exseis::utils::Verbosity and exseis_Verbosity aren't equivalent types!");


    // Check C++ and C API have the same values.
    //
    // A compiler flag should be used to test the switch statement covers the
    // entire enum.
    switch (Verbosity::none) {
        case Verbosity::none:
            static_assert(
                Verbosity::none == exseis_verbosity_none,
                "exseis::utils::Verbosity::none and exseis_verbosity_none should have the same value!");
            break;

        case Verbosity::minimal:
            static_assert(
                Verbosity::minimal == exseis_verbosity_minimal,
                "exseis::utils::Verbosity::minimal and exseis_verbosity_minimal should have the same value!");
            break;

        case Verbosity::extended:
            static_assert(
                Verbosity::extended == exseis_verbosity_extended,
                "exseis::utils::Verbosity::extended and exseis_verbosity_extended should have the same value!");
            break;

        case Verbosity::verbose:
            static_assert(
                Verbosity::verbose == exseis_verbosity_verbose,
                "exseis::utils::Verbosity::verbose and exseis_verbosity_verbose should have the same value!");
            break;

        case Verbosity::max:
            static_assert(
                Verbosity::max == exseis_verbosity_max,
                "exseis::utils::Verbosity::max and exseis_verbosity_max should have the same value!");
            break;
    }
}
