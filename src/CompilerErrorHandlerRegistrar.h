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

#include "CompilerErrorPrinter.h"

#include <cassert>
#include <cstdint>

class CompilerErrorHandlerRegistrar
{
private:
  static uint32_t _registeredID;

public:
  template <typename T>
  static void RegisterScopedCompilerErrorHandler(T* handler)
  {
    switch (T::ID) {
      case CompilerErrorPrinter::ID:
        _errorPrinter = handler;
        _registeredID = T::ID;
        break;
      default:
        break;
    }
  }

  static void RegisterCompilerError(CompilerError&& error)
  {
    switch (_registeredID) {
      case CompilerErrorPrinter::ID:
        assert(_errorPrinter && "Expects _errorPrinter to be present");
        _errorPrinter->printError(error);
        break;
      default:
        break;
    }
  }

  static void UnregisterScopedCompilerErrorHandler()
  {
    switch (_registeredID) {
      case CompilerErrorPrinter::ID:
        _errorPrinter = nullptr;
        break;
      default:
        break;
    }
    _registeredID = 0;
  }

private:
  static CompilerErrorPrinter* _errorPrinter;
};

template <typename T>
struct ScopedCompilerErrorHandlerRegister
{
  explicit ScopedCompilerErrorHandlerRegister(T* handler)
  {
    CompilerErrorHandlerRegistrar::RegisterScopedCompilerErrorHandler<T>(
        handler);
  }

  ~ScopedCompilerErrorHandlerRegister()
  {
    CompilerErrorHandlerRegistrar::UnregisterScopedCompilerErrorHandler();
  }
};
