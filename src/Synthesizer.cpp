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
#include "ASTVisitor.h"
#include "SynthesizerUtil.h"
#include "ast.h"
#include "format_defn.h"
#include "macros.h"
#include <cstdio>
#include <fstream>
#include <memory>
#include <unordered_map>

// -----------------------------------------------------------------------------

#define SYNTHESIZER_ASSERT(expr) ASSERT((expr))
#define SYNTHESIZER_DEFAULT_CLASS_NAME_PREFIX "Inference"

// -----------------------------------------------------------------------------

static size_t
get_incremental_int()
{
  static size_t val = 1;
  return val++;
}

// -----------------------------------------------------------------------------

typedef std::unordered_map<std::string, std::string> EnvDefnMap;

// -----------------------------------------------------------------------------

struct InferenceGroupSynthesisContext
{
  std::string cls_name;
  EnvDefnMap env_defn_map;
  std::unique_ptr<std::ofstream> header_file_ofs;
  std::unique_ptr<std::ofstream> cpp_file_ofs;
  ~InferenceGroupSynthesisContext();
};

// -----------------------------------------------------------------------------

class SynthesizerImpl : public ASTVisitor
{
public:
  explicit SynthesizerImpl(const Synthesizer::Options&, std::string*);

  bool run(const ASTModule&);

private:
  virtual bool previsit(const ASTInferenceGroup&);
  virtual bool postvisit(const ASTInferenceGroup&);

  virtual bool previsit(const ASTInferenceDefn&);
  virtual bool postvisit(const ASTInferenceDefn&);

  EnvDefnMap get_envn_defn_map_from_inference_group(const ASTInferenceGroup&);
  std::string get_class_name_from_env_defn(const EnvDefnMap&);

  void set_msg(const char*);

  const Synthesizer::Options& m_opts;
  std::string* m_msg;
  std::unique_ptr<InferenceGroupSynthesisContext> m_context;
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
  SynthesizerImpl impl(m_opts, &m_msg);
  return impl.run(module);
}

// -----------------------------------------------------------------------------

InferenceGroupSynthesisContext::~InferenceGroupSynthesisContext()
{
  if (header_file_ofs) {
    header_file_ofs->close();
    header_file_ofs.release();
  }
  if (cpp_file_ofs) {
    cpp_file_ofs->close();
    cpp_file_ofs.release();
  }
}

// -----------------------------------------------------------------------------

SynthesizerImpl::SynthesizerImpl(const Synthesizer::Options& opts,
                                 std::string* msg)
  : m_opts(opts)
  , m_msg(msg)
  , m_context()
{
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::set_msg(const char* msg)
{
  m_msg->assign(msg);
}

// -----------------------------------------------------------------------------

bool
SynthesizerImpl::run(const ASTModule& module)
{
  return visit(module);
}

// -----------------------------------------------------------------------------

/* virtual */
bool
SynthesizerImpl::previsit(const ASTInferenceGroup& inference_group)
{
  EnvDefnMap env_defn_map =
      get_envn_defn_map_from_inference_group(inference_group);

  const auto cls_name = get_class_name_from_env_defn(env_defn_map);

  // Create header file.
  std::string header_filepath(m_opts.output_path);
  if (!header_filepath.empty() && header_filepath.back() != FORWARD_SLASH) {
    header_filepath.push_back(FORWARD_SLASH);
  }
  header_filepath.append(cls_name);
  header_filepath.append(HEADER_FILE_EXT);

  std::unique_ptr<std::ofstream> header_file_ofs(
      new std::ofstream(header_filepath, std::ofstream::out));
  if (!header_file_ofs) {
    return false;
  }
  if (!header_file_ofs->good()) {
    header_file_ofs.release();
    return false;
  }

  // Create .cpp file.
  std::string cpp_filepath(m_opts.output_path);
  if (!cpp_filepath.empty() && cpp_filepath.back() != FORWARD_SLASH) {
    cpp_filepath.push_back(FORWARD_SLASH);
  }
  cpp_filepath.append(cls_name);
  cpp_filepath.append(CPP_FILE_EXT);

  std::unique_ptr<std::ofstream> cpp_file_ofs(
      new std::ofstream(cpp_filepath, std::ofstream::out));
  if (!cpp_file_ofs) {
    header_file_ofs.release();
    return false;
  }
  if (!cpp_file_ofs->good()) {
    header_file_ofs.release();
    cpp_file_ofs.release();
    return false;
  }

  m_context.reset(new InferenceGroupSynthesisContext());
  if (!m_context) {
    header_file_ofs.release();
    cpp_file_ofs.release();
    return false;
  }
  m_context->header_file_ofs = std::move(header_file_ofs);
  m_context->cpp_file_ofs = std::move(cpp_file_ofs);
  m_context->cls_name = std::move(cls_name);
  m_context->env_defn_map = std::move(env_defn_map);

  // Write to header file.
  {
    *(m_context->header_file_ofs) << SYNTHESIZED_PREFIX_COMMENT;
    *(m_context->header_file_ofs) << std::endl;
    *(m_context->header_file_ofs) << CPP_PRAGMA_ONCE << std::endl;
    *(m_context->header_file_ofs) << std::endl;
    *(m_context->header_file_ofs) << CPP_CLASS_KEYWORD << ' ';
    *(m_context->header_file_ofs) << m_context->cls_name;
    *(m_context->header_file_ofs) << std::endl;
    *(m_context->header_file_ofs) << CPP_OPEN_BRACE;
    *(m_context->header_file_ofs) << std::endl;
  }

  // Write to .cpp file.
  {
    *(m_context->cpp_file_ofs) << SYNTHESIZED_PREFIX_COMMENT;
    *(m_context->cpp_file_ofs) << std::endl;
  }

  return true;
}

// -----------------------------------------------------------------------------

/* virtual */
bool
SynthesizerImpl::postvisit(const ASTInferenceGroup&)
{
  SYNTHESIZER_ASSERT(m_context);
  SYNTHESIZER_ASSERT(m_context->header_file_ofs);
  SYNTHESIZER_ASSERT(m_context->cpp_file_ofs);

  *(m_context->header_file_ofs) << CPP_CLOSE_BRACE;
  *(m_context->header_file_ofs) << CPP_SEMICOLON;
  *(m_context->header_file_ofs) << std::endl;

  // Close and release header file stream.
  {
    m_context->header_file_ofs->close();
    m_context->header_file_ofs.release();
  }

  // Close and release .cpp file stream.
  {
    m_context->cpp_file_ofs->close();
    m_context->cpp_file_ofs.release();
  }

  // Close and release context.
  {
    m_context.release();
  }

  return true;
}

// -----------------------------------------------------------------------------

/* virtual */
bool
SynthesizerImpl::previsit(const ASTInferenceDefn&)
{
  SYNTHESIZER_ASSERT(m_context);
  SYNTHESIZER_ASSERT(m_context->header_file_ofs);
  SYNTHESIZER_ASSERT(m_context->cpp_file_ofs);

  // TODO:...

  return true;
}

// -----------------------------------------------------------------------------

/* virtual */
bool
SynthesizerImpl::postvisit(const ASTInferenceDefn&)
{
  SYNTHESIZER_ASSERT(m_context);
  SYNTHESIZER_ASSERT(m_context->header_file_ofs);
  SYNTHESIZER_ASSERT(m_context->cpp_file_ofs);

  // TODO:...

  return true;
}

// -----------------------------------------------------------------------------

EnvDefnMap
SynthesizerImpl::get_envn_defn_map_from_inference_group(
    const ASTInferenceGroup& inference_group)
{
  EnvDefnMap env_defn_map;

  for (const auto& envn_defn : inference_group.environment_defns()) {
    env_defn_map[envn_defn.field()] = envn_defn.value();
  }

  return env_defn_map;
}

// -----------------------------------------------------------------------------

std::string
SynthesizerImpl::get_class_name_from_env_defn(const EnvDefnMap& env_defn_map)
{
  const auto itr = env_defn_map.find(SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_CLASS);
  if (itr == env_defn_map.cend()) {
    char buf[16] = {0};
    snprintf(buf, sizeof(buf), "%s%lu", SYNTHESIZER_DEFAULT_CLASS_NAME_PREFIX,
             get_incremental_int());
    return std::string(buf);
  }
  return itr->second;
}

// -----------------------------------------------------------------------------
