#include "exseisdat/utils/Type.hh"

#include "gtest/gtest.h"

#include <type_traits>

// Test Type_from_native and Native_from_type return the expected values
TEST(Type, StaticTests)
{
    using exseis::utils::Type;

#ifdef EXSEISDAT_TYPE_STATIC_TEST
#error EXSEISDAT_TYPE_STATIC_TEST already defined!
#endif

// Test Type_from_native returns the correct Type,
// Test Native_from_type return the correct native type,
// Test Type_from_native is the inverse of Native_from_type,
// Test Native_from_type is the inverse of Type_from_native.
#define EXSEISDAT_TYPE_STATIC_TEST(TYPE, NATIVE)                               \
    case TYPE:                                                                 \
        static_assert(                                                         \
          exseis::utils::Type_from_native<NATIVE>::value == TYPE,              \
          "Type_from_native<" #NATIVE ">::value should be " #TYPE "!");        \
        static_assert(                                                         \
          std::is_same<                                                        \
            exseis::utils::Native_from_type<TYPE>::type, NATIVE>::value,       \
          "Native_from_type<" #TYPE ">::type should be " #NATIVE "!");         \
        static_assert(                                                         \
          exseis::utils::Type_from_native<                                     \
            exseis::utils::Native_from_type<TYPE>::type>::value                \
            == TYPE,                                                           \
          "Type_from_native<Native_from_type<" #TYPE ">> should return " #TYPE \
          "!");                                                                \
        static_assert(                                                         \
          std::is_same<                                                        \
            exseis::utils::Native_from_type<                                   \
              exseis::utils::Type_from_native<NATIVE>::value>::type,           \
            NATIVE>::value,                                                    \
          "Native_from_type<Type_from_native<" #NATIVE                         \
          ">> should have type " #NATIVE "!");                                 \
        break;


    switch (Type::Int8) {
        EXSEISDAT_TYPE_STATIC_TEST(Type::Double, double)
        EXSEISDAT_TYPE_STATIC_TEST(Type::Float, float)
        EXSEISDAT_TYPE_STATIC_TEST(Type::Int64, int64_t)
        EXSEISDAT_TYPE_STATIC_TEST(Type::UInt64, uint64_t)
        EXSEISDAT_TYPE_STATIC_TEST(Type::Int32, int32_t)
        EXSEISDAT_TYPE_STATIC_TEST(Type::UInt32, uint32_t)
        EXSEISDAT_TYPE_STATIC_TEST(Type::Int16, int16_t)
        EXSEISDAT_TYPE_STATIC_TEST(Type::UInt16, uint16_t)
        EXSEISDAT_TYPE_STATIC_TEST(Type::Int8, int8_t)
        EXSEISDAT_TYPE_STATIC_TEST(Type::UInt8, uint8_t)
        case Type::Index:
            // Explicitly ignored
            break;
        case Type::Copy:
            // Explicitly ignored
            break;
    }

#undef EXSEISDAT_TYPE_STATIC_TEST
}
