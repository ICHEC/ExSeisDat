#include "exseis/utils/types/Type.hh"

#include "exseis/test/catch2.hh"

#include <type_traits>


// Test exseis::type<T> and exseis::Native<Type> return the expected values
TEST_CASE("Type", "[utils][Type]")
{
    using exseis::Type;

    SECTION ("X Macro coverage") {
        // Compiler should complain if the switch doesn't cover the full enum.

        switch (static_cast<Type>(0)) {
#define EXSEIS_TEST_SWITCH_COVERAGE(ENUM, TYPE)                                \
    case exseis::Type::ENUM:                                                   \
        break;

            EXSEIS_X_TYPE(EXSEIS_TEST_SWITCH_COVERAGE)

#undef EXSEIS_TEST_SWITCH_COVERAGE
        }
    }


    SECTION ("Type enum support functions") {

// Test exseis::type<T> returns the correct Type,
// Test exseis::Native<Type> return the correct native type,
// Test exseis::type<T> is the inverse of exseis::Native<Type>,
// Test exseis::Native<Type> is the inverse of exseis::type<T>.
#define EXSEIS_TEST_TYPE_SUPPORT_FUNCTIONS(ENUM, TYPE)                         \
    STATIC_REQUIRE(exseis::type<TYPE> == exseis::Type::ENUM);                  \
    REQUIRE(exseis::type<TYPE> == exseis::Type::ENUM);                         \
    STATIC_REQUIRE(                                                            \
        std::is_same<exseis::Native<exseis::Type::ENUM>, TYPE>::value);        \
    STATIC_REQUIRE(                                                            \
        exseis::type<                                                          \
            exseis::Native<exseis::Type::ENUM>> == exseis::Type::ENUM);        \
    REQUIRE(                                                                   \
        exseis::type<                                                          \
            exseis::Native<exseis::Type::ENUM>> == exseis::Type::ENUM);        \
    STATIC_REQUIRE(                                                            \
        std::is_same<exseis::Native<exseis::type<TYPE>>, TYPE>::value);


        EXSEIS_X_TYPE(EXSEIS_TEST_TYPE_SUPPORT_FUNCTIONS)

#undef EXSEIS_TEST_TYPE_SUPPORT_FUNCTIONS
    }

    SECTION ("Native<type<X>> ~ X") {
        // A round trip of Native<type<X>> should return
        // a type that's bitwise compatible with X

#define EXSEIS_TEST_TYPE_SUPPORT_IDENTITY(TYPE)                                \
    {                                                                          \
        INFO("Testing " #TYPE);                                                \
        using Round_trip_type = exseis::Native<exseis::type<TYPE>>;            \
        STATIC_REQUIRE(sizeof(Round_trip_type) == sizeof(TYPE));               \
        STATIC_REQUIRE(                                                        \
            std::is_floating_point<Round_trip_type>::value                     \
            == std::is_floating_point<TYPE>::value);                           \
        STATIC_REQUIRE(                                                        \
            std::is_signed<Round_trip_type>::value                             \
            == std::is_signed<TYPE>::value);                                   \
    }

        EXSEIS_TEST_TYPE_SUPPORT_IDENTITY(float)
        EXSEIS_TEST_TYPE_SUPPORT_IDENTITY(double)
        // EXSEIS_TEST_TYPE_SUPPORT_IDENTITY(long double)

        EXSEIS_TEST_TYPE_SUPPORT_IDENTITY(short int)
        EXSEIS_TEST_TYPE_SUPPORT_IDENTITY(unsigned short int)
        EXSEIS_TEST_TYPE_SUPPORT_IDENTITY(int)
        EXSEIS_TEST_TYPE_SUPPORT_IDENTITY(unsigned int)
        EXSEIS_TEST_TYPE_SUPPORT_IDENTITY(long int)
        EXSEIS_TEST_TYPE_SUPPORT_IDENTITY(unsigned long int)
        EXSEIS_TEST_TYPE_SUPPORT_IDENTITY(long long int)
        EXSEIS_TEST_TYPE_SUPPORT_IDENTITY(unsigned long long int)

        EXSEIS_TEST_TYPE_SUPPORT_IDENTITY(signed char)
        EXSEIS_TEST_TYPE_SUPPORT_IDENTITY(unsigned char)
        EXSEIS_TEST_TYPE_SUPPORT_IDENTITY(char)
        EXSEIS_TEST_TYPE_SUPPORT_IDENTITY(wchar_t)
        EXSEIS_TEST_TYPE_SUPPORT_IDENTITY(char16_t)
        EXSEIS_TEST_TYPE_SUPPORT_IDENTITY(char32_t)

#undef EXSEIS_TEST_TYPE_SUPPORT_IDENTITY
    }
}
