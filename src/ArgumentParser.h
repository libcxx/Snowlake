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

#include <cstddef>
#include <string>
#include <vector>

class ArgumentParser
{
public:
  ArgumentParser();
  explicit ArgumentParser(const char*);

  void add_string_parameter(const char* name, const char* description,
                            bool required, std::string*,
                            const char* default_val = nullptr);

  void add_uint32_parameter(const char* name, const char* description,
                            bool required, uint32_t* res,
                            uint32_t default_val = 0);

  void add_uint64_parameter(const char* name, const char* description,
                            bool required, uint64_t* res,
                            uint64_t default_val = 0);

  void add_float_parameter(const char* name, const char* description,
                           bool required, float* res, float default_val = 0.0f);

  void add_boolean_parameter(const char* name, const char* description,
                             bool required, bool* res,
                             bool default_val = false);

  template <typename T>
  void add_array_parameter(const char* name, const char* description,
                           bool required, std::vector<T>* res)
  {
  }

  bool option_provided(const char* name) const;

  bool check_parameters() const;

  bool parse_args(int argc, char** argv);
};
