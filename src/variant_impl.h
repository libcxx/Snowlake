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

#include <cstdlib>
#include <type_traits>

namespace sl {
namespace variant {
namespace impl {

// -----------------------------------------------------------------------------

static constexpr std::size_t invalid_type_index = std::size_t(-1);

// -----------------------------------------------------------------------------

template <typename T, typename... Types>
struct direct_type;

// -----------------------------------------------------------------------------

template <typename T, typename First, typename... Types>
struct direct_type<T, First, Types...>
{
  static constexpr std::size_t index = std::is_same<T, First>::value
                                           ? sizeof...(Types)
                                           : direct_type<T, Types...>::index;
};

// -----------------------------------------------------------------------------

template <typename T>
struct direct_type<T>
{
  static constexpr std::size_t index = invalid_type_index;
};

// -----------------------------------------------------------------------------

template <typename T, typename... Types>
struct convertible_type;

// -----------------------------------------------------------------------------

template <typename T, typename First, typename... Types>
struct convertible_type<T, First, Types...>
{
  static constexpr std::size_t index =
      std::is_convertible<T, First>::value
          ? sizeof...(Types)
          : convertible_type<T, Types...>::index;
};

// -----------------------------------------------------------------------------

template <typename T>
struct convertible_type<T>
{
  static constexpr std::size_t index = invalid_type_index;
};

// -----------------------------------------------------------------------------

template <typename T, typename... Types>
struct value_traits
{
  static constexpr std::size_t direct_index = direct_type<T, Types...>::index;
  static constexpr std::size_t index =
      (direct_index == invalid_type_index)
          ? convertible_type<T, Types...>::index
          : direct_index;
};

// -----------------------------------------------------------------------------

template <typename T, typename... Types>
struct is_valid_type;

// -----------------------------------------------------------------------------

template <typename T, typename First, typename... Types>
struct is_valid_type<T, First, Types...>
{
  static constexpr bool value =
      std::is_convertible<T, First>::value || is_valid_type<T, Types...>::value;
};

// -----------------------------------------------------------------------------

template <typename T>
struct is_valid_type<T> : std::false_type
{
};

// -----------------------------------------------------------------------------

template <std::size_t N, typename... Types>
struct select_type
{
  static_assert(N < sizeof...(Types), "index out of bounds");
};

// -----------------------------------------------------------------------------

template <std::size_t N, typename T, typename... Types>
struct select_type<N, T, Types...>
{
  using type = typename select_type<N - 1, Types...>::type;
};

// -----------------------------------------------------------------------------

template <typename T, typename... Types>
struct select_type<0, T, Types...>
{
  using type = T;
};

// -----------------------------------------------------------------------------

template <typename T, typename R = void>
struct enable_if_type
{
  using type = R;
};

// -----------------------------------------------------------------------------

template <typename F, typename V, typename Enable = void>
struct result_of_unary_visit
{
  using type = typename std::result_of<F(V&)>::type;
};

// -----------------------------------------------------------------------------

template <typename F, typename V>
struct result_of_unary_visit<
    F, V, typename enable_if_type<typename F::result_type>::type>
{
  using type = typename F::result_type;
};

// -----------------------------------------------------------------------------

template <typename F, typename V, class Enable = void>
struct result_of_binary_visit
{
  using type = typename std::result_of<F(V&, V&)>::type;
};

// -----------------------------------------------------------------------------

template <typename F, typename V>
struct result_of_binary_visit<
    F, V, typename enable_if_type<typename F::result_type>::type>
{
  using type = typename F::result_type;
};

// -----------------------------------------------------------------------------

template <std::size_t arg1, std::size_t... others>
struct static_max;

// -----------------------------------------------------------------------------

template <std::size_t arg>
struct static_max<arg>
{
  static const std::size_t value = arg;
};

// -----------------------------------------------------------------------------

template <std::size_t arg1, std::size_t arg2, std::size_t... others>
struct static_max<arg1, arg2, others...>
{
  static const std::size_t value = arg1 >= arg2
                                       ? static_max<arg1, others...>::value
                                       : static_max<arg2, others...>::value;
};

// -----------------------------------------------------------------------------

template <typename... Types>
struct variant_helper;

// -----------------------------------------------------------------------------

template <typename T, typename... Types>
struct variant_helper<T, Types...>
{
  static void destroy(const std::size_t id, void* m_data)
  {
    if (id == sizeof...(Types)) {
      reinterpret_cast<T*>(m_data)->~T();
    } else {
      variant_helper<Types...>::destroy(id, m_data);
    }
  }

  static void move(const std::size_t old_id, void* old_value, void* new_value)
  {
    if (old_id == sizeof...(Types)) {
      new (new_value) T(std::move(*reinterpret_cast<T*>(old_value)));
    } else {
      variant_helper<Types...>::move(old_id, old_value, new_value);
    }
  }

  static void copy(const std::size_t old_id, const void* old_value,
                   void* new_value)
  {
    if (old_id == sizeof...(Types)) {
      new (new_value) T(*reinterpret_cast<const T*>(old_value));
    } else {
      variant_helper<Types...>::copy(old_id, old_value, new_value);
    }
  }
};

// -----------------------------------------------------------------------------

template <>
struct variant_helper<>
{
  static void destroy(const std::size_t, void*)
  {
  }
  static void move(const std::size_t, void*, void*)
  {
  }
  static void copy(const std::size_t, const void*, void*)
  {
  }
};

// -----------------------------------------------------------------------------

struct equal_to
{
  template <typename T>
  bool operator()(T const& lhs, T const& rhs) const
  {
    return lhs == rhs;
  }
};

// -----------------------------------------------------------------------------

struct less_than
{
  template <typename T>
  bool operator()(T const& lhs, T const& rhs) const
  {
    return lhs < rhs;
  }
};

// -----------------------------------------------------------------------------

template <typename Variant, typename Comp>
class comparer
{
public:
  explicit comparer(Variant const& lhs) noexcept
      : m_lhs(lhs)
  {
  }

  comparer& operator=(comparer const&) = delete;

  template <typename T>
  bool operator()(T const& rhs_val) const
  {
    T const& lhs_val = m_lhs.template get<T>();
    return Comp()(lhs_val, rhs_val);
  }

private:
  Variant const& m_lhs;
};

// -----------------------------------------------------------------------------

} /* end namespace impl */
} /* end namespace variant */
} /* end namespace sl */
