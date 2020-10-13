////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Utilities for passing type information around dynamically.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEIS_UTILS_TYPES_TYPE_HH
#define EXSEIS_UTILS_TYPES_TYPE_HH

#include <cassert>
#include <cstdint>
#include <type_traits>

namespace exseis {
inline namespace utils {
inline namespace types {

/// @brief X-Macro for Type ids
#define EXSEIS_X_TYPE(X)                                                       \
    X(Double, double)                                                          \
    X(Float, float)                                                            \
    X(Int64, int64_t)                                                          \
    X(UInt64, uint64_t)                                                        \
    X(Int32, int32_t)                                                          \
    X(UInt32, uint32_t)                                                        \
    X(Int16, int16_t)                                                          \
    X(UInt16, uint16_t)                                                        \
    X(Int8, int8_t)                                                            \
    X(UInt8, uint8_t)


/// @brief An enumeration of all the arithmetic types used by ExSeisDat.
enum class Type : uint8_t {
#define EXSEIS_DETAIL_MAKE_ENUM(ENUM, TYPE) /** Represents TYPE */ ENUM,

    EXSEIS_X_TYPE(EXSEIS_DETAIL_MAKE_ENUM)

#undef EXSEIS_DETAIL_MAKE_ENUM
};


namespace detail {

/// @brief Get the appropriate Type value from the type `T`.
/// @tparam  T The native type to get the Type value for.
/// @returns An appropriate Type value for T
/// @pre T is a built-in integer or floating point type.
template<typename T>
constexpr Type type_from_native_impl()
{
    static_assert(
        std::is_floating_point<T>::value || std::is_integral<T>::value,
        "Type_from_native is only defined for built-in floating point and integral types!");

    static_assert(
        (std::is_floating_point<T>::value && sizeof(T) == 8)
            || (std::is_floating_point<T>::value && sizeof(T) == 4)
            || (std::is_integral<T>::value && sizeof(T) == 8)
            || (std::is_integral<T>::value && sizeof(T) == 4)
            || (std::is_integral<T>::value && sizeof(T) == 2)
            || (std::is_integral<T>::value && sizeof(T) == 1),
        "Type_from_native only supports 64 and 32 bit floating-point types and 64, 32, 16, and 8 bit integral types.");

    constexpr Type failure = static_cast<Type>(-1);

    if (std::is_floating_point<T>::value) {
        switch (sizeof(T)) {
            case sizeof(double):
                return Type::Double;
            case sizeof(float):
                return Type::Float;
            default:
                return failure;
        }
    }

    if (std::is_integral<T>::value) {
        bool is_signed = std::is_signed<T>::value;
        switch (sizeof(T)) {
            case sizeof(int64_t):
                return is_signed ? Type::Int64 : Type::UInt64;
            case sizeof(int32_t):
                return is_signed ? Type::Int32 : Type::UInt32;
            case sizeof(int16_t):
                return is_signed ? Type::Int16 : Type::UInt16;
            case sizeof(int8_t):
                return is_signed ? Type::Int8 : Type::UInt8;
            default:
                return failure;
        }
    }

    return failure;
}


template<Type TypeId, typename T = void>
struct native_from_type_impl {
    static_assert(
        sizeof(T) == 0, "Native_from_type called for unknown Type Id.");
};

#define EXSEIS_DETAIL_MAKE_IMPL(ENUM, TYPE)                                    \
    template<>                                                                 \
    struct native_from_type_impl<Type::ENUM> {                                 \
        using type = TYPE;                                                     \
    };

EXSEIS_X_TYPE(EXSEIS_DETAIL_MAKE_IMPL)

#undef EXSEIS_DETAIL_MAKE_IMPL

}  // namespace detail


/// @brief Transforms a Type enum to an equivalent built-in type.
/// @tparam T The Type value to transform.
template<Type T>
using Native = typename detail::native_from_type_impl<T>::type;


/// @brief Transforms a built-in type to the equivalent Type enum.
/// @tparam T The built-in type to transform.
template<typename T>
constexpr Type type = detail::type_from_native_impl<T>();

}  // namespace types
}  // namespace utils
}  // namespace exseis

#endif  // EXSEIS_UTILS_TYPES_TYPE_HH
