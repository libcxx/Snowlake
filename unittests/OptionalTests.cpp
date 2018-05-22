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
#include <gtest/gtest.h>

#include "optional.h"

class OptionalTests : public ::testing::Test {};

// -----------------------------------------------------------------------------

TEST_F(OptionalTests, TestEmptyInitialization)
{
  sl::optional<int> opt;
  ASSERT_FALSE(opt.has_value());
}

// -----------------------------------------------------------------------------

TEST_F(OptionalTests, TestInitializationWithValue)
{
  const int val = 5;
  const sl::optional<int> opt(val);

  ASSERT_TRUE(opt.has_value());
  ASSERT_EQ(val, opt.value());
  ASSERT_EQ(val, *opt);
}

// -----------------------------------------------------------------------------

TEST_F(OptionalTests, TestUpdateValueAndReset)
{
  const int val = 5;
  const int val2 = 8;
  sl::optional<int> opt(val);

  ASSERT_TRUE(opt.has_value());
  ASSERT_EQ(val, opt.value());

  // Update value.
  *opt = val2;

  ASSERT_TRUE(opt.has_value());
  ASSERT_EQ(val2, opt.value());

  // Reset.
  opt.reset();
  ASSERT_FALSE(opt.has_value());

  // Bring back original value.
  opt = val;
  ASSERT_TRUE(opt.has_value());
  ASSERT_EQ(val, opt.value());
}

// -----------------------------------------------------------------------------
