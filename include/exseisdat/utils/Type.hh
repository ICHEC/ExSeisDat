////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Utilities for passing type information around dynamically.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_TYPE_HH
#define EXSEISDAT_UTILS_TYPE_HH

#include <cstdint>
#include <type_traits>

namespace exseis {
namespace utils {

/// An enumeration of all the arithmetic types used by ExSeisDat.
enum class Type : uint8_t {
    Double,
    Float,

    Int64,
    UInt64,

    Int32,
    UInt32,

    Int16,
    UInt16,

    Int8,
    UInt8,

    // Temporary!
    Index,
    Copy
};


/// @brief Get the \ref Type of a native data type.
template<typename T>
class Type_from_native;

/// @brief Get the native data type from the \ref Type.
template<Type T>
class Native_from_type;


namespace detail {

/// @brief Get the appropriate Type value from the type `T`.
/// @tparam  T The native type to get the Type value for.
/// @returns An appropriate Type value for T
/// @pre T is a built-in integer or floating point type.
template<typename T>
constexpr Type type_from_native_impl()
{
    static_assert(
      std::is_floating_point<T>() || std::is_integral<T>(),
      "Type_from_native is only defined for built-in floating point and integral types!");

    static_assert(
      sizeof(T) == 8 || sizeof(T) == 4 || sizeof(T) == 2 || sizeof(T) == 1,
      "Type_from_native is only supported for 64, 32, 16, or 8 bit types!");

    if (std::is_floating_point<T>::value) {
        switch (sizeof(T)) {
            case sizeof(double):
                return Type::Double;
            case sizeof(float):
                return Type::Float;
            default:
                return static_cast<Type>(-1);
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
        }
    }
}


/// @brief Helper mappings from Type value to native types
/// @{

template<Type T>
typename std::enable_if<T == Type::Double, double>::type
native_from_type_impl();

template<Type T>
typename std::enable_if<T == Type::Float, float>::type native_from_type_impl();

template<Type T>
typename std::enable_if<T == Type::Int64, int64_t>::type
native_from_type_impl();

template<Type T>
typename std::enable_if<T == Type::UInt64, uint64_t>::type
native_from_type_impl();

template<Type T>
typename std::enable_if<T == Type::Int32, int32_t>::type
native_from_type_impl();

template<Type T>
typename std::enable_if<T == Type::UInt32, uint32_t>::type
native_from_type_impl();

template<Type T>
typename std::enable_if<T == Type::Int16, int16_t>::type
native_from_type_impl();

template<Type T>
typename std::enable_if<T == Type::UInt16, uint16_t>::type
native_from_type_impl();

template<Type T>
typename std::enable_if<T == Type::Int8, int8_t>::type native_from_type_impl();

template<Type T>
typename std::enable_if<T == Type::UInt8, uint8_t>::type
native_from_type_impl();

/// @}

}  // namespace detail


template<typename T>
class Type_from_native {
  public:
    /// The Type value
    static constexpr Type value = detail::type_from_native_impl<T>();
};

template<Type T>
class Native_from_type {
  public:
    /// The native type
    using type = decltype(detail::native_from_type_impl<T>());
};


}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_UTILS_TYPE_HH
