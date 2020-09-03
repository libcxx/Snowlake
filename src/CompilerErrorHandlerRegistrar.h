#pragma once

#include <cstdint>

#include "CompilerErrorPrinter.h"

class CompilerErrorHandlerRegistrar {
private:
    static uint32_t _registeredID;

public:
    template <typename T>
    static void RegisterScopedCompilerErrorHandler(T* handler) {
        switch (T::ID) {
        case CompilerErrorPrinter::ID:
            _errorPrinter = handler;
            _registeredID = T::ID;
            break;
        default:
            break;
        }
    }

    static void RegisterCompilerError(CompilerError&& error) {
        switch (_registeredID) {
        case CompilerErrorPrinter::ID:
            _errorPrinter->printError(error);
            break;
        default:
            break;
        }
    }

    static void UnregisterScopedCompilerErrorHandler() {
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

uint32_t CompilerErrorHandlerRegistrar::_registeredID = 0;
CompilerErrorPrinter* CompilerErrorHandlerRegistrar::_errorPrinter = nullptr;

