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

#include "SemanticAnalyzer.h"

#include "ASTUtils.h"
#include "SemanticAnalysisErrorCodes.h"
#include "ast.h"
#include "format_defn.h"

#include <array>

// -----------------------------------------------------------------------------

#define INIT_RES bool res = true

// -----------------------------------------------------------------------------

#define DEFAULT_RETURN return res

// -----------------------------------------------------------------------------

#define ON_WARNING(code, msg, ...)                                             \
  do {                                                                         \
    addWarning(code, (msg), __VA_ARGS__);                                      \
    if (_opts.warningsAsErrors) {                                              \
      res = false;                                                             \
      if (_opts.bailOnFirstError) {                                            \
        return res;                                                            \
      }                                                                        \
    }                                                                          \
  } while (0)

// -----------------------------------------------------------------------------

#define ON_ERROR(code, msg, ...)                                               \
  do {                                                                         \
    res = false;                                                               \
    addError(code, (msg), __VA_ARGS__);                                        \
    if (_opts.bailOnFirstError) {                                              \
      return res;                                                              \
    }                                                                          \
  } while (0)

// -----------------------------------------------------------------------------

#define RETURN_ON_FAILURE(expr)                                                \
  if (!(expr))                                                                 \
  return false

// -----------------------------------------------------------------------------

struct InferenceDefnContext
{
  const std::string& name;
  SymbolSet symbolSet;
  TargetTable targetTbl;
};

// -----------------------------------------------------------------------------

SemanticAnalyzer::SemanticAnalyzer()
  : ASTVisitor()
  , _opts()
{
}

// -----------------------------------------------------------------------------

SemanticAnalyzer::SemanticAnalyzer(const Options& opts)
  : ASTVisitor()
  , _opts(opts)
{
}

// -----------------------------------------------------------------------------

const SemanticAnalyzer::Options&
SemanticAnalyzer::options() const
{
  return _opts;
}

// -----------------------------------------------------------------------------

bool
SemanticAnalyzer::run(const ASTModule& module)
{
  return visit(module);
}

// -----------------------------------------------------------------------------

/* override */
bool
SemanticAnalyzer::previsit(const ASTModule& module)
{
  INIT_RES;

  std::unordered_set<std::string> nameSet;
  for (const auto& inferenceGroup : module.inferenceGroups()) {
    const auto& name = inferenceGroup.name();
    if (nameSet.count(name)) {
      ON_ERROR(kSemanticAnalysisDuplicateInferenceGroupIdentifierError,
               "Found multiple inference group with name \"%s\".",
               name.c_str());
    } else {
      nameSet.insert(name);
    }
  }

  DEFAULT_RETURN;
}

// -----------------------------------------------------------------------------

/* override */
bool
SemanticAnalyzer::previsit(const ASTInferenceGroup& inferenceGroup)
{
  INIT_RES;

  // Environment definitions.
  {
    std::unordered_set<std::string> nameSet;
    for (const auto& environmentDefn : inferenceGroup.environmentDefns()) {
      const auto& field = environmentDefn.field();
      if (nameSet.count(field)) {
        ON_WARNING(kSemanticAnalysisDuplicateEnvironmentDefnFieldError,
                   "Found repeated environment field \"%s\".", field.c_str());
      } else {
        nameSet.insert(field);
      }
    }

    if (!checkRequiredEnvDefns(nameSet)) {
      return false;
    }
  }

  // Inference definitions.
  {
    std::unordered_set<std::string> nameSet;
    for (const auto& inferenceDefn : inferenceGroup.inferenceDefns()) {
      const auto& name = inferenceDefn.name();
      if (nameSet.count(name)) {
        ON_ERROR(kSemanticAnalysisDuplicateInferenceDefnIdentifierError,
                 "Found multiple inference definition with name \"%s\".",
                 name.c_str());
      } else {
        nameSet.insert(name);
      }
    }
  }

  DEFAULT_RETURN;
}

// -----------------------------------------------------------------------------

/* override */
bool
SemanticAnalyzer::previsit(const ASTInferenceDefn& inferenceDefn)
{
  INIT_RES;

  InferenceDefnContext context{.name = inferenceDefn.name()};

  // Global declarations.
  {
    for (const auto& decl : inferenceDefn.globalDecls()) {
      const auto& name = decl.name();
      if (context.symbolSet.count(name)) {
        ON_WARNING(
            kSemanticAnalysisDuplicateGlobalDefinitionError,
            "Found duplicate symbol (global declaration) with name \"%s\".",
            name.c_str());
      } else {
        context.symbolSet.insert(name);
      }
    }
  }

  // Arguments.
  {
    for (const auto& argument : inferenceDefn.arguments()) {
      const auto& name = argument.name();
      if (context.symbolSet.count(name)) {
        ON_ERROR(kSemanticAnalysisDuplicateArgumentIdentifierError,
                 "Found duplicate symbol (argument) with name \"%s\".",
                 name.c_str());
      } else {
        context.symbolSet.insert(name);
      }
    }
  }

  // Premise definitions.
  {
    for (const auto& premiseDefn : inferenceDefn.premiseDefns()) {
      if (!recursivePremiseDefnCheck(premiseDefn, &context)) {
        return false;
      }
    }
  }

  // Proposition.
  {
    const auto& proposition = inferenceDefn.propositionDefn();
    if (!ASTUtils::HasCompatibleTargetInTable(proposition.target(),
                                              context.targetTbl)) {
      ON_ERROR(kSemanticAnalysisInvalidTargetTypeError,
               "Invalid proposition target type in inference \"%s\".",
               context.name.c_str());
    }
  }

  DEFAULT_RETURN;
}

// -----------------------------------------------------------------------------

bool
SemanticAnalyzer::checkRequiredEnvDefns(const SymbolSet& envDefns)
{
  INIT_RES;

  static const std::array<const char*, 4> mandatoryEnvDefns = {
      SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_CLASS,
      SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_CLASS,
      SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_PROOF_METHOD,
      SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_CMP_METHOD,
  };

  for (auto defn : mandatoryEnvDefns) {
    if (envDefns.count(defn) == 0) {
      ON_ERROR(kSemanticAnalysisMissingRequiredEnvironmentDefnFieldError,
               "Missing required environment definition field \"%s\".", defn);
    }
  }

  DEFAULT_RETURN;
}

// -----------------------------------------------------------------------------

template <>
bool
SemanticAnalyzer::recursivePremiseDefnCheck(const ASTInferencePremiseDefn& defn,
                                            InferenceDefnContext* context)
{
  INIT_RES;

  // Handle source.
  {
    const auto& source = defn.source();
    const auto& sourceRoot = ASTUtils::GetRootOfASTIdentifiable(source);
    if (context->symbolSet.count(sourceRoot) == 0) {
      ON_ERROR(kSemanticAnalysisUnknownSymbolError,
               "Unknown symbol \"%s\" used in inference \"%s\".",
               sourceRoot.c_str(), context->name.c_str());
    }
  }

  // Handle target.
  {
    const auto& target = defn.deductionTarget();
    if (ASTUtils::HasIncompatibleTargetInTable(target, context->targetTbl)) {
      ON_ERROR(kSemanticAnalysisIncompatibleTargetTypeError,
               "Found duplicate and incompatible target in inference \"%s\".",
               context->name.c_str());
    }
    ASTUtils::AddTargetToTable(target, &context->targetTbl);
  }

  // Handle while-clause.
  {
    if (defn.hasWhileClause()) {
      const auto& whileClause = defn.whileClause();
      for (const auto& nestedDefn : whileClause.premiseDefns()) {
        RETURN_ON_FAILURE(recursivePremiseDefnCheck(nestedDefn, context));
      }
    }
  }

  DEFAULT_RETURN;
}

// -----------------------------------------------------------------------------

template <>
bool
SemanticAnalyzer::recursivePremiseDefnCheck(
    const ASTInferenceEqualityDefn& defn, InferenceDefnContext* context)
{
  INIT_RES;

  const auto& lhs = defn.lhs();
  const auto& rhs = defn.rhs();

  if (!ASTUtils::AreTargetsCompatible(lhs, rhs)) {
    ON_ERROR(kSemanticAnalysisIncompatibleTargetTypeError,
             "Incompatible targets in expression in inference \"%s\".",
             context->name.c_str());
  }

  // Handle range-clause.
  {
    if (defn.hasRangeClause()) {
      const auto& rangeClause = defn.rangeClause();
      const auto& target = rangeClause.deductionTarget();
      if (target.isType<ASTDeductionTargetSingular>()) {
        ON_ERROR(kSemanticAnalysisInvalidTargetTypeError,
                 "Invalid target in range clause in inference \"%s\".",
                 context->name.c_str());
      }
      if (!ASTUtils::HasCompatibleTargetInTable(target, context->targetTbl)) {
        ON_ERROR(kSemanticAnalysisInvalidTargetTypeError,
                 "Invalid target in range clause in inference \"%s\".",
                 context->name.c_str());
      }
    }
  }

  DEFAULT_RETURN;
}

// -----------------------------------------------------------------------------

bool
SemanticAnalyzer::recursivePremiseDefnCheck(const ASTPremiseDefn& premiseDefn,
                                            InferenceDefnContext* context)
{
  INIT_RES;

  const char* inferenceDefnName = context->name.c_str();

  if (premiseDefn.isType<ASTInferencePremiseDefn>()) {
    const auto& defnValue = premiseDefn.value<ASTInferencePremiseDefn>();
    RETURN_ON_FAILURE(recursivePremiseDefnCheck(defnValue, context));
  } else if (premiseDefn.isType<ASTInferenceEqualityDefn>()) {
    const auto& defnValue = premiseDefn.value<ASTInferenceEqualityDefn>();
    RETURN_ON_FAILURE(recursivePremiseDefnCheck(defnValue, context));
  } else {
    ON_ERROR(kSemanticAnalysisUnknownPremiseDefnError,
             "Found unknown type of premise definition in inference \"%s\".",
             inferenceDefnName);
  }

  DEFAULT_RETURN;
}

// -----------------------------------------------------------------------------
