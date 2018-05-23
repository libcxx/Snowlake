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
#include "ParserDriver.h"
#include "lex.yy.hh"
#include "parser.tab.hh"

#include <fstream>
#include <iostream>
#include <iterator>

// -----------------------------------------------------------------------------

ParserDriver::ParserDriver()
    : m_trace_lexer(false)
    , m_trace_parser(false)
    , m_input_file()
    , m_module()
{
}

// -----------------------------------------------------------------------------

ParserDriver::~ParserDriver()
{
}

// -----------------------------------------------------------------------------

bool
ParserDriver::trace_lexer() const
{
  return m_trace_lexer;
}

// -----------------------------------------------------------------------------

void
ParserDriver::set_trace_lexer(bool val)
{
  m_trace_lexer = val;
}

// -----------------------------------------------------------------------------

bool
ParserDriver::trace_parser() const
{
  return m_trace_parser;
}

// -----------------------------------------------------------------------------

void
ParserDriver::set_trace_parser(bool val)
{
  m_trace_parser = val;
}

// -----------------------------------------------------------------------------

int
ParserDriver::parse_from_file(const std::string& filepath)
{
  m_input_file.assign(filepath);
  std::ifstream infile(filepath.c_str());
  std::string file_contents((std::istreambuf_iterator<char>(infile)),
                            std::istreambuf_iterator<char>());
  return parse_from_string(file_contents.c_str());
}

// -----------------------------------------------------------------------------

int
ParserDriver::parse_from_string(const char* input)
{
  YY_BUFFER_STATE buf;
  buf = yy_scan_string(input);

  // Trace lexer.
  yyset_debug(trace_lexer());

  yy::Parser parser(*this);

  // Trace parser.
  parser.set_debug_level(trace_parser());

  const int res = parser.parse();

  // Teardown.
  {
    yy_delete_buffer(buf);
    yylex_destroy();
  }

  return res;
}

// -----------------------------------------------------------------------------

const std::string&
ParserDriver::input_file() const
{
  return m_input_file;
}

// -----------------------------------------------------------------------------

std::string&
ParserDriver::input_file()
{
  return m_input_file;
}

// -----------------------------------------------------------------------------

const ASTModule&
ParserDriver::module() const
{
  return m_module;
}

// -----------------------------------------------------------------------------

void
ParserDriver::set_module(ASTModule&& module)
{
  m_module = module;
}

// -----------------------------------------------------------------------------

void
ParserDriver::error(const yy::location& l, const std::string& m)
{
  std::cerr << "Parser error [" << l << "] : " << m << std::endl;
}

// -----------------------------------------------------------------------------

void
ParserDriver::error(const std::string& m)
{
  std::cerr << "Parser error: " << m << std::endl;
}

// -----------------------------------------------------------------------------
