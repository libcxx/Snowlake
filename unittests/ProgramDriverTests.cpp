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

#include "ProgramDriver.h"

#include <cstdlib>
#include <fstream>
#include <vector>

// -----------------------------------------------------------------------------

class ProgramDriverTests : public ::testing::Test
{
protected:
  bool setup_valid_run()
  {
    // clang-format off
    static const char* INPUT =
      "group MyGroup {"
        "EnvironmentClass          : ASTContext;"
        "EnvironmentName           : context;"
        ""
        "inference MethodStaticDispatch {"
        ""
          "globals: ["
            "SELF_TYPE,"
            "CLS_TYPE"
          "]"
          ""
          "arguments: ["
            "StaticMethodCallStmt : ASTExpr"
          "]"
          ""
          "premises: ["
            "StaticMethodCallStmt.argument_types : ArgumentsTypes[];"
            "ArgumentsTypes[0] != SELF_TYPE;"
          "]"
          ""
          "proposition : lub(T1, T2);"
        "}"
      "}"
      "";
    // clang-format on

    return save_input(INPUT);
  }

  bool setup_invalid_run()
  {
    // clang-format off
    static const char* INPUT =
      "group MyGroup {"
        "EnvironmentClass          : ASTContext;"
        "EnvironmentName           : context;"
        ""
        "inference MethodStaticDispatch {"
        ""
          "globals: ["
            "SELF_TYPE"
          "]"
          ""
          "arguments: ["
            "StaticMethodCallStmt : ASTExpr"
          "]"
          ""
          "premises: ["
            "StaticMethodCallStmt.argument_types : ArgumentsTypes[];"
            "ArgumentsTypes[0] != SELF_TYPE;"
          "]"
          ""
          "proposition : SomeRandomType;"
        "}"
      "}"
      "";
    // clang-format on

    return save_input(INPUT);
  }

private:
  bool save_input(const char* input)
  {
    std::ofstream ofs(m_input_path, std::ofstream::out);
    if (ofs.good()) {
      ofs << input;
    } else {
      return false;
    }
    ofs.close();
    return true;
  }

protected:
  const char* m_output_path = "./";
  const char* m_input_path = "test_input.txt";
};

// -----------------------------------------------------------------------------

TEST_F(ProgramDriverTests, TestDefaultInitialization)
{
  ProgramDriver driver;
}

// -----------------------------------------------------------------------------

TEST_F(ProgramDriverTests, TestRunWithSuccess)
{
  if (setup_valid_run()) {
    ProgramDriver driver;

    const std::vector<char*> args{"snowlake",
                                  "--errors",
                                  "--bail",
                                  "--output",
                                  const_cast<char*>(m_output_path),
                                  const_cast<char*>(m_input_path)};

    const int res = driver.run(args.size(), (char**)args.data());
    ASSERT_EQ(EXIT_SUCCESS, res);
  }
}

// -----------------------------------------------------------------------------

TEST_F(ProgramDriverTests, TestRunWithFailure)
{
  if (setup_invalid_run()) {
    ProgramDriver driver;

    const std::vector<char*> args{"snowlake",
                                  "--errors",
                                  "--bail",
                                  "--output",
                                  const_cast<char*>(m_output_path),
                                  const_cast<char*>(m_input_path)};

    const int res = driver.run(args.size(), (char**)args.data());
    ASSERT_EQ(EXIT_FAILURE, res);
  }
}

// -----------------------------------------------------------------------------

TEST_F(ProgramDriverTests, TestRunWithNoArguments)
{
  ProgramDriver driver;
  const std::vector<char*> args{"snowlake"};

  const int res = driver.run(args.size(), (char**)args.data());
  ASSERT_EQ(EXIT_FAILURE, res);
}

// -----------------------------------------------------------------------------
