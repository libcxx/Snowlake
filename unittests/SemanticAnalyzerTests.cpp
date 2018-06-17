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

#include "SemanticAnalyzer.h"
#include "parser/ParserDriver.h"

#include <cstdio>
#include <tuple>

// -----------------------------------------------------------------------------

class SemanticAnalyzerTests : public ::testing::Test
{
protected:
  void assert_first_error(const char* input, const char* msg)
  {
    ASTModule module;
    bool res;
    std::tie(module, res) = parse_from_string(input);
    ASSERT_EQ(0, res);
    SemanticAnalyzer analyzer;
    res = analyzer.run(module);
    ASSERT_FALSE(res);
    ASSERT_FALSE(analyzer.errors().empty());
    const auto& error = analyzer.errors()[0];
    ASSERT_STREQ(error.msg.c_str(), msg);
  }

  void assert_no_error(const char* input)
  {
    ASTModule module;
    bool res;
    std::tie(module, res) = parse_from_string(input);
    ASSERT_EQ(0, res);
    SemanticAnalyzer analyzer;
    res = analyzer.run(module);
    if (!analyzer.errors().empty()) {
      const auto& error = analyzer.errors()[0];
      printf("Unexpected Error: %s.\n", error.msg.c_str());
    }
    ASSERT_TRUE(res);
  }

private:
  std::tuple<ASTModule, bool> parse_from_string(const char* input) const
  {
    ParserDriver parser;
    int res = parser.parse_from_string(input);
    return std::make_tuple(parser.module(), res);
  }
};

// -----------------------------------------------------------------------------

TEST_F(SemanticAnalyzerTests, TestDefaultInitialization)
{
  SemanticAnalyzer analyzer;
}

// -----------------------------------------------------------------------------

TEST_F(SemanticAnalyzerTests, TestInitializationWithOptions)
{
  SemanticAnalyzer::Options opts;
  SemanticAnalyzer analyzer(opts);
}

// -----------------------------------------------------------------------------

TEST_F(SemanticAnalyzerTests, TestWithRepeatingGroup)
{
  // clang-format off
  const char* INPUT =
    "group MyGroup {"
    "}"
    ""
    "group MyGroup {"
    "}"
    "";
  // clang-format on

  const char* msg = "Found multiple inference group with name \"MyGroup\".";

  assert_first_error(INPUT, msg);
}

// -----------------------------------------------------------------------------

TEST_F(SemanticAnalyzerTests, TestWithRepeatingSymbolInInferenceDefinition)
{
  // clang-format off
  const char* INPUT =
    "group MyGroup {"
      "ClassName          : MyGroup;"
      "TypeClass          : TypeCls;"
      "ProofMethod        : proveType;"
      "TypeCmpMethod      : cmpType;"
      ""
      "inference MyInference {"
        "globals: ["
          "Arg1"
        "]"
        "arguments: [Arg1 : ArgType]"
        "premises: []"
        "proposition: HelloWorld;"
      "}"
    "}"
    "";
  // clang-format on

  const char* msg = "Found duplicate symbol (argument) with name \"Arg1\".";

  assert_first_error(INPUT, msg);
}

// -----------------------------------------------------------------------------

TEST_F(SemanticAnalyzerTests, TestWithValidInput)
{
  // clang-format off
  const char* INPUT =
    "group MyGroup {"
      "ClassName          : MyGroup;"
      "TypeClass          : TypeCls;"
      "ProofMethod        : proveType;"
      "TypeCmpMethod      : cmpType;"
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

  assert_no_error(INPUT);
}

// -----------------------------------------------------------------------------

TEST_F(SemanticAnalyzerTests, TestWithInvalidPropositionTarget)
{
  // clang-format off
  const char* INPUT =
    "group MyGroup {"
      "ClassName          : MyGroup;"
      "TypeClass          : TypeCls;"
      "ProofMethod        : proveType;"
      "TypeCmpMethod      : cmpType;"
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
        "]"
        ""
        "proposition : SomeType;"
      "}"
    "}"
    "";
  // clang-format on

  const char* msg =
      "Invalid proposition target type in inference \"MethodStaticDispatch\".";

  assert_first_error(INPUT, msg);
}

// -----------------------------------------------------------------------------

TEST_F(SemanticAnalyzerTests, TestWithInvalidRangeClauseTarget)
{
  // clang-format off
  const char* INPUT =
    "group MyGroup {"
      "ClassName          : MyGroup;"
      "TypeClass          : TypeCls;"
      "ProofMethod        : proveType;"
      "TypeCmpMethod      : cmpType;"
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
          "StaticMethodCallStmt.argument_types : ArgumentsTypes;"
          "StaticMethodCallStmt.parameter_types : ParameterTypes;"
          "StaticMethodCallStmt.return_type: ReturnType;"
          "ArgumentsTypes[] <= ParameterTypes[] inrange 0..1..ParameterTypes;"
        "]"
        ""
        "proposition : ReturnType;"
      "}"
    "}"
    "";
  // clang-format on

  const char* msg =
      "Invalid target in range clause in inference \"MethodStaticDispatch\".";

  assert_first_error(INPUT, msg);
}

// -----------------------------------------------------------------------------

TEST_F(SemanticAnalyzerTests, TestWithIncompatibleTargetsInRangeClause)
{
  // clang-format off
  const char* INPUT =
    "group MyGroup {"
      "ClassName          : MyGroup;"
      "TypeClass          : TypeCls;"
      "ProofMethod        : proveType;"
      "TypeCmpMethod      : cmpType;"
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
          "StaticMethodCallStmt.argument_types : ArgumentsTypes;"
          "StaticMethodCallStmt.parameter_types : ParameterTypes;"
          "StaticMethodCallStmt.return_type: ReturnType;"
          "ReturnType <= ParameterTypes[] inrange 0..1..ParameterTypes[];"
        "]"
        ""
        "proposition : ReturnType;"
      "}"
    "}"
    "";
  // clang-format on

  const char* msg = "Incompatible targets in expression in inference "
                    "\"MethodStaticDispatch\".";

  assert_first_error(INPUT, msg);
}

// -----------------------------------------------------------------------------

TEST_F(SemanticAnalyzerTests, TestWithInvalidPremiseInNestedWhileClause)
{
  // clang-format off
  const char* INPUT =
    "group MyGroup {"
      "ClassName          : MyGroup;"
      "TypeClass          : TypeCls;"
      "ProofMethod        : proveType;"
      "TypeCmpMethod      : cmpType;"
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
          "StaticMethodCallStmt.argument_types : ArgumentsTypes;"
          "StaticMethodCallStmt.parameter_types : ParameterTypes;"
          "StaticMethodCallStmt.return_type: ReturnType while {"
          "ReturnType <= ParameterTypes[] inrange 0..1..ParameterTypes[];"
          "};"
        "]"
        ""
        "proposition : ReturnType;"
      "}"
    "}"
    "";
  // clang-format on

  const char* msg = "Incompatible targets in expression in inference "
                    "\"MethodStaticDispatch\".";

  assert_first_error(INPUT, msg);
}

// -----------------------------------------------------------------------------

TEST_F(SemanticAnalyzerTests,
       TestWithRepeatedIncompatibleTargetInPremiseDefinition)
{
  // clang-format off
  const char* INPUT =
    "group MyGroup {"
      "ClassName          : MyGroup;"
      "TypeClass          : TypeCls;"
      "ProofMethod        : proveType;"
      "TypeCmpMethod      : cmpType;"
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
          "StaticMethodCallStmt.argument_types : ArgumentsTypes;"
          "StaticMethodCallStmt.parameter_types : ArgumentsTypes[];"
          "StaticMethodCallStmt.return_type: ReturnType;"
        "]"
        ""
        "proposition : ReturnType;"
      "}"
    "}"
    "";
  // clang-format on

  const char* msg = "Found duplicate and incompatible target in inference "
                    "\"MethodStaticDispatch\".";

  assert_first_error(INPUT, msg);
}

// ----------------------------------------------------------------------------

TEST_F(SemanticAnalyzerTests,
       TestWithRepeatedIncompatibleArrayTargetInPremiseDefinition)
{
  // clang-format off
  const char* INPUT =
    "group MyGroup {"
      "ClassName          : MyGroup;"
      "TypeClass          : TypeCls;"
      "ProofMethod        : proveType;"
      "TypeCmpMethod      : cmpType;"
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
          "StaticMethodCallStmt.argument_types : ArgumentsTypes[5];"
          "StaticMethodCallStmt.parameter_types : ArgumentsTypes[];"
          "StaticMethodCallStmt.return_type: ReturnType;"
        "]"
        ""
        "proposition : ReturnType;"
      "}"
    "}"
    "";
  // clang-format on

  const char* msg = "Found duplicate and incompatible target in inference "
                    "\"MethodStaticDispatch\".";

  assert_first_error(INPUT, msg);
}

// ---------------------------------------------------------------------------

TEST_F(SemanticAnalyzerTests, TestValidInputWithAllComputedTargetTypes)
{
  // clang-format off
  const char* INPUT =
    "group MyGroup {"
      "ClassName          : MyGroup;"
      "TypeClass          : TypeCls;"
      "ProofMethod        : proveType;"
      "TypeCmpMethod      : cmpType;"
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
          "StaticMethodCallStmt.argument_types : getArgumentsTypes();"
          "StaticMethodCallStmt.parameter_types : getParametersTypes();"
          "StaticMethodCallStmt.return_type: getReturnType();"
        "]"
        ""
        "proposition : getReturnType();"
      "}"
    "}"
    "";
  // clang-format on

  assert_no_error(INPUT);
}

// --------------------------------------------------------------------------
