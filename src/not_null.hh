////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
/// @details Tests for nullness and emptiness of pointers and shared_ptrs.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_SRC_NOT_NULL_HH
#define EXSEISDAT_SRC_NOT_NULL_HH

#include <memory>

/// Test if a pointer is null
/// @param t The pointer to test for nullness.
/// @return Returns true if \c t is null.
template<typename T>
static inline bool not_null(const T* t)
{
    return t != nullptr;
}

/// Test if a shared_ptr pointer is null, and test if its contents
/// is null.
/// @param t The pointer to shared_ptr to test for nullness.
/// @return Returns true if \c t is null, and its contents are not null.
template<typename T>
static inline bool not_null(const std::shared_ptr<T>* t)
{
    return t != nullptr && not_null(t->get());
}

#endif  // EXSEISDAT_SRC_NOT_NULL_HH
