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
#include <sstream>
#include <string>

// -----------------------------------------------------------------------------

#define ADVANCE_ARGV                                                           \
  do {                                                                         \
    ++argv;                                                                    \
    --argc;                                                                    \
  } while (0)

#define CHECK_ARGV                                                             \
  do {                                                                         \
    if (argc < 1 || argv == nullptr) {                                         \
      return false;                                                            \
    }                                                                          \
  } while (0)

// -----------------------------------------------------------------------------

ArgumentParser::ArgumentParser()
  : m_name()
  , m_desc()
  , m_opts()
  , m_positional_args()
  , m_positional_args_required(0)
{
}

// -----------------------------------------------------------------------------

ArgumentParser::ArgumentParser(const char* name)
  : m_name(name)
  , m_desc()
  , m_opts()
  , m_positional_args()
  , m_positional_args_required(0)
{
}

// -----------------------------------------------------------------------------

ArgumentParser::ArgumentParser(const char* name, const char* description)
  : m_name(name)
  , m_desc(description)
  , m_opts()
  , m_positional_args()
  , m_positional_args_required(0)
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
ArgumentParser::__check_parameters() const
{
  // Check if we meet the required number of positional arguments.
  if (m_positional_args.size() < m_positional_args_required) {
    return false;
  }

  for (const auto& pair : m_opts) {
    const auto& option = pair.second;
    if (option.required && !option.value.value.valid()) {
      return false;
    }
  }
  return true;
}

// -----------------------------------------------------------------------------

void
ArgumentParser::set_positional_args_required(size_t n)
{
  m_positional_args_required = n;
}

// -----------------------------------------------------------------------------

bool
ArgumentParser::parse_args(int argc, char** argv)
{
  if (!__parse_args(argc, argv)) {
    return false;
  }

  if (!__check_parameters()) {
    return false;
  }

  __assign_values();

  return true;
}

// -----------------------------------------------------------------------------

const ArgumentParser::PositionalArgumentList&
ArgumentParser::positional_args() const
{
  return m_positional_args;
}

// -----------------------------------------------------------------------------

template <typename Stream>
void
ArgumentParser::__print_help(Stream& stream) const
{
  stream << (m_name.empty() ? "PROGRAM" : m_name) << std::endl;

  if (!m_desc.empty()) {
    stream << std::endl;
    stream << m_desc;
    stream << std::endl;
  }
  stream << std::endl;

#define HELP_MENU_DASHDASH "--"
#define HELP_MENU_VALUE_PLACEHOLDER " <value>"
#define HELP_MENU_INDENT "\t\t\t"
#define HELP_MENU_OPTIONS_LABEL "OPTIONS:"

  stream << HELP_MENU_OPTIONS_LABEL << std::endl;

  for (const auto& pair : m_opts) {
    const auto& name = pair.first;
    const auto& option = pair.second;
    const auto& default_value = option.default_value.value;

    stream << HELP_MENU_DASHDASH << name;
    if (!default_value.is<bool>()) {
      stream << HELP_MENU_VALUE_PLACEHOLDER;
    }
    stream << std::endl;
    stream << HELP_MENU_INDENT << option.description;
    if (option.required) {
      stream << " (Optional)";
    }
    stream << '.' << std::endl;
  }
}

// -----------------------------------------------------------------------------

std::string
ArgumentParser::help() const
{
  std::stringstream ss;
  __print_help(ss);
  return ss.str();
}

// -----------------------------------------------------------------------------

void
ArgumentParser::print_help() const
{
  __print_help(std::cout);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::__add_positional_parameter(std::string&& arg)
{
  m_positional_args.emplace_back(arg);
}

// -----------------------------------------------------------------------------

bool
ArgumentParser::__parse_args(int argc, char** argv)
{
  CHECK_ARGV;

  // Skip program name.
  ADVANCE_ARGV;

  while (argc) {
    char* arg = *(argv);
    if (arg == nullptr) {
      return false;
    }
    std::string s(arg);
    if (s.find("--") == 0) {
      std::string key = s.substr(2);
      if (key.empty()) {
        return false;
      }
      if (!m_opts.count(key)) {
        return false;
      }
      if (__defined_boolean_option(key)) {
        m_opts[key].value.value = true;
      } else {
        ADVANCE_ARGV;
        CHECK_ARGV;
        std::string val(*argv);
        // Update option value.
        __update_option_value(key, val);
      }
    } else {
      __add_positional_parameter(std::move(s));
    }
    ADVANCE_ARGV;
  }

  return true;
}

// -----------------------------------------------------------------------------

bool
ArgumentParser::__defined_boolean_option(const std::string& name) const
{
  auto itr = m_opts.find(name);
  if (itr == m_opts.end()) {
    return false;
  }
  const auto& option = itr->second;
  return option.default_value.value.is<bool>();
}

// -----------------------------------------------------------------------------

void
ArgumentParser::__update_option_value(const std::string& key,
                                      const std::string& value)
{
  // TODO: Rework this mechanism.
  auto& option = m_opts.at(key);
  const auto& default_value = option.default_value.value;
  if (default_value.is<std::string>()) {
    option.value.value = value;
  } else if (default_value.is<uint32_t>()) {
    uint32_t val = 0;
    std::stringstream ss(value);
    ss >> val;
    option.value.value = val;
  } else if (default_value.is<uint64_t>()) {
    uint64_t val = 0;
    std::stringstream ss(value);
    ss >> val;
    option.value.value = val;
  } else if (default_value.is<float>()) {
    float val = 0.0f;
    std::stringstream ss(value);
    ss >> val;
    option.value.value = val;
  } else if (default_value.is<double>()) {
    double val = 0.0;
    std::stringstream ss(value);
    ss >> val;
    option.value.value = val;
  }
}

// -----------------------------------------------------------------------------

void
ArgumentParser::__assign_values()
{
  // TODO: Rework this mechanism.
  for (auto& pair : m_opts) {
    auto& option = pair.second;
    const auto& value = option.value.value;
    const auto& default_value = option.default_value.value;
    if (default_value.is<std::string>()) {
      std::string* dst = reinterpret_cast<std::string*>(option.dst);
      if (value.valid()) {
        dst->assign(value.get<std::string>());
      } else {
        dst->assign(default_value.get<std::string>());
      }
    } else if (default_value.is<bool>()) {
      bool* dst = reinterpret_cast<bool*>(option.dst);
      *dst = (value.valid() ? value : default_value).get<bool>();
    } else if (default_value.is<uint32_t>()) {
      uint32_t* dst = reinterpret_cast<uint32_t*>(option.dst);
      *dst = (value.valid() ? value : default_value).get<uint32_t>();
    } else if (default_value.is<uint64_t>()) {
      uint64_t* dst = reinterpret_cast<uint64_t*>(option.dst);
      *dst = (value.valid() ? value : default_value).get<uint64_t>();
    } else if (default_value.is<float>()) {
      float* dst = reinterpret_cast<float*>(option.dst);
      *dst = (value.valid() ? value : default_value).get<float>();
    } else if (default_value.is<double>()) {
      double* dst = reinterpret_cast<double*>(option.dst);
      *dst = (value.valid() ? value : default_value).get<double>();
    }
  }
}

// -----------------------------------------------------------------------------
