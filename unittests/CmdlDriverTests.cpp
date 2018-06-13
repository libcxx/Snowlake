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

#include "CmdlDriver.h"

#include <vector>

// -----------------------------------------------------------------------------

class CmdlDriverTests : public ::testing::Test
{
};

// -----------------------------------------------------------------------------

TEST_F(CmdlDriverTests, TestDefaultInitialization)
{
  CmdlDriver driver;

  ASSERT_FALSE(driver.options().warningsAsErrors);
  ASSERT_FALSE(driver.options().bailOnFirstError);
  ASSERT_FALSE(driver.options().debugMode);
  ASSERT_FALSE(driver.options().verbose);
  ASSERT_STREQ("", driver.options().output_path.c_str());
}

// -----------------------------------------------------------------------------

TEST_F(CmdlDriverTests, TestRun)
{
  const std::vector<char*> args{"MyProgram", "--errors", "--bail",   "--debug",
                                "--verbose", "--output", "/tmp/out", "/tmp/in"};

  CmdlDriver driver;
  const bool res = driver.run(args.size(), (char**)args.data());
  ASSERT_TRUE(res);

  ASSERT_TRUE(driver.options().warningsAsErrors);
  ASSERT_TRUE(driver.options().bailOnFirstError);
  ASSERT_TRUE(driver.options().debugMode);
  ASSERT_TRUE(driver.options().verbose);
  ASSERT_STREQ("/tmp/out", driver.options().output_path.c_str());
  ASSERT_STREQ("/tmp/in", driver.options().input_path.c_str());
}

// -----------------------------------------------------------------------------

TEST_F(CmdlDriverTests, TestWithoutInputPathSpecified)
{
  const std::vector<char*> args{"MyProgram", "--errors", "--bail",
                                "--debug",   "--output", "/tmp/out"};

  CmdlDriver driver;
  const bool res = driver.run(args.size(), (char**)args.data());
  ASSERT_FALSE(res);
}

// -----------------------------------------------------------------------------
