#include "exseis/utils/types/Type.hh"

#include "exseis/test/catch2.hh"

#include <type_traits>


// X-Macro for the "interesting" Types.
#define EXSEIS_X_TEST_TYPES(X)                                                 \
    X(exseis::Type::Double, double)                                            \
    X(exseis::Type::Float, float)                                              \
    X(exseis::Type::Int64, int64_t)                                            \
    X(exseis::Type::UInt64, uint64_t)                                          \
    X(exseis::Type::Int32, int32_t)                                            \
    X(exseis::Type::UInt32, uint32_t)                                          \
    X(exseis::Type::Int16, int16_t)                                            \
    X(exseis::Type::UInt16, uint16_t)                                          \
    X(exseis::Type::Int8, int8_t)                                              \
    X(exseis::Type::UInt8, uint8_t)


// Test Type_from_native and Native_from_type return the expected values
TEST_CASE("Type", "[utils][Type]")
{
    using exseis::Type;

    SECTION ("X Macro coverage") {
        // Compiler should complain if the switch doesn't cover the full enum.

#define EXSEIS_TEST_SWITCH_COVERAGE(ENUM_VALUE, TYPE)                          \
    case ENUM_VALUE:                                                           \
        break;

        switch (Type::Double) {
            EXSEIS_X_TEST_TYPES(EXSEIS_TEST_SWITCH_COVERAGE)

            case Type::Index:
                // Explicitly ignored
                break;
            case Type::Copy:
                // Explicitly ignored
                break;
        }

#undef EXSEIS_TEST_SWITCH_COVERAGE
    }


    SECTION ("Type enum support functions") {

// Test Type_from_native returns the correct Type,
// Test Native_from_type return the correct native type,
// Test Type_from_native is the inverse of Native_from_type,
// Test Native_from_type is the inverse of Type_from_native.
#define EXSEIS_TEST_TYPE_SUPPORT_FUNCTIONS(TYPE, NATIVE)                       \
    STATIC_REQUIRE(exseis::Type_from_native<NATIVE>::value == TYPE);           \
    STATIC_REQUIRE(                                                            \
        std::is_same<exseis::Native_from_type<TYPE>::type, NATIVE>::value);    \
    STATIC_REQUIRE(                                                            \
        exseis::Type_from_native<exseis::Native_from_type<TYPE>::type>::value  \
        == TYPE);                                                              \
    STATIC_REQUIRE(std::is_same<                                               \
                   exseis::Native_from_type<                                   \
                       exseis::Type_from_native<NATIVE>::value>::type,         \
                   NATIVE>::value);


        EXSEIS_X_TEST_TYPES(EXSEIS_TEST_TYPE_SUPPORT_FUNCTIONS)

#undef EXSEIS_TEST_TYPE_SUPPORT_FUNCTIONS
    }
}
