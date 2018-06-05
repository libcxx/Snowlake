/*******************************************************************************
The MIT License (MIT)

Copyright (c) 2018 William Li

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*******************************************************************************/

#pragma once

#include "variant_dispatcher.h"
#include "variant_impl.h"

#include <cstdlib>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace sl {
namespace variant {

// -----------------------------------------------------------------------------

template <typename... Types>
class variant
{
private:
  static const std::size_t data_size =
      impl::static_max<sizeof(Types)...>::value;
  static const std::size_t data_align =
      impl::static_max<alignof(Types)...>::value;

  using data_type = typename std::aligned_storage<data_size, data_align>::type;
  using helper_type = impl::variant_helper<Types...>;

  std::size_t m_type_index;
  data_type m_data;

  void swap(variant<Types...>& lhs, variant<Types...>& rhs)
  {
    std::swap(lhs.m_type_index, rhs.m_type_index);
    std::swap(lhs.m_data, rhs.m_data);
  }

public:
  variant()
    : m_type_index(impl::invalid_type_index)
  {
  }

  variant(variant<Types...> const& other)
    : m_type_index(other.m_type_index)
  {
    helper_type::copy(other.m_type_index, &other.m_data, &m_data);
  }

  variant(variant<Types...>&& other) noexcept
    : m_type_index(other.m_type_index)
  {
    helper_type::move(other.m_type_index, &other.m_data, &m_data);
    other.m_type_index = impl::invalid_type_index;
  }

  template <
      typename T,
      class = typename std::enable_if<impl::is_valid_type<
          typename std::remove_reference<T>::type, Types...>::value>::type>
  variant(T&& val) noexcept
    : m_type_index(impl::value_traits<typename std::remove_reference<T>::type,
                                      Types...>::index)
  {
    constexpr std::size_t index =
        sizeof...(Types) -
        impl::value_traits<typename std::remove_reference<T>::type,
                           Types...>::index -
        1;

    using target_type = typename impl::select_type<index, Types...>::type;
    new (&m_data) target_type(std::forward<T>(val));
  }

  ~variant() noexcept
  {
    helper_type::destroy(m_type_index, &m_data);
  }

  variant<Types...>& operator=(variant<Types...> other)
  {
    swap(*this, other);
    return *this;
  }

  template <typename T>
  variant<Types...>& operator=(T const& other)
  {
    variant<Types...> temp(other);
    swap(*this, temp);
    return *this;
  }

  template <typename T>
  variant<Types...>& operator=(T&& rhs) noexcept
  {
    variant<Types...> temp(std::forward<T>(rhs));
    swap(*this, temp);
    return *this;
  }

  bool operator==(variant const& rhs) const
  {
    if (type_index() != rhs.type_index()) {
      return false;
    }

    impl::comparer<variant, impl::equal_to> visitor(*this);
    return visit(rhs, visitor);
  }

  bool operator!=(variant const& rhs) const
  {
    return !operator==(rhs);
  }

  template <typename... Types2>
  bool operator==(variant<Types2...> const& /* rhs */) const
  {
    return false;
  }

  template <typename... Types2>
  bool operator!=(variant<Types2...> const& rhs) const
  {
    return !operator==(rhs);
  }

  bool operator<(variant const& rhs) const
  {
    if (which() != rhs.which()) {
      return which() < rhs.which();
    }

    impl::comparer<variant, impl::less_than> visitor(*this);
    return visit(rhs, visitor);
  }

  template <typename T>
  bool is() const
  {
    return (m_type_index == impl::direct_type<T, Types...>::index);
  }

  bool valid() const
  {
    return (m_type_index != impl::invalid_type_index);
  }

  template <
      typename T,
      typename std::enable_if<(impl::direct_type<T, Types...>::index !=
                               impl::invalid_type_index)>::type* = nullptr>
  T& get()
  {
    if (m_type_index == impl::direct_type<T, Types...>::index) {
      return *reinterpret_cast<T*>(&m_data);
    } else {
      THROW(std::runtime_error("failed get<T>() in variant type"));
    }
  }

  template <
      typename T,
      typename std::enable_if<(impl::direct_type<T, Types...>::index !=
                               impl::invalid_type_index)>::type* = nullptr>
  T const& get() const
  {
    if (m_type_index == impl::direct_type<T, Types...>::index) {
      return *reinterpret_cast<T const*>(&m_data);
    } else {
      THROW(std::runtime_error("failed get<T>() in variant type"));
    }
  }

  /**
   * Returns the zero-based type index of the current type used internally,
   * starting from the last type in the template list.
   */
  std::size_t type_index() const
  {
    return m_type_index;
  }

  /**
   * Returns the zero-based type index of the current type, starting
   * from the first type in the template list.
   */
  int which() const noexcept
  {
    return static_cast<int>(sizeof...(Types) - m_type_index - 1);
  }

  /** Unary visitation (const operand) */
  template <typename F, typename V>
  auto static visit(V const& v, F f) -> decltype(
      impl::dispatcher<
          F, V, typename impl::result_of_unary_visit<
                    F, typename impl::select_type<0, Types...>::type>::type,
          Types...>::apply_const(v, f))
  {
    using R = typename impl::result_of_unary_visit<
        F, typename impl::select_type<0, Types...>::type>::type;
    return impl::dispatcher<F, V, R, Types...>::apply_const(v, f);
  }

  /** Unary visitation (non-const operand) */
  template <typename F, typename V>
  auto static visit(V& v, F f) -> decltype(
      impl::dispatcher<
          F, V, typename impl::result_of_unary_visit<
                    F, typename impl::select_type<0, Types...>::type>::type,
          Types...>::apply(v, f))
  {
    using R = typename impl::result_of_unary_visit<
        F, typename impl::select_type<0, Types...>::type>::type;
    return impl::dispatcher<F, V, R, Types...>::apply(v, f);
  }

  /** Binary visitation (const operands) */
  template <typename F, typename V>
  auto static binary_visit(V const& v0, V const& v1, F f) -> decltype(
      impl::binary_dispatcher<
          F, V, typename impl::result_of_binary_visit<
                    F, typename impl::select_type<0, Types...>::type>::type,
          Types...>::apply_const(v0, v1, f))
  {
    using R = typename impl::result_of_binary_visit<
        F, typename impl::select_type<0, Types...>::type>::type;
    return impl::binary_dispatcher<F, V, R, Types...>::apply_const(v0, v1, f);
  }

  /** Binary visitation (non-const operands) */
  template <typename F, typename V>
  auto static binary_visit(V& v0, V& v1, F f) -> decltype(
      impl::binary_dispatcher<
          F, V, typename impl::result_of_binary_visit<
                    F, typename impl::select_type<0, Types...>::type>::type,
          Types...>::apply(v0, v1, f))
  {
    using R = typename impl::result_of_binary_visit<
        F, typename impl::select_type<0, Types...>::type>::type;
    return impl::binary_dispatcher<F, V, R, Types...>::apply(v0, v1, f);
  }
};

// -----------------------------------------------------------------------------

/** Apply unary visitor (const operand) */
template <typename V, typename F>
auto
apply_visitor(F f, V const& v) -> decltype(V::visit(v, f))
{
  return V::visit(v, f);
}

// -----------------------------------------------------------------------------

/** Apply unary visitor (non-const operand) */
template <typename V, typename F>
auto
apply_visitor(F f, V& v) -> decltype(V::visit(v, f))
{
  return V::visit(v, f);
}

// -----------------------------------------------------------------------------

/** Apply binary visitor (const operands) */
template <typename V, typename F>
auto
apply_visitor(F f, V const& v0, V const& v1)
    -> decltype(V::binary_visit(v0, v1, f))
{
  return V::binary_visit(v0, v1, f);
}

// -----------------------------------------------------------------------------

/** Apply binary visitor (non-const operands) */
template <typename V, typename F>
auto
apply_visitor(F f, V& v0, V& v1) -> decltype(V::binary_visit(v0, v1, f))
{
  return V::binary_visit(v0, v1, f);
}

// -----------------------------------------------------------------------------

} /* end namespace variant */
} /* end namespace sl */
