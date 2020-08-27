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
#include "ast.h"
#include "ast_util.h"
#include "format_defn.h"

// -----------------------------------------------------------------------------

#define INIT_RES bool res = true

#define DEFAULT_RETURN return res

#define ON_WARNING(msg, ...)                                                   \
  do {                                                                         \
    addWarning((msg), __VA_ARGS__);                                           \
    if (m_opts.warningsAsErrors) {                                             \
      res = false;                                                             \
      if (m_opts.bailOnFirstError) {                                           \
        return res;                                                            \
      }                                                                        \
    }                                                                          \
  } while (0)

#define ON_ERROR(msg, ...)                                                     \
  do {                                                                         \
    res = false;                                                               \
    add_error((msg), __VA_ARGS__);                                             \
    if (m_opts.bailOnFirstError) {                                             \
      return res;                                                              \
    }                                                                          \
  } while (0)

#define RETURN_ON_FAILURE(expr)                                                \
  if (!(expr))                                                                 \
  return false

// -----------------------------------------------------------------------------

SemanticAnalyzer::SemanticAnalyzer()
  : ASTVisitor()
  , m_errors()
  , m_opts()
{
}

// -----------------------------------------------------------------------------

SemanticAnalyzer::SemanticAnalyzer(const Options& opts)
  : ASTVisitor()
  , m_errors()
  , m_opts(opts)
{
}

// -----------------------------------------------------------------------------

const SemanticAnalyzer::ErrorList&
SemanticAnalyzer::errors() const
{
  return m_errors;
}

// -----------------------------------------------------------------------------

const SemanticAnalyzer::Options&
SemanticAnalyzer::options() const
{
  return m_opts;
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
      ON_ERROR("Found multiple inference group with name \"%s\".",
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
        ON_WARNING("Found repeated environment field \"%s\".", field.c_str());
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
        ON_ERROR("Found multiple inference definition with name \"%s\".",
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
        ON_ERROR("Found duplicate symbol (argument) with name \"%s\".",
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
    const auto& proposition = inferenceDefn.proposition_defn();
    if (!hasCompatibleTargetInTable(proposition.target(),
                                        context.targetTbl)) {
      ON_ERROR("Invalid proposition target type in inference \"%s\".",
               context.name.c_str());
    }
  }

  DEFAULT_RETURN;
}

// -----------------------------------------------------------------------------

bool
SemanticAnalyzer::checkRequiredEnvDefns(const SymbolSet& env_defns)
{
  INIT_RES;

  static const char* mandatoryEnvDefns[]{
      SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_CLASS,
      SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_CLASS,
      SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_PROOF_METHOD,
      SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_CMP_METHOD,
  };

  for (size_t i = 0; i < sizeof(mandatoryEnvDefns) / sizeof(char*); ++i) {
    const char* defn = mandatoryEnvDefns[i];
    if (env_defns.count(defn) == 0) {
      ON_ERROR("Missing required environment definition field \"%s\".", defn);
    }
  }

  DEFAULT_RETURN;
}

// -----------------------------------------------------------------------------

template <>
bool
SemanticAnalyzer::recursivePremiseDefnCheck(
    const ASTInferencePremiseDefn& defn, InferenceDefnContext* context)
{
  INIT_RES;

  // Handle source.
  {
    const auto& source = defn.source();
    const auto& sourceRoot = getRootOfASTIdentifiable(source);
    if (context->symbolSet.count(sourceRoot) == 0) {
      ON_ERROR("Unknown symbol \"%s\" used in inference \"%s\".",
               sourceRoot.c_str(), context->name.c_str());
    }
  }

  // Handle target.
  {
    const auto& target = defn.deductionTarget();
    if (hasIncompatibleTargetInTable(target, context->targetTbl)) {
      ON_ERROR("Found duplicate and incompatible target in inference \"%s\".",
               context->name.c_str());
    }
    addTargetToTable(target, &context->targetTbl);
  }

  // Handle while-clause.
  {
    if (defn.has_while_clause()) {
      const auto& whileClause = defn.while_clause();
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

  if (!areTargetsCompatible(lhs, rhs)) {
    ON_ERROR("Incompatible targets in expression in inference \"%s\".",
             context->name.c_str());
  }

  // Handle range-clause.
  {
    if (defn.has_range_clause()) {
      const auto& rangeClause = defn.range_clause();
      const auto& target = rangeClause.deductionTarget();
      if (target.is_type<ASTDeductionTargetSingular>()) {
        ON_ERROR("Invalid target in range clause in inference \"%s\".",
                 context->name.c_str());
      }
      if (!hasCompatibleTargetInTable(target, context->targetTbl)) {
        ON_ERROR("Invalid target in range clause in inference \"%s\".",
                 context->name.c_str());
      }
    }
  }

  DEFAULT_RETURN;
}

// -----------------------------------------------------------------------------

bool
SemanticAnalyzer::recursivePremiseDefnCheck(
    const ASTPremiseDefn& premiseDefn, InferenceDefnContext* context)
{
  INIT_RES;

  const char* inferenceDefnName = context->name.c_str();

  if (premiseDefn.is_type<ASTInferencePremiseDefn>()) {
    const auto& defnValue = premiseDefn.value<ASTInferencePremiseDefn>();
    RETURN_ON_FAILURE(recursivePremiseDefnCheck(defnValue, context));
  } else if (premiseDefn.is_type<ASTInferenceEqualityDefn>()) {
    const auto& defnValue = premiseDefn.value<ASTInferenceEqualityDefn>();
    RETURN_ON_FAILURE(recursivePremiseDefnCheck(defnValue, context));
  } else {
    ON_ERROR("Found unknown type of premise definition in inference \"%s\".",
             inferenceDefnName);
  }

  DEFAULT_RETURN;
}

// -----------------------------------------------------------------------------
