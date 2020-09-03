#pragma once

#include <cstdint>

enum ParserErrors : uint32_t
{
  kParserBadInputError,
  kParserInvalidSyntaxError
};
