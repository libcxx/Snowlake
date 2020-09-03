/*******************************************************************************
The MIT License (MIT)

Copyright (c) 2020 Tomiko

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

#include "CompilerError.h"

#include <algorithm>
#include <iostream>
#include <string>

struct CompilerErrorPrinter
{
  CompilerErrorPrinter(const std::string& inputFilepath, std::ostream& stream)
    : _inputFilepath(inputFilepath)
    , _out(stream)
    , _errorsCount(0)
    , _warningsCount(0)
  {
  }

  ~CompilerErrorPrinter()
  {
    if (_warningsCount || _errorsCount) {
      _out << '\n';
    }
    if (_warningsCount) {
      _out << _warningsCount;
      _out << " warning";
      if (_warningsCount > 1) {
        _out << 's';
      }
      _out << " generated." << std::endl;
    }
    if (_errorsCount) {
      _out << _errorsCount;
      _out << " error";
      if (_errorsCount > 1) {
        _out << 's';
      }
      _out << " generated." << std::endl;
    }
    _out.flush();
  }

  template <typename Iterator>
  void printErrors(Iterator first, Iterator last)
  {
    std::for_each(first, last, [&](const auto& error) { printError(error); });
  }

  void printError(const CompilerError& error)
  {
    if (error.type == CompilerError::Type::Error) {
      ++_errorsCount;
      _out << _inputFilepath << ": error: ";
    } else if (error.type == CompilerError::Type::Warning) {
      ++_warningsCount;
      _out << _inputFilepath << ": warning: ";
    }
    _out << error.message() << '\n';
  }
  const std::string& _inputFilepath;
  std::ostream& _out;
  uint32_t _errorsCount;
  uint32_t _warningsCount;
};
