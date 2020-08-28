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
#include "variant.h"
#include "variant_static_visitor.h"

#include <cstdio>
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

// -----------------------------------------------------------------------------

const char* HELLO_WORLD = "Hello world";

// -----------------------------------------------------------------------------

class VariantTests : public ::testing::Test
{
protected:
  using VariantType =
      typename sl::variant::variant<int, double, std::string, std::vector<int>>;
};

// -----------------------------------------------------------------------------

TEST_F(VariantTests, TestEmptyInitialization)
{
  VariantType v;

  ASSERT_EQ(false, v.valid());
  ASSERT_EQ(sl::variant::impl::invalid_type_index, v.type_index());

  ASSERT_EQ(false, v.is<int>());
  ASSERT_EQ(false, v.is<double>());
  ASSERT_EQ(false, v.is<std::string>());
  ASSERT_EQ(false, v.is<std::vector<int>>());

  ASSERT_THROW({ v.get<int>(); }, std::runtime_error);

  ASSERT_THROW({ v.get<std::vector<int>>(); }, std::runtime_error);
}

// -----------------------------------------------------------------------------

TEST_F(VariantTests, TestSameTypeCopyConstructor)
{
  const std::string s(HELLO_WORLD);

  VariantType v0(s);

  ASSERT_EQ(true, v0.valid());
  ASSERT_EQ(2, v0.which());
  ASSERT_EQ(true, v0.is<std::string>());
  ASSERT_EQ(s, v0.get<std::string>());

  // Copy
  VariantType v1(v0);

  ASSERT_EQ(true, v1.valid());
  ASSERT_EQ(2, v1.which());
  ASSERT_EQ(true, v1.is<std::string>());

  ASSERT_EQ(s, v1.get<std::string>());

  // Old value should still be valid.
  ASSERT_EQ(true, v0.valid());
  ASSERT_EQ(2, v0.which());
  ASSERT_EQ(true, v0.is<std::string>());
  ASSERT_EQ(s, v0.get<std::string>());
}

// -----------------------------------------------------------------------------

TEST_F(VariantTests, TestSameTypeMoveConstructor)
{
  const std::string s(HELLO_WORLD);

  VariantType v0(s);

  ASSERT_EQ(true, v0.valid());
  ASSERT_EQ(true, v0.is<std::string>());
  ASSERT_EQ(s, v0.get<std::string>());
  ASSERT_EQ(2, v0.which());

  // Move
  VariantType v1(std::move(v0));

  ASSERT_EQ(true, v1.valid());
  ASSERT_EQ(true, v1.is<std::string>());

  ASSERT_EQ(s, v1.get<std::string>());

  // Old value should NOT be valid anymore.
  ASSERT_EQ(false, v0.valid());
  ASSERT_EQ(false, v0.is<std::string>());

  ASSERT_THROW({ v0.get<std::string>(); }, std::runtime_error);
}

// -----------------------------------------------------------------------------

TEST_F(VariantTests, TestTypeSpecificMoveConstructor)
{
  const std::string s(HELLO_WORLD);

  VariantType v(std::move(s));

  ASSERT_EQ(true, v.valid());
  ASSERT_EQ(2, v.which());

  ASSERT_EQ(false, v.is<int>());
  ASSERT_EQ(false, v.is<double>());
  ASSERT_EQ(true, v.is<std::string>());
  ASSERT_EQ(false, v.is<std::vector<int>>());

  ASSERT_EQ(s, v.get<std::string>());

  ASSERT_THROW({ v.get<int>(); }, std::runtime_error);

  // Original value should still be valid.
  ASSERT_STREQ(HELLO_WORLD, s.c_str());
}

// -----------------------------------------------------------------------------

TEST_F(VariantTests, TestAssignmentInitialization)
{
  std::vector<int> nums = {1, 2, 3};

  VariantType v0(nums);

  ASSERT_EQ(true, v0.valid());
  ASSERT_EQ(3, v0.which());
  ASSERT_EQ(true, v0.is<std::vector<int>>());
  ASSERT_EQ(1, v0.get<std::vector<int>>().front());

  VariantType v1 = v0;

  ASSERT_EQ(true, v1.valid());
  ASSERT_EQ(3, v1.which());
  ASSERT_EQ(true, v1.is<std::vector<int>>());
  ASSERT_EQ(1, v1.get<std::vector<int>>().front());

  // Original value should still be valid.
  ASSERT_EQ(true, v0.valid());
  ASSERT_EQ(3, v0.which());
  ASSERT_EQ(true, v0.is<std::vector<int>>());
  ASSERT_EQ(1, v0.get<std::vector<int>>().front());
}

// -----------------------------------------------------------------------------

TEST_F(VariantTests, TestAssignmentInitializationWithSpecificTypeByValue)
{
  VariantType v = (int)1;

  ASSERT_EQ(true, v.valid());
  ASSERT_EQ(0, v.which());

  ASSERT_EQ(true, v.is<int>());
  ASSERT_EQ(false, v.is<double>());
  ASSERT_EQ(false, v.is<std::string>());
  ASSERT_EQ(false, v.is<std::vector<int>>());

  ASSERT_EQ(1, v.get<int>());

  ASSERT_THROW({ v.get<std::vector<int>>(); }, std::runtime_error);
}

// -----------------------------------------------------------------------------

TEST_F(VariantTests, TestAssignmentInitializationWithSpecificTypeByRvalue)
{
  const std::string s(HELLO_WORLD);

  VariantType v = std::move(s);

  ASSERT_EQ(true, v.valid());
  ASSERT_EQ(2, v.which());

  ASSERT_EQ(false, v.is<int>());
  ASSERT_EQ(false, v.is<double>());
  ASSERT_EQ(true, v.is<std::string>());
  ASSERT_EQ(false, v.is<std::vector<int>>());

  ASSERT_STREQ(HELLO_WORLD, v.get<std::string>().c_str());

  ASSERT_THROW({ v.get<std::vector<int>>(); }, std::runtime_error);

  ASSERT_STREQ(HELLO_WORLD, s.c_str());
}

// -----------------------------------------------------------------------------

TEST_F(VariantTests, TestEqualityOperatorOnSameType)
{
  const std::string s(HELLO_WORLD);

  VariantType v = s;
  VariantType v1 = s;

  // Self equality.
  ASSERT_EQ(v, v);

  // Equality on instances of same type with same value.
  ASSERT_EQ(v, v1);

  // Inequality on instances of same type with different value.
  VariantType v2 = std::string("Greetings");

  ASSERT_NE(v, v2);
}

// -----------------------------------------------------------------------------

TEST_F(VariantTests, TestFullSwap)
{
  std::vector<int> nums = {1, 2, 3};

  VariantType v0(nums);

  const auto checkIsVector = [&](const auto& v) {
    ASSERT_EQ(true, v.valid());
    ASSERT_EQ(3, v.which());
    ASSERT_EQ(true, v.template is<std::vector<int>>());
    ASSERT_EQ(nums.size(), v.template get<std::vector<int>>().size());
    ASSERT_EQ(nums.front(), v.template get<std::vector<int>>().front());
    ASSERT_EQ(nums.back(), v.template get<std::vector<int>>().back());
  };

  const auto checkIsString = [&](const auto& v) {
    ASSERT_EQ(true, v.valid());
    ASSERT_EQ(2, v.which());
    ASSERT_EQ(true, v.template is<std::string>());
    ASSERT_STREQ(HELLO_WORLD, v.template get<std::string>().c_str());
  };

  checkIsVector(v0);

  VariantType v1(HELLO_WORLD);

  checkIsString(v1);

  const auto swap = [&]() {
    auto tmp = v1;
    v1 = v0;
    v0 = tmp;
  };

  // Now we swap
  swap();

  // Now v0 and v1 should have been swapped entirely.
  checkIsString(v0);
  checkIsVector(v1);

  // Now swap back.
  swap();
  checkIsVector(v0);
  checkIsString(v1);
}

// -----------------------------------------------------------------------------

TEST_F(VariantTests, TestPartialSwap)
{
  std::vector<int> nums = {1, 2, 3};

  VariantType v0(nums);

  const auto checkIsVector = [&](const auto& v) {
    ASSERT_EQ(true, v.valid());
    ASSERT_EQ(3, v.which());
    ASSERT_EQ(true, v.template is<std::vector<int>>());
    ASSERT_EQ(nums.size(), v.template get<std::vector<int>>().size());
    ASSERT_EQ(nums.front(), v.template get<std::vector<int>>().front());
    ASSERT_EQ(nums.back(), v.template get<std::vector<int>>().back());
  };

  const auto checkInvalid = [](const auto& v) {
    ASSERT_EQ(false, v.valid());
    ASSERT_EQ(sl::variant::impl::invalid_type_index, v.type_index());
  };

  checkIsVector(v0);

  VariantType v1; // v1 is incomplete

  checkInvalid(v1);

  const auto swap = [&]() {
    auto tmp = v1;
    v1 = v0;
    v0 = tmp;
  };

  // Now we swap
  swap();

  // Now v0 and v1 should have been swapped entirely.
  checkInvalid(v0);
  checkIsVector(v1);

  // Now swap back.
  swap();
  checkIsVector(v0);
  checkInvalid(v1);
}

// -----------------------------------------------------------------------------

TEST_F(VariantTests, TestIncompleteSwap)
{
  const auto checkInvalid = [](const auto& v) {
    ASSERT_EQ(false, v.valid());
    ASSERT_EQ(sl::variant::impl::invalid_type_index, v.type_index());
  };

  VariantType v0; // v0 is incomplete

  checkInvalid(v0);

  VariantType v1; // v1 is incomplete

  checkInvalid(v1);

  const auto swap = [&]() {
    auto tmp = v1;
    v1 = v0;
    v0 = tmp;
  };

  // Now we swap
  swap();

  // Now v0 and v1 should have been swapped entirely.
  checkInvalid(v0);
  checkInvalid(v1);

  // Now swap back.
  swap();
  checkInvalid(v0);
  checkInvalid(v1);
}

// -----------------------------------------------------------------------------

TEST_F(VariantTests, TestEqualityOperatorOnDifferentTypes)
{
  typedef sl::variant::variant<int, double> VariantType1;
  typedef sl::variant::variant<int, float> VariantType2;

  VariantType1 v1 = (int)1;

  VariantType2 v2 = (int)1;

  ASSERT_NE(v1, v2);
}

// -----------------------------------------------------------------------------

TEST_F(VariantTests, TestLessThanOperatorOnSameType)
{
  VariantType v1 = 1;
  VariantType v2 = 2;

  ASSERT_LT(v1, v2);
}

// -----------------------------------------------------------------------------

TEST_F(VariantTests, TestLessThanOperatorOnDifferentTypes)
{
  VariantType v1 = 100;
  VariantType v2 = 3.14;

  // Even though the integer value of 100 is greater than the floating point
  // value of 3.14, it is still considered less in the comparison since
  // the `int` type comes before the `double` type in the variant type
  // template declaration.
  ASSERT_LT(v1, v2);
}

// -----------------------------------------------------------------------------

class VariantUnaryVisitationUnitTest : public VariantTests
{
protected:
  struct tostring_visitor : public sl::variant::static_visitor<std::string> {
    std::string operator()(const int& val) const
    {
      return std::to_string(val);
    }

    std::string operator()(const double& val) const
    {
      char buf[5];
      sprintf(buf, "%.2f", val);

      return std::string(buf);
    }

    std::string operator()(const std::string& val) const
    {
      return val;
    }

    std::string operator()(const std::vector<int>& val) const
    {
      std::stringstream ss;

      ss << '[';
      for (auto itr = val.cbegin(); itr != val.cend(); ++itr) {
        ss << *itr;

        auto itr_ = itr;
        if (++itr_ != val.cend()) {
          ss << ", ";
        }
      }
      ss << ']';

      return ss.str();
    }
  };
};

// -----------------------------------------------------------------------------

TEST_F(VariantUnaryVisitationUnitTest, TestVisitation)
{
  VariantType v1 = (int)1;
  VariantType v2 = (double)3.14;
  VariantType v3 = std::string(HELLO_WORLD);
  VariantType v4 = std::vector<int>{1, 2, 3};

  const std::string val1 = sl::variant::apply_visitor(tostring_visitor(), v1);
  ASSERT_STREQ("1", val1.c_str());

  const std::string val2 = sl::variant::apply_visitor(tostring_visitor(), v2);
  ASSERT_STREQ("3.14", val2.c_str());

  const std::string val3 = sl::variant::apply_visitor(tostring_visitor(), v3);
  ASSERT_STREQ(HELLO_WORLD, val3.c_str());

  const std::string val4 = sl::variant::apply_visitor(tostring_visitor(), v4);
  ASSERT_STREQ("[1, 2, 3]", val4.c_str());
}

// -----------------------------------------------------------------------------

class VariantBinaryVisitationUnitTest : public VariantTests
{
protected:
  struct equality_visitor : public sl::variant::static_visitor<bool> {
    template <typename T>
    bool operator()(const T& lhs, const T& rhs) const
    {
      return lhs == rhs;
    }

    template <typename T, typename T2>
    bool operator()(const T& /* lhs */, const T2& /* rhs */) const
    {
      return false;
    }
  };
};

// -----------------------------------------------------------------------------

TEST_F(VariantBinaryVisitationUnitTest, TestVisitation)
{
  VariantType v1_int = (int)1;
  VariantType v2_int = (int)2;

  VariantType v1_double = (double)3.14;
  VariantType v2_double = (double)3.14;

  VariantType v1_str = "Hello";
  VariantType v2_str = "Hi";

  VariantType v1_vec = std::vector<int>{1, 2, 3};
  VariantType v2_vec = std::vector<int>{1, 2, 3};

  bool res1 = sl::variant::apply_visitor(equality_visitor(), v1_int, v2_int);
  ASSERT_EQ(false, res1);

  bool res2 =
      sl::variant::apply_visitor(equality_visitor(), v1_double, v2_double);
  ASSERT_EQ(true, res2);

  bool res3 = sl::variant::apply_visitor(equality_visitor(), v1_str, v2_str);
  ASSERT_EQ(false, res3);

  bool res4 = sl::variant::apply_visitor(equality_visitor(), v1_vec, v2_vec);
  ASSERT_EQ(true, res4);

  VariantType v3_double = (double)1.0;
  bool res5 = sl::variant::apply_visitor(equality_visitor(), v1_int, v3_double);
  ASSERT_EQ(false, res5);
}

// -----------------------------------------------------------------------------

class VariantMemoryIntegrityUnitTest : public VariantTests
{
protected:
  struct _MyStruct {
    std::string name;
  };

  using MyVariantType = typename sl::variant::variant<_MyStruct, int>;
};

// -----------------------------------------------------------------------------

TEST_F(VariantMemoryIntegrityUnitTest, TestWithAssociatedMap)
{
  const char* MY_NAME = "William";

  const std::string myName(MY_NAME);

  {
    std::unordered_map<std::string, MyVariantType> m;

    sl::variant::variant<_MyStruct, int> v(_MyStruct{.name = "Some name"});

    m[myName] = v;
  }
}

// -----------------------------------------------------------------------------

TEST_F(VariantMemoryIntegrityUnitTest, TestWithCopySemantics)
{
  const char* MY_NAME = "William";

  const std::string myName(MY_NAME);

  using MyVariantType = sl::variant::variant<_MyStruct, int>;

  MyVariantType v(_MyStruct{.name = myName});

  {
    std::vector<MyVariantType> vec;
    vec.reserve(16);

    sl::variant::variant<_MyStruct, int> v2(v);

    vec.push_back(v2);

    vec.resize(32);
  }

  ASSERT_STREQ(MY_NAME, v.get<_MyStruct>().name.c_str());
}

// -----------------------------------------------------------------------------
