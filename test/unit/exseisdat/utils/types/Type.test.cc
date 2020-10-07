#include "exseisdat/utils/types/Type.hh"

#include "exseisdat/test/catch2.hh"

#include <type_traits>


// X-Macro for the "interesting" Types.
#define EXSEISDAT_X_TEST_TYPES(X)                                              \
    X(exseis::utils::Type::Double, double)                                     \
    X(exseis::utils::Type::Float, float)                                       \
    X(exseis::utils::Type::Int64, int64_t)                                     \
    X(exseis::utils::Type::UInt64, uint64_t)                                   \
    X(exseis::utils::Type::Int32, int32_t)                                     \
    X(exseis::utils::Type::UInt32, uint32_t)                                   \
    X(exseis::utils::Type::Int16, int16_t)                                     \
    X(exseis::utils::Type::UInt16, uint16_t)                                   \
    X(exseis::utils::Type::Int8, int8_t)                                       \
    X(exseis::utils::Type::UInt8, uint8_t)


// Test Type_from_native and Native_from_type return the expected values
TEST_CASE("Type", "[utils][Type]")
{
    using exseis::utils::Type;

    SECTION ("X Macro coverage") {
        // Compiler should complain if the switch doesn't cover the full enum.

#define EXSEISDAT_TEST_SWITCH_COVERAGE(ENUM_VALUE, TYPE)                       \
    case ENUM_VALUE:                                                           \
        break;

        switch (Type::Double) {
            EXSEISDAT_X_TEST_TYPES(EXSEISDAT_TEST_SWITCH_COVERAGE)

            case Type::Index:
                // Explicitly ignored
                break;
            case Type::Copy:
                // Explicitly ignored
                break;
        }

#undef EXSEISDAT_TEST_SWITCH_COVERAGE
    }


    SECTION ("Type enum support functions") {

// Test Type_from_native returns the correct Type,
// Test Native_from_type return the correct native type,
// Test Type_from_native is the inverse of Native_from_type,
// Test Native_from_type is the inverse of Type_from_native.
#define EXSEISDAT_TEST_TYPE_SUPPORT_FUNCTIONS(TYPE, NATIVE)                    \
    STATIC_REQUIRE(exseis::utils::Type_from_native<NATIVE>::value == TYPE);    \
    STATIC_REQUIRE(                                                            \
        std::is_same<                                                          \
            exseis::utils::Native_from_type<TYPE>::type, NATIVE>::value);      \
    STATIC_REQUIRE(                                                            \
        exseis::utils::Type_from_native<                                       \
            exseis::utils::Native_from_type<TYPE>::type>::value                \
        == TYPE);                                                              \
    STATIC_REQUIRE(std::is_same<                                               \
                   exseis::utils::Native_from_type<                            \
                       exseis::utils::Type_from_native<NATIVE>::value>::type,  \
                   NATIVE>::value);


        EXSEISDAT_X_TEST_TYPES(EXSEISDAT_TEST_TYPE_SUPPORT_FUNCTIONS)

#undef EXSEISDAT_TEST_TYPE_SUPPORT_FUNCTIONS
    }
}
