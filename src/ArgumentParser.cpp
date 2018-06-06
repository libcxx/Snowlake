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
  : m_desc()
  , m_opts()
{
}

// -----------------------------------------------------------------------------

ArgumentParser::ArgumentParser(const char* desc)
  : m_desc(desc)
  , m_opts()
{
}

// -----------------------------------------------------------------------------

template <typename T>
void
ArgumentParser::add_parameter(const char* name, const char* description,
                              bool required, T* res, T default_val)
{
  CmdlOption opts{.description = description,
                  .required = required,
                  .dst = reinterpret_cast<void*>(res),
                  .default_value = {default_val}};
  m_opts[name] = opts;
}

// -----------------------------------------------------------------------------

void
ArgumentParser::add_string_parameter(const char* name, const char* description,
                                     bool required, std::string* res,
                                     const char* default_val)
{
  add_parameter<std::string>(name, description, required, res, default_val);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::add_uint32_parameter(const char* name, const char* description,
                                     bool required, uint32_t* res,
                                     uint32_t default_val)
{
  add_parameter<uint32_t>(name, description, required, res, default_val);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::add_uint64_parameter(const char* name, const char* description,
                                     bool required, uint64_t* res,
                                     uint64_t default_val)
{
  add_parameter<uint64_t>(name, description, required, res, default_val);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::add_float_parameter(const char* name, const char* description,
                                    bool required, float* res,
                                    float default_val)
{
  add_parameter<float>(name, description, required, res, default_val);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::add_double_parameter(const char* name, const char* description,
                                     bool required, double* res,
                                     double default_val)
{
  add_parameter<double>(name, description, required, res, default_val);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::add_boolean_parameter(const char* name, const char* description,
                                      bool required, bool* res,
                                      bool default_val)
{
  add_parameter<bool>(name, description, required, res, default_val);
}

// -----------------------------------------------------------------------------

bool
ArgumentParser::option_provided(const char* name) const
{
  auto itr = m_opts.find(name);
  if (itr == m_opts.end()) {
    return false;
  }

  const auto& option = itr->second;
  return option.value.value.valid();
}

// -----------------------------------------------------------------------------

bool
ArgumentParser::check_parameters() const
{
  for (const auto& pair : m_opts) {
    const auto& option = pair.second;
    if (option.required && !option.value.value.valid()) {
      return false;
    }
  }
  return true;
}

// -----------------------------------------------------------------------------

bool
ArgumentParser::parse_args(int argc, char** argv)
{
  // Parsing logic.
  // TODO: to be implemented.

  if (!check_parameters()) {
    return false;
  }

  assign_default_values();

  return true;
}

// -----------------------------------------------------------------------------

void
ArgumentParser::assign_default_values()
{
  // TODO: Rework this mechanism.
  for (auto& pair : m_opts) {
    auto& option = pair.second;
    if (!option.value.value.valid()) {
      const auto& default_value = option.default_value.value;
      if (default_value.is<std::string>()) {
        std::string* dst = reinterpret_cast<std::string*>(option.dst);
        dst->assign(default_value.get<std::string>());
      } else if (default_value.is<bool>()) {
        bool* dst = reinterpret_cast<bool*>(option.dst);
        *dst = default_value.get<bool>();
      } else if (default_value.is<uint32_t>()) {
        uint32_t* dst = reinterpret_cast<uint32_t*>(option.dst);
        *dst = default_value.get<uint32_t>();
      } else if (default_value.is<uint64_t>()) {
        uint64_t* dst = reinterpret_cast<uint64_t*>(option.dst);
        *dst = default_value.get<uint64_t>();
      } else if (default_value.is<float>()) {
        float* dst = reinterpret_cast<float*>(option.dst);
        *dst = default_value.get<float>();
      } else if (default_value.is<double>()) {
        double* dst = reinterpret_cast<double*>(option.dst);
        *dst = default_value.get<double>();
      }
    }
  }
}

// -----------------------------------------------------------------------------
