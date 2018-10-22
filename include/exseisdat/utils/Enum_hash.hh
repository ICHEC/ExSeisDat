////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The EnumHash class provides a hashing function for enum types,
///        needed for older compilers where std::hash doesn't handle enum
///        classes as expected.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_ENUM_HASH_HH
#define EXSEISDAT_UTILS_ENUM_HASH_HH

namespace exseis {
namespace utils {

/// @brief A hashing function for enums and enum classes.
///
/// Older compilers don't properly specialize std::hash for enums.
/// This class provides hashing for enums by casting them to their underlying
/// type and calling std::hash on that.
///
/// @tparam Enum The enum type to hash.
///
template<typename Enum>
class EnumHash {
  public:
    /// The key type for the hash
    using Key_type = Enum;

    /// The underlying type of the enum, used for the actual hashing
    using Underlying_type = typename std::underlying_type<Key_type>::type;

    /// The hasher type for the underlying type.
    using Underlying_hash = std::hash<Underlying_type>;

    /// The result of the underlying hasher, also the result of this hasher.
    using Result_type = typename Underlying_hash::result_type;

    /// An instance of the underlying hasher, used as the hasher for the
    /// enum values.
    Underlying_hash underlying_hash;


    /// @brief Calculate the hash of the argument.
    ///
    /// @param[in] key The enum value.
    ///
    /// @return Return a hash of the enum.
    ///
    Result_type operator()(Key_type key) const
    {
        return underlying_hash(static_cast<Underlying_type>(key));
    }
};

}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_UTILS_ENUM_HASH_HH
