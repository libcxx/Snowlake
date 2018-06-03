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


// -----------------------------------------------------------------------------

class SemanticAnalyzerTests : public ::testing::Test
{
protected:
  void assert_first_error(const char* input, const char* msg) {
    ASTModule module = parse_from_string(input);
    SemanticAnalyzer analyzer;
    bool res = analyzer.run(module);
    ASSERT_FALSE(res);
    ASSERT_FALSE(analyzer.errors().empty());
    const auto& error = analyzer.errors()[0];
    ASSERT_STREQ(error.msg.c_str(), msg);
  }

private:
  ASTModule parse_from_string(const char* input) const {
    ParserDriver parser;
    parser.parse_from_string(input);
    return parser.module();
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
  const char* INPUT =
    "group MyGroup {"
    "}"
    ""
    "group MyGroup {"
    "}"
  "";

  const char* msg = "Found multiple inference group with name \"MyGroup\".";

  assert_first_error(INPUT, msg);
}

// -----------------------------------------------------------------------------

TEST_F(SemanticAnalyzerTests, TestWithRepeatingSymbolInInferenceDefinition)
{
  const char* INPUT =
    "group MyGroup {"
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

  const char* msg = "Found duplicate symbol (argument) with name \"Arg1\".";

  assert_first_error(INPUT, msg);
}

// -----------------------------------------------------------------------------
