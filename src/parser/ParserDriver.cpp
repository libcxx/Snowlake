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
  : m_opts(ParserDriver::Options{.traceLexer = false,
                                 .traceParser = false,
                                 .suppressErrorMessages = false})
  , m_input_file()
  , m_module()
{
}

// -----------------------------------------------------------------------------

ParserDriver::ParserDriver(Options opts)
  : m_opts(opts)
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
ParserDriver::traceLexer() const
{
  return m_opts.traceLexer;
}

// -----------------------------------------------------------------------------

void
ParserDriver::setTraceLexer(bool val)
{
  m_opts.traceLexer = val;
}

// -----------------------------------------------------------------------------

bool
ParserDriver::traceParser() const
{
  return m_opts.traceParser;
}

// -----------------------------------------------------------------------------

void
ParserDriver::setTraceParser(bool val)
{
  m_opts.traceParser = val;
}

// -----------------------------------------------------------------------------

bool
ParserDriver::suppressErrorMessages() const
{
  return m_opts.suppressErrorMessages;
}

// -----------------------------------------------------------------------------

void
ParserDriver::setSuppressErrorMessages(bool val)
{
  m_opts.suppressErrorMessages = val;
}

// -----------------------------------------------------------------------------

int
ParserDriver::parseFromFile(const std::string& filepath)
{
  m_input_file.assign(filepath);
  std::ifstream infile(filepath.c_str());
  if (!infile.good()) {
    return -1;
  }
  std::string file_contents((std::istreambuf_iterator<char>(infile)),
                            std::istreambuf_iterator<char>());
  infile.close();
  return parseFromString(file_contents.c_str());
}

// -----------------------------------------------------------------------------

int
ParserDriver::parseFromString(const char* input)
{
  YY_BUFFER_STATE buf;
  buf = yy_scan_string(input);

  // Trace lexer.
  yyset_debug(traceLexer());

  yy::Parser parser(*this);

  // Trace parser.
  parser.set_debug_level(traceParser());

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
ParserDriver::inputFile() const
{
  return m_input_file;
}

// -----------------------------------------------------------------------------

std::string&
ParserDriver::inputFile()
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
ParserDriver::setModule(ASTModule&& module)
{
  m_module = module;
}

// -----------------------------------------------------------------------------

void
ParserDriver::error(const yy::location& l, const std::string& m)
{
  if (!suppressErrorMessages()) {
    std::cerr << "Parser error [" << l << "] : " << m << std::endl;
  }
}

// -----------------------------------------------------------------------------

void
ParserDriver::error(const std::string& m)
{
  if (!suppressErrorMessages()) {
    std::cerr << "Parser error: " << m << std::endl;
  }
}

// -----------------------------------------------------------------------------
