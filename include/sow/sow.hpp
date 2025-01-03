/**
 * @file sow.hpp
 * @author Daniel Santos (dsantosp12@gmail.com)
 * @brief Save-On-Write (SOW) provides a way to persist a data structure when
 * it is modified. The persistency strategy is delegated to the data structure
 * itself via writer functions implemented by the user.
 * @date 2025-01-02
 *
 * @license MIT
 */
#ifndef SOW_SOW_HPP_
#define SOW_SOW_HPP_

#include <functional>
#include <type_traits>
#include <utility>

namespace sow {

// Forward declaration
template <typename S>
class SowGuard;

/**
 * @brief Helper function to create a SowGuard and execute a function with it.
 * 
 * @tparam S the Sow type
 * @tparam Func the function type
 * @param sow the Sow object
 * @param func the function to be executed
 */
template <typename S, typename Func>
void it(S& sow, Func&& func) {
  auto guard = sow.get_mut();
  std::forward<Func>(func)(*guard);
}

/**
 * @brief Owning class for a Save-On-Write (SOW) that holds the data.
 * 
 * @tparam T Type of the data.
* @tparam Writer Type of the writer that will be used to save the data.
 */
template <typename T, typename WriteFn>
class Sow {
 public:
  using value_type = T;
  using writer_type = WriteFn;
  using guard_type = SowGuard<Sow<value_type, writer_type>>;

  friend guard_type;

  /**
   * @brief Default constructor.
   */
  Sow() : _data(T{}), _writerFn(WriteFn{}) {}

  /**
   * @brief Construct a new Sow object.
   * 
   * @param data Data to be held.
   */
  explicit Sow(T data) : _data(data), _writerFn(WriteFn{}) {}

  /**
   * @brief Construct a new Sow object.
   * 
   * @param data Data to be held.
   * @param writeFn Function to be used to save the data.
   */
  Sow(T data, WriteFn writeFn) : _data(data), _writerFn(writeFn) {}

  // Disallow copy semantics
  Sow(const Sow&) = delete;
  Sow& operator=(const Sow&) = delete;

  // Disallow move semantics
  Sow(Sow&&) = delete;
  Sow& operator=(Sow&&) = delete;

  ~Sow() = default;

  // Read-only access
  /**
   * @brief Get a const reference to the data.
   * 
   * @return const T&
   */
  const value_type& get() const { return _data; }

  /**
   * @brief Get a const reference to the data.
   * 
   * @return const T*
   */
  const value_type& operator*() const { return _data; }

  /**
   * @brief Get a const pointer to the data.
   * 
   * @return const T*
   */
  const value_type* operator->() const { return &_data; }

  // Write access
  /**
   * @brief Get a mutable reference to the data in a guard.
   * 
   * @return SoWGuard<T>
   */
  guard_type get_mut() { return guard_type{*this}; }

 private:
  // Helper functions
  /**
   * @brief Save the data using the writer function.
   * 
   * @param data Data to be saved.
   */
  static void save(Sow<value_type, writer_type>& sow) {
    auto writer = sow._writerFn;
    if constexpr (std::is_pointer_v<writer_type>) {
      std::invoke(*writer, std::as_const(sow._data));
    } else {
      std::invoke(writer, std::as_const(sow._data));
    }
  }

  // Member variables
  value_type _data;
  writer_type _writerFn;
};

/**
 * @brief Guard class that saves the data when it goes out of scope.
 * 
 * @tparam S the Sow type
 */
template <typename S>
class SowGuard {
 public:
  using value_type = S;

  /**
   * @brief Construct a new Sow Guard object
   * 
   * @param sow the Sow object to guard.
   */
  explicit SowGuard(value_type& sow) : _sow(sow) {}

  /**
   * @brief Destroy the Sow Guard object and save the data.
   */
  ~SowGuard() { value_type::save(_sow); }

  // Disallow const constructor
  SowGuard(const value_type& data) = delete;

  // Disallow copy and move semantics
  SowGuard(const SowGuard&) = delete;
  SowGuard(SowGuard&&) = delete;
  SowGuard& operator=(const SowGuard&) = delete;
  SowGuard& operator=(SowGuard&&) = delete;

  /**
   * @brief Get a mutable pointer to the data.
   * 
   * @return S::value_type*
   */
  typename S::value_type* operator->() { return &_sow._data; }

  /**
   * @brief Get a mutable reference to the data.
   * 
   * @return S::value_type&
   */
  typename S::value_type& operator*() { return _sow._data; }

 private:
  value_type& _sow;
};

}  // namespace sow

#endif  // SOW_SOW_HPP_
