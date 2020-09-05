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
#include "CompilerErrorHandlerRegistrar.h"
#include "SynthesisErrorCategory.h"
#include "SynthesizerUtil.h"
#include "ast.h"
#include "format_defn.h"
#include "macros.h"

#include <array>
#include <cstdio>
#include <fstream>
#include <limits>
#include <unordered_map>
#include <vector>

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
  std::ofstream headerFileOfs;
  std::ofstream cppFileOfs;
  size_t headerFileIndentLvl;
  size_t cppFileIndentLvl;
  uint32_t nameId;

  InferenceGroupSynthesisContext();
  ~InferenceGroupSynthesisContext();
};

// -----------------------------------------------------------------------------

template <typename T>
struct ScopedIndentationGuard
{
  explicit ScopedIndentationGuard(T& indentLvl)
    : _indentLvl(indentLvl)
  {
    ASSERT(_indentLvl < std::numeric_limits<T>::max());
    ++_indentLvl;
  }

  ~ScopedIndentationGuard()
  {
    ASSERT(_indentLvl >= 1);
    --_indentLvl;
  }

private:
  T& _indentLvl;
};

// -----------------------------------------------------------------------------

class SynthesizerImpl : public ASTVisitor
{
public:
  SynthesizerImpl(const Synthesizer::Options&);

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

  void
  synthesizeInferencePremiseDefnWithWhileClause(const ASTInferencePremiseDefn&);

  void synthesizeInferencePremiseDefnWithoutWhileClause(
      const ASTInferencePremiseDefn&);

  void synthesizeArgumentList(const ASTInferenceArgumentList&, std::ostream&);

  void synthesizeDeductionTarget(const ASTDeductionTarget&,
                                 const DeductionTargetArraySynthesisMode,
                                 std::ostream&);

  void synthesizeDeductionTargetForDeclaration(const ASTDeductionTarget&,
                                               std::ostream&);

  void synthesizeIdentifiable(const ASTIdentifiable&, std::ostream&);

  void synthesizeEqualityOperator(const EqualityOperator, std::ostream&);

  void renderIndentation(const size_t, std::ostream&);

  void renderIndentationInHeaderFile();

  void renderIndentationInCppFile();

  void renderCustomInclude(const char*, std::ostream&);

  void renderSystemHeaderIncludes(std::ostream&);

  template <typename Iterator>
  void __renderSystemHeaderIncludes(Iterator first, Iterator last,
                                    std::ostream&);

  void renderInferenceErrorCategory(std::ostream&);

  void renderTypeAnnotationSetupTeardownFixtrue(
      const ASTInferencePremiseDefn& premiseDefn,
      const std::string& method_name, std::ostream&);

  void renderInputSourceAnnotationComment(std::ostream&);

  void renderClassAnnotationComment(std::ostream&);

  void renderInferenceDefinitionMethodAnnotationComment(
      const std::string& inferenceDefnName, std::ostream&,
      bool isHeaderFile = false);

  void renderErrorHandling();

  void indentCppFile();

  void dedentCppFile();

  bool initializeAndSynthesizeErrorCodeFiles();

  void handleErrorWithMessageAndCode(const char*, CompilerError::Code);

  std::string __getNextVarName();

private:
  const Synthesizer::Options& _opts;
  InferenceGroupSynthesisContext _context;
};

// -----------------------------------------------------------------------------

Synthesizer::Synthesizer()
  : _opts()
{
}

// -----------------------------------------------------------------------------

Synthesizer::Synthesizer(const Options& opts)
  : _opts(opts)
{
}

// -----------------------------------------------------------------------------

bool
Synthesizer::run(const ASTModule& module) const
{
  SynthesizerImpl impl(_opts);
  return impl.run(module);
}

// -----------------------------------------------------------------------------

InferenceGroupSynthesisContext::InferenceGroupSynthesisContext()
  : clsName()
  , typeCls()
  , envDefnMap()
  , headerFileOfs()
  , cppFileOfs()
  , headerFileIndentLvl(0)
  , cppFileIndentLvl(0)
  , nameId(0)
{
}

// -----------------------------------------------------------------------------

InferenceGroupSynthesisContext::~InferenceGroupSynthesisContext()
{
  headerFileOfs.close();
  cppFileOfs.close();
}

// -----------------------------------------------------------------------------

SynthesizerImpl::SynthesizerImpl(const Synthesizer::Options& opts)
  : _opts(opts)
  , _context()
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
SynthesizerImpl::previsit(const ASTInferenceGroup& inferenceGroup)
{
  EnvDefnMap envDefnMap = getEnvnDefnMapFromInferenceGroup(inferenceGroup);

  const auto clsName = getClassNameFromEnvDefn(envDefnMap);

  const auto typeCls =
      envDefnMap.at(SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_CLASS);

  // Create header file.
  std::string headerFilepath(_opts.outputPath);
  if (!headerFilepath.empty() && headerFilepath.back() != FORWARD_SLASH) {
    headerFilepath.push_back(FORWARD_SLASH);
  }
  headerFilepath.append(clsName);
  headerFilepath.append(HEADER_FILE_EXT);

  _context.headerFileOfs.open(headerFilepath, std::ofstream::out);
  if (!_context.headerFileOfs.good()) {
    handleErrorWithMessageAndCode("Failed to create output .h file",
                                  kSynthesisInvalidOutputError);
    return false;
  }

  // Create .cpp file.
  std::string cppFilepath(_opts.outputPath);
  if (!cppFilepath.empty() && cppFilepath.back() != FORWARD_SLASH) {
    cppFilepath.push_back(FORWARD_SLASH);
  }
  cppFilepath.append(clsName);
  cppFilepath.append(CPP_FILE_EXT);

  _context.cppFileOfs.open(cppFilepath, std::ofstream::out);
  if (!_context.cppFileOfs.good()) {
    handleErrorWithMessageAndCode("Failed to create output .cpp file",
                                  kSynthesisInvalidOutputError);
    return false;
  }

  _context.clsName = std::move(clsName);
  _context.typeCls = std::move(typeCls);
  _context.envDefnMap = std::move(envDefnMap);

  // Write to header file.
  {
    auto& headerFileOfsRef = _context.headerFileOfs;
    headerFileOfsRef << SYNTHESIZED_AUTHORING_COMMENT_BLOCK;
    headerFileOfsRef << CPP_NEWLINE;
    headerFileOfsRef << CPP_NEWLINE;
    renderInputSourceAnnotationComment(headerFileOfsRef);
    headerFileOfsRef << CPP_NEWLINE;
    headerFileOfsRef << CPP_PRAGMA_ONCE << CPP_NEWLINE;
    headerFileOfsRef << CPP_NEWLINE;
    renderSystemHeaderIncludes(_context.headerFileOfs);
    headerFileOfsRef << CPP_NEWLINE;
    renderClassAnnotationComment(headerFileOfsRef);
    headerFileOfsRef << CPP_CLASS_KEYWORD << ' ';
    headerFileOfsRef << _context.clsName;
    headerFileOfsRef << CPP_NEWLINE;
    headerFileOfsRef << CPP_OPEN_BRACE;
    headerFileOfsRef << CPP_NEWLINE;
    headerFileOfsRef << CPP_PUBLIC_KEYWORD << CPP_COLON;
    headerFileOfsRef << CPP_NEWLINE;
  }

  // Write to .cpp file.
  {
    auto& cppFileOfsRef = _context.cppFileOfs;
    cppFileOfsRef << SYNTHESIZED_AUTHORING_COMMENT_BLOCK;
    cppFileOfsRef << CPP_NEWLINE;
    cppFileOfsRef << CPP_NEWLINE;
    renderInputSourceAnnotationComment(cppFileOfsRef);
    cppFileOfsRef << CPP_NEWLINE;
    renderCustomInclude(_context.clsName.c_str(), _context.cppFileOfs);
    renderCustomInclude(SYNTHESIZED_ERROR_CODE_HEADER_FILENAME_BASE,
                        _context.cppFileOfs);
  }

  return true;
}

// -----------------------------------------------------------------------------

/* virtual */
bool
SynthesizerImpl::postvisit(const ASTInferenceGroup&)
{
  // Write closing };
  {
    auto& headerFileOfs = _context.headerFileOfs;
    headerFileOfs << CPP_CLOSE_BRACE;
    headerFileOfs << CPP_SEMICOLON;
    headerFileOfs << CPP_NEWLINE;
  }

  // Close and release header file stream.
  if (_context.headerFileOfs.good()) {
    _context.headerFileOfs.close();
  }

  // Close and release .cpp file stream.
  if (_context.cppFileOfs.good()) {
    _context.cppFileOfs.close();
  }

  return true;
}

// -----------------------------------------------------------------------------

/* virtual */
bool
SynthesizerImpl::previsit(const ASTInferenceDefn& inferenceDefn)
{
  // Synthesize member function declaration.
  {
    ScopedIndentationGuard scopedIndentation(_context.headerFileIndentLvl);

    auto& headerFileOfs = _context.headerFileOfs;
    renderInferenceDefinitionMethodAnnotationComment(
        inferenceDefn.name(), headerFileOfs, true /** isHeaderFile */);
    renderIndentationInHeaderFile();
    headerFileOfs << _context.typeCls << CPP_SPACE;
    headerFileOfs << inferenceDefn.name();
    headerFileOfs << CPP_OPEN_PAREN;
    synthesizeArgumentList(inferenceDefn.arguments(), _context.headerFileOfs);
    if (!_opts.useException) {
      headerFileOfs << CPP_COMA << CPP_SPACE;
      headerFileOfs << CPP_STD_ERROR_CODE << CPP_STAR;
    }
    headerFileOfs << CPP_CLOSE_PAREN;
    headerFileOfs << CPP_SEMICOLON;
    headerFileOfs << CPP_NEWLINE;
    headerFileOfs << CPP_NEWLINE;
  }

  // Synthesize member function definition.
  {
    auto& cppFileOfs = _context.cppFileOfs;
    cppFileOfs << CPP_NEWLINE;
    renderInferenceDefinitionMethodAnnotationComment(inferenceDefn.name(),
                                                     cppFileOfs);
    cppFileOfs << _context.typeCls << CPP_NEWLINE;
    cppFileOfs << _context.clsName;
    cppFileOfs << CPP_COLON << CPP_COLON;
    cppFileOfs << inferenceDefn.name();
    cppFileOfs << CPP_OPEN_PAREN;
    synthesizeArgumentList(inferenceDefn.arguments(), _context.cppFileOfs);
    if (!_opts.useException) {
      cppFileOfs << CPP_COMA << CPP_SPACE;
      cppFileOfs << CPP_STD_ERROR_CODE << CPP_STAR;
      cppFileOfs << CPP_SPACE
                 << SYNTHESIZER_DEFAULT_ERROR_OUTPUT_PARAMETER_NAME;
    }
    cppFileOfs << CPP_CLOSE_PAREN;
    cppFileOfs << CPP_NEWLINE;
    cppFileOfs << CPP_OPEN_BRACE;
    cppFileOfs << CPP_NEWLINE;

    indentCppFile();
  }

  return true;
}

// -----------------------------------------------------------------------------

/* virtual */
bool
SynthesizerImpl::postvisit(const ASTInferenceDefn&)
{
  dedentCppFile();

  auto& cppFileOfs = _context.cppFileOfs;
  cppFileOfs << CPP_CLOSE_BRACE;
  cppFileOfs << CPP_NEWLINE;

  return true;
}

// -----------------------------------------------------------------------------

/* virtual */
bool
SynthesizerImpl::previsit(const ASTInferencePremiseDefn& premiseDefn)
{
  const bool hasWhileClause = premiseDefn.hasWhileClause();

  if (hasWhileClause) {
    synthesizeInferencePremiseDefnWithWhileClause(premiseDefn);
  } else {
    synthesizeInferencePremiseDefnWithoutWhileClause(premiseDefn);
  }

  return true;
}

// -----------------------------------------------------------------------------

/* virtual */
bool
SynthesizerImpl::previsit(const ASTInferenceEqualityDefn& premiseDefn)
{
  const auto& typeCmpMethodName =
      _context.envDefnMap.at(SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_CMP_METHOD);

  const bool hasRangeClause = premiseDefn.hasRangeClause();

  static const char var1 = 'i';
  static const char var2 = 'j';

  auto& cppFileOfs = _context.cppFileOfs;

  if (hasRangeClause) {
    const auto& rangeClause = premiseDefn.rangeClause();

    renderIndentationInCppFile();

    cppFileOfs << CPP_FOR_KEYWORD << CPP_SPACE << CPP_OPEN_PAREN;

    // For-loop initializers.
    {
      cppFileOfs << CPP_SIZE_T << CPP_SPACE << var1 << CPP_SPACE << CPP_ASSIGN
                 << CPP_SPACE << rangeClause.lhsIdx();
      cppFileOfs << CPP_COMA << CPP_SPACE;
      cppFileOfs << CPP_SIZE_T << CPP_SPACE << var2 << CPP_SPACE << CPP_ASSIGN
                 << CPP_SPACE << rangeClause.rhsIdx();
      cppFileOfs << CPP_SEMICOLON << CPP_SPACE;
    }

    // For-loop termination predicates.
    {
      cppFileOfs << var1 << CPP_SPACE << CPP_LESS_THAN << CPP_SPACE;
      synthesizeDeductionTarget(rangeClause.deductionTarget(),
                                DeductionTargetArraySynthesisMode::AS_SINGULAR,
                                _context.cppFileOfs);
      cppFileOfs << CPP_DOT_SIZE << CPP_SEMICOLON << CPP_SPACE;
    }

    // For-loop increments.
    {
      cppFileOfs << CPP_INCREMENT_OPERATOR << var1;
      cppFileOfs << CPP_COMA << CPP_SPACE;
      cppFileOfs << CPP_INCREMENT_OPERATOR << var2;
    }

    cppFileOfs << CPP_CLOSE_PAREN << CPP_SPACE << CPP_OPEN_BRACE;
    cppFileOfs << CPP_NEWLINE;
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
                              _context.cppFileOfs);
    if (hasRangeClause) {
      _context.cppFileOfs << CPP_OPEN_BRACKET << var1 << CPP_CLOSE_BRACKET;
    }
    cppFileOfs << CPP_COMA << CPP_SPACE;
    synthesizeDeductionTarget(premiseDefn.rhs(),
                              DeductionTargetArraySynthesisMode::AS_SINGULAR,
                              _context.cppFileOfs);
    if (hasRangeClause) {
      _context.cppFileOfs << CPP_OPEN_BRACKET << var2 << CPP_CLOSE_BRACKET;
    }
    cppFileOfs << CPP_COMA << CPP_SPACE;
    synthesizeEqualityOperator(premiseDefn.oprt(), _context.cppFileOfs);
    cppFileOfs << CPP_CLOSE_PAREN;
    cppFileOfs << CPP_CLOSE_PAREN << CPP_SPACE << CPP_OPEN_BRACE;
    cppFileOfs << CPP_NEWLINE;

    // Body of if statement
    {
      ScopedIndentationGuard scopedIndentation(_context.cppFileIndentLvl);
      renderErrorHandling();
    }

    renderIndentationInCppFile();
    cppFileOfs << CPP_CLOSE_BRACE;
    cppFileOfs << CPP_NEWLINE;

    if (hasRangeClause) {
      dedentCppFile();
    }
  }

  if (hasRangeClause) {
    renderIndentationInCppFile();
    cppFileOfs << CPP_CLOSE_BRACE << CPP_NEWLINE << CPP_NEWLINE;
  }

  return true;
}

// -----------------------------------------------------------------------------

EnvDefnMap
SynthesizerImpl::getEnvnDefnMapFromInferenceGroup(
    const ASTInferenceGroup& inferenceGroup)
{
  EnvDefnMap envDefnMap;

  for (const auto& envnDefn : inferenceGroup.environmentDefns()) {
    envDefnMap[envnDefn.field()] = envnDefn.value();
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
    const ASTInferencePremiseDefn& premiseDefn)
{
  SYNTHESIZER_ASSERT(premiseDefn.hasWhileClause());

  auto& cppFileOfs = _context.cppFileOfs;

  // Type annotation setup fixtrue.
  {
    cppFileOfs << CPP_NEWLINE;

    const auto& typeAnnotationSetupMethod = _context.envDefnMap.at(
        SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_ANNOTATION_SETUP_METHOD);

    // Synthesize type annotation setup comment.
    renderIndentationInCppFile();
    cppFileOfs << SYNTHESIZED_TYPE_ANNOTATION_SETUP_COMMENT << CPP_NEWLINE;

    // Synthesize type annotation setup code.
    renderTypeAnnotationSetupTeardownFixtrue(
        premiseDefn, typeAnnotationSetupMethod, _context.cppFileOfs);

    cppFileOfs << CPP_NEWLINE;
  }

  // Synthesize body of while-clause.
  {
    const auto& whileClause = premiseDefn.whileClause();

    for (const auto& defn : whileClause.premiseDefns()) {
      // FIXME: This const_cast here is not ideal.
      // Also because we have to make the `visit` members in `ASTVisitor`
      // protected instead of private.
      (const_cast<SynthesizerImpl*>(this))->visit(defn);
    }
  }

  // Type annotation teardown fixtrue.
  {
    const auto& typeAnnotationTeardownMethod = _context.envDefnMap.at(
        SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_ANNOTATION_TEARDOWN_METHOD);

    // Synthesize type annotation teardown comment.
    renderIndentationInCppFile();
    cppFileOfs << SYNTHESIZED_TYPE_ANNOTATION_TEARDOWN_COMMENT << CPP_NEWLINE;

    // Synthesize type annotation teardown code.
    renderTypeAnnotationSetupTeardownFixtrue(
        premiseDefn, typeAnnotationTeardownMethod, _context.cppFileOfs);
  }

  cppFileOfs << CPP_NEWLINE;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::synthesizeInferencePremiseDefnWithoutWhileClause(
    const ASTInferencePremiseDefn& premiseDefn)
{
  const auto& proofMethodName =
      _context.envDefnMap.at(SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_PROOF_METHOD);

  auto& cppFileOfs = _context.cppFileOfs;

  const auto& deductionTarget = premiseDefn.deductionTarget();

  // Synthesize deduction.
  if (deductionTarget.isType<ASTDeductionTargetComputed>()) {
    /**
     * Computed deduction targets get special treatment.
     * Here we deduce the types of the lhs and rhs of the
     * premise definition respectively, and check
     * to see if they are evaluated to be equivalent.
     */
    const auto& typeCls = _context.typeCls;

    const auto& type_cmp_method_name =
        _context.envDefnMap.at(SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_CMP_METHOD);

    const auto name1 = __getNextVarName();

    renderIndentationInCppFile();
    cppFileOfs << typeCls << CPP_SPACE << name1 << CPP_SPACE << CPP_ASSIGN
               << CPP_SPACE;
    synthesizeDeductionTarget(deductionTarget,
                              DeductionTargetArraySynthesisMode::AS_SINGULAR,
                              _context.cppFileOfs);
    cppFileOfs << CPP_SEMICOLON << CPP_NEWLINE;

    const auto name2 = __getNextVarName();
    renderIndentationInCppFile();
    cppFileOfs << typeCls << CPP_SPACE << name2 << CPP_SPACE << CPP_ASSIGN
               << CPP_SPACE;
    cppFileOfs << proofMethodName << CPP_OPEN_PAREN;
    synthesizeIdentifiable(premiseDefn.source(), _context.cppFileOfs);
    cppFileOfs << CPP_CLOSE_PAREN << CPP_SEMICOLON << CPP_NEWLINE;

    renderIndentationInCppFile();
    cppFileOfs << CPP_IF << CPP_SPACE << CPP_OPEN_PAREN;
    cppFileOfs << CPP_NEGATION;
    cppFileOfs << type_cmp_method_name << CPP_OPEN_PAREN;
    cppFileOfs << name1 << CPP_COMA << CPP_SPACE << name2 << CPP_COMA
               << CPP_SPACE << CPP_STD_EQUAL_TO_DEFAULT_INSTANTIATION;
    cppFileOfs << CPP_CLOSE_PAREN << CPP_CLOSE_PAREN << CPP_SPACE
               << CPP_OPEN_BRACE << CPP_NEWLINE;

    // Body of if-statement.
    {
      ScopedIndentationGuard scopedIndentation(_context.cppFileIndentLvl);
      renderErrorHandling();
    }

    renderIndentationInCppFile();
    cppFileOfs << CPP_CLOSE_BRACE;
    cppFileOfs << CPP_NEWLINE;
  } else {
    renderIndentationInCppFile();
    synthesizeDeductionTargetForDeclaration(premiseDefn.deductionTarget(),
                                            _context.cppFileOfs);
    cppFileOfs << CPP_SPACE << CPP_ASSIGN << CPP_SPACE;
    cppFileOfs << proofMethodName << CPP_OPEN_PAREN;
    synthesizeIdentifiable(premiseDefn.source(), _context.cppFileOfs);
    cppFileOfs << CPP_CLOSE_PAREN << CPP_SEMICOLON;
  }

  cppFileOfs << CPP_NEWLINE;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::synthesizeArgumentList(const ASTInferenceArgumentList& args,
                                        std::ostream& ofsRef)
{
  for (size_t i = 0; i < args.size(); ++i) {
    const auto& arg = args[i];
    ofsRef << CPP_CONST_KEYWORD << CPP_SPACE << arg.typeName() << CPP_AMPERSAND
           << CPP_SPACE << arg.name();
    if (i + 1 < args.size()) {
      ofsRef << CPP_COMA << CPP_SPACE;
    }
  }
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::synthesizeDeductionTarget(
    const ASTDeductionTarget& deductionTarget,
    const DeductionTargetArraySynthesisMode arrayMode, std::ostream& ofsRef)
{
  if (deductionTarget.isType<ASTDeductionTargetSingular>()) {
    const auto& value = deductionTarget.value<ASTDeductionTargetSingular>();
    ofsRef << value.name();
  } else if (deductionTarget.isType<ASTDeductionTargetArray>()) {
    const auto& value = deductionTarget.value<ASTDeductionTargetArray>();
    // clang-format off
    switch (arrayMode) {
      case DeductionTargetArraySynthesisMode::AS_ARRAY:
        {
          if (value.hasSizeLiteral()) {
            ofsRef << value.name();
            ofsRef << CPP_OPEN_BRACKET;
            ofsRef << value.sizeLiteral();
            ofsRef << CPP_CLOSE_BRACKET;
          } else {
            // If the target does not have a size literal,
            // then just render it as a raw pointer star.
            ofsRef << CPP_STAR;
            ofsRef << value.name();
          }
        }
        break;
      case DeductionTargetArraySynthesisMode::AS_RAW_POINTER_ARRAY:
        {
          ofsRef << CPP_STAR;
          ofsRef << value.name();
        }
        break;
      case DeductionTargetArraySynthesisMode::AS_SINGULAR:
        {
          ofsRef << value.name();
        }
        break;
      case DeductionTargetArraySynthesisMode::AS_STD_VECTOR:
        {
          const auto& typeCls = _context.typeCls;
          ofsRef << "std::vector<" << typeCls << '>' << CPP_SPACE << value.name();
        }
        break;
      default:
        break;
    }
    // clang-format on
  } else if (deductionTarget.isType<ASTDeductionTargetComputed>()) {
    const auto& value = deductionTarget.value<ASTDeductionTargetComputed>();
    ofsRef << value.name();
    ofsRef << CPP_OPEN_PAREN;
    const auto& arguments = value.arguments();
    for (size_t i = 0; i < arguments.size(); ++i) {
      synthesizeDeductionTarget(
          arguments[i], DeductionTargetArraySynthesisMode::AS_SINGULAR, ofsRef);
      if (i + 1 < arguments.size()) {
        ofsRef << CPP_COMA << CPP_SPACE;
      }
    }
    ofsRef << CPP_CLOSE_PAREN;
  } else {
    SYNTHESIZER_ASSERT(0 && "Unknown type of deduction target.");
  }
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::synthesizeDeductionTargetForDeclaration(
    const ASTDeductionTarget& deductionTarget, std::ostream& ofsRef)
{
  const auto& typeCls = _context.typeCls;

  if (deductionTarget.isType<ASTDeductionTargetSingular>()) {
    ofsRef << typeCls << CPP_SPACE;
    synthesizeDeductionTarget(deductionTarget,
                              DeductionTargetArraySynthesisMode::AS_STD_VECTOR,
                              ofsRef);
  } else if (deductionTarget.isType<ASTDeductionTargetArray>()) {
    synthesizeDeductionTarget(deductionTarget,
                              DeductionTargetArraySynthesisMode::AS_STD_VECTOR,
                              ofsRef);
  } else {
    SYNTHESIZER_ASSERT(0 && "Unsupported deduction target.");
  }
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::synthesizeIdentifiable(const ASTIdentifiable& identifiable,
                                        std::ostream& ofsRef)
{
  const auto& identifiers = identifiable.identifiers();
  for (size_t i = 0; i < identifiers.size(); ++i) {
    ofsRef << identifiers[i].value();
    if (i + 1 < identifiers.size()) {
      ofsRef << CPP_DOT;
    }
  }
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::synthesizeEqualityOperator(const EqualityOperator oprt,
                                            std::ostream& ofsRef)
{
  const auto& typeCls = _context.typeCls;
  switch (oprt) {
    case EqualityOperator::OPERATOR_EQ:
      ofsRef << "std::equal_to";
      break;
    case EqualityOperator::OPERATOR_NEQ:
      ofsRef << "std::not_equal_to";
      break;
    case EqualityOperator::OPERATOR_LT:
      ofsRef << "std::less";
      break;
    case EqualityOperator::OPERATOR_LTE:
      ofsRef << "std::less_equal";
      break;
    default:
      SYNTHESIZER_ASSERT(0 && "Unsupported EqualityOperator value.");
  }
  ofsRef << '<' << typeCls << '>' << CPP_OPEN_PAREN << CPP_CLOSE_PAREN;
}

// -----------------------------------------------------------------------------

/* virtual */
bool
SynthesizerImpl::previsit(const ASTPropositionDefn& propositionDefn)
{
  renderIndentationInCppFile();
  auto& cppFileOfs = _context.cppFileOfs;
  cppFileOfs << CPP_RETURN_KEYWORD << CPP_SPACE;
  synthesizeDeductionTarget(propositionDefn.target(),
                            DeductionTargetArraySynthesisMode::AS_ARRAY,
                            _context.cppFileOfs);
  cppFileOfs << CPP_SEMICOLON << CPP_NEWLINE;

  return true;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::renderIndentation(const size_t indentLvl, std::ostream& ofsRef)
{
  for (size_t i = 0; i < indentLvl; ++i) {
    ofsRef << CPP_INDENTATION;
  }
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::renderIndentationInHeaderFile()
{
  renderIndentation(_context.headerFileIndentLvl, _context.headerFileOfs);
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::renderIndentationInCppFile()
{
  renderIndentation(_context.cppFileIndentLvl, _context.cppFileOfs);
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::indentCppFile()
{
  ASSERT(_context.cppFileIndentLvl <
         std::numeric_limits<decltype(_context.cppFileIndentLvl)>::max());
  ++_context.cppFileIndentLvl;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::dedentCppFile()
{
  ASSERT(_context.cppFileIndentLvl >= 1);
  --_context.cppFileIndentLvl;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::renderCustomInclude(const char* headerName,
                                     std::ostream& ofsRef)
{
  ofsRef << CPP_INCLUDE_DIRECTIVE << CPP_SPACE << CPP_DOUBLE_QUOTE << headerName
         << HEADER_FILE_EXT << CPP_DOUBLE_QUOTE << CPP_NEWLINE;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::renderSystemHeaderIncludes(std::ostream& ofsRef)
{
  std::vector<const char*> systemHeaders{"cstdlib", "cstddef", "vector"};
  if (_opts.useException) {
    systemHeaders.push_back("exception");
  } else {
    systemHeaders.push_back("system_error");
  }

  __renderSystemHeaderIncludes(systemHeaders.begin(), systemHeaders.end(),
                               ofsRef);
}

// -----------------------------------------------------------------------------

template <typename Iterator>
void
SynthesizerImpl::__renderSystemHeaderIncludes(Iterator first, Iterator last,
                                              std::ostream& ofsRef)
{
  for (auto it = first; it != last; ++it) {
    ofsRef << CPP_INCLUDE_DIRECTIVE_PREFIX << (*it) << '>' << CPP_NEWLINE;
  }
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::renderInferenceErrorCategory(std::ostream& ofsRef)
{
  ofsRef << CPP_NEWLINE;
  ofsRef << SYNTHESIZED_CUSTOM_ERROR_CATEGORY_DEFINITION;
  ofsRef << CPP_NEWLINE;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::renderTypeAnnotationSetupTeardownFixtrue(
    const ASTInferencePremiseDefn& premiseDefn, const std::string& methodName,
    std::ostream& ofsRef)
{
  // Synthesize type annotation setup code.
  renderIndentationInCppFile();
  ofsRef << methodName << CPP_OPEN_PAREN;
  synthesizeIdentifiable(premiseDefn.source(), ofsRef);
  auto& cppFileOfs = _context.cppFileOfs;
  cppFileOfs << CPP_COMA << CPP_SPACE;
  // Should assert that this deduction target here is singular form only.
  // [SNOWLAKE-17] Optimize and refine code synthesis pipeline
  synthesizeDeductionTarget(premiseDefn.deductionTarget(),
                            DeductionTargetArraySynthesisMode::AS_SINGULAR,
                            ofsRef);
  ofsRef << CPP_CLOSE_PAREN << CPP_SEMICOLON;
  ofsRef << CPP_NEWLINE;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::renderErrorHandling()
{
  const auto typeCls =
      _context.envDefnMap.at(SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_CLASS);

  auto& cppFileOfs = _context.cppFileOfs;

  // Assign to output error parameter.
  renderIndentationInCppFile();
  cppFileOfs << CPP_STAR << SYNTHESIZER_DEFAULT_ERROR_OUTPUT_PARAMETER_NAME
             << CPP_SPACE << CPP_ASSIGN << CPP_SPACE << CPP_STD_ERROR_CODE
             << CPP_OPEN_PAREN << '0' << CPP_COMA << CPP_SPACE
             << SYNTHESIZED_GLOBAL_ERROR_CATEGORY_INSTANCE_NAME
             << CPP_CLOSE_PAREN << CPP_SEMICOLON << CPP_NEWLINE;

  // Return default type class instance.
  renderIndentationInCppFile();
  cppFileOfs << CPP_RETURN_KEYWORD << CPP_SPACE << typeCls << CPP_OPEN_PAREN
             << CPP_CLOSE_PAREN << CPP_SEMICOLON << CPP_NEWLINE;
}

// -----------------------------------------------------------------------------

bool
SynthesizerImpl::initializeAndSynthesizeErrorCodeFiles()
{
  // Initialize header file.
  std::string ecHeaderFilepath(_opts.outputPath);
  if (!ecHeaderFilepath.empty() && ecHeaderFilepath.back() != FORWARD_SLASH) {
    ecHeaderFilepath.push_back(FORWARD_SLASH);
  }
  ecHeaderFilepath.append(SYNTHESIZED_ERROR_CODE_HEADER_FILENAME);

  std::ofstream ecHeaderFileOfs(ecHeaderFilepath, std::ofstream::out);
  if (!ecHeaderFileOfs.good()) {
    return false;
  }

  // Initialize .cpp file.
  std::string ecCppFilepath(_opts.outputPath);
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
    ecHeaderFileOfs << SYNTHESIZED_AUTHORING_COMMENT_BLOCK << CPP_NEWLINE;
    ecHeaderFileOfs << CPP_PRAGMA_ONCE << CPP_NEWLINE;
    ecHeaderFileOfs << CPP_NEWLINE;
    ecHeaderFileOfs << SYNTHESIZED_ERROR_CODE_ENUM_DEFINITION << CPP_NEWLINE;
    ecHeaderFileOfs.close();
  }

  // Synthesize .cpp file.
  {
    ecCppFileOfs << SYNTHESIZED_AUTHORING_COMMENT_BLOCK << CPP_NEWLINE;
    renderCustomInclude(SYNTHESIZED_ERROR_CODE_HEADER_FILENAME_BASE,
                        ecCppFileOfs);
    static const std::array<const char*, 2> system_headers{"string",
                                                           "system_error"};
    __renderSystemHeaderIncludes(system_headers.begin(), system_headers.end(),
                                 ecCppFileOfs);
    ecCppFileOfs << CPP_NEWLINE;
    ecCppFileOfs << SYNTHESIZED_CUSTOM_ERROR_CATEGORY_DEFINITION << CPP_NEWLINE;
    ecCppFileOfs.close();
  }

  return true;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::renderInputSourceAnnotationComment(std::ostream& ofs)
{
  ofs << COMMENT_BLOCK_BEGIN;

  ofs << " * ";

  char buf[1024] = {0};
  snprintf(buf, sizeof(buf), "This file was synthesized from %s",
           _opts.inputFilepath.c_str());

  ofs << buf << '\n';

  ofs << COMMENT_BLOCK_END;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::renderClassAnnotationComment(std::ostream& ofs)
{
  ofs << COMMENT_BLOCK_BEGIN;

  ofs << " * ";

  char buf[1024] = {0};
  snprintf(buf, sizeof(buf),
           "This class was synthesized from the \"%s\" rules group.",
           _context.clsName.c_str());

  ofs << buf << '\n';

  ofs << COMMENT_BLOCK_END;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::renderInferenceDefinitionMethodAnnotationComment(
    const std::string& inferenceDefnName, std::ostream& ofs, bool isHeaderFile)
{
  if (isHeaderFile)
    renderIndentation(_context.headerFileIndentLvl, ofs);

  ofs << COMMENT_BLOCK_BEGIN;

  char buf[1024] = {0};
  snprintf(buf, sizeof(buf),
           "This method was synthesized from the \"%s\" inference definition.",
           inferenceDefnName.c_str());

  if (isHeaderFile)
    renderIndentation(_context.headerFileIndentLvl, ofs);

  ofs << " * ";
  ofs << buf << '\n';

  if (isHeaderFile)
    renderIndentation(_context.headerFileIndentLvl, ofs);

  ofs << COMMENT_BLOCK_END;
}

// -----------------------------------------------------------------------------

void
SynthesizerImpl::handleErrorWithMessageAndCode(const char* msg,
                                               CompilerError::Code code)
{
  CompilerErrorHandlerRegistrar::RegisterCompilerError(
      SynthesisErrorCategory::CreateCompilerErrorWithTypeAndMessage(
          CompilerError::Type::Error, code, msg));
}

// -----------------------------------------------------------------------------

std::string
SynthesizerImpl::__getNextVarName()
{
  static const std::string defaultPrefix("var");
  return defaultPrefix + std::to_string(_context.nameId++);
}

// -----------------------------------------------------------------------------
