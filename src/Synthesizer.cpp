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

#include "Synthesizer.h"
#include "ast.h"
#include "macros.h"
#include "format_defn.h"
#include <cstdio>
#include <unordered_map>

// -----------------------------------------------------------------------------

#define SYNTHESIZER_ASSERT(expr) ASSERT( (expr) )
#define SYNTHESIZER_DEFAULT_CLASS_NAME_PREFIX "Inference"

// -----------------------------------------------------------------------------

static size_t
get_incremental_int()
{
  static size_t val = 1;
  return val++;
}

// -----------------------------------------------------------------------------

class SynthesizerImpl
{
public:
  explicit SynthesizerImpl(std::string*);

  typedef std::unordered_map<std::string, std::string> EnvDefnMap;

  bool synthesize_group(const ASTInferenceGroup&);

private:
  EnvDefnMap get_envn_defn_map_from_inference_group(const ASTInferenceGroup&);
  std::string get_class_name_from_env_defn(const EnvDefnMap&);

  std::string* m_msg;
};

// -----------------------------------------------------------------------------

Synthesizer::Synthesizer()
  : m_opts()
  , m_msg()
{
}

// -----------------------------------------------------------------------------

Synthesizer::Synthesizer(const Options& opts)
  : m_opts(opts)
  , m_msg()
{
}

// -----------------------------------------------------------------------------

const std::string&
Synthesizer::msg() const
{
  return m_msg;
}

// -----------------------------------------------------------------------------
bool
Synthesizer::run(const ASTModule& module)
{
  // TODO: [SNOWLAKE-15] Design and implement code synthesis pipeline
  return true;
}

// -----------------------------------------------------------------------------

SynthesizerImpl::SynthesizerImpl(std::string* msg)
  : m_msg(msg)
{
}

// -----------------------------------------------------------------------------

bool
SynthesizerImpl::synthesize_group(const ASTInferenceGroup& inference_group)
{
  EnvDefnMap env_defn_map = get_envn_defn_map_from_inference_group(inference_group);
  const auto cls_name = get_class_name_from_env_defn(env_defn_map);
  return true;
}

// -----------------------------------------------------------------------------

SynthesizerImpl::EnvDefnMap
SynthesizerImpl::get_envn_defn_map_from_inference_group(const ASTInferenceGroup& inference_group)
{
  SynthesizerImpl::EnvDefnMap env_defn_map;

  for (const auto& envn_defn : inference_group.environment_defns()) {
    env_defn_map[envn_defn.field()] = envn_defn.value();
  }

  return env_defn_map;
}

// -----------------------------------------------------------------------------

std::string
SynthesizerImpl::get_class_name_from_env_defn(const SynthesizerImpl::EnvDefnMap& env_defn_map)
{
  const auto itr = env_defn_map.find(SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_CLASS);
  if (itr == env_defn_map.cend()) {
    char buf[16] = {0};
    snprintf(buf, sizeof(buf), "%s%lu", SYNTHESIZER_DEFAULT_CLASS_NAME_PREFIX, get_incremental_int());
    return std::string(buf);
  }
  return itr->second;
}

// -----------------------------------------------------------------------------
