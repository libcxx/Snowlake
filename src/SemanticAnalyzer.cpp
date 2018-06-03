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

// -----------------------------------------------------------------------------

#define INIT_RES        bool res = true

#define DEFAULT_RETURN  return res

#define ON_WARNING(msg, ...)              \
  do {                                    \
    add_warning( (msg), __VA_ARGS__ );    \
    if (m_opts.warningsAsErrors) {        \
      res = false;                        \
      if (m_opts.bailOnFirstError) {      \
        return res;                       \
      }                                   \
    }                                     \
  } while (0)

#define ON_ERROR(msg, ...)                \
  do {                                    \
    res = false;                          \
    add_error( (msg), __VA_ARGS__ );      \
    if (m_opts.bailOnFirstError) {        \
      return res;                         \
    }                                     \
  } while (0)

// -----------------------------------------------------------------------------

SemanticAnalyzer::SemanticAnalyzer()
  : ASTVisitor(),
    m_errors(),
    m_opts()
{
}

// -----------------------------------------------------------------------------

SemanticAnalyzer::SemanticAnalyzer(const Options& opts)
  : ASTVisitor(),
    m_errors(),
    m_opts(opts)
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
  for (const auto& inference_group : module.inference_groups())
  {
    const auto& name = inference_group.name();
    if (name_set.count(name)) {
      ON_ERROR("Found multiple inference group with name \"%s\".", name.c_str());
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
    for (const auto& environment_defn : inference_group.environment_defns())
    {
      const auto& field = environment_defn.field();
      if (name_set.count(field)) {
        ON_WARNING("Found repeated environment field \"%s\".", field.c_str());
      } else {
        name_set.insert(field);
      }
    }
  }

  // Inference definitions.
  {
    std::unordered_set<std::string> name_set;
    for (const auto& inference_defn : inference_group.inference_defns())
    {
      const auto& name = inference_defn.name();
      if (name_set.count(name)) {
        ON_ERROR("Found multiple inference definition with name \"%s\".", name.c_str());
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

  InferenceDefnContext context {
    .name = inference_defn.name()
  };

  // Global declarations.
  {
    for (const auto& decl: inference_defn.global_decls())
    {
      const auto& name = decl.name();
      if (context.symbol_set.count(name)) {
        ON_WARNING("Found duplicate symbol (global declaration) with name \"%s\".", name.c_str());
      } else {
        context.symbol_set.insert(name);
      }
    }
  }

  // Arguments.
  {
    for (const auto& argument: inference_defn.arguments())
    {
      const auto& name = argument.name();
      if (context.symbol_set.count(name)) {
        ON_ERROR("Found duplicate symbol (argument) with name \"%s\".", name.c_str());
      } else {
        context.symbol_set.insert(name);
      }
    }
  }

  // Premise definitions.
  {
    for (const auto& premise_defn : inference_defn.premise_defns())
    {
      if (!recursive_premise_defn_check(premise_defn, &context))
      {
        return res;
      }
    }
  }

  DEFAULT_RETURN;
}

// -----------------------------------------------------------------------------

template <>
bool
SemanticAnalyzer::recursive_premise_defn_check(const ASTInferencePremiseDefn& defn,
                                               InferenceDefnContext* context)
{
  INIT_RES;

  // Handle source.
  {
    const auto& source = defn.source();
    const auto& source_root = get_root_of_ASTIdentifiable(source);
    if (context->symbol_set.count(source_root))
    {
      ON_ERROR("Unknown symbol \"%s\" used in inference \"%s\".",
        source_root.c_str(), context->name.c_str());
    }
  }

  // Handle target.
  {
    // TODO: Check if incompatble target already exists.
    const auto& target = defn.deduction_target();
    add_target_to_table(target, &context->target_tbl);
  }

  DEFAULT_RETURN;
}

// -----------------------------------------------------------------------------

template <>
bool
SemanticAnalyzer::recursive_premise_defn_check(const ASTInferenceEqualityDefn&,
                                               InferenceDefnContext*)
{
  INIT_RES;
  // TODO: to be implemented...
  DEFAULT_RETURN;
}

// -----------------------------------------------------------------------------

bool
SemanticAnalyzer::recursive_premise_defn_check(const ASTPremiseDefn& premise_defn,
                                               InferenceDefnContext* context)
{
  INIT_RES;

  const char* inference_defn_name = context->name.c_str();

  if (premise_defn.is_type<ASTInferencePremiseDefn>())
  {
    const auto& defn_value = premise_defn.value<ASTInferencePremiseDefn>();
  }
  else if (premise_defn.is_type<ASTInferenceEqualityDefn>())
  {
    const auto& defn_value = premise_defn.value<ASTInferenceEqualityDefn>();
    recursive_premise_defn_check(defn_value, context);
  }
  else
  {
    ON_ERROR("Found unknown type of premise definition in inference \"%s\".",
      inference_defn_name);
  }

  DEFAULT_RETURN;
}

// -----------------------------------------------------------------------------
