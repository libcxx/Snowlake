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

class ParserTests : public ::testing::Test
{
};

TEST_F(ParserTests, TestDriverInitialization)
{
  ParserDriver driver;

  // Default value of trace parsing is false.
  ASSERT_FALSE(driver.trace_parsing());
}

TEST_F(ParserTests, TestParsingSuccessful)
{
  ParserDriver driver;

  // clang-format off
  const char* VALID_INPUT =
    "group MyGroup {"
      "EnvironmentClass          : ASTContext;"
      "EnvironmentName           : context;"
      "TypeClass                 : TypeDefn;"
      "ExprClass                 : AstExpr;"
      "TypeCheckMethod           : TypeCheck;"
      "SubtypeCheckMethod        : isSubType;"
      "AssumptionSetupMethod     : SetupTypeAssumption;"
      "AssumptionTeardownMethod  : TeardownTypeAssumption;"
      ""
      "inference MethodStaticDispatch {"
        ""
        "arguments: ["
          "StaticMethodCallStmt   : ASTExpr,"
          "context                : ASTContext"
        "]"
        ""
        "premises: ["
          "StaticMethodCallStmt.caller              : CallerType;"
          "StaticMethodCallStmt.callee.return_type  : ReturnType;"
          "StaticMethodCallStmt.class_type          : MethodClassType;"
          "StaticMethodCallStmt.function_arguments  : ArgumentsTypes[] while {"
            "StaticMethodCallStmt.callee            : CalleeType;"
            "StaticMethodCallStmt.class             : ClassType;"
          "};"
          "CallerType <= MethodClassType;"
          "ArgumentTypes <= ParameterTypes;"
        "]"
        ""
        "proposition : ReturnType;"
      "}"
    "}"
  "";
  // clang-format on

  int res = driver.parse_from_string(VALID_INPUT);
  ASSERT_EQ(0, res);
}

TEST_F(ParserTests, TestParsingInvalidInput)
{
  ParserDriver driver;

  // clang-format off
  const char* INVALID_INPUT =
    "group MyGroup {"
      "EnvironmentClass : ASTContext;"
      ""
      "inference MethodStaticDispatch {"
        ""
        "arguments: ["
        "]"
        ""
        "premises: ["
          "StaticMethodCallStmt.function_arguments  : ArgumentsTypes[] if {"
            "StaticMethodCallStmt.callee            : CalleeType;"
          "};"
          "StaticMethodCallStmt.callee.return_type  : ReturnType;"
        "]"
        ""
        "proposition : ReturnType;"
      "}"
    "}"
  "";
  // clang-format on

  int res = driver.parse_from_string(INVALID_INPUT);
  ASSERT_EQ(1, res);
}

TEST_F(ParserTests, TestParsingVariousDeducedTypes)
{
  ParserDriver driver;

  // clang-format off
  const char* INPUT =
    "group MyGroup {"
      "EnvironmentClass          : ASTContext;"
      "EnvironmentName           : context;"
      ""
      "inference MethodStaticDispatch {"
        ""
        "arguments: ["
          "IfThenElseExpr : ASTExpr"
        "]"
        ""
        "premises: ["
          "IfThenElseExpr.predicate   : Bool;"
          "IfThenElseExpr.expr1       : T1;"
          "IfThenElseExpr.expr2       : T2;"
        "]"
        ""
        "proposition : lub(T1, T2);"
      "}"
    "}"
  "";
  // clang-format on

  int res = driver.parse_from_string(INPUT);
  ASSERT_EQ(0, res);
}

TEST_F(ParserTests, TestParsingPremiseDefnWithRangeClause)
{
  ParserDriver driver;

  // clang-format off
  const char* INPUT =
    "group MyGroup {"
      "EnvironmentClass          : ASTContext;"
      "EnvironmentName           : context;"
      ""
      "inference MethodStaticDispatch {"
        ""
        "arguments: ["
          "StaticMethodCallStmt : ASTExpr"
        "]"
        ""
        "premises: ["
          "StaticMethodCallStmt.argument_types            : ArgumentsTypes[];"
          "StaticMethodCallStmt.callee.parameter_types    : ParameterTypes[];"
          "ArgumentsTypes[] <= ParameterTypes[] inrange 0..1..ParameterTypes[];"
        "]"
        ""
        "proposition : lub(T1, T2);"
      "}"
    "}"
  "";
  // clang-format on

  int res = driver.parse_from_string(INPUT);
  ASSERT_EQ(0, res);
}
