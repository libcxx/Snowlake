#include "CompilerErrorHandlerRegistrar.h"

uint32_t CompilerErrorHandlerRegistrar::_registeredID = 0;
CompilerErrorPrinter* CompilerErrorHandlerRegistrar::_errorPrinter = nullptr;
