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
#include "SemanticAnalyzer.h"
#include "Synthesizer.h"
#include "SynthesizerUtil.h"
#include "ast.h"
#include "parser/ParserDriver.h"

#include <cstdio>
#include <fstream>
#include <gtest/gtest.h>
#include <streambuf>
#include <string>
#include <tuple>

// -----------------------------------------------------------------------------

class SynthesizerTests : public ::testing::Test
{
protected:
  void testSynthesisWithSuccess(const char* input)
  {
    ASTModule module;
    bool res;
    std::tie(module, res) = parseFromString(input);
    ASSERT_EQ(0, res);
    SemanticAnalyzer analyzer;
    res = analyzer.run(module);
    ASSERT_TRUE(res);

    Synthesizer::Options opts{
        .useException = false,
        .inputFilepath = "./SampleInput.sl", // give it a dummy filepath
        .outputPath = outputPath,
    };
    Synthesizer synthesizer(opts);

    res = synthesizer.run(module);
    ASSERT_TRUE(res);
  }

  std::tuple<ASTModule, bool> parseFromString(const char* input) const
  {
    ParserDriver parser;
    int res = parser.parseFromString(input);
    return std::make_tuple(parser.module(), res);
  }

  std::string readFromOutputFile(const char* filepath) const
  {
    std::ifstream ifs(filepath);
    return std::string((std::istreambuf_iterator<char>(ifs)),
                       std::istreambuf_iterator<char>());
  }

  void assertOutputFileContent(const char* expected_res,
                               const char* filename) const
  {
    char outputFilepath[32] = {0};
    snprintf(outputFilepath, sizeof(outputFilepath), "%s%s", outputPath,
             filename);
    const auto actual_res = readFromOutputFile(outputFilepath);

    ASSERT_STREQ(expected_res, actual_res.c_str());
  }

  const char* outputPath = "./";
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
      "ClassName                      : MyInference;"
      "TypeClass                      : TypeCls;"
      "ProofMethod                    : proveType;"
      "TypeCmpMethod                  : cmpType;"
      "TypeAnnotationSetupMethod      : typeAnnotationSetup;"
      "TypeAnnotationTeardownMethod   : typeAnnotationTeardown;"
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
          "StaticMethodCallStmt.caller_type : CLS_TYPE while {"
            "ArgumentsTypes[] <= ParameterTypes[] inrange 1..1..ParameterTypes[];"
          "};"
          "StaticMethodCallStmt.caller_type               : getBaseType();"
          "StaticMethodCallStmt.return_type               : returnType;"
        "]"
        ""
        "proposition : baseType(returnType);"
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

  testSynthesisWithSuccess(INPUT);

  // Check header file content.
  {
    // clang-format off
    const char* EXPECTED_HEADER_RES =
      "/**\n"
      " * Auto-generated by Snowlake compiler (version 0.0.1).\n"
      " */\n"
      "\n"
      "/**\n"
      " * This file was synthesized from ./SampleInput.sl\n"
      " */\n"
      "\n"
      "#pragma once\n"
      "\n"
      "#include <cstdlib>\n"
      "#include <cstddef>\n"
      "#include <vector>\n"
      "#include <system_error>\n"
      "\n"
      "/**\n"
      " * This class was synthesized from the \"MyInference\" rules group.\n"
      " */\n"
      "class MyInference\n"
      "{\n"
      "public:\n"
      "    TypeCls MethodStaticDispatch(const ASTExpr& StaticMethodCallStmt, std::error_code*);\n"
      "    TypeCls BinaryExpressionInference(const Expr& expr, std::error_code*);\n"
      "};\n"
      "";
    // clang-format on

    assertOutputFileContent(EXPECTED_HEADER_RES, "MyInference.h");
  }

  // Check .cpp file content.
  {
    // clang-format off
    const char* EXPECTED_CPP_RES =
      "/**\n"
      " * Auto-generated by Snowlake compiler (version 0.0.1).\n"
      " */\n"
      "\n"
      "/**\n"
      " * This file was synthesized from ./SampleInput.sl\n"
      " */\n"
      "\n"
      "#include \"MyInference.h\"\n"
      "#include \"InferenceErrorDefn.h\"\n"
      "\n"
      "TypeCls\n"
      "MyInference::MethodStaticDispatch(const ASTExpr& StaticMethodCallStmt, std::error_code* err)\n"
      "{\n"
      "    std::vector<TypeCls> ArgumentsTypes = proveType(StaticMethodCallStmt.argument_types);\n"
      "    std::vector<TypeCls> ParameterTypes = proveType(StaticMethodCallStmt.callee.parameter_types);\n"
      "    for (size_t i = 0, size_t j = 1; i < ParameterTypes.size(); ++i, ++j) {\n"
      "        if (!cmpType(ArgumentsTypes[i], ParameterTypes[j], std::less_equal<TypeCls>())) {\n"
      "            *err = std::error_code(0, inference_error_category);\n"
      "            return TypeCls();\n"
      "        }\n"
      "    }\n"
      "\n"
      "    if (!cmpType(ArgumentsTypes, SELF_TYPE, std::not_equal_to<TypeCls>())) {\n"
      "        *err = std::error_code(0, inference_error_category);\n"
      "        return TypeCls();\n"
      "    }\n"
      "\n"
      "    // Type annotation setup.\n"
      "    typeAnnotationSetup(StaticMethodCallStmt.caller_type, CLS_TYPE);\n"
      "\n"
      "    for (size_t i = 1, size_t j = 1; i < ParameterTypes.size(); ++i, ++j) {\n"
      "        if (!cmpType(ArgumentsTypes[i], ParameterTypes[j], std::less_equal<TypeCls>())) {\n"
      "            *err = std::error_code(0, inference_error_category);\n"
      "            return TypeCls();\n"
      "        }\n"
      "    }\n"
      "\n"
      "    // Type annotation teardown.\n"
      "    typeAnnotationTeardown(StaticMethodCallStmt.caller_type, CLS_TYPE);\n"
      "\n"
      "    TypeCls var0 = getBaseType();\n"
      "    TypeCls var1 = proveType(StaticMethodCallStmt.caller_type);\n"
      "    if (!cmpType(var0, var1, std::equal_to<>())) {\n"
      "        *err = std::error_code(0, inference_error_category);\n"
      "        return TypeCls();\n"
      "    }\n"
      "\n"
      "    TypeCls returnType = proveType(StaticMethodCallStmt.return_type);\n"
      "    return baseType(returnType);\n"
      "}\n"
      "\n"
      "TypeCls\n"
      "MyInference::BinaryExpressionInference(const Expr& expr, std::error_code* err)\n"
      "{\n"
      "    TypeCls Expr = proveType(expr.lhs);\n"
      "    TypeCls Expr = proveType(expr.rhs);\n"
      "    return Expr;\n"
      "}\n"
      "";
    // clang-format on

    assertOutputFileContent(EXPECTED_CPP_RES, "MyInference.cpp");
  }

  // Check error code definition header file content.
  {
    // clang-format off
    const char* EXPECTED_EC_HEADER_RES =
      "/**\n"
      " * Auto-generated by Snowlake compiler (version 0.0.1).\n"
      " */\n"
      "#pragma once\n"
      "\n"
      "enum InferenceError\n"
      "{\n"
      "    InferenceErrorInferredTypeMismatch = 0x01,\n"
      "    InferenceErrorTypeComparisonFailed,\n"
      "};\n"
      "\n"
      "class InferenceErrorCategory;\n"
      "extern const InferenceErrorCategory inference_error_category;\n"
      "";
    // clang-format on

    assertOutputFileContent(EXPECTED_EC_HEADER_RES,
                            SYNTHESIZED_ERROR_CODE_HEADER_FILENAME);
  }

  // Check error code definition .cpp file content.
  {
    // clang-format off
    const char* EXPECTED_EC_CPP_RES =
      "/**\n"
      " * Auto-generated by Snowlake compiler (version 0.0.1).\n"
      " */\n"
      "#include \"InferenceErrorDefn.h\"\n"
      "#include <string>\n"
      "#include <system_error>\n"
      "\n"
      "class InferenceErrorCategory : public std::error_category\n"
      "{\n"
      "    virtual const char* name() const except override {\n"
      "        return \"Inference error\";\n"
      "    }\n"
      "\n"
      "    virtual std::string message(int condition) const override {\n"
      "        switch (condition) {\n"
      "            case InferenceErrorInferredTypeMismatch:\n"
      "                return \"Inferred type does not match with expected.\";\n"
      "            case InferenceErrorTypeComparisonFailed:\n"
      "                return \"Type comparison failed.\";\n"
      "            default:\n"
      "                return \"Inference failed (unknown error).\";\n"
      "        }\n"
      "    }\n"
      "};\n"
      "\n"
      "const InferenceErrorCategory inference_error_category {};\n"
      "";
    // clang-format on

    assertOutputFileContent(EXPECTED_EC_CPP_RES,
                            SYNTHESIZED_ERROR_CODE_CPP_FILENAME);
  }
}

// -----------------------------------------------------------------------------
