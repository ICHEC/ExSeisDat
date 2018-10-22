////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief A Generic array type for providing an interface using one type but
///        storage using another.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_GENERIC_ARRAY_HH
#define EXSEISDAT_UTILS_GENERIC_ARRAY_HH

#include "exseisdat/utils/Type.hh"

#include <limits>
#include <memory>
#include <type_traits>
#include <vector>

///
/// @namespace exseis::utils::generic_array
///
/// @brief The Generic_array class and related functions, for providing an
///        interface using one type, and underlying storage of another.
///

namespace exseis {
namespace utils {
inline namespace generic_array {


/// @brief A type-erased array.
///
/// This array provides an interface using one type, but stores the data
/// using another.
///
/// @tparam InterfaceType The type used in the get/set interfaces. It should be
///                       capable of representing the values of the underlying
///                       array.
///
/// @see make_generic_array
///
template<typename InterfaceType>
class Generic_array {

    static_assert(
      std::is_arithmetic<InterfaceType>::value,
      "Generic_array is only defined for arithmetic types!");

  public:
    /// The value type accepted and returned by get/set interfaces.
    using value_type = InterfaceType;

    /// The type used for indexing.
    using size_type = size_t;

  private:
    /// @brief Abstract base class to wrap the underlying array.
    class Concept;

    /// @brief Concrete instance of the abstract Concept class, implementing
    ///        storing the array of type StoredArray and providing an interface
    ///        using the InterfaceType.
    template<typename StoredArray>
    class Model;

    /// A pointer to the type-erased container.
    /// This will be an instance of Model<StoredArray>.
    std::unique_ptr<Concept> m_concept;

  public:
    /// @brief Templated constructor, accepts an array of type `StoredArray` and
    ///        uses it for the array storage.
    ///
    /// The input array is taken as a copy, so a user will likely want to
    /// pass the array as an rvalue to this constructor to avoid copies either
    /// by constructing it in-place, or using std::move.
    /// e.g.
    /// @code
    ///     auto generic_array =
    ///         Generic_array<InterfaceType>(std::vector<StorageType>(size));
    /// @endcode
    /// or
    /// @code
    ///     auto my_vector = std::vector<StorageType>;
    ///     my_vector.push_back(x);
    ///     my_vector.push_back(y);
    ///     my_vector.push_back(z);
    ///
    ///     auto generic_array =
    ///         Generic_array<InterfaceType>(std::move(my_vector));
    /// @endcode
    ///
    /// @tparam    StoredArray  The array type to use for the underlying
    ///                         storage.
    /// @param[in] stored_array The array to use for the underlying storage.
    ///
    /// @pre  min(StoredArray::value_type) >= min(InterfaceType)
    /// @pre  max(StoredArray::value_type) <= max(InterfaceType)
    /// @post size() == stored_array.size()
    /// @post capacity() == stored_array.capacity()
    ///
    template<typename StoredArray>
    Generic_array(StoredArray stored_array) :
        m_concept(std::make_unique<Model<StoredArray>>(std::move(stored_array)))
    {
        static_assert(
          std::numeric_limits<InterfaceType>::min()
            <= std::numeric_limits<typename StoredArray::value_type>::min(),
          "StoredArray must satisfy min(StoredArray::value_type) >= min(InterfaceType)");

        static_assert(
          std::numeric_limits<InterfaceType>::max()
            >= std::numeric_limits<typename StoredArray::value_type>::max(),
          "StoredArray must satisfy max(StoredArray::value_type) <= max(InterfaceType)");
    }


    /// @name Implicit members
    /// @{

    Generic_array(const Generic_array& other) :
        m_concept(other.m_concept->clone())
    {
    }

    Generic_array& operator=(const Generic_array& other)
    {
        if (this != &other) {
            Generic_array<value_type> copy(other);
            *this = std::move(copy);
        }
        return *this;
    }

    Generic_array(Generic_array&&) = default;
    Generic_array& operator=(Generic_array&&) = default;

    ~Generic_array() = default;

    /// @}


    /// @brief Set a value in the array.
    ///
    /// @param[in] index The index to set.
    /// @param[in] value The value to set.
    ///
    /// @pre  index < size()
    /// @post get(index) == value
    ///
    void set(size_type index, InterfaceType value)
    {
        m_concept->set(index, value);
    }


    /// @brief Get a value from the array.
    ///
    /// @param[in] index The index of the value to get.
    ///
    /// @returns The value at the given index.
    ///
    /// @pre index < size()
    ///
    InterfaceType get(size_type index) const { return m_concept->get(index); }


    /// @brief Get the `Type` of the underlying data.
    ///
    /// @returns The `Type` of the underlying data.
    ///
    Type type() const { return m_concept->type(); }


    /// @brief Get the number of entries in the array.
    ///
    /// @returns The number of entries in the array.
    ///
    size_type size() const { return m_concept->size(); }


    /// @brief Get the current reserved size of the array.
    ///
    /// @returns The current reserved size of the array.
    ///
    size_type capacity() const { return m_concept->capacity(); }


    /// @brief Get a pointer to the underlying data.
    ///
    /// @returns A pointer to the underlying data.
    ///
    const unsigned char* data() const { return m_concept->data(); }


    /// @copydoc Generic_array::data() const
    unsigned char* data() { return m_concept->data(); }
};


/// @brief Abstract base class to wrap the underlying array.
template<typename InterfaceType>
class Generic_array<InterfaceType>::Concept {
  public:
    /// The value type used in the get/set functions.
    using value_type = Generic_array<InterfaceType>::value_type;

    /// The type used for indexing.
    using size_type = Generic_array<InterfaceType>::size_type;


    /// Virtual destructor.
    virtual ~Concept() = default;

    /// @copydoc Generic_array::set()
    virtual void set(size_type index, value_type value) = 0;

    /// @copydoc Generic_array::get()
    virtual value_type get(size_type index) const = 0;

    /// @copydoc Generic_array::type()
    virtual Type type() const = 0;

    /// @copydoc Generic_array::size()
    virtual size_type size() const = 0;

    /// @copydoc Generic_array::capacity()
    virtual size_type capacity() const = 0;

    /// @copydoc Generic_array::data()
    virtual const unsigned char* data() const = 0;

    /// @copydoc Generic_array::data()
    virtual unsigned char* data() = 0;

    /// @brief Create a copy of the current instance.
    ///
    /// @returns A copy of the current instance.
    ///
    virtual std::unique_ptr<Concept> clone() const = 0;
};


/// @brief Concrete instance of the abstract base class, implementing
///        storing the array of type StoredArray and providing an interface
///        using the InterfaceType.
template<typename InterfaceType>
template<typename StoredArray>
class Generic_array<InterfaceType>::Model final
    : public Generic_array<InterfaceType>::Concept {
  public:
    /// The value type used by the underlying storage
    using stored_type = typename StoredArray::value_type;

    /// The value type used by the get/set functions
    using value_type = typename Concept::value_type;

    /// The type used for indexing.
    using size_type = typename Concept::size_type;


    /// The underlying storage
    StoredArray stored_array;


    /// @brief Initialize the stored_array.
    ///
    /// @param[in] stored_array The array to copy/move into the underlying
    ///                         storage.
    ///
    Model(StoredArray stored_array) : stored_array(std::move(stored_array)) {}

    void set(size_type index, value_type value) override
    {
        stored_array[index] = value;
    }

    value_type get(size_type index) const override
    {
        return stored_array[index];
    }

    Type type() const override { return Type_from_native<value_type>::value; }

    size_type size() const override { return stored_array.size(); }

    size_type capacity() const override { return stored_array.capacity(); }

    const unsigned char* data() const override
    {
        return reinterpret_cast<const unsigned char*>(stored_array.data());
    }

    unsigned char* data() override
    {
        return reinterpret_cast<unsigned char*>(stored_array.data());
    }

    std::unique_ptr<Concept> clone() const override
    {
        return std::make_unique<Model>(this->stored_array);
    }
};


}  // namespace generic_array
}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_UTILS_GENERIC_ARRAY_HH
