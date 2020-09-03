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

#include "ParserErrorCategory.h"
#include "ParserErrorCodes.h"
#include "lex.yy.hh"
#include "parser.tab.hh"

#include <cstdio>
#include <fstream>
#include <iterator>
#include <sstream>

// -----------------------------------------------------------------------------

ParserDriver::ParserDriver()
  : _opts(ParserDriver::Options{.traceLexer = false,
                                .traceParser = false,
                                .suppressErrorMessages = false})
  , _inputFile()
  , _module()
  , _errorPrinter(nullptr)
{
}

// -----------------------------------------------------------------------------

ParserDriver::ParserDriver(Options opts)
  : _opts(opts)
  , _inputFile()
  , _module()
  , _errorPrinter(nullptr)
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
  return _opts.traceLexer;
}

// -----------------------------------------------------------------------------

void
ParserDriver::setTraceLexer(bool val)
{
  _opts.traceLexer = val;
}

// -----------------------------------------------------------------------------

bool
ParserDriver::traceParser() const
{
  return _opts.traceParser;
}

// -----------------------------------------------------------------------------

void
ParserDriver::setTraceParser(bool val)
{
  _opts.traceParser = val;
}

// -----------------------------------------------------------------------------

bool
ParserDriver::suppressErrorMessages() const
{
  return _opts.suppressErrorMessages;
}

// -----------------------------------------------------------------------------

void
ParserDriver::setSuppressErrorMessages(bool val)
{
  _opts.suppressErrorMessages = val;
}

// -----------------------------------------------------------------------------

int
ParserDriver::parseFromFile(const std::string& filepath)
{
  _inputFile.assign(filepath);
  std::ifstream infile(filepath.c_str());
  if (!infile.good()) {
    handleErrorWithMessageAndCode("Failed to open input file",
                                  kParserBadInputError);
    return -1;
  }
  std::string fileContent((std::istreambuf_iterator<char>(infile)),
                          std::istreambuf_iterator<char>());
  infile.close();
  return parseFromString(fileContent.c_str());
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
  return _inputFile;
}

// -----------------------------------------------------------------------------

std::string&
ParserDriver::inputFile()
{
  return _inputFile;
}

// -----------------------------------------------------------------------------

const ASTModule&
ParserDriver::module() const
{
  return _module;
}

// -----------------------------------------------------------------------------

void
ParserDriver::setModule(ASTModule&& module)
{
  _module = module;
}

// -----------------------------------------------------------------------------

void
ParserDriver::setCompilerErrorPrinter(CompilerErrorPrinter* errorPrinter)
{
  _errorPrinter = errorPrinter;
}

// -----------------------------------------------------------------------------

void
ParserDriver::handleErrorWithMessageAndCode(const char* msg,
                                            CompilerError::Code code)
{
  if (_errorPrinter) {
    _errorPrinter->printError(
        ParserErrorCategory::CreateCompilerErrorWithTypeAndMessage(
            CompilerError::Type::Error, code, msg));
  } else {
    fprintf(stderr, "%s\n", msg);
  }
}

// -----------------------------------------------------------------------------

void
ParserDriver::error(const yy::location& l, const std::string& m)
{
  if (!suppressErrorMessages()) {
    std::stringstream ss;
    ss << l;
    char buf[1024] = {0};
    snprintf(buf, sizeof(buf), "Parser error: %s [%s]", m.c_str(),
             ss.str().c_str());
    handleErrorWithMessageAndCode(buf, kParserInvalidSyntaxError);
  }
}

// -----------------------------------------------------------------------------

void
ParserDriver::error(const std::string& m)
{
  if (!suppressErrorMessages()) {
    char buf[1024] = {0};
    snprintf(buf, sizeof(buf), "Parser error: %s", m.c_str());
    handleErrorWithMessageAndCode(buf, kParserInvalidSyntaxError);
  }
}

// -----------------------------------------------------------------------------
