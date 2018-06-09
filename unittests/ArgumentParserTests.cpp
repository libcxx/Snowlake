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

#include "ArgumentParser.h"

#include <vector>

// -----------------------------------------------------------------------------

class ArgumentParserTests : public ::testing::Test
{
};

// -----------------------------------------------------------------------------

TEST_F(ArgumentParserTests, TestDefaultInitialization)
{
  ArgumentParser argparser;
  ASSERT_TRUE(argparser.positional_args().empty());
}

// -----------------------------------------------------------------------------

TEST_F(ArgumentParserTests, TestParseStringOption)
{
  ArgumentParser argparser;
  std::string dst;
  const char* default_val = "DefaltName";
  argparser.add_string_parameter("name", "Name of character", true, &dst,
                                 default_val);

  const std::vector<char*> args{"MyProgram", "--name", "HelloWorld"};

  bool res = argparser.parse_args(args.size(), (char**)args.data());
  ASSERT_TRUE(res);

  ASSERT_STREQ("HelloWorld", dst.c_str());
}

// -----------------------------------------------------------------------------

TEST_F(ArgumentParserTests, TestParseStringAndUInt32Options)
{
  ArgumentParser argparser;
  std::string name_dst;
  uint32_t age_dst;
  argparser.add_string_parameter("name", "Name of character", true, &name_dst,
                                 "DefaltName");
  argparser.add_uint32_parameter("age", "Age of character", true, &age_dst, 20);

  const std::vector<char*> args{"MyProgram", "--name", "NoName", "--age", "18"};

  bool res = argparser.parse_args(args.size(), (char**)args.data());
  ASSERT_TRUE(res);

  ASSERT_STREQ("NoName", name_dst.c_str());
  ASSERT_EQ(18, age_dst);
}

// -----------------------------------------------------------------------------

TEST_F(ArgumentParserTests, TestParseWithMissingRequiredOption)
{
  ArgumentParser argparser;
  std::string dst;
  const char* default_val = "DefaltName";
  argparser.add_string_parameter("name", "Name of character", true, &dst,
                                 default_val);

  const std::vector<char*> args{
      "MyProgram",
  };

  bool res = argparser.parse_args(args.size(), (char**)args.data());
  ASSERT_FALSE(res);
}

// -----------------------------------------------------------------------------

TEST_F(ArgumentParserTests, TestParseWithMissingNonRequiredOption)
{
  ArgumentParser argparser;
  std::string dst;
  const char* default_val = "DefaltName";
  argparser.add_string_parameter("name", "Name of character", false, &dst,
                                 default_val);

  const std::vector<char*> args{
      "MyProgram",
  };

  const bool res = argparser.parse_args(args.size(), (char**)args.data());
  ASSERT_TRUE(res);

  ASSERT_STREQ(default_val, dst.c_str());
}

// -----------------------------------------------------------------------------

TEST_F(ArgumentParserTests, TestParseWithAllOptionTypes)
{
  ArgumentParser argparser;
  std::string str_dst;
  uint32_t uint32_dst = 0;
  uint64_t uint64_dst = 0;
  bool bool_dst = false;
  float float_dst = 0.0f;
  double double_dst = 0.0;
  argparser.add_string_parameter("str", "String value", true, &str_dst);
  argparser.add_uint32_parameter("uint32", "UInt32 value", true, &uint32_dst);
  argparser.add_uint64_parameter("uint64", "UInt64 value", true, &uint64_dst);
  argparser.add_boolean_parameter("bool", "Boolean value", true, &bool_dst);
  argparser.add_float_parameter("float", "Float value", true, &float_dst);
  argparser.add_double_parameter("double", "Double value", true, &double_dst);

  const std::vector<char*> args{"MyProgram", "--str",    "MyStringValue",
                                "--uint32",  "32",       "--uint64",
                                "64",        "--bool",   "--float",
                                "3.14",      "--double", "2.71828"};

  bool res = argparser.parse_args(args.size(), (char**)args.data());
  ASSERT_TRUE(res);

  ASSERT_STREQ("MyStringValue", str_dst.c_str());
  ASSERT_EQ(32, uint32_dst);
  ASSERT_EQ(64, uint64_dst);
  ASSERT_EQ(true, bool_dst);
  ASSERT_DOUBLE_EQ(3.14f, float_dst);
  ASSERT_DOUBLE_EQ(2.71828, double_dst);
}

// -----------------------------------------------------------------------------

TEST_F(ArgumentParserTests, TestParseWithAllOptionTypesAndDefaultValues)
{
  ArgumentParser argparser;
  std::string str_dst;
  uint32_t uint32_dst = 0;
  uint64_t uint64_dst = 0;
  bool bool_dst = false;
  float float_dst = 0.0f;
  double double_dst = 0.0;

  const char* str_dst_default = "DefaultStringValue";
  const uint32_t uint32_dst_default = 32;
  const uint64_t uint64_dst_default = 64;
  const bool bool_dst_default = false;
  const float float_dst_default = 3.14f;
  const double double_dst_default = 2.71828;

  argparser.add_string_parameter("str", "String value", false, &str_dst,
                                 str_dst_default);
  argparser.add_uint32_parameter("uint32", "UInt32 value", false, &uint32_dst,
                                 uint32_dst_default);
  argparser.add_uint64_parameter("uint64", "UInt64 value", false, &uint64_dst,
                                 uint64_dst_default);
  argparser.add_boolean_parameter("bool", "Boolean value", false, &bool_dst,
                                  bool_dst_default);
  argparser.add_float_parameter("float", "Float value", false, &float_dst,
                                float_dst_default);
  argparser.add_double_parameter("double", "Double value", false, &double_dst,
                                 double_dst_default);

  const std::vector<char*> args{
      "MyProgram",
  };

  bool res = argparser.parse_args(args.size(), (char**)args.data());
  ASSERT_TRUE(res);

  ASSERT_STREQ(str_dst_default, str_dst.c_str());
  ASSERT_EQ(uint32_dst_default, uint32_dst);
  ASSERT_EQ(uint64_dst_default, uint64_dst);
  ASSERT_EQ(bool_dst_default, bool_dst);
  ASSERT_DOUBLE_EQ(float_dst_default, float_dst);
  ASSERT_DOUBLE_EQ(double_dst_default, double_dst);
}

// -----------------------------------------------------------------------------

TEST_F(ArgumentParserTests, TestParseWithAllOptionlAndPositionalArguments)
{
  ArgumentParser argparser;
  std::string str_dst;
  uint32_t uint32_dst = 0;
  uint64_t uint64_dst = 0;
  bool bool_dst = false;
  float float_dst = 0.0f;
  double double_dst = 0.0;
  const char* input_path = "/tmp/hellworld";

  argparser.add_string_parameter("str", "String value", true, &str_dst);
  argparser.add_uint32_parameter("uint32", "UInt32 value", true, &uint32_dst);
  argparser.add_uint64_parameter("uint64", "UInt64 value", true, &uint64_dst);
  argparser.add_boolean_parameter("bool", "Boolean value", true, &bool_dst);
  argparser.add_float_parameter("float", "Float value", true, &float_dst);
  argparser.add_double_parameter("double", "Double value", true, &double_dst);

  const std::vector<char*> args{"MyProgram",
                                "--str",
                                "MyStringValue",
                                "--uint32",
                                "32",
                                "--uint64",
                                "64",
                                "--bool",
                                "--float",
                                "3.14",
                                "--double",
                                "2.71828",
                                const_cast<char*>(input_path)};

  bool res = argparser.parse_args(args.size(), (char**)args.data());
  ASSERT_TRUE(res);

  ASSERT_STREQ("MyStringValue", str_dst.c_str());
  ASSERT_EQ(32, uint32_dst);
  ASSERT_EQ(64, uint64_dst);
  ASSERT_EQ(true, bool_dst);
  ASSERT_DOUBLE_EQ(3.14f, float_dst);
  ASSERT_DOUBLE_EQ(2.71828, double_dst);

  // Check positional argument.
  const auto& positional_args = argparser.positional_args();
  ASSERT_FALSE(positional_args.empty());
  ASSERT_EQ(1, positional_args.size());
  ASSERT_STREQ(input_path, positional_args.front().c_str());
}

// -----------------------------------------------------------------------------

TEST_F(ArgumentParserTests, TestPrintHelpMessage)
{
  ArgumentParser argparser("My Program", "1.0.0", "This is a test program.");
  std::string str_dst;
  uint32_t uint32_dst = 0;
  uint64_t uint64_dst = 0;
  bool bool_dst = false;
  float float_dst = 0.0f;
  double double_dst = 0.0;

  argparser.add_string_parameter("str", "String value", true, &str_dst);
  argparser.add_uint32_parameter("uint32", "UInt32 value", true, &uint32_dst);
  argparser.add_uint64_parameter("uint64", "UInt64 value", true, &uint64_dst);
  argparser.add_boolean_parameter("bool", "Boolean value", true, &bool_dst);
  argparser.add_float_parameter("float", "Float value", true, &float_dst);
  argparser.add_double_parameter("double", "Double value", true, &double_dst);

  // Get help message.
  argparser.print_help();
}

// -----------------------------------------------------------------------------

TEST_F(ArgumentParserTests, TestsWithInvalidNumberOfPositionalArguments)
{
  ArgumentParser argparser;
  argparser.set_minimum_positional_args_required(1);

  const std::vector<char*> args{"MyProgram"};

  const bool res = argparser.parse_args(args.size(), (char**)args.data());
  ASSERT_FALSE(res);
}

// -----------------------------------------------------------------------------
