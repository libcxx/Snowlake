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

#pragma once

#include "ast.h"
#include "location.hh"
#include "parser.tab.hh"
#include <string>

// Tell Flex the lexer's prototype ...
#define YY_DECL yy::Parser::symbol_type yylex(ParserDriver& driver)

// ... and declare it for the parser's sake.
YY_DECL;

class ParserDriver
{
public:
  struct Options
  {
    bool traceLexer;
    bool traceParser;
    bool suppressErrorMessages;
  };

public:
  ParserDriver();
  explicit ParserDriver(Options);
  ~ParserDriver();

  /**
   * Getter and setter for tracing lexer.
   */
  bool traceLexer() const;
  void setTraceLexer(bool);

  /**
   * Getter and setter for tracing parser.
   */
  bool traceParser() const;
  void setTraceParser(bool);

  /**
   * Getter and setter for suppressing error messages.
   */
  bool suppressErrorMessages() const;
  void setSuppressErrorMessages(bool);

  /**
   * Run the parser on input file.
   * Return 0 on success.
   */
  int parseFromFile(const std::string& filepath);

  /**
   * Run the parser on input string.
   * Return 0 on success.
   */
  int parseFromString(const char*);

  /**
   * The name of the file being parsed.
   * Used later to pass the file name to the location tracker.
   */
  const std::string& inputFile() const;
  std::string& inputFile();

  /**
   * Getter and setter for module.
   */
  const ASTModule& module() const;
  void setModule(ASTModule&&);

  /**
   * Error handlings.
   */
  void error(const yy::location& l, const std::string& m);
  void error(const std::string& m);

private:
  Options m_opts;
  std::string m_input_file;
  ASTModule m_module;
};
