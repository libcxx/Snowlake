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

#include "ArgumentParser.h"

// -----------------------------------------------------------------------------

ArgumentParser::ArgumentParser()
{
}

// -----------------------------------------------------------------------------

ArgumentParser::ArgumentParser(const char*)
{
}

// -----------------------------------------------------------------------------

void
ArgumentParser::add_string_parameter(const char* name, const char* description,
                                     std::string*)
{
}

// -----------------------------------------------------------------------------

void
ArgumentParser::add_uint32_parameter(const char* name, const char* description,
                                     uint32_t* res)
{
}

// -----------------------------------------------------------------------------

void
ArgumentParser::add_uint64_parameter(const char* name, const char* description,
                                     uint64_t* res)
{
}

// -----------------------------------------------------------------------------

void
ArgumentParser::add_float_parameter(const char* name, const char* description,
                                    float* res)
{
}

// -----------------------------------------------------------------------------

bool
ArgumentParser::option_provided(const char* name) const
{
}

// -----------------------------------------------------------------------------

bool
ArgumentParser::check_parameters() const
{
}

// -----------------------------------------------------------------------------

bool
ArgumentParser::parse_args(int argc, char** argv)
{
}

// -----------------------------------------------------------------------------
