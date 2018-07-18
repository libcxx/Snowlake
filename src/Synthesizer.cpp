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
  std::string type_cls;
  EnvDefnMap env_defn_map;
  std::unique_ptr<std::ofstream> header_file_ofs;
  std::unique_ptr<std::ofstream> cpp_file_ofs;
  size_t header_file_indent_lvl;
  size_t cpp_file_indent_lvl;
  uint32_t name_id;

  InferenceGroupSynthesisContext();
  ~InferenceGroupSynthesisContext();
};

// -----------------------------------------------------------------------------

class SynthesizerImpl : public ASTVisitor
{
public:
  explicit SynthesizerImpl(const Synthesizer::Options&);

  bool run(const ASTModule&);

private:
  virtual bool previsit(const ASTInferenceGroup&);
  virtual bool postvisit(const ASTInferenceGroup&);

  virtual bool previsit(const ASTInferenceDefn&);
  virtual bool postvisit(const ASTInferenceDefn&);

  virtual bool previsit(const ASTInferencePremiseDefn&);

  virtual bool previsit(const ASTInferenceEqualityDefn&);

  virtual bool previsit(const ASTPropositionDefn&);

  EnvDefnMap get_envn_defn_map_from_inference_group(const ASTInferenceGroup&);
  std::string get_class_name_from_env_defn(const EnvDefnMap&);

  enum class DeductionTargetArraySynthesisMode : uint32_t
  {
    AS_SINGULAR = 0x01,
    AS_ARRAY = 0x02,
    AS_RAW_POINTER_ARRAY = 0x04,
    AS_STD_VECTOR = 0x08,
  };

  void synthesize_inference_premise_defn_with_while_clause(
      const ASTInferencePremiseDefn&) const;

  void synthesize_inference_premise_defn_without_while_clause(
      const ASTInferencePremiseDefn&) const;

  void synthesize_argument_list(const ASTInferenceArgumentList&,
                                std::ostream*) const;

  void synthesize_deduction_target(const ASTDeductionTarget&,
                                   const DeductionTargetArraySynthesisMode,
                                   std::ostream*) const;

  void synthesize_deduction_target_for_declaration(const ASTDeductionTarget&,
                                                   std::ostream*) const;

  void synthesize_identifiable(const ASTIdentifiable&, std::ostream*) const;

  void synthesize_equality_operator(const EqualityOperator,
                                    std::ostream*) const;

  void render_indentation(const size_t, std::ostream*) const;

  void render_indentation_in_header_file() const;

  void render_indentation_in_cpp_file() const;

  void render_custom_include(const char*, std::ostream*) const;

  void render_system_header_includes(std::ostream*) const;

  void __render_system_header_includes(
      const std::vector<const char*>& system_headers, std::ostream*) const;

  void render_inference_error_category(std::ostream*) const;

  void render_type_annotation_setup_teardown_fixture(
      const ASTInferencePremiseDefn& premise_defn,
      const std::string& method_name, std::ostream*) const;

  void render_error_handling() const;

  void indent_header_file() const;

  void dedent_header_file() const;

  void indent_cpp_file() const;

  void dedent_cpp_file() const;

  bool initialize_and_synthesize_error_code_files() const;

  std::string __get_next_var_name() const;

  const Synthesizer::Options& m_opts;
  std::unique_ptr<InferenceGroupSynthesisContext> m_context;
};

// -----------------------------------------------------------------------------

Synthesizer::Synthesizer()
  : m_opts()
{
}

// -----------------------------------------------------------------------------

Synthesizer::Synthesizer(const Options& opts)
  : m_opts(opts)
{
}

// -----------------------------------------------------------------------------

bool
Synthesizer::run(const ASTModule& module)
{
  SynthesizerImpl impl(m_opts);
  return impl.run(module);
}

// -----------------------------------------------------------------------------

InferenceGroupSynthesisContext::InferenceGroupSynthesisContext()
  : cls_name()
  , type_cls()
  , env_defn_map()
  , header_file_ofs()
  , cpp_file_ofs()
  , header_file_indent_lvl(0)
  , cpp_file_indent_lvl(0)
  , name_id(0)
{
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

SynthesizerImpl::SynthesizerImpl(const Synthesizer::Options& opts)
  : m_opts(opts)
  , m_context()
{
}

// -----------------------------------------------------------------------------

bool
SynthesizerImpl::run(const ASTModule& module)
{
  if (!initialize_and_synthesize_error_code_files()) {
    return false;
  }
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

  const auto type_cls =
      env_defn_map.at(SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_CLASS);

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
  m_context->type_cls = std::move(type_cls);
  m_context->env_defn_map = std::move(env_defn_map);

  // Write to header file.
  {
    auto& header_file_ofs = *(m_context->header_file_ofs);
    header_file_ofs << SYNTHESIZED_AUTHORING_COMMENT_BLOCK;
    header_file_ofs << std::endl;
    header_file_ofs << std::endl;
    header_file_ofs << CPP_PRAGMA_ONCE << std::endl;
    header_file_ofs << std::endl;
    render_system_header_includes(m_context->header_file_ofs.get());
    header_file_ofs << std::endl;
    header_file_ofs << CPP_CLASS_KEYWORD << ' ';
    header_file_ofs << m_context->cls_name;
    header_file_ofs << std::endl;
    header_file_ofs << CPP_OPEN_BRACE;
    header_file_ofs << std::endl;
    header_file_ofs << CPP_PUBLIC_KEYWORD << CPP_COLON;
    header_file_ofs << std::endl;
  }

  // Write to .cpp file.
  {
    auto& cpp_file_ofs = *(m_context->cpp_file_ofs);
    cpp_file_ofs << SYNTHESIZED_AUTHORING_COMMENT_BLOCK;
    cpp_file_ofs << std::endl;
    render_custom_include(m_context->cls_name.c_str(),
                          m_context->cpp_file_ofs.get());
    render_custom_include(SYNTHESIZED_ERROR_CODE_HEADER_FILENAME_BASE,
                          m_context->cpp_file_ofs.get());
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

  // Write closing };
  {
    auto& header_file_ofs = *(m_context->header_file_ofs);
    header_file_ofs << CPP_CLOSE_BRACE;
    header_file_ofs << CPP_SEMICOLON;
    header_file_ofs << std::endl;
  }

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
SynthesizerImpl::previsit(const ASTInferenceDefn& inference_defn)
{
  SYNTHESIZER_ASSERT(m_context);
  SYNTHESIZER_ASSERT(m_context->header_file_ofs);
  SYNTHESIZER_ASSERT(m_context->cpp_file_ofs);

  // Synthesize member function declaration.
  {
    indent_header_file();
    render_indentation_in_header_file();
    auto& header_file_ofs = *(m_context->header_file_ofs);
    header_file_ofs << m_context->type_cls << CPP_SPACE;
    header_file_ofs << inference_defn.name();
    header_file_ofs << CPP_OPEN_PAREN;
    synthesize_argument_list(inference_defn.arguments(),
                             m_context->header_file_ofs.get());
    if (!m_opts.use_exception) {
      header_file_ofs << CPP_COMA << CPP_SPACE;
      header_file_ofs << CPP_STD_ERROR_CODE << CPP_STAR;
    }
    header_file_ofs << CPP_CLOSE_PAREN;
    header_file_ofs << CPP_SEMICOLON;
    header_file_ofs << std::endl;
    dedent_header_file();
  }

  // Synthesize member function definition.
  {
    auto& cpp_file_ofs = *(m_context->cpp_file_ofs);
    cpp_file_ofs << std::endl;
    cpp_file_ofs << m_context->type_cls << std::endl;
    cpp_file_ofs << m_context->cls_name;
    cpp_file_ofs << CPP_COLON << CPP_COLON;
    cpp_file_ofs << inference_defn.name();
    cpp_file_ofs << CPP_OPEN_PAREN;
    synthesize_argument_list(inference_defn.arguments(),
                             m_context->cpp_file_ofs.get());
    if (!m_opts.use_exception) {
      cpp_file_ofs << CPP_COMA << CPP_SPACE;
      cpp_file_ofs << CPP_STD_ERROR_CODE << CPP_STAR;
      cpp_file_ofs
            << CPP_SPACE << SYNTHESIZER_DEFAULT_ERROR_OUTPUT_PARAMETER_NAME;
    }
    cpp_file_ofs << CPP_CLOSE_PAREN;
    cpp_file_ofs << std::endl;
    cpp_file_ofs << CPP_OPEN_BRACE;
    cpp_file_ofs << std::endl;

    indent_cpp_file();
  }

  return true;
}

// -----------------------------------------------------------------------------

/* virtual */
bool
SynthesizerImpl::postvisit(const ASTInferenceDefn&)
{
  SYNTHESIZER_ASSERT(m_context);
  SYNTHESIZER_ASSERT(m_context->cpp_file_ofs);

  dedent_cpp_file();

  auto& cpp_file_ofs = *(m_context->cpp_file_ofs);
  cpp_file_ofs << CPP_CLOSE_BRACE;
  cpp_file_ofs << std::endl;

  return true;
}

// -----------------------------------------------------------------------------

/* virtual */
bool
SynthesizerImpl::previsit(const ASTInferencePremiseDefn& premise_defn)
{
  const bool has_while_clause = premise_defn.has_while_clause();

  if (has_while_clause) {
    synthesize_inference_premise_defn_with_while_clause(premise_defn);
  } else {
    synthesize_inference_premise_defn_without_while_clause(premise_defn);
  }

  return true;
}

// -----------------------------------------------------------------------------

/* virtual */
bool
SynthesizerImpl::previsit(const ASTInferenceEqualityDefn& premise_defn)
{
  const auto& type_cmp_method_name = m_context->env_defn_map.at(
      SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_CMP_METHOD);

  const bool has_range_clause = premise_defn.has_range_clause();

  static const char var1 = 'i';
  static const char var2 = 'j';

  auto& cpp_file_ofs = *(m_context->cpp_file_ofs);

  if (has_range_clause) {
    const auto& range_clause = premise_defn.range_clause();

    render_indentation_in_cpp_file();

    cpp_file_ofs << CPP_FOR_KEYWORD << CPP_SPACE
                               << CPP_OPEN_PAREN;

    // For-loop initializers.
    {
      cpp_file_ofs << CPP_SIZE_T << CPP_SPACE << var1 << CPP_SPACE
                                 << CPP_ASSIGN << CPP_SPACE
                                 << range_clause.lhs_idx();
      cpp_file_ofs << CPP_COMA << CPP_SPACE;
      cpp_file_ofs << CPP_SIZE_T << CPP_SPACE << var2 << CPP_SPACE
                                 << CPP_ASSIGN << CPP_SPACE
                                 << range_clause.rhs_idx();
      cpp_file_ofs << CPP_SEMICOLON << CPP_SPACE;
    }

    // For-loop termination predicates.
    {
      cpp_file_ofs << var1 << CPP_SPACE << CPP_LESS_THAN
                                 << CPP_SPACE;
      synthesize_deduction_target(
          range_clause.deduction_target(),
          DeductionTargetArraySynthesisMode::AS_SINGULAR,
          m_context->cpp_file_ofs.get());
      cpp_file_ofs << CPP_DOT_SIZE << CPP_SEMICOLON << CPP_SPACE;
    }

    // For-loop increments.
    {
      cpp_file_ofs << CPP_INCREMENT_OPERATOR << var1;
      cpp_file_ofs << CPP_COMA << CPP_SPACE;
      cpp_file_ofs << CPP_INCREMENT_OPERATOR << var2;
    }

    cpp_file_ofs << CPP_CLOSE_PAREN << CPP_SPACE
                               << CPP_OPEN_BRACE;
    cpp_file_ofs << std::endl;
  }

  // Synthesize premise body.
  {
    if (has_range_clause) {
      indent_cpp_file();
    }

    render_indentation_in_cpp_file();
    cpp_file_ofs << CPP_IF << CPP_SPACE << CPP_OPEN_PAREN;
    cpp_file_ofs << CPP_NEGATION;
    cpp_file_ofs << type_cmp_method_name << CPP_OPEN_PAREN;
    synthesize_deduction_target(premise_defn.lhs(),
                                DeductionTargetArraySynthesisMode::AS_SINGULAR,
                                m_context->cpp_file_ofs.get());
    if (has_range_clause) {
      *(m_context->cpp_file_ofs) << CPP_OPEN_BRACKET << var1
                                 << CPP_CLOSE_BRACKET;
    }
    cpp_file_ofs << CPP_COMA << CPP_SPACE;
    synthesize_deduction_target(premise_defn.rhs(),
                                DeductionTargetArraySynthesisMode::AS_SINGULAR,
                                m_context->cpp_file_ofs.get());
    if (has_range_clause) {
      *(m_context->cpp_file_ofs) << CPP_OPEN_BRACKET << var2
                                 << CPP_CLOSE_BRACKET;
    }
    cpp_file_ofs << CPP_COMA << CPP_SPACE;
    synthesize_equality_operator(premise_defn.oprt(),
                                 m_context->cpp_file_ofs.get());
    cpp_file_ofs << CPP_CLOSE_PAREN;
    cpp_file_ofs << CPP_CLOSE_PAREN << CPP_SPACE
                               << CPP_OPEN_BRACE;
    cpp_file_ofs << std::endl;

    // Body of if statement
    {
      indent_cpp_file();
      render_error_handling();
      dedent_cpp_file();
    }

    render_indentation_in_cpp_file();
    cpp_file_ofs << CPP_CLOSE_BRACE;
    cpp_file_ofs << std::endl;

    if (has_range_clause) {
      dedent_cpp_file();
    }
  }

  if (has_range_clause) {
    render_indentation_in_cpp_file();
    cpp_file_ofs << CPP_CLOSE_BRACE << std::endl << std::endl;
  }

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

void
SynthesizerImpl::synthesize_inference_premise_defn_with_while_clause(
    const ASTInferencePremiseDefn& premise_defn) const
{
  SYNTHESIZER_ASSERT(premise_defn.has_while_clause());

  auto& cpp_file_ofs = *(m_context->cpp_file_ofs);

  // Type annotation setup fixture.
  {
    cpp_file_ofs << std::endl;

    const auto& type_annotation_setup_method = m_context->env_defn_map.at(
        SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_ANNOTATION_SETUP_METHOD);

    // Synthesize type annotation setup comment.
    render_indentation_in_cpp_file();
    cpp_file_ofs << SYNTHESIZED_TYPE_ANNOTATION_SETUP_COMMENT
                               << std::endl;

    // Synthesize type annotation setup code.
    render_type_annotation_setup_teardown_fixture(
        premise_defn, type_annotation_setup_method,
        m_context->cpp_file_ofs.get());

    cpp_file_ofs << std::endl;
  }

  // Synthesize body of while-clause.
  {
    const auto& while_clause = premise_defn.while_clause();

    for (const auto& defn : while_clause.premise_defns()) {
      // FIXME: This const_cast here is not ideal.
      // Also because we have to make the `visit` members in `ASTVisitor`
      // protected instead of private.
      (const_cast<SynthesizerImpl*>(this))->visit(defn);
    }
  }

  // Type annotation teardown fixture.
  {
    const auto& type_annotation_teardown_method = m_context->env_defn_map.at(
        SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_ANNOTATION_TEARDOWN_METHOD);

    // Synthesize type annotation teardown comment.
    render_indentation_in_cpp_file();
    cpp_file_ofs << SYNTHESIZED_TYPE_ANNOTATION_TEARDOWN_COMMENT
                               << std::endl;

    // Synthesize type annotation teardown code.
    render_type_annotation_setup_teardown_fixture(
        premise_defn, type_annotation_teardown_method,
        m_context->cpp_file_ofs.get());
  }

  cpp_file_ofs << std::endl;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::synthesize_inference_premise_defn_without_while_clause(
    const ASTInferencePremiseDefn& premise_defn) const
{
  const auto& proof_method_name =
      m_context->env_defn_map.at(SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_PROOF_METHOD);

  auto& cpp_file_ofs = *(m_context->cpp_file_ofs);

  const auto& deduction_target = premise_defn.deduction_target();

  // Synthesize deduction.
  if (deduction_target.is_type<ASTDeductionTargetComputed>()) {
    /**
     * Computed deduction targets get special treatment.
     * Here we deduce the types of the lhs and rhs of the
     * premise definition respectively, and check
     * to see if they are evaluated to be equivalent.
     */
    const auto& type_cls = m_context->type_cls;

    const auto& type_cmp_method_name = m_context->env_defn_map.at(
        SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_CMP_METHOD);

    const auto name1 = __get_next_var_name();

    render_indentation_in_cpp_file();
    cpp_file_ofs << type_cls << CPP_SPACE << name1 << CPP_SPACE
                               << CPP_ASSIGN << CPP_SPACE;
    synthesize_deduction_target(deduction_target,
                                DeductionTargetArraySynthesisMode::AS_SINGULAR,
                                m_context->cpp_file_ofs.get());
    cpp_file_ofs << CPP_SEMICOLON << std::endl;

    const auto name2 = __get_next_var_name();
    render_indentation_in_cpp_file();
    cpp_file_ofs << type_cls << CPP_SPACE << name2 << CPP_SPACE
                               << CPP_ASSIGN << CPP_SPACE;
    cpp_file_ofs << proof_method_name << CPP_OPEN_PAREN;
    synthesize_identifiable(premise_defn.source(),
                            m_context->cpp_file_ofs.get());
    cpp_file_ofs << CPP_CLOSE_PAREN << CPP_SEMICOLON << std::endl;

    render_indentation_in_cpp_file();
    cpp_file_ofs << CPP_IF << CPP_SPACE << CPP_OPEN_PAREN;
    cpp_file_ofs << CPP_NEGATION;
    cpp_file_ofs << type_cmp_method_name << CPP_OPEN_PAREN;
    cpp_file_ofs << name1 << CPP_COMA << CPP_SPACE << name2
                               << CPP_COMA << CPP_SPACE << "std::equal_to<>()";
    cpp_file_ofs << CPP_CLOSE_PAREN << CPP_CLOSE_PAREN
                               << CPP_SPACE << CPP_OPEN_BRACE << std::endl;

    // Body of if-statement.
    {
      indent_cpp_file();
      render_error_handling();
      dedent_cpp_file();
    }

    render_indentation_in_cpp_file();
    cpp_file_ofs << CPP_CLOSE_BRACE;
    cpp_file_ofs << std::endl;
  } else {
    render_indentation_in_cpp_file();
    synthesize_deduction_target_for_declaration(premise_defn.deduction_target(),
                                                m_context->cpp_file_ofs.get());
    cpp_file_ofs << CPP_SPACE << CPP_ASSIGN << CPP_SPACE;
    cpp_file_ofs << proof_method_name << CPP_OPEN_PAREN;
    synthesize_identifiable(premise_defn.source(),
                            m_context->cpp_file_ofs.get());
    cpp_file_ofs << CPP_CLOSE_PAREN << CPP_SEMICOLON;
  }

  cpp_file_ofs << std::endl;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::synthesize_argument_list(const ASTInferenceArgumentList& args,
                                          std::ostream* ofs) const
{
  for (size_t i = 0; i < args.size(); ++i) {
    const auto& arg = args[i];
    (*ofs) << CPP_CONST_KEYWORD << CPP_SPACE << arg.type_name() << CPP_AMPERSAND
           << CPP_SPACE << arg.name();
    if (i + 1 < args.size()) {
      (*ofs) << CPP_COMA << CPP_SPACE;
    }
  }
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::synthesize_deduction_target(
    const ASTDeductionTarget& deduction_target,
    const DeductionTargetArraySynthesisMode array_mode, std::ostream* ofs) const
{
  if (deduction_target.is_type<ASTDeductionTargetSingular>()) {
    const auto& value = deduction_target.value<ASTDeductionTargetSingular>();
    (*ofs) << value.name();
  } else if (deduction_target.is_type<ASTDeductionTargetArray>()) {
    const auto& value = deduction_target.value<ASTDeductionTargetArray>();
    // clang-format off
    switch (array_mode) {
      case DeductionTargetArraySynthesisMode::AS_ARRAY:
        {
          if (value.has_size_literal()) {
            (*ofs) << value.name();
            (*ofs) << CPP_OPEN_BRACKET;
            (*ofs) << value.size_literal();
            (*ofs) << CPP_CLOSE_BRACKET;
          } else {
            // If the target does not have a size literal,
            // then just render it as a raw pointer star.
            (*ofs) << CPP_STAR;
            (*ofs) << value.name();
          }
        }
        break;
      case DeductionTargetArraySynthesisMode::AS_RAW_POINTER_ARRAY:
        {
          (*ofs) << CPP_STAR;
          (*ofs) << value.name();
        }
        break;
      case DeductionTargetArraySynthesisMode::AS_SINGULAR:
        {
          (*ofs) << value.name();
        }
        break;
      case DeductionTargetArraySynthesisMode::AS_STD_VECTOR:
        {
          const auto& type_cls = m_context->type_cls;
          (*ofs) << "std::vector<" << type_cls << '>' << CPP_SPACE << value.name();
        }
        break;
      default:
        break;
    }
    // clang-format on
  } else if (deduction_target.is_type<ASTDeductionTargetComputed>()) {
    const auto& value = deduction_target.value<ASTDeductionTargetComputed>();
    (*ofs) << value.name();
    (*ofs) << CPP_OPEN_PAREN;
    const auto& arguments = value.arguments();
    for (size_t i = 0; i < arguments.size(); ++i) {
      synthesize_deduction_target(
          arguments[i], DeductionTargetArraySynthesisMode::AS_SINGULAR, ofs);
      if (i + 1 < arguments.size()) {
        (*ofs) << CPP_COMA << CPP_SPACE;
      }
    }
    (*ofs) << CPP_CLOSE_PAREN;
  } else {
    SYNTHESIZER_ASSERT(0);
  }
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::synthesize_deduction_target_for_declaration(
    const ASTDeductionTarget& deduction_target, std::ostream* ofs) const
{
  const auto& type_cls = m_context->type_cls;

  if (deduction_target.is_type<ASTDeductionTargetSingular>()) {
    (*ofs) << type_cls << CPP_SPACE;
    synthesize_deduction_target(
        deduction_target, DeductionTargetArraySynthesisMode::AS_STD_VECTOR,
        ofs);
  } else if (deduction_target.is_type<ASTDeductionTargetArray>()) {
    synthesize_deduction_target(
        deduction_target, DeductionTargetArraySynthesisMode::AS_STD_VECTOR,
        ofs);
  } else {
    SYNTHESIZER_ASSERT(0);
  }
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::synthesize_identifiable(const ASTIdentifiable& identifiable,
                                         std::ostream* ofs) const
{
  const auto& identifiers = identifiable.identifiers();
  for (size_t i = 0; i < identifiers.size(); ++i) {
    (*ofs) << identifiers[i].value();
    if (i + 1 < identifiers.size()) {
      (*ofs) << CPP_DOT;
    }
  }
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::synthesize_equality_operator(const EqualityOperator oprt,
                                              std::ostream* ofs) const
{
  const auto& type_cls = m_context->type_cls;
  switch (oprt) {
    case EqualityOperator::OPERATOR_EQ:
      (*ofs) << "std::equal_to";
      break;
    case EqualityOperator::OPERATOR_NEQ:
      (*ofs) << "std::not_equal_to";
      break;
    case EqualityOperator::OPERATOR_LT:
      (*ofs) << "std::less";
      break;
    case EqualityOperator::OPERATOR_LTE:
      (*ofs) << "std::less_equal";
      break;
    default:
      SYNTHESIZER_ASSERT(0);
      break;
  }
  (*ofs) << '<' << type_cls << '>' << CPP_OPEN_PAREN << CPP_CLOSE_PAREN;
}

// -----------------------------------------------------------------------------

/* virtual */
bool
SynthesizerImpl::previsit(const ASTPropositionDefn& proposition_defn)
{
  render_indentation_in_cpp_file();
  auto& cpp_file_ofs = *(m_context->cpp_file_ofs);
  cpp_file_ofs << CPP_RETURN_KEYWORD << CPP_SPACE;
  synthesize_deduction_target(proposition_defn.target(),
                              DeductionTargetArraySynthesisMode::AS_ARRAY,
                              m_context->cpp_file_ofs.get());
  cpp_file_ofs << CPP_SEMICOLON << std::endl;

  return true;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::render_indentation(const size_t indent_lvl,
                                    std::ostream* ofs) const
{
  for (size_t i = 0; i < indent_lvl; ++i) {
    (*ofs) << CPP_INDENTATION;
  }
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::render_indentation_in_header_file() const
{
  render_indentation(m_context->header_file_indent_lvl,
                     m_context->header_file_ofs.get());
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::render_indentation_in_cpp_file() const
{
  render_indentation(m_context->cpp_file_indent_lvl,
                     m_context->cpp_file_ofs.get());
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::indent_header_file() const
{
  ++m_context->header_file_indent_lvl;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::dedent_header_file() const
{
  --m_context->header_file_indent_lvl;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::indent_cpp_file() const
{
  ++m_context->cpp_file_indent_lvl;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::dedent_cpp_file() const
{
  --m_context->cpp_file_indent_lvl;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::render_custom_include(const char* header_name,
                                       std::ostream* ofs) const
{
  (*ofs) << CPP_INCLUDE_DIRECTIVE << CPP_SPACE << CPP_DOUBLE_QUOTE
         << header_name << HEADER_FILE_EXT << CPP_DOUBLE_QUOTE << std::endl;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::render_system_header_includes(std::ostream* ofs) const
{
  std::vector<const char*> system_headers{"cstdlib", "cstddef", "vector"};
  if (m_opts.use_exception) {
    system_headers.push_back("exception");
  } else {
    system_headers.push_back("system_error");
  }

  __render_system_header_includes(system_headers, ofs);
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::__render_system_header_includes(
    const std::vector<const char*>& system_headers, std::ostream* ofs) const
{
  for (const auto& header : system_headers) {
    (*ofs) << CPP_INCLUDE_DIRECTIVE_PREFIX << header << '>' << std::endl;
  }
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::render_inference_error_category(std::ostream* ofs) const
{
  (*ofs) << std::endl;
  (*ofs) << SYNTHESIZED_CUSTOM_ERROR_CATEGORY_DEFINITION;
  (*ofs) << std::endl;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::render_type_annotation_setup_teardown_fixture(
    const ASTInferencePremiseDefn& premise_defn, const std::string& method_name,
    std::ostream* ofs) const
{
  // Synthesize type annotation setup code.
  render_indentation_in_cpp_file();
  *(ofs) << method_name << CPP_OPEN_PAREN;
  synthesize_identifiable(premise_defn.source(), ofs);
  *(m_context->cpp_file_ofs) << CPP_COMA << CPP_SPACE;
  // Should assert that this deduction target here is singular form only.
  // [SNOWLAKE-17] Optimize and refine code synthesis pipeline
  synthesize_deduction_target(premise_defn.deduction_target(),
                              DeductionTargetArraySynthesisMode::AS_SINGULAR,
                              ofs);
  *(ofs) << CPP_CLOSE_PAREN << CPP_SEMICOLON;
  *(ofs) << std::endl;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::render_error_handling() const
{
  const auto type_cls =
      m_context->env_defn_map.at(SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_CLASS);

  auto& cpp_file_ofs = *(m_context->cpp_file_ofs);

  // Assign to output error parameter.
  render_indentation_in_cpp_file();
  cpp_file_ofs
      << CPP_STAR << SYNTHESIZER_DEFAULT_ERROR_OUTPUT_PARAMETER_NAME
      << CPP_SPACE << CPP_ASSIGN << CPP_SPACE << CPP_STD_ERROR_CODE
      << CPP_OPEN_PAREN << '0' << CPP_COMA << CPP_SPACE
      << SYNTHESIZED_GLOBAL_ERROR_CATEGORY_INSTANCE_NAME << CPP_CLOSE_PAREN
      << CPP_SEMICOLON << std::endl;

  // Return default type class instance.
  render_indentation_in_cpp_file();
  cpp_file_ofs << CPP_RETURN_KEYWORD << CPP_SPACE << type_cls
                             << CPP_OPEN_PAREN << CPP_CLOSE_PAREN
                             << CPP_SEMICOLON << std::endl;
}

// -----------------------------------------------------------------------------

bool
SynthesizerImpl::initialize_and_synthesize_error_code_files() const
{
  // Initialize header file.
  std::string ec_header_filepath(m_opts.output_path);
  if (!ec_header_filepath.empty() &&
      ec_header_filepath.back() != FORWARD_SLASH) {
    ec_header_filepath.push_back(FORWARD_SLASH);
  }
  ec_header_filepath.append(SYNTHESIZED_ERROR_CODE_HEADER_FILENAME);

  std::ofstream ec_header_file_ofs(ec_header_filepath, std::ofstream::out);
  if (!ec_header_file_ofs.good()) {
    return false;
  }

  // Initialize .cpp file.
  std::string ec_cpp_filepath(m_opts.output_path);
  if (!ec_cpp_filepath.empty() && ec_cpp_filepath.back() != FORWARD_SLASH) {
    ec_cpp_filepath.push_back(FORWARD_SLASH);
  }
  ec_cpp_filepath.append(SYNTHESIZED_ERROR_CODE_CPP_FILENAME);

  std::ofstream ec_cpp_file_ofs(ec_cpp_filepath, std::ofstream::out);
  if (!ec_cpp_file_ofs.good()) {
    return false;
  }

  // Synthesize header file.
  {
    ec_header_file_ofs << SYNTHESIZED_AUTHORING_COMMENT_BLOCK << std::endl;
    ec_header_file_ofs << CPP_PRAGMA_ONCE << std::endl;
    ec_header_file_ofs << std::endl;
    ec_header_file_ofs << SYNTHESIZED_ERROR_CODE_ENUM_DEFINITION << std::endl;
    ec_header_file_ofs.close();
  }

  // Synthesize .cpp file.
  {
    ec_cpp_file_ofs << SYNTHESIZED_AUTHORING_COMMENT_BLOCK << std::endl;
    render_custom_include(SYNTHESIZED_ERROR_CODE_HEADER_FILENAME_BASE,
                          &ec_cpp_file_ofs);
    __render_system_header_includes(
        std::vector<const char*>{"string", "system_error"}, &ec_cpp_file_ofs);
    ec_cpp_file_ofs << std::endl;
    ec_cpp_file_ofs << SYNTHESIZED_CUSTOM_ERROR_CATEGORY_DEFINITION
                    << std::endl;
    ec_cpp_file_ofs.close();
  }

  return true;
}

// -----------------------------------------------------------------------------

std::string
SynthesizerImpl::__get_next_var_name() const
{
  static const std::string default_prefix("var");
  return default_prefix + std::to_string(m_context->name_id++);
}

// -----------------------------------------------------------------------------
