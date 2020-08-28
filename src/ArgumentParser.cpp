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
  : _name()
  , _version()
  , _desc()
  , _longDesc()
  , _usage()
  , _opts()
  , _shorthandMap()
  , _positionalArgs()
  , _minPositionalArgsRequired(0)
{
}

// -----------------------------------------------------------------------------

ArgumentParser::ArgumentParser(const char* name)
  : _name(name)
  , _version()
  , _desc()
  , _longDesc()
  , _usage()
  , _opts()
  , _shorthandMap()
  , _positionalArgs()
  , _minPositionalArgsRequired(0)
{
}

// -----------------------------------------------------------------------------

ArgumentParser::ArgumentParser(const char* name, const char* version)
  : _name(name)
  , _version(version)
  , _desc()
  , _longDesc()
  , _usage()
  , _opts()
  , _shorthandMap()
  , _positionalArgs()
  , _minPositionalArgsRequired(0)
{
}

// -----------------------------------------------------------------------------

ArgumentParser::ArgumentParser(const char* name, const char* version,
                               const char* description)
  : _name(name)
  , _version(version)
  , _desc(description)
  , _longDesc()
  , _usage()
  , _opts()
  , _shorthandMap()
  , _positionalArgs()
  , _minPositionalArgsRequired(0)
{
}

// -----------------------------------------------------------------------------

ArgumentParser::ArgumentParser(const char* name, const char* version,
                               const char* description,
                               const char* long_description)
  : _name(name)
  , _version(version)
  , _desc(description)
  , _longDesc(long_description)
  , _usage()
  , _opts()
  , _shorthandMap()
  , _positionalArgs()
  , _minPositionalArgsRequired(0)
{
}

// -----------------------------------------------------------------------------

template <typename T>
void
ArgumentParser::addParameter(const char* name, const char shorthand,
                              const char* description, bool required, T* res,
                              T defaultValue)
{
  CmdlOption opts{.shorthand = shorthand,
                  .description = description,
                  .required = required,
                  .defaultValue = {defaultValue},
                  .value = CmdlOptionValue(),
                  .dst = reinterpret_cast<void*>(res),
                };
  _shorthandMap[shorthand] = name;
  _opts[name] = opts;
}

// -----------------------------------------------------------------------------

void
ArgumentParser::addStringParameter(const char* name, const char shorthand,
                                     const char* description, bool required,
                                     std::string* res, std::string defaultValue)
{
  addParameter<std::string>(name, shorthand, description, required, res,
                             defaultValue);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::addUint32Parameter(const char* name, const char shorthand,
                                     const char* description, bool required,
                                     uint32_t* res, uint32_t defaultValue)
{
  addParameter<uint32_t>(name, shorthand, description, required, res,
                          defaultValue);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::addUint64Parameter(const char* name, const char shorthand,
                                     const char* description, bool required,
                                     uint64_t* res, uint64_t defaultValue)
{
  addParameter<uint64_t>(name, shorthand, description, required, res,
                          defaultValue);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::addFloatParameter(const char* name, const char shorthand,
                                    const char* description, bool required,
                                    float* res, float defaultValue)
{
  addParameter<float>(name, shorthand, description, required, res,
                       defaultValue);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::addDoubleParameter(const char* name, const char shorthand,
                                     const char* description, bool required,
                                     double* res, double defaultValue)
{
  addParameter<double>(name, shorthand, description, required, res,
                        defaultValue);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::addBooleanParameter(const char* name, const char shorthand,
                                      const char* description, bool required,
                                      bool* res, bool defaultValue)
{
  addParameter<bool>(name, shorthand, description, required, res,
                      defaultValue);
}

// -----------------------------------------------------------------------------

bool
ArgumentParser::optionProvided(const char* name) const
{
  auto itr = _opts.find(name);
  if (itr == _opts.end()) {
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
  if (_shorthandMap.size() != _opts.size()) {
    return false;
  }

  // Sanity check.
  for (const auto& pair : _shorthandMap) {
    const char shorthand = pair.first;
    const auto& option_key = pair.second;
    const auto iter = _opts.find(option_key);
    if (iter == _opts.end()) {
      return false;
    }
    const auto& option = iter->second;
    if (option.shorthand != shorthand) {
      return false;
    }
  }

  // Check if we meet the required number of positional arguments.
  if (_positionalArgs.size() < _minPositionalArgsRequired) {
    return false;
  }

  for (const auto& pair : _opts) {
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
  _minPositionalArgsRequired = n;
}

// -----------------------------------------------------------------------------

void
ArgumentParser::setUsageString(const char* usage)
{
  _usage.assign(usage);
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
  return _positionalArgs;
}

// -----------------------------------------------------------------------------

template <typename Stream>
void
ArgumentParser::__printHelp(Stream& stream) const
{
  // Name and version.
  stream << (_name.empty() ? "PROGRAM" : _name);
  if (!_version.empty()) {
    stream << " (version " << _version << ')';
  }
  stream << std::endl;

  // Description.
  if (!_desc.empty()) {
    stream << std::endl;
    stream << _desc;
  }
  stream << std::endl;

  // Long description.
  if (!_longDesc.empty()) {
    stream << std::endl;
    stream << _longDesc;
    stream << std::endl;
  }
  stream << std::endl;

  // Usage.
  if (!_usage.empty()) {
    stream << "Usage: ";
    stream << (_name.empty() ? "PROGRAM" : _name) << ' ';
    stream << _usage;
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

  for (const auto& pair : _opts) {
    const auto& name = pair.first;
    const auto& option = pair.second;
    const auto& defaultValue = option.defaultValue.value;

    stream << HELP_MENU_SINGLE_DASH << option.shorthand << ", ";
    stream << HELP_MENU_DOUBLE_DASH << name;
    if (!defaultValue.is<bool>()) {
      stream << HELP_MENU_VALUE_PLACEHOLDER;
    }

    stream << std::endl;
    stream << HELP_MENU_INDENT << option.description << '.';
    stream << std::endl;

    // Show default value for non-required options.
    if (!option.required) {
      stream << HELP_MENU_INDENT << "Optional. Default value: ";
      if (defaultValue.is<bool>()) {
        stream << defaultValue.get<bool>();
      } else if (defaultValue.is<std::string>()) {
        stream << defaultValue.get<std::string>();
      } else if (defaultValue.is<uint32_t>()) {
        stream << defaultValue.get<uint32_t>();
      } else if (defaultValue.is<uint64_t>()) {
        stream << defaultValue.get<uint64_t>();
      } else if (defaultValue.is<float>()) {
        stream << defaultValue.get<float>();
      } else if (defaultValue.is<double>()) {
        stream << defaultValue.get<double>();
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
  _positionalArgs.emplace_back(arg);
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
      std::string shorthand = s.substr(1);
      if (shorthand.size() != 1) {
        return false;
      }
      const char c = shorthand[0];
      const auto iter = _shorthandMap.find(c);
      if (iter == _shorthandMap.end()) {
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
  if (!_opts.count(key)) {
    return false;
  }
  if (__definedBooleanOption(key)) {
    _opts[key].value.value = true;
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
  auto itr = _opts.find(name);
  if (itr == _opts.end()) {
    return false;
  }
  const auto& option = itr->second;
  return option.defaultValue.value.is<bool>();
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
  value.value = sl::variant::apply_visitor(cast, defaultValue.value);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::__updateOptionValue(const std::string& key,
                                      const std::string& value)
{
  auto& option = _opts.at(key);
  option.updateValue(value);
}

// -----------------------------------------------------------------------------

void
ArgumentParser::__assignValues()
{
  for (auto& pair : _opts) {
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
  T operator()(T& defaultValue)
  {
    T* dst_ = reinterpret_cast<T*>(m_dst);
    if (dst_) {
      *dst_ = m_value.valid() ? m_value.get<T>() : defaultValue;
    }
    return defaultValue;
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
  sl::variant::apply_visitor(update, defaultValue.value);
}

// -----------------------------------------------------------------------------
