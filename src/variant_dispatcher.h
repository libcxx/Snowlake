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

#include <stdexcept>
#include <string>

#include "macros.h"

namespace sl
{
namespace variant
{
namespace impl
{

// -----------------------------------------------------------------------------

template <typename F, typename V, typename R, typename... Types>
struct dispatcher;

// -----------------------------------------------------------------------------

template <typename F, typename V, typename R, typename T, typename... Types>
struct dispatcher<F, V, R, T, Types...>
{
  using result_type = R;

  static result_type apply_const(V const& v, F f)
  {
    if (v.type_index() == sizeof...(Types))
      {
        return f(v.template get<T>());
      }
    else
      {
        return dispatcher<F, V, R, Types...>::apply_const(v, f);
      }
  }

  static result_type apply(V& v, F f)
  {
    if (v.type_index() == sizeof...(Types))
      {
        return f(v.template get<T>());
      }
    else
      {
        return dispatcher<F, V, R, Types...>::apply(v, f);
      }
  }
};

// -----------------------------------------------------------------------------

template <typename F, typename V, typename R> struct dispatcher<F, V, R>
{
  using result_type = R;

  static result_type apply_const(V const&, F)
  {
    THROW(std::runtime_error("unary dispatch failed"));
  }

  static result_type apply(V&, F)
  {
    THROW(std::runtime_error("unary dispatch failed"));
  }
};

// -----------------------------------------------------------------------------

template <typename F, typename V, typename R, typename T, typename... Types>
struct binary_dispatcher_rhs;

// -----------------------------------------------------------------------------

template <typename F, typename V, typename R, typename T0, typename T1,
          typename... Types>
struct binary_dispatcher_rhs<F, V, R, T0, T1, Types...>
{
  using result_type = R;

  static result_type apply_const(V const& lhs, V const& rhs, F f)
  {
    if (rhs.type_index() == sizeof...(Types))
      {
        return f(lhs.template get<T0>(), rhs.template get<T1>());
      }
    else
      {
        return binary_dispatcher_rhs<F, V, R, T0, Types...>::apply_const(
            lhs, rhs, f);
      }
  }

  static result_type apply(V& lhs, V& rhs, F f)
  {
    if (rhs.type_index() == sizeof...(Types))
      {
        return f(lhs.template get<T0>(), rhs.template get<T1>());
      }
    else
      {
        return binary_dispatcher_rhs<F, V, R, T0, Types...>::apply(lhs, rhs, f);
      }
  }
};

// -----------------------------------------------------------------------------

template <typename F, typename V, typename R, typename T>
struct binary_dispatcher_rhs<F, V, R, T>
{
  using result_type = R;

  static result_type apply_const(V const&, V const&, F)
  {
    THROW(std::runtime_error("binary dispatch failed"));
  }

  static result_type apply(V&, V&, F)
  {
    THROW(std::runtime_error("binary dispatch failed"));
  }
};

// -----------------------------------------------------------------------------

template <typename F, typename V, typename R, typename T, typename... Types>
struct binary_dispatcher_lhs;

// -----------------------------------------------------------------------------

template <typename F, typename V, typename R, typename T0, typename T1,
          typename... Types>
struct binary_dispatcher_lhs<F, V, R, T0, T1, Types...>
{
  using result_type = R;

  static result_type apply_const(V const& lhs, V const& rhs, F f)
  {
    if (lhs.type_index() == sizeof...(Types))
      {
        return f(lhs.template get<T1>(), rhs.template get<T0>());
      }
    else
      {
        return binary_dispatcher_lhs<F, V, R, T0, Types...>::apply_const(
            lhs, rhs, f);
      }
  }

  static result_type apply(V& lhs, V& rhs, F f)
  {
    if (lhs.type_index() == sizeof...(Types))
      {
        return f(lhs.template get<T1>(), rhs.template get<T0>());
      }
    else
      {
        return binary_dispatcher_lhs<F, V, R, T0, Types...>::apply(lhs, rhs, f);
      }
  }
};

// -----------------------------------------------------------------------------

template <typename F, typename V, typename R, typename T>
struct binary_dispatcher_lhs<F, V, R, T>
{
  using result_type = R;

  static result_type apply_const(V const&, V const&, F)
  {
    THROW(std::runtime_error("binary dispatch failed"));
  }

  static result_type apply(V&, V&, F)
  {
    THROW(std::runtime_error("binary dispatch failed"));
  }
};

// -----------------------------------------------------------------------------

template <typename F, typename V, typename R, typename... Types>
struct binary_dispatcher;

// -----------------------------------------------------------------------------

template <typename F, typename V, typename R, typename T, typename... Types>
struct binary_dispatcher<F, V, R, T, Types...>
{
  using result_type = R;

  static result_type apply_const(V const& v0, V const& v1, F f)
  {
    if (v0.type_index() == sizeof...(Types))
      {
        if (v0.type_index() == v1.type_index())
          {
            return f(v0.template get<T>(), v1.template get<T>());
          }
        else
          {
            return binary_dispatcher_rhs<F, V, R, T, Types...>::apply_const(
                v0, v1, f);
          }
      }
    else if (v1.type_index() == sizeof...(Types))
      {
        return binary_dispatcher_lhs<F, V, R, T, Types...>::apply_const(v0, v1,
                                                                        f);
      }

    return binary_dispatcher<F, V, R, Types...>::apply_const(v0, v1, f);
  }

  static result_type apply(V& v0, V& v1, F f)
  {
    if (v0.type_index() == sizeof...(Types))
      {
        if (v0.type_index() == v1.type_index())
          {
            return f(v0.template get<T>(), v1.template get<T>());
          }
        else
          {
            return binary_dispatcher_rhs<F, V, R, T, Types...>::apply(v0, v1,
                                                                      f);
          }
      }
    else if (v1.type_index() == sizeof...(Types))
      {
        return binary_dispatcher_lhs<F, V, R, T, Types...>::apply(v0, v1, f);
      }

    return binary_dispatcher<F, V, R, Types...>::apply(v0, v1, f);
  }
};

// -----------------------------------------------------------------------------

template <typename F, typename V, typename R> struct binary_dispatcher<F, V, R>
{
  using result_type = R;

  static result_type apply_const(V const&, V const&, F)
  {
    THROW(std::runtime_error("binary dispatch failed"));
  }

  static result_type apply(V&, V&, F)
  {
    THROW(std::runtime_error("binary dispatch failed"));
  }
};

// -----------------------------------------------------------------------------

} /* end namespace impl */
} /* end namespace variant */
} /* end namespace sl */
