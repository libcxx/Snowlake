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
#include "version.h"
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
  , m_version()
  , m_desc()
  , m_long_desc()
  , m_usage()
  , m_opts()
  , m_shorthand_map()
  , m_positional_args()
  , m_min_positional_args_required(0)
{
}

// -----------------------------------------------------------------------------

ArgumentParser::ArgumentParser(const char* name)
  : m_name(name)
  , m_version()
  , m_desc()
  , m_long_desc()
  , m_usage()
  , m_opts()
  , m_shorthand_map()
  , m_positional_args()
  , m_min_positional_args_required(0)
{
}

// -----------------------------------------------------------------------------

ArgumentParser::ArgumentParser(const char* name, const char* version)
  : m_name(name)
  , m_version(version)
  , m_desc()
  , m_long_desc()
  , m_usage()
  , m_opts()
  , m_shorthand_map()
  , m_positional_args()
  , m_min_positional_args_required(0)
{
}

// -----------------------------------------------------------------------------

ArgumentParser::ArgumentParser(const char* name, const char* version,
                               const char* description)
  : m_name(name)
  , m_version(version)
  , m_desc(description)
  , m_long_desc()
  , m_usage()
  , m_opts()
  , m_shorthand_map()
  , m_positional_args()
  , m_min_positional_args_required(0)
{
}

// -----------------------------------------------------------------------------

ArgumentParser::ArgumentParser(const char* name, const char* version,
                               const char* description,
                               const char* long_description)
  : m_name(name)
  , m_version(version)
  , m_desc(description)
  , m_long_desc(long_description)
  , m_usage()
  , m_opts()
  , m_shorthand_map()
  , m_positional_args()
  , m_min_positional_args_required(0)
{
}

// -----------------------------------------------------------------------------

template <typename T>
void
ArgumentParser::add_parameter(const char* name, const char short_hand,
                              const char* description, bool required, T* res,
                              T default_val)
{
  CmdlOption opts{.short_hand = short_hand,
                  .description = description,
                  .required = required,
                  .dst = reinterpret_cast<void*>(res),
                  .default_value = {default_val}};
  m_shorthand_map[short_hand] = name;
  m_opts[name] = opts;
}

// -----------------------------------------------------------------------------

void
ArgumentParser::add_string_parameter(const char* name, const char short_hand,
                                     const char* description, bool required,
                                     std::string* res, const char* default_val)
{
  add_parameter<std::string>(name, short_hand, description, required, res,
                             default_val);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::add_uint32_parameter(const char* name, const char short_hand,
                                     const char* description, bool required,
                                     uint32_t* res, uint32_t default_val)
{
  add_parameter<uint32_t>(name, short_hand, description, required, res,
                          default_val);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::add_uint64_parameter(const char* name, const char short_hand,
                                     const char* description, bool required,
                                     uint64_t* res, uint64_t default_val)
{
  add_parameter<uint64_t>(name, short_hand, description, required, res,
                          default_val);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::add_float_parameter(const char* name, const char short_hand,
                                    const char* description, bool required,
                                    float* res, float default_val)
{
  add_parameter<float>(name, short_hand, description, required, res,
                       default_val);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::add_double_parameter(const char* name, const char short_hand,
                                     const char* description, bool required,
                                     double* res, double default_val)
{
  add_parameter<double>(name, short_hand, description, required, res,
                        default_val);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::add_boolean_parameter(const char* name, const char short_hand,
                                      const char* description, bool required,
                                      bool* res, bool default_val)
{
  add_parameter<bool>(name, short_hand, description, required, res,
                      default_val);
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
  // Check if the short-hand map count matches with the number of options.
  // If not, it means there are short-hands that are duplicate.
  if (m_shorthand_map.size() != m_opts.size()) {
    return false;
  }

  // Sanity check.
  for (const auto& pair : m_shorthand_map) {
    const char short_hand = pair.first;
    const auto& option_key = pair.second;
    const auto iter = m_opts.find(option_key);
    if (iter == m_opts.end()) {
      return false;
    }
    const auto& option = iter->second;
    if (option.short_hand != short_hand) {
      return false;
    }
  }

  // Check if we meet the required number of positional arguments.
  if (m_positional_args.size() < m_min_positional_args_required) {
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
ArgumentParser::set_minimum_positional_args_required(size_t n)
{
  m_min_positional_args_required = n;
}

// -----------------------------------------------------------------------------

void
ArgumentParser::set_usage_string(const char* usage)
{
  m_usage.assign(usage);
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
  // Name and version.
  stream << (m_name.empty() ? "PROGRAM" : m_name);
  if (!m_version.empty()) {
    stream << " (version " << m_version << ')';
  }
  stream << std::endl;

  // Description.
  if (!m_desc.empty()) {
    stream << std::endl;
    stream << m_desc;
  }
  stream << std::endl;

  // Long description.
  if (!m_long_desc.empty()) {
    stream << std::endl;
    stream << m_long_desc;
    stream << std::endl;
  }
  stream << std::endl;

  // Usage.
  if (!m_usage.empty()) {
    stream << "Usage: ";
    stream << (m_name.empty() ? "PROGRAM" : m_name) << ' ';
    stream << m_usage;
    stream << std::endl;
  }
  stream << std::endl;

// Menu.

#define HELP_MENU_DOUBLE_DASH "--"
#define HELP_MENU_SINGLE_DASH '-'
#define HELP_MENU_VALUE_PLACEHOLDER " <value>"
#define HELP_MENU_INDENT "\t\t\t"
#define HELP_MENU_OPTIONS_LABEL "OPTIONS:"

  stream << std::endl;
  stream << HELP_MENU_OPTIONS_LABEL << std::endl;
  stream << std::endl;

  for (const auto& pair : m_opts) {
    const auto& name = pair.first;
    const auto& option = pair.second;
    const auto& default_value = option.default_value.value;

    stream << HELP_MENU_SINGLE_DASH << option.short_hand << ", ";
    stream << HELP_MENU_DOUBLE_DASH << name;
    if (!default_value.is<bool>()) {
      stream << HELP_MENU_VALUE_PLACEHOLDER;
    }

    stream << std::endl;
    stream << HELP_MENU_INDENT << option.description << '.';
    stream << std::endl;

    // Show default value for non-required options.
    if (!option.required) {
      stream << HELP_MENU_INDENT << "Optional. Default value: ";
      if (default_value.is<bool>()) {
        stream << default_value.get<bool>();
      } else if (default_value.is<std::string>()) {
        stream << default_value.get<std::string>();
      } else if (default_value.is<uint32_t>()) {
        stream << default_value.get<uint32_t>();
      } else if (default_value.is<uint64_t>()) {
        stream << default_value.get<uint64_t>();
      } else if (default_value.is<float>()) {
        stream << default_value.get<float>();
      } else if (default_value.is<double>()) {
        stream << default_value.get<double>();
      }
      stream << std::endl;
    }
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
      if (!__register_cmdl_option(key, argc, argv)) {
        return false;
      }
    } else if (s.find("-") == 0) {
      std::string short_hand = s.substr(1);
      if (short_hand.size() != 1) {
        return false;
      }
      const char c = short_hand[0];
      const auto iter = m_shorthand_map.find(c);
      if (iter == m_shorthand_map.end()) {
        return false;
      }
      std::string key = iter->second;
      if (!__register_cmdl_option(key, argc, argv)) {
        return false;
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
ArgumentParser::__register_cmdl_option(const std::string& key, int& argc,
                                       char**& argv)
{
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

template <typename T>
void
ArgumentParser::CmdlOption::__update_value(const std::string& new_value)
{
  T val;
  std::stringstream ss(new_value);
  ss >> val;
  value.value = val;
}

// -----------------------------------------------------------------------------

void
ArgumentParser::CmdlOption::update_value(const std::string& new_value)
{
  if (default_value.value.is<std::string>()) {
    value.value = new_value;
  } else if (default_value.value.is<uint32_t>()) {
    __update_value<uint32_t>(new_value);
  } else if (default_value.value.is<uint64_t>()) {
    __update_value<uint64_t>(new_value);
  } else if (default_value.value.is<float>()) {
    __update_value<float>(new_value);
  } else if (default_value.value.is<double>()) {
    __update_value<double>(new_value);
  }
}

// -----------------------------------------------------------------------------

void
ArgumentParser::__update_option_value(const std::string& key,
                                      const std::string& value)
{
  auto& option = m_opts.at(key);
  option.update_value(value);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::__assign_values()
{
  for (auto& pair : m_opts) {
    auto& option = pair.second;
    option.assign_value_to_dst();
  }
}

// -----------------------------------------------------------------------------

template <typename T>
void
ArgumentParser::CmdlOption::__assign_value_to_dst()
{
  T* dst_ = reinterpret_cast<T*>(dst);
  *dst_ = (value.value.valid() ? value.value : default_value.value).get<T>();
}

// -----------------------------------------------------------------------------

void
ArgumentParser::CmdlOption::assign_value_to_dst()
{
  if (default_value.value.is<std::string>()) {
    __assign_value_to_dst<std::string>();
  } else if (default_value.value.is<bool>()) {
    __assign_value_to_dst<bool>();
  } else if (default_value.value.is<uint32_t>()) {
    __assign_value_to_dst<uint32_t>();
  } else if (default_value.value.is<uint64_t>()) {
    __assign_value_to_dst<uint64_t>();
  } else if (default_value.value.is<float>()) {
    __assign_value_to_dst<float>();
  } else if (default_value.value.is<double>()) {
    __assign_value_to_dst<double>();
  }
}

// -----------------------------------------------------------------------------
