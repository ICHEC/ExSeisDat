////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_DISTRIBUTED_VECTOR_HH
#define EXSEISDAT_UTILS_DISTRIBUTED_VECTOR_HH

#include <cassert>
#include <cstddef>
#include <memory>

namespace exseis {
namespace utils {

/// @brief  A structure for global arrays.
///
/// This classes uses a sort of Type Erasure to hide the implementation details
/// of deriving classes. This allows for both inheritance of an abstract
/// interface and value semantics.
///
/// @tparam T The type of variable to store in the vector.
///
template<typename T>
class Distributed_vector {
  protected:
    /// @brief An abstract interface for implementing the `Distributed_vector`.
    ///
    /// This class is used to implement the Type Erasure used in
    /// `Distributed_vector`.
    ///
    /// Classes deriving from Distributed_vector should derive from this
    /// class and put their actual implementation in here.
    /// A specialized instance of this `Concept` class should then be
    /// passed to the `Distributed_vector` constructor during construction of a
    /// child class.
    /// The `Distributed_vector` class will then call the virtual member
    /// functions of this class.
    ///
    /// This will be held in a std::unique_ptr in the `Distributed_vector`
    /// instance. This allows for specialization of the `Distributed_vector`
    /// class, along with value semantics while passing it around.
    ///
    class Concept {
      public:
        virtual ~Concept() = default;

        /// @copydoc Distributed_vector::set
        virtual void set(size_t i, const T& val) = 0;

        /// @copydoc Distributed_vector::get
        virtual T get(size_t i) const = 0;

        /// @copydoc Distributed_vector::size
        virtual size_t size() const = 0;
    };

    /// The instance of the private implementation for Distributed_vector.
    std::unique_ptr<Concept> m_concept;

    /// @brief Constructor passes instance of Concept to the
    ///        `m_concept` member variable.
    ///
    /// This is intended to be the point of entry for inheriting classes.
    ///
    /// @param[in] concept The inherited and specialized Concept
    ///                    class defined by a child class.
    Distributed_vector(std::unique_ptr<Concept> concept) :
        m_concept(std::move(concept))
    {
    }

  public:
    /// @name Defaulted implicit member functions.
    /// Move construction and assignment are enabled.
    /// @{
    ~Distributed_vector()                    = default;
    Distributed_vector(Distributed_vector&&) = default;
    Distributed_vector& operator=(Distributed_vector&&) = default;
    /// @}

    /// @name Deleted implicit member functions.
    /// Copy construction and assignment are disabled.
    /// @{
    Distributed_vector()                          = delete;
    Distributed_vector(const Distributed_vector&) = delete;
    Distributed_vector& operator=(const Distributed_vector&) = delete;
    /// @}

    /// @brief %Set the global ith element with the given tuple.
    ///
    /// @param[in] i The index into the global array.
    /// @param[in] val The value to be set.
    ///
    /// @pre  i < size()
    /// @post get(i) == val on all processes. (Explicit checking is subject to
    ///                                        race conditions!)
    ///
    void set(size_t i, const T& val)
    {
        assert(i < size());
        m_concept->set(i, val);
    }

    /// @brief Get the global ith element.
    ///
    /// @param[in] i The index into the global array.
    /// @return Return the value of the requested tuple.
    ///
    /// @pre i < size()
    ///
    T get(size_t i) const
    {
        assert(i < size());
        return m_concept->get(i);
    }

    /// @copydoc get
    T operator[](size_t i) const { return get(i); }

    /// @brief Get the number of elements in the global array.
    ///
    /// @return Return the number of elements in the global array.
    ///
    size_t size() const { return m_concept->size(); }
};

}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_UTILS_DISTRIBUTED_VECTOR_HH
