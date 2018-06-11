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
#include "Synthesizer.h"
#include "ast.h"
#include "parser/ParserDriver.h"

#include <cstdio>
#include <fstream>
#include <streambuf>
#include <string>
#include <tuple>

// -----------------------------------------------------------------------------

class SynthesizerTests : public ::testing::Test
{
protected:
  void test_synthesis_with_success(const char* input)
  {
    ASTModule module;
    bool res;
    std::tie(module, res) = parse_from_string(input);
    ASSERT_EQ(0, res);
    SemanticAnalyzer analyzer;
    res = analyzer.run(module);
    ASSERT_TRUE(res);

    Synthesizer::Options opts{.output_path = output_path};
    Synthesizer synthesizer(opts);

    res = synthesizer.run(module);
    ASSERT_TRUE(res);
  }

  std::tuple<ASTModule, bool> parse_from_string(const char* input) const
  {
    ParserDriver parser;
    int res = parser.parse_from_string(input);
    return std::make_tuple(parser.module(), res);
  }

  std::string read_from_output_file(const char* filepath) const
  {
    std::ifstream ifs(filepath);
    return std::string((std::istreambuf_iterator<char>(ifs)),
                       std::istreambuf_iterator<char>());
  }

  const char* output_path = "./";
};

// -----------------------------------------------------------------------------

TEST_F(SynthesizerTests, TestDefaultInitialization)
{
  Synthesizer Synthesizer;
}

// -----------------------------------------------------------------------------

TEST_F(SynthesizerTests, TestSynthesisWithSuccess)
{
  // clang-format off
  static const char* INPUT =
    "group MyGroup {"
      "ClassName                 : MyInference;"
      "TypeClass                 : TypeCls;"
      "ProofMethod               : proveType;"
      "TypeCmpMethod             : cmpType;"
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
          "StaticMethodCallStmt.argument_types            : ArgumentsTypes[];"
          "StaticMethodCallStmt.callee.parameter_types    : ParameterTypes[];"
          "ArgumentsTypes[] <= ParameterTypes[] inrange 0..1..ParameterTypes[];"
          "ArgumentsTypes[0] != SELF_TYPE;"
        "]"
        ""
        "proposition : lub(T1, T2);"
      "}"
      ""
      "inference BinaryExpressionInference {"
        ""
        "arguments: ["
          "expr : Expr"
        "]"
        ""
        "premises: ["
          "expr.lhs   : Expr;"
          "expr.rhs   : Expr;"
        "]"
        ""
        "proposition  : Expr;"
      "}"
    "}"
    "";
  // clang-format on

  // clang-format off
  const char* EXPECTED_RES =
    "/**\n"
    " * Auto-generated by Snowlake compiler.\n"
    " */\n"
    "#include <cstdlib>\n"
    "#include <cstddef>\n"
    "#include <vector>\n"
    "\n"
    "\n"
    "TypeCls\n"
    "MyInference::MethodStaticDispatch(const ASTExpr& StaticMethodCallStmt)\n"
    "{\n"
    "    std::vector<TypeCls> ArgumentsTypes = proveType(StaticMethodCallStmt.argument_types);\n"
    "    std::vector<TypeCls> ParameterTypes = proveType(StaticMethodCallStmt.callee.parameter_types);\n"
    "    for (size_t i = 0, size_t j = 1; i < ParameterTypes.size(); ++i, ++j) {\n"
    "        if (cmpType(ArgumentsTypes[i], ParameterTypes[j], std::less_equal<TypeCls>())) {\n"
    "        }\n"
    "    }\n"
    "\n"
    "    if (cmpType(ArgumentsTypes, SELF_TYPE, std::not_equal_to<TypeCls>())) {\n"
    "    }\n"
    "    return lub(T1, T2);\n"
    "}\n"
    "\n"
    "\n"
    "TypeCls\n"
    "MyInference::BinaryExpressionInference(const Expr& expr)\n"
    "{\n"
    "    TypeCls Expr = proveType(expr.lhs);\n"
    "    TypeCls Expr = proveType(expr.rhs);\n"
    "    return Expr;\n"
    "}\n"
    "";
  // clang-format on

  test_synthesis_with_success(INPUT);

  char output_filepath[32] = {0};
  snprintf(output_filepath, sizeof(output_filepath), "%s%s", output_path,
           "MyInference.cpp");

  const auto actual_res = read_from_output_file(output_filepath);

  ASSERT_STREQ(EXPECTED_RES, actual_res.c_str());
}

// -----------------------------------------------------------------------------
