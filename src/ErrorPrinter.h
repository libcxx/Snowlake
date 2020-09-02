#pragma once

#include "Error.h"

#include <algorithm>
#include <iostream>
#include <string>

struct ErrorPrinter
{
  ErrorPrinter(const std::string& inputFilepath, std::ostream& stream)
    : _inputFilepath(inputFilepath)
    , _out(stream)
    , _errorsCount(0)
    , _warningsCount(0)
  {
  }

  ~ErrorPrinter()
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

  void printError(const Error& error)
  {
    if (error.code == Error::ErrorCode::Error) {
      ++_errorsCount;
      _out << _inputFilepath << ": error: ";
    } else if (error.code == Error::ErrorCode::Warning) {
      ++_warningsCount;
      _out << _inputFilepath << ": warning: ";
    }
    _out << error.msg << '\n';
  }
  const std::string& _inputFilepath;
  std::ostream& _out;
  uint32_t _errorsCount;
  uint32_t _warningsCount;
};
