#ifndef EXSEISDAT_TEST_TEST_CASE_CACHE_HH
#define EXSEISDAT_TEST_TEST_CASE_CACHE_HH

#include "exseisdat/test/catch2.hh"

#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace exseis {
namespace test {

class Test_case_cache {
    /// @brief A list of deleter functions
    using Deleters = std::vector<std::function<void()>>;

    /// @brief   The list of deleters for items currently in the cache
    /// @returns The list of deleters for items currently in the cache
    static Deleters& deleters()
    {
        static Deleters deleters;
        return deleters;
    }

  public:
    /// @brief Get an item from the cache. If it doesn't exist, create it.
    ///
    /// @detail The cache is indexed by the types T and Tag.
    ///         For a type MyType, calling Test_case_cache::get<MyType>()
    ///         twice will return the same instance. To store multiple instances
    ///         of a type, use unique Tag types, e.g.
    ///             Test_case_cache::get<MyType, class tag_1>();
    ///             Test_case_cache::get<MyType, class tag_2>();
    ///
    /// @tparam T   The type to initialize and store in the cache.
    ///             Must be default constructable.
    /// @tparam Tag A tag type to distinguish between instances of T.
    ///
    /// @returns A reference to a default initialized T which will remain
    ///          alive until Test_case_cache::clear() is called, or until
    ///          the program's static destructors are called.
    ///
    template<typename T, typename Tag = void>
    static T& get()
    {
        // This will be nullptr if the item is not in the cache
        static std::unique_ptr<T> ptr = nullptr;

        if (ptr == nullptr) {
            // Create the item in the cache, and add a function to delete it
            // to deleters().
            ptr = std::make_unique<T>();
            deleters().push_back([]() { ptr = nullptr; });
        }

        return *ptr;
    }

    /// @brief Clear all the items from the cache
    static void clear()
    {
        // Run all deleters in reverse order of construction
        for (auto rit = deleters().rbegin(); rit != deleters().rend(); rit++) {
            auto& deleter = *rit;
            deleter();
        }
        // Drop all deleters
        deleters().clear();
    }
};


/// @brief Clear Test_case_cache at the start and end of each TEST_CASE
///
/// @detail This assumes Catch2 runs 1 TEST_CASE at a time.
struct Test_case_cache_listener : Catch::TestEventListenerBase {

    using TestEventListenerBase::TestEventListenerBase;

    // Test assumption that Catch2 is single-threaded
    bool m_test_case_running = false;

    void testCaseStarting(Catch::TestCaseInfo const& /*unused*/) override
    {
        Test_case_cache::clear();

        assert(m_test_case_running == false);
        m_test_case_running = true;
    }

    void testCaseEnded(Catch::TestCaseStats const& /*unused*/) override
    {
        Test_case_cache::clear();

        assert(m_test_case_running == true);
        m_test_case_running = false;
    }
};


}  // namespace test
}  // namespace exseis

#endif  // EXSEISDAT_TEST_TEST_CASE_CACHE_HH
