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
#include "variant_static_visitor.h"
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
ArgumentParser::addParameter(const char* name, const char short_hand,
                              const char* description, bool required, T* res,
                              T default_val)
{
  CmdlOption opts{.short_hand = short_hand,
                  .description = description,
                  .required = required,
                  .dst = reinterpret_cast<void*>(res),
                  .default_value = {default_val}
                };
  m_shorthand_map[short_hand] = name;
  m_opts[name] = opts;
}

// -----------------------------------------------------------------------------

void
ArgumentParser::addStringParameter(const char* name, const char short_hand,
                                     const char* description, bool required,
                                     std::string* res, std::string default_val)
{
  addParameter<std::string>(name, short_hand, description, required, res,
                             default_val);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::addUint32Parameter(const char* name, const char short_hand,
                                     const char* description, bool required,
                                     uint32_t* res, uint32_t default_val)
{
  addParameter<uint32_t>(name, short_hand, description, required, res,
                          default_val);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::addUint64Parameter(const char* name, const char short_hand,
                                     const char* description, bool required,
                                     uint64_t* res, uint64_t default_val)
{
  addParameter<uint64_t>(name, short_hand, description, required, res,
                          default_val);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::addFloatParameter(const char* name, const char short_hand,
                                    const char* description, bool required,
                                    float* res, float default_val)
{
  addParameter<float>(name, short_hand, description, required, res,
                       default_val);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::addDoubleParameter(const char* name, const char short_hand,
                                     const char* description, bool required,
                                     double* res, double default_val)
{
  addParameter<double>(name, short_hand, description, required, res,
                        default_val);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::addBooleanParameter(const char* name, const char short_hand,
                                      const char* description, bool required,
                                      bool* res, bool default_val)
{
  addParameter<bool>(name, short_hand, description, required, res,
                      default_val);
}

// -----------------------------------------------------------------------------

bool
ArgumentParser::optionProvided(const char* name) const
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
ArgumentParser::__checkParameters() const
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
ArgumentParser::setMinimumPositionalArgsRequired(size_t n)
{
  m_min_positional_args_required = n;
}

// -----------------------------------------------------------------------------

void
ArgumentParser::setUsageString(const char* usage)
{
  m_usage.assign(usage);
}

// -----------------------------------------------------------------------------

bool
ArgumentParser::parseArgs(int argc, char** argv)
{
  if (!__parseArgs(argc, argv)) {
    return false;
  }

  if (!__checkParameters()) {
    return false;
  }

  __assignValues();

  return true;
}

// -----------------------------------------------------------------------------

const ArgumentParser::PositionalArgumentList&
ArgumentParser::positionalArgs() const
{
  return m_positional_args;
}

// -----------------------------------------------------------------------------

template <typename Stream>
void
ArgumentParser::__printHelp(Stream& stream) const
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
  __printHelp(ss);
  return ss.str();
}

// -----------------------------------------------------------------------------

void
ArgumentParser::printHelp() const
{
  __printHelp(std::cout);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::__addPositionalParameter(std::string&& arg)
{
  m_positional_args.emplace_back(arg);
}

// -----------------------------------------------------------------------------

bool
ArgumentParser::__parseArgs(int argc, char** argv)
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
      if (!__registerCmdlOption(key, argc, argv)) {
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
      if (!__registerCmdlOption(key, argc, argv)) {
        return false;
      }
    } else {
      __addPositionalParameter(std::move(s));
    }
    ADVANCE_ARGV;
  }

  return true;
}

// -----------------------------------------------------------------------------

bool
ArgumentParser::__registerCmdlOption(const std::string& key, int& argc,
                                       char**& argv)
{
  if (key.empty()) {
    return false;
  }
  if (!m_opts.count(key)) {
    return false;
  }
  if (__definedBooleanOption(key)) {
    m_opts[key].value.value = true;
  } else {
    ADVANCE_ARGV;
    CHECK_ARGV;
    std::string val(*argv);
    // Update option value.
    __updateOptionValue(key, val);
  }

  return true;
}

// -----------------------------------------------------------------------------

bool
ArgumentParser::__definedBooleanOption(const std::string& name) const
{
  auto itr = m_opts.find(name);
  if (itr == m_opts.end()) {
    return false;
  }
  const auto& option = itr->second;
  return option.default_value.value.is<bool>();
}

// -----------------------------------------------------------------------------

struct CmdlOptionValueCast
    : public sl::variant::static_visitor<ArgumentParser::value_type>
{
  explicit CmdlOptionValueCast(const std::string& raw_value)
    : m_raw_value(raw_value)
  {
  }

  ~CmdlOptionValueCast()
  {
  }

  template <typename T>
  T operator()(T&)
  {
    T val;
    std::stringstream ss(m_raw_value);
    ss >> val;
    return val;
  }

  std::string operator()(std::string&)
  {
    return m_raw_value;
  }

private:
  const std::string& m_raw_value;
};

// -----------------------------------------------------------------------------

void
ArgumentParser::CmdlOption::updateValue(const std::string& new_value)
{
  CmdlOptionValueCast cast(new_value);
  value.value = sl::variant::apply_visitor(cast, default_value.value);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::__updateOptionValue(const std::string& key,
                                      const std::string& value)
{
  auto& option = m_opts.at(key);
  option.updateValue(value);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::__assignValues()
{
  for (auto& pair : m_opts) {
    auto& option = pair.second;
    option.assignValueToDst();
  }
}

// -----------------------------------------------------------------------------

struct CmdlOptionDestinationValueUpdate
    : public sl::variant::static_visitor<ArgumentParser::value_type>
{
  CmdlOptionDestinationValueUpdate(const ArgumentParser::value_type& value,
                                   void* dst)
    : m_value(value)
    , m_dst(dst)
  {
  }

  ~CmdlOptionDestinationValueUpdate()
  {
  }

  template <typename T>
  T operator()(T& default_value)
  {
    T* dst_ = reinterpret_cast<T*>(m_dst);
    if (dst_) {
      *dst_ = m_value.valid() ? m_value.get<T>() : default_value;
    }
    return default_value;
  }

private:
  const ArgumentParser::value_type& m_value;
  void* m_dst;
};

// -----------------------------------------------------------------------------

void
ArgumentParser::CmdlOption::assignValueToDst()
{
  CmdlOptionDestinationValueUpdate update(value.value, dst);
  sl::variant::apply_visitor(update, default_value.value);
}

// -----------------------------------------------------------------------------
