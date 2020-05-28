////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_DISTRIBUTED_VECTOR_DISTRIBUTED_VECTOR_HH
#define EXSEISDAT_UTILS_DISTRIBUTED_VECTOR_DISTRIBUTED_VECTOR_HH

#include <cassert>
#include <cstddef>
#include <memory>

namespace exseis {
namespace utils {
inline namespace distributed_vector {

template<typename ValueType>
class Distributed_vector_reference;

/// @brief  A structure for global arrays.
///
/// This classes uses a sort of Type Erasure to hide the implementation details
/// of deriving classes. This allows for both inheritance of an abstract
/// interface and value semantics.
///
/// @tparam ValueType The type of variable to store in the vector.
///
template<typename ValueType>
class Distributed_vector {
  public:
    /// The data type stored by the vector.
    using value_type = ValueType;

    /// An indirect "reference" type for the vector.
    using reference = Distributed_vector_reference<value_type>;

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
        /// @virtual_destructor
        virtual ~Concept() = default;

        /// @copydoc Distributed_vector::get
        virtual value_type get(size_t i) const = 0;

        /// @copydoc Distributed_vector::set
        virtual void set(size_t i, const value_type& val) = 0;

        /// @copydoc Distributed_vector::get_n
        virtual void get_n(
            size_t offset, size_t count, value_type* buffer) const = 0;

        /// @copydoc Distributed_vector::set_n
        virtual void set_n(
            size_t offset, size_t count, const value_type* buffer) = 0;

        /// @copydoc Distributed_vector::size
        virtual size_t size() const = 0;

        /// @copydoc Distributed_vector::resize
        virtual void resize(size_t new_size) = 0;

        /// @copydoc Distributed_vector::sync
        virtual void sync() = 0;
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
    /// @name @special_member_functions
    /// @{

    /// @default_constructor{delete}
    Distributed_vector() = delete;

    /// @default_destructor
    ~Distributed_vector() = default;

    /// @move_constructor{default}
    Distributed_vector(Distributed_vector&&) = default;
    /// @move_assignment{default}
    Distributed_vector& operator=(Distributed_vector&&) = default;

    /// @copy_constructor{delete}
    Distributed_vector(const Distributed_vector&) = delete;
    /// @copy_assignment{delete}
    Distributed_vector& operator=(const Distributed_vector&) = delete;

    /// @}

    /// @brief Get the global ith element.
    ///
    /// @param[in] i The index into the global array.
    /// @return Return the value of the requested element.
    ///
    /// @pre i < size()
    ///
    value_type get(size_t i) const
    {
        assert(i < size());
        return m_concept->get(i);
    }

    /// @brief %Set the global ith element with the given value.
    ///
    /// @param[in] i The index into the global array.
    /// @param[in] val The value to be set.
    ///
    /// @pre  i < size()
    /// @post get(i) == val on all processes. (Explicit checking is subject to
    ///                                        race conditions!)
    ///
    void set(size_t i, const value_type& val)
    {
        assert(i < size());
        m_concept->set(i, val);
    }

    /// @copydoc get
    value_type operator[](size_t i) const { return get(i); }

    /// @brief Get a settable object representing the global ith element.
    ///
    /// @param[in] i The index into the global array.
    ///
    /// @returns A "reference" object containing a pointer to the current
    ///          instance and the requested index, along with a copy assignment
    ///          operator.
    ///
    reference operator[](size_t i) { return {this, i}; }

    /// @brief %Get a contiguous chunk of the distributed array.
    ///
    /// @param[in]  offset The global offset into the distributed array.
    /// @param[in]  count  The number of elements to read into `buffer`.
    /// @param[out] buffer A pointer to an array buffer of size `count` to
    ///                    get the data into.
    void get_n(size_t offset, size_t count, value_type* buffer) const
    {
        m_concept->get_n(offset, count, buffer);
    }

    /// @brief %Set a contiguous chunk of the distributed array.
    ///
    /// @param[in]  offset The global offset into the distributed array.
    /// @param[in]  count  The number of elements to read into `buffer`.
    /// @param[out] buffer A pointer to an array buffer of size `count` to
    ///                    set the data from.
    void set_n(size_t offset, size_t count, const value_type* buffer)
    {
        m_concept->set_n(offset, count, buffer);
    }

    /// @brief Get the number of elements in the global array.
    ///
    /// @return Return the number of elements in the global array.
    ///
    size_t size() const { return m_concept->size(); }

    /// @brief Resize the global array.
    ///
    /// Values in the array before resizing, at an index less than `new_size`,
    /// will continue to be available in the newly sized array.
    ///
    /// If `new_size` is larger than the current size, the newly added
    /// elements will be default initialized.
    ///
    /// @param[in] new_size The new size of the global array.
    void resize(size_t new_size) { m_concept->resize(new_size); }

    /// @brief Make writes visible to all processes
    void sync() { m_concept->sync(); }
};


/// @brief A "reference" object to provide \ref Distributed_vector with
///        subscript-assignment semantics.
///
/// @tparam ValueType The type the "reference" is referencing.
///
template<typename ValueType>
class Distributed_vector_reference {
  public:
    /// The type of the referenced value.
    using value_type = ValueType;

  private:
    /// A pointer to container the reference references
    Distributed_vector<value_type>* m_distributed_vector;

    /// An index into the container to the element the reference references.
    size_t m_index;

  public:
    /// @name @special_member_functions
    /// @{

    /// @brief Construct a reference to `distributed_vector`
    /// @param[in] distributed_vector A pointer to the `Distributed_vector` to
    ///                               reference.
    /// @param[in] index              A global index into `distributed_vector`
    ///                               to the element to reference.
    Distributed_vector_reference(
        Distributed_vector<value_type>* distributed_vector, size_t index) :
        m_distributed_vector(distributed_vector), m_index(index)
    {
    }

    /// @default_constructor{deleted}
    Distributed_vector_reference() = delete;

    /// @copy_constructor{deleted}
    Distributed_vector_reference(const Distributed_vector_reference&) = delete;

    /// @copy_assignment{reference semantics}
    /// @param[in] other The value to copy from.
    Distributed_vector_reference& operator=(
        const Distributed_vector_reference& other)
    {
        if (this != &other) {
            set(other.get(m_index));
        }
        return *this;
    }

    /// @copy_assignment{reference semantics}
    /// @param[in] other The value to copy from.
    Distributed_vector_reference& operator=(const value_type& other)
    {
        set(other);
        return *this;
    }

    /// @move_constructor{deleted}
    Distributed_vector_reference(Distributed_vector_reference&&) = delete;

    /// @move_assignment{reference semantics}
    /// @param[inout] other The value to move from.
    Distributed_vector_reference& operator=(
        Distributed_vector_reference&& other)
    {
        if (this != &other) {
            set(std::move(other.get()));
        }
    }

    /// @move_assignment{reference}
    /// @param[inout] other The value to move from.
    Distributed_vector_reference& operator=(value_type&& other)
    {
        set(other);
        return *this;
    }

    /// @default_destructor
    ~Distributed_vector_reference() = default;

    /// @}

    /// @brief Implicitly convert to `value_type`.
    operator value_type() const { return get(); }

    /// @brief Get the referenced value.
    /// @returns The referenced value.
    value_type get() const { return m_distributed_vector->get(m_index); }

    /// @brief Set the referenced value.
    /// @param[in] value The value to set from.
    void set(const value_type& value)
    {
        m_distributed_vector->set(m_index, value);
    }
};

}  // namespace distributed_vector
}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_UTILS_DISTRIBUTED_VECTOR_DISTRIBUTED_VECTOR_HH
