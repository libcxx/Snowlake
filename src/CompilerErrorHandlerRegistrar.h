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
