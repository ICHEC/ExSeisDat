#ifndef EXSEISDAT_TEST_INDEX_HASH_HH
#define EXSEISDAT_TEST_INDEX_HASH_HH

#include <cstddef>
#include <cstdint>

namespace exseis {
namespace test {

/// @brief An index hash for generating indexable data patterns
///
/// From Hash Prospector by Chris Wellons
/// https://nullprogram.com/blog/2018/07/31/
template<
    uint64_t A = UINT64_C(0xbf58476d1ce4e5b9),
    uint64_t B = UINT64_C(0x94d049bb133111eb)>
class Index_hash_64_impl {
  public:
    template<typename T = uint64_t>
    static constexpr T get(uint64_t x)
    {
        static_assert(
            sizeof(T) <= sizeof(uint64_t),
            "Expect sizeof(T) is <= sizeof(uint64_t)");

        x ^= x >> 30;
        x *= A;
        x ^= x >> 27;
        x *= B;
        x ^= x >> 31;

        return static_cast<T>(x);
    }

    constexpr uint64_t operator()(uint64_t x) const { return get<uint64_t>(x); }
};

/// @brief Default Index_hash
using Index_hash = Index_hash_64_impl<>;

}  // namespace test
}  // namespace exseis

#endif  // EXSEISDAT_TEST_INDEX_HASH_HH
