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

#include "macros.h"
#include "variant.h"

namespace sl
{

namespace
{
struct nullopt_t
{
};
}

/**
 * Simple implementation of an optional container.
 * Use this until `std::optional` support lands in Clang.
 */
template <class T>
class optional
{
private:
  typedef sl::variant::variant<nullopt_t, T> ValueType;

public:
  optional()
      : m_value(nullopt_t{})
  {
  }

  optional(const T& val)
      : m_value(val)
  {
  }

  optional& operator=(T&& value)
  {
    m_value = value;
    return *this;
  }

  const T* operator->() const
  {
    return &value();
  }

  T* operator->()
  {
    return &value();
  }

  const T& operator*() const
  {
    return value();
  }

  T& operator*()
  {
    return value();
  }

  bool has_value() const
  {
    return m_value.template is<T>();
  }

  T& value()
  {
    ASSERT(has_value());
    return m_value.template get<T>();
  }

  const T& value() const
  {
    ASSERT(has_value());
    return m_value.template get<T>();
  }

  void reset()
  {
    ValueType value;
    m_value = value; // Do a swap internally.
  }

private:
  ValueType m_value;
};

} /* end namespace sl */
