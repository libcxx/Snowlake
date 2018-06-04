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
    bool trace_lexer;
    bool trace_parser;
    bool suppress_error_messages;
  };

public:
  ParserDriver();
  explicit ParserDriver(Options);
  ~ParserDriver();

  /**
   * Getter and setter for tracing lexer.
   */
  bool trace_lexer() const;
  void set_trace_lexer(bool);

  /**
   * Getter and setter for tracing parser.
   */
  bool trace_parser() const;
  void set_trace_parser(bool);

  /**
   * Getter and setter for suppressing error messages.
   */
  bool suppress_error_messages() const;
  void set_suppress_error_messages(bool);

  /**
   * Run the parser on input file.
   * Return 0 on success.
   */
  int parse_from_file(const std::string& filepath);

  /**
   * Run the parser on input string.
   * Return 0 on success.
   */
  int parse_from_string(const char*);

  /**
   * The name of the file being parsed.
   * Used later to pass the file name to the location tracker.
   */
  const std::string& input_file() const;
  std::string& input_file();

  /**
   * Getter and setter for module.
   */
  const ASTModule& module() const;
  void set_module(ASTModule&&);

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
