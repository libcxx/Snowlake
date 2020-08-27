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
__getIncrementalInt()
{
  static size_t val = 1;
  return val++;
}

// -----------------------------------------------------------------------------

typedef std::unordered_map<std::string, std::string> EnvDefnMap;

// -----------------------------------------------------------------------------

struct InferenceGroupSynthesisContext
{
  std::string clsName;
  std::string typeCls;
  EnvDefnMap envDefnMap;
  std::unique_ptr<std::ofstream> headerFileOfs;
  std::unique_ptr<std::ofstream> cppFileOfs;
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

  EnvDefnMap getEnvnDefnMapFromInferenceGroup(const ASTInferenceGroup&);
  std::string getClassNameFromEnvDefn(const EnvDefnMap&);

  enum class DeductionTargetArraySynthesisMode : uint32_t
  {
    AS_SINGULAR = 0x01,
    AS_ARRAY = 0x02,
    AS_RAW_POINTER_ARRAY = 0x04,
    AS_STD_VECTOR = 0x08,
  };

  void synthesizeInferencePremiseDefnWithWhileClause(
      const ASTInferencePremiseDefn&) const;

  void synthesizeInferencePremiseDefnWithoutWhileClause(
      const ASTInferencePremiseDefn&) const;

  void synthesizeArgumentList(const ASTInferenceArgumentList&,
                                std::ostream*) const;

  void synthesizeDeductionTarget(const ASTDeductionTarget&,
                                   const DeductionTargetArraySynthesisMode,
                                   std::ostream*) const;

  void synthesizeDeductionTargetForDeclaration(const ASTDeductionTarget&,
                                                   std::ostream*) const;

  void synthesizeIdentifiable(const ASTIdentifiable&, std::ostream*) const;

  void synthesizeEqualityOperator(const EqualityOperator,
                                    std::ostream*) const;

  void renderIndentation(const size_t, std::ostream*) const;

  void renderIndentationInHeaderFile() const;

  void renderIndentationInCppFile() const;

  void renderCustomInclude(const char*, std::ostream*) const;

  void renderSystemHeaderIncludes(std::ostream*) const;

  void __renderSystemHeaderIncludes(
      const std::vector<const char*>& system_headers, std::ostream*) const;

  void renderInferenceErrorCategory(std::ostream*) const;

  void renderTypeAnnotationSetupTeardownFixture(
      const ASTInferencePremiseDefn& premise_defn,
      const std::string& method_name, std::ostream*) const;

  void renderErrorHandling() const;

  void indentHeaderFile() const;

  void dedentHeaderFile() const;

  void indentCppFile() const;

  void dedentCppFile() const;

  bool initializeAndSynthesizeErrorCodeFiles() const;

  std::string __getNextVarName() const;

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
  : clsName()
  , typeCls()
  , envDefnMap()
  , headerFileOfs()
  , cppFileOfs()
  , header_file_indent_lvl(0)
  , cpp_file_indent_lvl(0)
  , name_id(0)
{
}

// -----------------------------------------------------------------------------

InferenceGroupSynthesisContext::~InferenceGroupSynthesisContext()
{
  if (headerFileOfs) {
    headerFileOfs->close();
    headerFileOfs.release();
  }
  if (cppFileOfs) {
    cppFileOfs->close();
    cppFileOfs.release();
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
  if (!initializeAndSynthesizeErrorCodeFiles()) {
    return false;
  }
  return visit(module);
}

// -----------------------------------------------------------------------------

/* virtual */
bool
SynthesizerImpl::previsit(const ASTInferenceGroup& inference_group)
{
  EnvDefnMap envDefnMap =
      getEnvnDefnMapFromInferenceGroup(inference_group);

  const auto clsName = getClassNameFromEnvDefn(envDefnMap);

  const auto typeCls =
      envDefnMap.at(SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_CLASS);

  // Create header file.
  std::string headerFilepath(m_opts.outputPath);
  if (!headerFilepath.empty() && headerFilepath.back() != FORWARD_SLASH) {
    headerFilepath.push_back(FORWARD_SLASH);
  }
  headerFilepath.append(clsName);
  headerFilepath.append(HEADER_FILE_EXT);

  std::unique_ptr<std::ofstream> headerFileOfs(
      new std::ofstream(headerFilepath, std::ofstream::out));
  if (!headerFileOfs) {
    return false;
  }
  if (!headerFileOfs->good()) {
    headerFileOfs.release();
    return false;
  }

  // Create .cpp file.
  std::string cppFilepath(m_opts.outputPath);
  if (!cppFilepath.empty() && cppFilepath.back() != FORWARD_SLASH) {
    cppFilepath.push_back(FORWARD_SLASH);
  }
  cppFilepath.append(clsName);
  cppFilepath.append(CPP_FILE_EXT);

  std::unique_ptr<std::ofstream> cppFileOfs(
      new std::ofstream(cppFilepath, std::ofstream::out));
  if (!cppFileOfs) {
    headerFileOfs.release();
    return false;
  }
  if (!cppFileOfs->good()) {
    headerFileOfs.release();
    cppFileOfs.release();
    return false;
  }

  m_context.reset(new InferenceGroupSynthesisContext());
  if (!m_context) {
    headerFileOfs.release();
    cppFileOfs.release();
    return false;
  }
  m_context->headerFileOfs = std::move(headerFileOfs);
  m_context->cppFileOfs = std::move(cppFileOfs);
  m_context->clsName = std::move(clsName);
  m_context->typeCls = std::move(typeCls);
  m_context->envDefnMap = std::move(envDefnMap);

  // Write to header file.
  {
    auto& headerFileOfsRef = *(m_context->headerFileOfs);
    headerFileOfsRef << SYNTHESIZED_AUTHORING_COMMENT_BLOCK;
    headerFileOfsRef << std::endl;
    headerFileOfsRef << std::endl;
    headerFileOfsRef << CPP_PRAGMA_ONCE << std::endl;
    headerFileOfsRef << std::endl;
    renderSystemHeaderIncludes(m_context->headerFileOfs.get());
    headerFileOfsRef << std::endl;
    headerFileOfsRef << CPP_CLASS_KEYWORD << ' ';
    headerFileOfsRef << m_context->clsName;
    headerFileOfsRef << std::endl;
    headerFileOfsRef << CPP_OPEN_BRACE;
    headerFileOfsRef << std::endl;
    headerFileOfsRef << CPP_PUBLIC_KEYWORD << CPP_COLON;
    headerFileOfsRef << std::endl;
  }

  // Write to .cpp file.
  {
    auto& cppFileOfsRef = *(m_context->cppFileOfs);
    cppFileOfsRef << SYNTHESIZED_AUTHORING_COMMENT_BLOCK;
    cppFileOfsRef << std::endl;
    renderCustomInclude(m_context->clsName.c_str(),
                          m_context->cppFileOfs.get());
    renderCustomInclude(SYNTHESIZED_ERROR_CODE_HEADER_FILENAME_BASE,
                          m_context->cppFileOfs.get());
  }

  return true;
}

// -----------------------------------------------------------------------------

/* virtual */
bool
SynthesizerImpl::postvisit(const ASTInferenceGroup&)
{
  SYNTHESIZER_ASSERT(m_context);
  SYNTHESIZER_ASSERT(m_context->headerFileOfs);
  SYNTHESIZER_ASSERT(m_context->cppFileOfs);

  // Write closing };
  {
    auto& headerFileOfs = *(m_context->headerFileOfs);
    headerFileOfs << CPP_CLOSE_BRACE;
    headerFileOfs << CPP_SEMICOLON;
    headerFileOfs << std::endl;
  }

  // Close and release header file stream.
  {
    m_context->headerFileOfs->close();
    m_context->headerFileOfs.release();
  }

  // Close and release .cpp file stream.
  {
    m_context->cppFileOfs->close();
    m_context->cppFileOfs.release();
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
SynthesizerImpl::previsit(const ASTInferenceDefn& inferenceDefn)
{
  SYNTHESIZER_ASSERT(m_context);
  SYNTHESIZER_ASSERT(m_context->headerFileOfs);
  SYNTHESIZER_ASSERT(m_context->cppFileOfs);

  // Synthesize member function declaration.
  {
    indentHeaderFile();
    renderIndentationInHeaderFile();
    auto& headerFileOfs = *(m_context->headerFileOfs);
    headerFileOfs << m_context->typeCls << CPP_SPACE;
    headerFileOfs << inferenceDefn.name();
    headerFileOfs << CPP_OPEN_PAREN;
    synthesizeArgumentList(inferenceDefn.arguments(),
                             m_context->headerFileOfs.get());
    if (!m_opts.useException) {
      headerFileOfs << CPP_COMA << CPP_SPACE;
      headerFileOfs << CPP_STD_ERROR_CODE << CPP_STAR;
    }
    headerFileOfs << CPP_CLOSE_PAREN;
    headerFileOfs << CPP_SEMICOLON;
    headerFileOfs << std::endl;
    dedentHeaderFile();
  }

  // Synthesize member function definition.
  {
    auto& cppFileOfs = *(m_context->cppFileOfs);
    cppFileOfs << std::endl;
    cppFileOfs << m_context->typeCls << std::endl;
    cppFileOfs << m_context->clsName;
    cppFileOfs << CPP_COLON << CPP_COLON;
    cppFileOfs << inferenceDefn.name();
    cppFileOfs << CPP_OPEN_PAREN;
    synthesizeArgumentList(inferenceDefn.arguments(),
                             m_context->cppFileOfs.get());
    if (!m_opts.useException) {
      cppFileOfs << CPP_COMA << CPP_SPACE;
      cppFileOfs << CPP_STD_ERROR_CODE << CPP_STAR;
      cppFileOfs << CPP_SPACE
                   << SYNTHESIZER_DEFAULT_ERROR_OUTPUT_PARAMETER_NAME;
    }
    cppFileOfs << CPP_CLOSE_PAREN;
    cppFileOfs << std::endl;
    cppFileOfs << CPP_OPEN_BRACE;
    cppFileOfs << std::endl;

    indentCppFile();
  }

  return true;
}

// -----------------------------------------------------------------------------

/* virtual */
bool
SynthesizerImpl::postvisit(const ASTInferenceDefn&)
{
  SYNTHESIZER_ASSERT(m_context);
  SYNTHESIZER_ASSERT(m_context->cppFileOfs);

  dedentCppFile();

  auto& cppFileOfs = *(m_context->cppFileOfs);
  cppFileOfs << CPP_CLOSE_BRACE;
  cppFileOfs << std::endl;

  return true;
}

// -----------------------------------------------------------------------------

/* virtual */
bool
SynthesizerImpl::previsit(const ASTInferencePremiseDefn& premise_defn)
{
  const bool has_while_clause = premise_defn.has_while_clause();

  if (has_while_clause) {
    synthesizeInferencePremiseDefnWithWhileClause(premise_defn);
  } else {
    synthesizeInferencePremiseDefnWithoutWhileClause(premise_defn);
  }

  return true;
}

// -----------------------------------------------------------------------------

/* virtual */
bool
SynthesizerImpl::previsit(const ASTInferenceEqualityDefn& premiseDefn)
{
  const auto& typeCmpMethodName = m_context->envDefnMap.at(
      SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_CMP_METHOD);

  const bool hasRangeClause = premiseDefn.has_range_clause();

  static const char var1 = 'i';
  static const char var2 = 'j';

  auto& cppFileOfs = *(m_context->cppFileOfs);

  if (hasRangeClause) {
    const auto& rangeClause = premiseDefn.range_clause();

    renderIndentationInCppFile();

    cppFileOfs << CPP_FOR_KEYWORD << CPP_SPACE << CPP_OPEN_PAREN;

    // For-loop initializers.
    {
      cppFileOfs << CPP_SIZE_T << CPP_SPACE << var1 << CPP_SPACE << CPP_ASSIGN
                   << CPP_SPACE << rangeClause.lhs_idx();
      cppFileOfs << CPP_COMA << CPP_SPACE;
      cppFileOfs << CPP_SIZE_T << CPP_SPACE << var2 << CPP_SPACE << CPP_ASSIGN
                   << CPP_SPACE << rangeClause.rhs_idx();
      cppFileOfs << CPP_SEMICOLON << CPP_SPACE;
    }

    // For-loop termination predicates.
    {
      cppFileOfs << var1 << CPP_SPACE << CPP_LESS_THAN << CPP_SPACE;
      synthesizeDeductionTarget(
          rangeClause.deduction_target(),
          DeductionTargetArraySynthesisMode::AS_SINGULAR,
          m_context->cppFileOfs.get());
      cppFileOfs << CPP_DOT_SIZE << CPP_SEMICOLON << CPP_SPACE;
    }

    // For-loop increments.
    {
      cppFileOfs << CPP_INCREMENT_OPERATOR << var1;
      cppFileOfs << CPP_COMA << CPP_SPACE;
      cppFileOfs << CPP_INCREMENT_OPERATOR << var2;
    }

    cppFileOfs << CPP_CLOSE_PAREN << CPP_SPACE << CPP_OPEN_BRACE;
    cppFileOfs << std::endl;
  }

  // Synthesize premise body.
  {
    if (hasRangeClause) {
      indentCppFile();
    }

    renderIndentationInCppFile();
    cppFileOfs << CPP_IF << CPP_SPACE << CPP_OPEN_PAREN;
    cppFileOfs << CPP_NEGATION;
    cppFileOfs << typeCmpMethodName << CPP_OPEN_PAREN;
    synthesizeDeductionTarget(premiseDefn.lhs(),
                                DeductionTargetArraySynthesisMode::AS_SINGULAR,
                                m_context->cppFileOfs.get());
    if (hasRangeClause) {
      *(m_context->cppFileOfs) << CPP_OPEN_BRACKET << var1
                                 << CPP_CLOSE_BRACKET;
    }
    cppFileOfs << CPP_COMA << CPP_SPACE;
    synthesizeDeductionTarget(premiseDefn.rhs(),
                                DeductionTargetArraySynthesisMode::AS_SINGULAR,
                                m_context->cppFileOfs.get());
    if (hasRangeClause) {
      *(m_context->cppFileOfs) << CPP_OPEN_BRACKET << var2
                                 << CPP_CLOSE_BRACKET;
    }
    cppFileOfs << CPP_COMA << CPP_SPACE;
    synthesizeEqualityOperator(premiseDefn.oprt(),
                                 m_context->cppFileOfs.get());
    cppFileOfs << CPP_CLOSE_PAREN;
    cppFileOfs << CPP_CLOSE_PAREN << CPP_SPACE << CPP_OPEN_BRACE;
    cppFileOfs << std::endl;

    // Body of if statement
    {
      indentCppFile();
      renderErrorHandling();
      dedentCppFile();
    }

    renderIndentationInCppFile();
    cppFileOfs << CPP_CLOSE_BRACE;
    cppFileOfs << std::endl;

    if (hasRangeClause) {
      dedentCppFile();
    }
  }

  if (hasRangeClause) {
    renderIndentationInCppFile();
    cppFileOfs << CPP_CLOSE_BRACE << std::endl << std::endl;
  }

  return true;
}

// -----------------------------------------------------------------------------

EnvDefnMap
SynthesizerImpl::getEnvnDefnMapFromInferenceGroup(
    const ASTInferenceGroup& inference_group)
{
  EnvDefnMap envDefnMap;

  for (const auto& envn_defn : inference_group.environment_defns()) {
    envDefnMap[envn_defn.field()] = envn_defn.value();
  }

  return envDefnMap;
}

// -----------------------------------------------------------------------------

std::string
SynthesizerImpl::getClassNameFromEnvDefn(const EnvDefnMap& envDefnMap)
{
  const auto itr = envDefnMap.find(SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_CLASS);
  if (itr == envDefnMap.cend()) {
    char buf[16] = {0};
    snprintf(buf, sizeof(buf), "%s%lu", SYNTHESIZER_DEFAULT_CLASS_NAME_PREFIX,
             __getIncrementalInt());
    return std::string(buf);
  }
  return itr->second;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::synthesizeInferencePremiseDefnWithWhileClause(
    const ASTInferencePremiseDefn& premiseDefn) const
{
  SYNTHESIZER_ASSERT(premiseDefn.has_while_clause());

  auto& cppFileOfs = *(m_context->cppFileOfs);

  // Type annotation setup fixture.
  {
    cppFileOfs << std::endl;

    const auto& typeAnnotationSetupMethod = m_context->envDefnMap.at(
        SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_ANNOTATION_SETUP_METHOD);

    // Synthesize type annotation setup comment.
    renderIndentationInCppFile();
    cppFileOfs << SYNTHESIZED_TYPE_ANNOTATION_SETUP_COMMENT << std::endl;

    // Synthesize type annotation setup code.
    renderTypeAnnotationSetupTeardownFixture(
        premiseDefn, typeAnnotationSetupMethod,
        m_context->cppFileOfs.get());

    cppFileOfs << std::endl;
  }

  // Synthesize body of while-clause.
  {
    const auto& whileClause = premiseDefn.while_clause();

    for (const auto& defn : whileClause.premise_defns()) {
      // FIXME: This const_cast here is not ideal.
      // Also because we have to make the `visit` members in `ASTVisitor`
      // protected instead of private.
      (const_cast<SynthesizerImpl*>(this))->visit(defn);
    }
  }

  // Type annotation teardown fixture.
  {
    const auto& typeAnnotationTeardownMethod = m_context->envDefnMap.at(
        SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_ANNOTATION_TEARDOWN_METHOD);

    // Synthesize type annotation teardown comment.
    renderIndentationInCppFile();
    cppFileOfs << SYNTHESIZED_TYPE_ANNOTATION_TEARDOWN_COMMENT << std::endl;

    // Synthesize type annotation teardown code.
    renderTypeAnnotationSetupTeardownFixture(
        premiseDefn, typeAnnotationTeardownMethod,
        m_context->cppFileOfs.get());
  }

  cppFileOfs << std::endl;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::synthesizeInferencePremiseDefnWithoutWhileClause(
    const ASTInferencePremiseDefn& premise_defn) const
{
  const auto& proofMethodName =
      m_context->envDefnMap.at(SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_PROOF_METHOD);

  auto& cppFileOfs = *(m_context->cppFileOfs);

  const auto& deductionTarget = premise_defn.deduction_target();

  // Synthesize deduction.
  if (deductionTarget.is_type<ASTDeductionTargetComputed>()) {
    /**
     * Computed deduction targets get special treatment.
     * Here we deduce the types of the lhs and rhs of the
     * premise definition respectively, and check
     * to see if they are evaluated to be equivalent.
     */
    const auto& typeCls = m_context->typeCls;

    const auto& type_cmp_method_name = m_context->envDefnMap.at(
        SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_CMP_METHOD);

    const auto name1 = __getNextVarName();

    renderIndentationInCppFile();
    cppFileOfs << typeCls << CPP_SPACE << name1 << CPP_SPACE << CPP_ASSIGN
                 << CPP_SPACE;
    synthesizeDeductionTarget(deductionTarget,
                                DeductionTargetArraySynthesisMode::AS_SINGULAR,
                                m_context->cppFileOfs.get());
    cppFileOfs << CPP_SEMICOLON << std::endl;

    const auto name2 = __getNextVarName();
    renderIndentationInCppFile();
    cppFileOfs << typeCls << CPP_SPACE << name2 << CPP_SPACE << CPP_ASSIGN
                 << CPP_SPACE;
    cppFileOfs << proofMethodName << CPP_OPEN_PAREN;
    synthesizeIdentifiable(premise_defn.source(),
                            m_context->cppFileOfs.get());
    cppFileOfs << CPP_CLOSE_PAREN << CPP_SEMICOLON << std::endl;

    renderIndentationInCppFile();
    cppFileOfs << CPP_IF << CPP_SPACE << CPP_OPEN_PAREN;
    cppFileOfs << CPP_NEGATION;
    cppFileOfs << type_cmp_method_name << CPP_OPEN_PAREN;
    cppFileOfs << name1 << CPP_COMA << CPP_SPACE << name2 << CPP_COMA
                 << CPP_SPACE << CPP_STD_EQUAL_TO_DEFAULT_INSTANTIATION;
    cppFileOfs << CPP_CLOSE_PAREN << CPP_CLOSE_PAREN << CPP_SPACE
                 << CPP_OPEN_BRACE << std::endl;

    // Body of if-statement.
    {
      indentCppFile();
      renderErrorHandling();
      dedentCppFile();
    }

    renderIndentationInCppFile();
    cppFileOfs << CPP_CLOSE_BRACE;
    cppFileOfs << std::endl;
  } else {
    renderIndentationInCppFile();
    synthesizeDeductionTargetForDeclaration(premise_defn.deduction_target(),
                                                m_context->cppFileOfs.get());
    cppFileOfs << CPP_SPACE << CPP_ASSIGN << CPP_SPACE;
    cppFileOfs << proofMethodName << CPP_OPEN_PAREN;
    synthesizeIdentifiable(premise_defn.source(),
                            m_context->cppFileOfs.get());
    cppFileOfs << CPP_CLOSE_PAREN << CPP_SEMICOLON;
  }

  cppFileOfs << std::endl;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::synthesizeArgumentList(const ASTInferenceArgumentList& args,
                                          std::ostream* ofs) const
{
  auto& ofs_ref = *ofs;

  for (size_t i = 0; i < args.size(); ++i) {
    const auto& arg = args[i];
    ofs_ref << CPP_CONST_KEYWORD << CPP_SPACE << arg.type_name()
            << CPP_AMPERSAND << CPP_SPACE << arg.name();
    if (i + 1 < args.size()) {
      ofs_ref << CPP_COMA << CPP_SPACE;
    }
  }
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::synthesizeDeductionTarget(
    const ASTDeductionTarget& deductionTarget,
    const DeductionTargetArraySynthesisMode arrayMode, std::ostream* ofs) const
{
  auto& ofs_ref = *ofs;

  if (deductionTarget.is_type<ASTDeductionTargetSingular>()) {
    const auto& value = deductionTarget.value<ASTDeductionTargetSingular>();
    ofs_ref << value.name();
  } else if (deductionTarget.is_type<ASTDeductionTargetArray>()) {
    const auto& value = deductionTarget.value<ASTDeductionTargetArray>();
    // clang-format off
    switch (arrayMode) {
      case DeductionTargetArraySynthesisMode::AS_ARRAY:
        {
          if (value.has_size_literal()) {
            ofs_ref << value.name();
            ofs_ref << CPP_OPEN_BRACKET;
            ofs_ref << value.size_literal();
            ofs_ref << CPP_CLOSE_BRACKET;
          } else {
            // If the target does not have a size literal,
            // then just render it as a raw pointer star.
            ofs_ref << CPP_STAR;
            ofs_ref << value.name();
          }
        }
        break;
      case DeductionTargetArraySynthesisMode::AS_RAW_POINTER_ARRAY:
        {
          ofs_ref << CPP_STAR;
          ofs_ref << value.name();
        }
        break;
      case DeductionTargetArraySynthesisMode::AS_SINGULAR:
        {
          ofs_ref << value.name();
        }
        break;
      case DeductionTargetArraySynthesisMode::AS_STD_VECTOR:
        {
          const auto& typeCls = m_context->typeCls;
          ofs_ref << "std::vector<" << typeCls << '>' << CPP_SPACE << value.name();
        }
        break;
      default:
        break;
    }
    // clang-format on
  } else if (deductionTarget.is_type<ASTDeductionTargetComputed>()) {
    const auto& value = deductionTarget.value<ASTDeductionTargetComputed>();
    ofs_ref << value.name();
    ofs_ref << CPP_OPEN_PAREN;
    const auto& arguments = value.arguments();
    for (size_t i = 0; i < arguments.size(); ++i) {
      synthesizeDeductionTarget(
          arguments[i], DeductionTargetArraySynthesisMode::AS_SINGULAR, ofs);
      if (i + 1 < arguments.size()) {
        ofs_ref << CPP_COMA << CPP_SPACE;
      }
    }
    ofs_ref << CPP_CLOSE_PAREN;
  } else {
    SYNTHESIZER_ASSERT(0 && "Unknown type of deduction target.");
  }
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::synthesizeDeductionTargetForDeclaration(
    const ASTDeductionTarget& deductionTarget, std::ostream* ofs) const
{
  const auto& typeCls = m_context->typeCls;

  auto& ofs_ref = *ofs;

  if (deductionTarget.is_type<ASTDeductionTargetSingular>()) {
    ofs_ref << typeCls << CPP_SPACE;
    synthesizeDeductionTarget(
        deductionTarget, DeductionTargetArraySynthesisMode::AS_STD_VECTOR,
        ofs);
  } else if (deductionTarget.is_type<ASTDeductionTargetArray>()) {
    synthesizeDeductionTarget(
        deductionTarget, DeductionTargetArraySynthesisMode::AS_STD_VECTOR,
        ofs);
  } else {
    SYNTHESIZER_ASSERT(0 && "Unsupported deduction target.");
  }
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::synthesizeIdentifiable(const ASTIdentifiable& identifiable,
                                         std::ostream* ofs) const
{
  auto& ofs_ref = *ofs;
  const auto& identifiers = identifiable.identifiers();
  for (size_t i = 0; i < identifiers.size(); ++i) {
    ofs_ref << identifiers[i].value();
    if (i + 1 < identifiers.size()) {
      ofs_ref << CPP_DOT;
    }
  }
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::synthesizeEqualityOperator(const EqualityOperator oprt,
                                              std::ostream* ofs) const
{
  auto& ofs_ref = *ofs;
  const auto& typeCls = m_context->typeCls;
  switch (oprt) {
    case EqualityOperator::OPERATOR_EQ:
      ofs_ref << "std::equal_to";
      break;
    case EqualityOperator::OPERATOR_NEQ:
      ofs_ref << "std::not_equal_to";
      break;
    case EqualityOperator::OPERATOR_LT:
      ofs_ref << "std::less";
      break;
    case EqualityOperator::OPERATOR_LTE:
      ofs_ref << "std::less_equal";
      break;
    default:
      SYNTHESIZER_ASSERT(0 && "Unsupported EqualityOperator value.");
      break;
  }
  ofs_ref << '<' << typeCls << '>' << CPP_OPEN_PAREN << CPP_CLOSE_PAREN;
}

// -----------------------------------------------------------------------------

/* virtual */
bool
SynthesizerImpl::previsit(const ASTPropositionDefn& propositionDefn)
{
  renderIndentationInCppFile();
  auto& cppFileOfs = *(m_context->cppFileOfs);
  cppFileOfs << CPP_RETURN_KEYWORD << CPP_SPACE;
  synthesizeDeductionTarget(propositionDefn.target(),
                              DeductionTargetArraySynthesisMode::AS_ARRAY,
                              m_context->cppFileOfs.get());
  cppFileOfs << CPP_SEMICOLON << std::endl;

  return true;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::renderIndentation(const size_t indentLvl,
                                    std::ostream* ofs) const
{
  auto& ofs_ref = *ofs;
  for (size_t i = 0; i < indentLvl; ++i) {
    ofs_ref << CPP_INDENTATION;
  }
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::renderIndentationInHeaderFile() const
{
  renderIndentation(m_context->header_file_indent_lvl,
                     m_context->headerFileOfs.get());
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::renderIndentationInCppFile() const
{
  renderIndentation(m_context->cpp_file_indent_lvl,
                     m_context->cppFileOfs.get());
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::indentHeaderFile() const
{
  ++m_context->header_file_indent_lvl;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::dedentHeaderFile() const
{
  --m_context->header_file_indent_lvl;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::indentCppFile() const
{
  ++m_context->cpp_file_indent_lvl;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::dedentCppFile() const
{
  --m_context->cpp_file_indent_lvl;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::renderCustomInclude(const char* header_name,
                                       std::ostream* ofs) const
{
  auto& ofs_ref = *ofs;
  ofs_ref << CPP_INCLUDE_DIRECTIVE << CPP_SPACE << CPP_DOUBLE_QUOTE
          << header_name << HEADER_FILE_EXT << CPP_DOUBLE_QUOTE << std::endl;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::renderSystemHeaderIncludes(std::ostream* ofs) const
{
  std::vector<const char*> systemHeaders{"cstdlib", "cstddef", "vector"};
  if (m_opts.useException) {
    systemHeaders.push_back("exception");
  } else {
    systemHeaders.push_back("system_error");
  }

  __renderSystemHeaderIncludes(systemHeaders, ofs);
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::__renderSystemHeaderIncludes(
    const std::vector<const char*>& systemHeaders, std::ostream* ofs) const
{
  auto& ofs_ref = *ofs;
  for (const auto& header : systemHeaders) {
    ofs_ref << CPP_INCLUDE_DIRECTIVE_PREFIX << header << '>' << std::endl;
  }
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::renderInferenceErrorCategory(std::ostream* ofs) const
{
  auto& ofs_ref = *ofs;
  ofs_ref << std::endl;
  ofs_ref << SYNTHESIZED_CUSTOM_ERROR_CATEGORY_DEFINITION;
  ofs_ref << std::endl;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::renderTypeAnnotationSetupTeardownFixture(
    const ASTInferencePremiseDefn& premiseDefn, const std::string& methodName,
    std::ostream* ofs) const
{
  // Synthesize type annotation setup code.
  renderIndentationInCppFile();
  auto& ofs_ref = *ofs;
  ofs_ref << methodName << CPP_OPEN_PAREN;
  synthesizeIdentifiable(premiseDefn.source(), ofs);
  auto& cppFileOfs = *(m_context->cppFileOfs);
  cppFileOfs << CPP_COMA << CPP_SPACE;
  // Should assert that this deduction target here is singular form only.
  // [SNOWLAKE-17] Optimize and refine code synthesis pipeline
  synthesizeDeductionTarget(premiseDefn.deduction_target(),
                              DeductionTargetArraySynthesisMode::AS_SINGULAR,
                              ofs);
  ofs_ref << CPP_CLOSE_PAREN << CPP_SEMICOLON;
  ofs_ref << std::endl;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::renderErrorHandling() const
{
  const auto typeCls =
      m_context->envDefnMap.at(SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_CLASS);

  auto& cppFileOfs = *(m_context->cppFileOfs);

  // Assign to output error parameter.
  renderIndentationInCppFile();
  cppFileOfs << CPP_STAR << SYNTHESIZER_DEFAULT_ERROR_OUTPUT_PARAMETER_NAME
               << CPP_SPACE << CPP_ASSIGN << CPP_SPACE << CPP_STD_ERROR_CODE
               << CPP_OPEN_PAREN << '0' << CPP_COMA << CPP_SPACE
               << SYNTHESIZED_GLOBAL_ERROR_CATEGORY_INSTANCE_NAME
               << CPP_CLOSE_PAREN << CPP_SEMICOLON << std::endl;

  // Return default type class instance.
  renderIndentationInCppFile();
  cppFileOfs << CPP_RETURN_KEYWORD << CPP_SPACE << typeCls << CPP_OPEN_PAREN
               << CPP_CLOSE_PAREN << CPP_SEMICOLON << std::endl;
}

// -----------------------------------------------------------------------------

bool
SynthesizerImpl::initializeAndSynthesizeErrorCodeFiles() const
{
  // Initialize header file.
  std::string ecHeaderFilepath(m_opts.outputPath);
  if (!ecHeaderFilepath.empty() &&
      ecHeaderFilepath.back() != FORWARD_SLASH) {
    ecHeaderFilepath.push_back(FORWARD_SLASH);
  }
  ecHeaderFilepath.append(SYNTHESIZED_ERROR_CODE_HEADER_FILENAME);

  std::ofstream ecHeaderFileOfs(ecHeaderFilepath, std::ofstream::out);
  if (!ecHeaderFileOfs.good()) {
    return false;
  }

  // Initialize .cpp file.
  std::string ecCppFilepath(m_opts.outputPath);
  if (!ecCppFilepath.empty() && ecCppFilepath.back() != FORWARD_SLASH) {
    ecCppFilepath.push_back(FORWARD_SLASH);
  }
  ecCppFilepath.append(SYNTHESIZED_ERROR_CODE_CPP_FILENAME);

  std::ofstream ecCppFileOfs(ecCppFilepath, std::ofstream::out);
  if (!ecCppFileOfs.good()) {
    return false;
  }

  // Synthesize header file.
  {
    ecHeaderFileOfs << SYNTHESIZED_AUTHORING_COMMENT_BLOCK << std::endl;
    ecHeaderFileOfs << CPP_PRAGMA_ONCE << std::endl;
    ecHeaderFileOfs << std::endl;
    ecHeaderFileOfs << SYNTHESIZED_ERROR_CODE_ENUM_DEFINITION << std::endl;
    ecHeaderFileOfs.close();
  }

  // Synthesize .cpp file.
  {
    ecCppFileOfs << SYNTHESIZED_AUTHORING_COMMENT_BLOCK << std::endl;
    renderCustomInclude(SYNTHESIZED_ERROR_CODE_HEADER_FILENAME_BASE,
                          &ecCppFileOfs);
    static const std::vector<const char*> system_headers{"string",
                                                         "system_error"};
    __renderSystemHeaderIncludes(system_headers, &ecCppFileOfs);
    ecCppFileOfs << std::endl;
    ecCppFileOfs << SYNTHESIZED_CUSTOM_ERROR_CATEGORY_DEFINITION
                    << std::endl;
    ecCppFileOfs.close();
  }

  return true;
}

// -----------------------------------------------------------------------------

std::string
SynthesizerImpl::__getNextVarName() const
{
  static const std::string defaultPrefix("var");
  return defaultPrefix + std::to_string(m_context->name_id++);
}

// -----------------------------------------------------------------------------
