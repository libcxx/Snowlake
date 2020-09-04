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

#pragma once

/*
 * Defines version number of the build of Snowlake.
 */

// SNOWLAKE_VERSION / 100000 is the major version.
// SNOWLAKE_VERSION / 100 % 1000 is the minor version.
// SNOWLAKE_VERSION % 100 is the patch level.
#define SNOWLAKE_VERSION 101

#define SNOWLAKE_VERSION_MAJOR (SNOWLAKE_VERSION / 100000)
#define SNOWLAKE_VERSION_MINOR (SNOWLAKE_VERSION / 100 % 1000)
#define SNOWLAKE_VERSION_PATCH (SNOWLAKE_VERSION % 100)

// Canonical version.
#define SNOWLAKE_VERSION_STRING "0.1.1"

// Default program name.
#define SNOWLAKE_PROG_NAME "snowlakec"

// Default program description.
#define SNOWLAKE_PROG_DESC "Snowlake compiler."

// Default program long description.
#define SNOWLAKE_PROG_DESC_LONG                                                \
  "Snowlake is both a declarative language of regular rules of inference\n"    \
  "and propositional logic for defining static type inference rules of\n"      \
  "programming languages, as well as a compiler-compiler that can\n"           \
  "synthesize such inference rule definitions into code used for\n"            \
  "static type checking, typically used for semantic analysis\n"               \
  "in language compilers."

// Default program usage string.
#define SNOWLAKE_PROG_USAGE "[OPTION]... INPUT"
