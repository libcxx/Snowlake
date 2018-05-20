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
#include "parser/ParserDriver.h"
#include <gtest/gtest.h>


class ParserTests : public ::testing::Test {};

TEST_F(ParserTests, TestDriverInitialization)
{
  ParserDriver driver;

  // Default value of trace parsing is false.
  ASSERT_FALSE(driver.trace_parsing());
}

TEST_F(ParserTests, TestParsingSuccessful)
{
  ParserDriver driver;

  const char* VALID_INPUT =
    "group MyGroup {"
      "EnvironmentClass          : ASTContext"
      "EnvironmentName           : context"
      "TypeClass                 : TypeDefn"
      "ExprClass                 : AstExpr"
      "TypeCheckMethod           : TypeCheck"
      "SubtypeCheckMethod        : isSubType"
      "AssumptionSetupMethod     : SetupTypeAssumption"
      "AssumptionTeardownMethod  : TeardownTypeAssumption"
      "inference MethodStaticDispatch {"
        ""
        "arguments: ["
          "StaticMethodCallStmt"
        "]"
        ""
        "premises: ["
          "StaticMethodCallStmt.caller : CallerType"
          "StaticMethodCallStmt.arguments : ArgumentsTypes[]"
          "CallerType <= StaticMethodCallStmt.MethodClassType"
          "ArgumentTypes <= ParameterTypes"
        "]"
        ""
        "proposition: StaticMethodCallStmt.return_type"
      "}"
    "}"
  "";

  int res = driver.parse_from_string(VALID_INPUT);
  ASSERT_EQ(0, res);
}
