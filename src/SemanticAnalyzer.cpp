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

  std::unordered_set<std::string> name_set;
  for (const auto& inference_group : module.inference_groups()) {
    const auto& name = inference_group.name();
    if (name_set.count(name)) {
      ON_ERROR("Found multiple inference group with name \"%s\".",
               name.c_str());
    } else {
      name_set.insert(name);
    }
  }

  DEFAULT_RETURN;
}

// -----------------------------------------------------------------------------

/* override */
bool
SemanticAnalyzer::previsit(const ASTInferenceGroup& inference_group)
{
  INIT_RES;

  // Environment definitions.
  {
    std::unordered_set<std::string> name_set;
    for (const auto& environment_defn : inference_group.environment_defns()) {
      const auto& field = environment_defn.field();
      if (name_set.count(field)) {
        ON_WARNING("Found repeated environment field \"%s\".", field.c_str());
      } else {
        name_set.insert(field);
      }
    }

    if (!checkRequiredEnvDefns(name_set)) {
      return false;
    }
  }

  // Inference definitions.
  {
    std::unordered_set<std::string> name_set;
    for (const auto& inference_defn : inference_group.inference_defns()) {
      const auto& name = inference_defn.name();
      if (name_set.count(name)) {
        ON_ERROR("Found multiple inference definition with name \"%s\".",
                 name.c_str());
      } else {
        name_set.insert(name);
      }
    }
  }

  DEFAULT_RETURN;
}

// -----------------------------------------------------------------------------

/* override */
bool
SemanticAnalyzer::previsit(const ASTInferenceDefn& inference_defn)
{
  INIT_RES;

  InferenceDefnContext context{.name = inference_defn.name()};

  // Global declarations.
  {
    for (const auto& decl : inference_defn.global_decls()) {
      const auto& name = decl.name();
      if (context.symbol_set.count(name)) {
        ON_WARNING(
            "Found duplicate symbol (global declaration) with name \"%s\".",
            name.c_str());
      } else {
        context.symbol_set.insert(name);
      }
    }
  }

  // Arguments.
  {
    for (const auto& argument : inference_defn.arguments()) {
      const auto& name = argument.name();
      if (context.symbol_set.count(name)) {
        ON_ERROR("Found duplicate symbol (argument) with name \"%s\".",
                 name.c_str());
      } else {
        context.symbol_set.insert(name);
      }
    }
  }

  // Premise definitions.
  {
    for (const auto& premise_defn : inference_defn.premise_defns()) {
      if (!recursivePremiseDefnCheck(premise_defn, &context)) {
        return false;
      }
    }
  }

  // Proposition.
  {
    const auto& proposition = inference_defn.proposition_defn();
    if (!has_compatible_target_in_table(proposition.target(),
                                        context.target_tbl)) {
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

  static const char* mandatory_env_defns[]{
      SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_CLASS,
      SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_CLASS,
      SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_PROOF_METHOD,
      SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_CMP_METHOD,
  };

  for (size_t i = 0; i < sizeof(mandatory_env_defns) / sizeof(char*); ++i) {
    const char* defn = mandatory_env_defns[i];
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
    const auto& source_root = get_root_of_ASTIdentifiable(source);
    if (context->symbol_set.count(source_root) == 0) {
      ON_ERROR("Unknown symbol \"%s\" used in inference \"%s\".",
               source_root.c_str(), context->name.c_str());
    }
  }

  // Handle target.
  {
    const auto& target = defn.deduction_target();
    if (has_incompatible_target_in_table(target, context->target_tbl)) {
      ON_ERROR("Found duplicate and incompatible target in inference \"%s\".",
               context->name.c_str());
    }
    add_target_to_table(target, &context->target_tbl);
  }

  // Handle while-clause.
  {
    if (defn.has_while_clause()) {
      const auto& while_clause = defn.while_clause();
      for (const auto& nested_defn : while_clause.premise_defns()) {
        RETURN_ON_FAILURE(recursivePremiseDefnCheck(nested_defn, context));
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

  if (!are_targets_compatible(lhs, rhs)) {
    ON_ERROR("Incompatible targets in expression in inference \"%s\".",
             context->name.c_str());
  }

  // Handle range-clause.
  {
    if (defn.has_range_clause()) {
      const auto& range_clause = defn.range_clause();
      const auto& target = range_clause.deduction_target();
      if (target.is_type<ASTDeductionTargetSingular>()) {
        ON_ERROR("Invalid target in range clause in inference \"%s\".",
                 context->name.c_str());
      }
      if (!has_compatible_target_in_table(target, context->target_tbl)) {
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
    const ASTPremiseDefn& premise_defn, InferenceDefnContext* context)
{
  INIT_RES;

  const char* inference_defn_name = context->name.c_str();

  if (premise_defn.is_type<ASTInferencePremiseDefn>()) {
    const auto& defn_value = premise_defn.value<ASTInferencePremiseDefn>();
    RETURN_ON_FAILURE(recursivePremiseDefnCheck(defn_value, context));
  } else if (premise_defn.is_type<ASTInferenceEqualityDefn>()) {
    const auto& defn_value = premise_defn.value<ASTInferenceEqualityDefn>();
    RETURN_ON_FAILURE(recursivePremiseDefnCheck(defn_value, context));
  } else {
    ON_ERROR("Found unknown type of premise definition in inference \"%s\".",
             inference_defn_name);
  }

  DEFAULT_RETURN;
}

// -----------------------------------------------------------------------------
